/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractSubsetWithSeed.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkExtractSubsetWithSeed.h"

#include "vtkBoundingBox.h"
#include "vtkCell.h"
#include "vtkCompositeDataIterator.h"
#include "vtkDIYExplicitAssigner.h"
#include "vtkDIYUtilities.h"
#include "vtkDataObjectTree.h"
#include "vtkExtractGrid.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLogger.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkMultiPieceDataSet.h"
#include "vtkMultiProcessController.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPartitionedDataSetCollection.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkStaticCellLocator.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStructuredData.h"
#include "vtkStructuredGrid.h"
#include "vtkVector.h"
#include "vtkVectorOperators.h"

// clang-format off
#include "vtk_diy2.h"
// #define DIY_USE_SPDLOG
#include VTK_DIY2(diy/mpi.hpp)
#include VTK_DIY2(diy/master.hpp)
#include VTK_DIY2(diy/link.hpp)
#include VTK_DIY2(diy/reduce.hpp)
#include VTK_DIY2(diy/reduce-operations.hpp)
#include VTK_DIY2(diy/partners/swap.hpp)
#include VTK_DIY2(diy/algorithms.hpp)
// clang-format on

#include <set>

template<typename T, int Size>
bool operator<(const vtkVector<T, Size>& lhs, const vtkVector<T, Size>& rhs)
{
  for (int cc = 0; cc < Size; ++cc)
  {
    if (lhs[cc] != rhs[cc])
    {
      return lhs[cc] < rhs[cc];
    }
  }
  return false;
}

namespace
{

struct BlockT
{
  vtkSmartPointer<vtkStructuredGrid> Input;
  vtkNew<vtkStaticCellLocator> CellLocator;
  std::set<vtkVector<int, 6> > Regions;

  // used to debugging, empty otherwise.
  std::vector<vtkSmartPointer<vtkDataSet> > Seeds;

  // these are generated in GenerateExtracts.
  std::vector<vtkSmartPointer<vtkDataSet> > Extracts;

  void GenerateExtracts();

  bool CopyToOutput(vtkMultiBlockDataSet* mb)
  {
    if (!mb)
    {
      return false;
    }
    for (unsigned int cc = 0, max = mb->GetNumberOfBlocks(); cc < max; ++cc)
    {
      auto child = mb->GetBlock(cc);
      if (child == this->Input)
      {
        vtkNew<vtkMultiPieceDataSet> mp;
        mp->SetNumberOfPieces(static_cast<unsigned int>(this->Extracts.size()));
        for (size_t kk = 0; kk < this->Extracts.size(); ++kk)
        {
          mp->SetPiece(static_cast<unsigned int>(kk), this->Extracts[kk]);
        }
        mb->SetBlock(cc, mp);
        return true;
      }
      else if (auto mp = vtkPartitionedDataSet::SafeDownCast(child))
      {
        if (this->CopyToOutput(mp))
        {
          return true;
        }
      }
      else if (auto childmb = vtkMultiBlockDataSet::SafeDownCast(child))
      {
        if (this->CopyToOutput(childmb))
        {
          return true;
        }
      }
    }
    return false;
  }
  bool CopyToOutput(vtkPartitionedDataSetCollection* pdc)
  {
    if (!pdc)
    {
      return false;
    }
    for (unsigned int cc = 0, max = pdc->GetNumberOfPartitionedDataSets(); cc < max; ++cc)
    {
      if (this->CopyToOutput(pdc->GetPartitionedDataSet(cc)))
      {
        return true;
      }
    }
    return false;
  }

  bool CopyToOutput(vtkPartitionedDataSet* mp)
  {
    if (!mp)
    {
      return false;
    }
    for (unsigned int cc = 0, max = mp->GetNumberOfPartitions(); cc < max; ++cc)
    {
      auto child = mp->GetPartition(cc);
      if (child == this->Input)
      {
        mp->SetPartition(cc, nullptr);
        if (this->Extracts.size() > 0)
        {
          mp->SetPartition(cc, this->Extracts[0]);
        }
        // the following will cause issues with multi-piece datasets in parallel
        // unless we explicitly sync up composite tree across ranks.
        for (auto& extract : this->Extracts)
        {
          mp->SetPartition(mp->GetNumberOfPartitions(), extract);
        }
        return true;
      }
    }
    return false;
  }
};

void BlockT::GenerateExtracts()
{
  this->Extracts.clear();
  vtkNew<vtkExtractGrid> extractor; // TODO: avoid creating new one on each iteration.
  for (const auto& voi : this->Regions)
  {
    extractor->SetInputDataObject(this->Input);
    extractor->SetVOI(voi[0], voi[1], voi[2], voi[3], voi[4], voi[5]);
    extractor->Update();

    vtkStructuredGrid* clone = vtkStructuredGrid::New();
    clone->ShallowCopy(extractor->GetOutputDataObject(0));
    this->Extracts.push_back(clone);
    clone->FastDelete();
  }
}

using OrientationT = vtkVector3d;
using SeedT = std::tuple<vtkVector3d, vtkVector3d, vtkVector3d>;

vtkVector<int, 6> ComputeVOI(
  vtkStructuredGrid* input, const int ijk[3], const int propagation_mask[3])
{
  vtkVector<int, 6> voi;
  int data_ext[6];
  input->GetExtent(data_ext);
  for (int cc = 0; cc < 3; ++cc)
  {
    if (propagation_mask[cc] == 0)
    {
      voi[2 * cc] = ijk[cc];
      voi[2 * cc + 1] = ijk[cc] + 1;
    }
    else
    {
      voi[2 * cc] = data_ext[2 * cc];
      voi[2 * cc + 1] = data_ext[2 * cc + 1];
    }
  }

  return voi;
}

/**
 * Returns 3 unit vectors that identify the i,j,k directions for the cell.
 * Assumes vtkCell is vtkHexahedron.
 */
vtkVector3<vtkVector3d> GetCellOrientationVectors(vtkCell* cell)
{
  assert(cell->GetCellType() == VTK_HEXAHEDRON);
  const std::pair<int, int> indexes[] = { std::make_pair(0, 1), std::make_pair(0, 3),
    std::make_pair(0, 4) };

  vtkVector3<vtkVector3d> values;
  for (int axis = 0; axis < 3; axis++)
  {
    const auto& idx = indexes[axis];
    vtkVector3d p0, p1;
    cell->GetPoints()->GetPoint(idx.first, p0.GetData());
    cell->GetPoints()->GetPoint(idx.second, p1.GetData());
    values[axis] = (p1 - p0);
    values[axis].Normalize();
  }
  return values;
}

std::pair<vtkVector3d, vtkVector3d> GetPropagationVectors(
  vtkCell* cell, const int progation_mask[3])
{
  const auto cell_orientation = GetCellOrientationVectors(cell);
  vtkVector3d values[3];
  int v_idx = 0;
  for (int axis = 0; axis < 3; axis++)
  {
    values[axis] = vtkVector3d(0.0);
    if (progation_mask[axis] > 0)
    {
      assert(v_idx < 2);
      values[v_idx++] = cell_orientation(axis);
    }
  }
  return std::make_pair(values[0], values[1]);
}

vtkVector3d GetFaceCenter(vtkCell* cell, int face_id)
{
  double weights[8];
  auto face = cell->GetFace(face_id);
  double center[3], pcoords[3];
  int subId = face->GetParametricCenter(pcoords);
  face->EvaluateLocation(subId, pcoords, center, weights);
  return vtkVector3d(center);
}

std::vector<SeedT> ExtractSliceFromSeed(const vtkVector3d& seed,
  const std::vector<vtkVector3d>& dirs, BlockT* b, const diy::Master::ProxyWithLink&)
{
  auto sg = b->Input;
  assert(vtkStructuredData::GetDataDescriptionFromExtent(sg->GetExtent()) == VTK_XYZ_GRID);

  const vtkIdType cellid = b->CellLocator->FindCell(const_cast<double*>(seed.GetData()));
  if (cellid <= 0)
  {
    return std::vector<SeedT>();
  }

  // okay, we've determined that the seed lies in this block's grid. now we need to determine the
  // voi to extract based on the propagation directions provided.

  // using the cell's orientation, first determine which ijk axes the propagation directions
  // correspond to.
  auto cell_vectors = ::GetCellOrientationVectors(b->Input->GetCell(cellid));
  int propagation_mask[3] = { 0, 0, 0 };
  for (const auto& dir : dirs)
  {
    assert(dir.SquaredNorm() != 0);

    double max = 0.0;
    int axis = -1;
    for (int cc = 0; cc < 3; ++cc)
    {
      const auto dot = std::abs(dir.Dot(cell_vectors[cc]));
      if (dot > max)
      {
        max = dot;
        axis = cc;
      }
    }
    if (axis != -1)
    {
      propagation_mask[axis] = 1;
    }
  }
  assert(std::accumulate(propagation_mask, propagation_mask + 3, 0) < 3);

  int ijk[3];
  vtkStructuredData::ComputeCellStructuredCoordsForExtent(cellid, sg->GetExtent(), ijk);

  const auto voi = ComputeVOI(sg, ijk, propagation_mask);
  if (b->Regions.find(voi) != b->Regions.end())
  {
    return std::vector<SeedT>();
  }

  b->Regions.insert(voi);

  vtkVector<int, 6> cell_voi;
  vtkStructuredData::GetCellExtentFromPointExtent(voi.GetData(), cell_voi.GetData());

  std::vector<SeedT> next_seeds;
  for (int axis = 0; axis < 3; ++axis)
  {
    if (propagation_mask[axis] == 0)
    {
      continue;
    }

    // generate new seeds along each of the propagation axis e.g for i axis,
    // we add seeds along the j-k plane for min and max i values.

    // get the other two axes
    int dir_ii = (axis + 1) % 3;
    int dir_jj = (axis + 2) % 3;

    for (int iter = 0; iter < 2; ++iter)
    {
      ijk[axis] = cell_voi[2 * axis + iter];
      for (int ii = cell_voi[2 * dir_ii]; ii <= cell_voi[2 * dir_ii + 1]; ++ii)
      {
        for (int jj = cell_voi[2 * dir_jj]; jj <= cell_voi[2 * dir_jj + 1]; ++jj)
        {
          ijk[dir_ii] = ii;
          ijk[dir_jj] = jj;

          const auto acellid = vtkStructuredData::ComputeCellIdForExtent(sg->GetExtent(), ijk);
          auto cell = sg->GetCell(acellid);
          switch (sg->GetCellType(acellid))
          {
            case VTK_HEXAHEDRON:
            {
              const auto new_seed = ::GetFaceCenter(cell, 2 * axis + iter);
              const auto pvecs = ::GetPropagationVectors(cell, propagation_mask);
              next_seeds.push_back(std::make_tuple(new_seed, pvecs.first, pvecs.second));
            }
            break;

            default:
              continue;
          }
        }
      }
    }
  }

#if 0
  vtkNew<vtkPoints> pts;
  pts->SetNumberOfPoints(next_seeds.size());
  for (size_t cc=0; cc < next_seeds.size(); ++cc)
  {
    pts->SetPoint(cc, std::get<0>(next_seeds[cc]).GetData());
  }

  vtkNew<vtkPolyData> pd;
  pd->SetPoints(pts);
  b->Seeds.push_back(pd);
#endif

  return next_seeds;
}

/**
 * shallow copy except those blocks where the predicate returns true.
 */
template<typename UnaryPredicate>
void ShallowCopyIfNot(vtkCompositeDataSet* input, vtkCompositeDataSet* output, UnaryPredicate p)
{
  output->CopyStructure(input);
  auto iter = input->NewIterator();
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
  {
    auto dobj = iter->GetCurrentDataObject();
    if (!p(dobj))
    {
      output->SetDataSet(iter, dobj);
    }
  }
  iter->Delete();
}

} // namespace {}

vtkStandardNewMacro(vtkExtractSubsetWithSeed);
vtkCxxSetObjectMacro(vtkExtractSubsetWithSeed, Controller, vtkMultiProcessController);
//----------------------------------------------------------------------------
vtkExtractSubsetWithSeed::vtkExtractSubsetWithSeed()
{
  this->SetController(vtkMultiProcessController::GetGlobalController());
}

//----------------------------------------------------------------------------
vtkExtractSubsetWithSeed::~vtkExtractSubsetWithSeed()
{
  this->SetController(nullptr);
}

//----------------------------------------------------------------------------
void vtkExtractSubsetWithSeed::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Controller: " << this->Controller << endl;
  os << indent << "Direction: ";
  switch (this->Direction)
  {
    case LINE_I:
      os << "LINE_I" << endl;
      break;
    case LINE_J:
      os << "LINE_J" << endl;
      break;
    case LINE_K:
      os << "LINE_K" << endl;
      break;
    case PLANE_IJ:
      os << "PLANE_IJ" << endl;
      break;
    case PLANE_JK:
      os << "PLANE_JK" << endl;
      break;
    case PLANE_KI:
      os << "PLANE_KI" << endl;
      break;
    default:
      os << "(UNKNOWN)" << endl;
      break;
  }
}

//----------------------------------------------------------------------------
int vtkExtractSubsetWithSeed::RequestDataObject(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  auto inputDO = vtkDataObject::GetData(inputVector[0], 0);
  auto outputDO = vtkDataObject::GetData(outputVector, 0);

  vtkSmartPointer<vtkDataObject> newoutput;
  if (vtkStructuredGrid::SafeDownCast(inputDO))
  {
    if (!vtkPartitionedDataSet::SafeDownCast(outputDO))
    {
      newoutput.TakeReference(vtkPartitionedDataSet::New());
    }
  }
  else if (auto inDOT = vtkDataObjectTree::SafeDownCast(inputDO))
  {
    if (outputDO == nullptr || !outputDO->IsA(inDOT->GetClassName()))
    {
      newoutput.TakeReference(inDOT->NewInstance());
    }
  }

  if (newoutput)
  {
    outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), newoutput);
  }

  return 1;
}

//----------------------------------------------------------------------------
int vtkExtractSubsetWithSeed::RequestData(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  auto input = vtkDataObject::GetData(inputVector[0], 0);

  auto datasets = vtkDIYUtilities::GetDataSets(input);
  // prune non-structured grid datasets.
  auto prunePredicate = [](vtkDataObject* ds) {
    auto sg = vtkStructuredGrid::SafeDownCast(ds);
    // skip empty or non-3D grids.
    return sg == nullptr ||
      vtkStructuredData::GetDataDescriptionFromExtent(sg->GetExtent()) != VTK_XYZ_GRID;
  };

  datasets.erase(std::remove_if(datasets.begin(), datasets.end(), prunePredicate), datasets.end());

  diy::mpi::communicator comm = vtkDIYUtilities::GetCommunicator(this->GetController());
  const int local_num_blocks = static_cast<int>(datasets.size());

  vtkDIYExplicitAssigner assigner(comm, local_num_blocks);

  diy::Master master(comm, 1, -1, []() { return static_cast<void*>(new BlockT); },
    [](void* b) { delete static_cast<BlockT*>(b); });

  vtkLogStartScope(TRACE, "populate master");
  std::vector<int> gids;
  assigner.local_gids(comm.rank(), gids);
  assert(gids.size() == datasets.size());
  for (size_t lid = 0; lid < gids.size(); ++lid)
  {
    auto block = new BlockT();
    block->Input = vtkStructuredGrid::SafeDownCast(datasets[lid]);
    assert(block->Input != nullptr &&
      vtkStructuredData::GetDataDescriptionFromExtent(block->Input->GetExtent()) == VTK_XYZ_GRID);

    block->CellLocator->SetDataSet(block->Input);
    block->CellLocator->BuildLocator();
    master.add(gids[lid], block, new diy::Link);
  }
  vtkLogEndScope("populate master");

  // exchange bounding boxes to determine neighbours.
  vtkLogStartScope(TRACE, "populate block neighbours");
  std::map<int, std::vector<int> > neighbors;
  diy::all_to_all(master, assigner, [&neighbors](BlockT* b, const diy::ReduceProxy& rp) {
    double bds[6];
    b->Input->GetBounds(bds);
    vtkBoundingBox bbox(bds);
    bbox.Inflate(0.000001);

    if (rp.round() == 0)
    {
      for (int i = 0; i < rp.out_link().size(); ++i)
      {
        const auto dest = rp.out_link().target(i);
        rp.enqueue(dest, bds, 6);
      };
    }
    else
    {
      for (int i = 0; i < rp.in_link().size(); ++i)
      {
        const auto src = rp.in_link().target(i);
        double in_bds[6];
        rp.dequeue(src, in_bds, 6);
        vtkBoundingBox in_bbx(in_bds);
        if (src.gid != rp.gid() && in_bbx.IsValid() && in_bbx.Intersects(bbox))
        {
          vtkLogF(TRACE, "%d --> %d", rp.gid(), src.gid);
          neighbors[rp.gid()].push_back(src.gid);
        }
      }
    }
  });

  // update local links.
  for (auto& pair : neighbors)
  {
    auto l = new diy::Link();
    for (const auto& nid : pair.second)
    {
      l->add_neighbor(diy::BlockID(nid, assigner.rank(nid)));
    }
    master.replace_link(master.lid(pair.first), l);
  }

  vtkLogEndScope("populate block neighbours");

  int propagation_mask[3] = { 0, 0, 0 };
  switch (this->Direction)
  {
    case LINE_I:
      propagation_mask[0] = 1;
      break;
    case LINE_J:
      propagation_mask[1] = 1;
      break;
    case LINE_K:
      propagation_mask[2] = 1;
      break;
    case PLANE_IJ:
      propagation_mask[0] = 1;
      propagation_mask[1] = 1;
      break;
    case PLANE_JK:
      propagation_mask[1] = 1;
      propagation_mask[2] = 1;
      break;
    case PLANE_KI:
      propagation_mask[2] = 1;
      propagation_mask[0] = 1;
      break;
    default:
      break;
  }
  bool all_done = false;
  int round = 0;
  while (!all_done)
  {
    master.foreach (
      [this, &round, &propagation_mask](BlockT* b, const diy::Master::ProxyWithLink& cp) {
        std::vector<SeedT> seeds;
        if (round == 0)
        {
          const vtkIdType cellid = b->CellLocator->FindCell(this->GetSeed());
          if (cellid >= 0)
          {
            auto p_vecs = ::GetPropagationVectors(b->Input->GetCell(cellid), propagation_mask);
            seeds.push_back(
              std::make_tuple(vtkVector3d(this->GetSeed()), p_vecs.first, p_vecs.second));
          }
        }
        else
        {
          // dequeue
          std::vector<int> incoming;
          cp.incoming(incoming);
          for (const int& gid : incoming)
          {
            if (cp.incoming(gid).size() > 0)
            {
              std::vector<SeedT> next_seeds;
              cp.dequeue(gid, next_seeds);
              seeds.insert(seeds.end(), next_seeds.begin(), next_seeds.end());
            }
          }
        }

        std::vector<SeedT> next_seeds;
        for (const auto& seed : seeds)
        {
          std::vector<vtkVector3d> dirs;
          if (std::get<1>(seed).SquaredNorm() != 0)
          {
            dirs.push_back(std::get<1>(seed));
          }
          if (std::get<2>(seed).SquaredNorm() != 0)
          {
            dirs.push_back(std::get<2>(seed));
          }
          auto new_seeds = ::ExtractSliceFromSeed(std::get<0>(seed), dirs, b, cp);
          next_seeds.insert(next_seeds.end(), new_seeds.begin(), new_seeds.end());
        }

        if (next_seeds.size() > 0)
        {
          // enqueue
          for (const auto& neighbor : cp.link()->neighbors())
          {
            vtkLogF(TRACE, "r=%d: enqueing %d --> (%d, %d)", round, cp.gid(), neighbor.gid, neighbor.proc);
            cp.enqueue(neighbor, next_seeds);
          }
        }

        cp.collectives()->clear();

        const int has_seeds = (next_seeds.size() > 0) ? 1 : 0;
        cp.all_reduce(has_seeds, std::logical_or<int>());
      });
    vtkLogF(TRACE, "r=%d, exchange", round);
    master.exchange();
    all_done = (master.proxy(master.loaded_block()).read<int>() == 0);
    ++round;
  }

  // iterate over each block to combine the regions and extract.
  master.foreach ([&](BlockT* b, const diy::Master::ProxyWithLink&) { b->GenerateExtracts(); });

  if (auto outputPD = vtkPartitionedDataSet::GetData(outputVector, 0))
  {
    unsigned int idx = 0;
    master.foreach ([&](BlockT* b, const diy::Master::ProxyWithLink&) {
      for (auto& extract : b->Extracts)
      {
        outputPD->SetPartition(idx++, extract);
      }
      for (auto& seed : b->Seeds)
      {
        outputPD->SetPartition(idx++, seed);
      }
    });
  }
  else if (auto outputMB = vtkMultiBlockDataSet::GetData(outputVector, 0))
  {
    auto inputMB = vtkMultiBlockDataSet::GetData(inputVector[0], 0);
    ::ShallowCopyIfNot(inputMB, outputMB, prunePredicate);
    master.foreach (
      [&](BlockT* b, const diy::Master::ProxyWithLink&) { b->CopyToOutput(outputMB); });
  }
  else if (auto outputPDC = vtkPartitionedDataSetCollection::GetData(outputVector, 0))
  {
    auto inputPDC = vtkPartitionedDataSetCollection::GetData(inputVector[0], 0);
    assert(inputPDC);
    ::ShallowCopyIfNot(inputPDC, outputPDC, prunePredicate);
    master.foreach (
      [&](BlockT* b, const diy::Master::ProxyWithLink&) { b->CopyToOutput(outputPDC); });
  }

  vtkInformation* info = outputVector->GetInformationObject(0);
  info->Remove(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
  return 1;
}

//----------------------------------------------------------------------------
int vtkExtractSubsetWithSeed::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkMultiBlockDataSet");
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPartitionedDataSetCollection");
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPartitionedDataSet");
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkStructuredGrid");
  return 1;
}

//----------------------------------------------------------------------------
int vtkExtractSubsetWithSeed::RequestInformation(
  vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
  vtkInformation* info = outputVector->GetInformationObject(0);
  info->Remove(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
  return 1;
}
