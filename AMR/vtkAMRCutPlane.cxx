/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vtkAMRCutPlane.cxx

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/
#include "vtkAMRCutPlane.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkMultiProcessController.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIndent.h"
#include "vtkPlane.h"
#include "vtkAMRBox.h"
#include "vtkAMRUtilities.h"
#include "vtkUniformGrid.h"
#include "vtkCutter.h"
#include "vtkIncrementalOctreePointLocator.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkOverlappingAMR.h"

#include <cassert>
#include <algorithm>


vtkStandardNewMacro(vtkAMRCutPlane);

//------------------------------------------------------------------------------
vtkAMRCutPlane::vtkAMRCutPlane()
{
  this->SetNumberOfInputPorts( 1 );
  this->SetNumberOfOutputPorts( 1 );
  this->LevelOfResolution = 0;
  this->initialRequest    = true;
  for( int i=0; i < 3; ++i )
    {
    this->Center[i] = 0.0;
    this->Normal[i] = 0.0;
    }
  this->Controller       = vtkMultiProcessController::GetGlobalController();
  this->Plane            = NULL;
  this->UseNativeCutter  = 1;
}

//------------------------------------------------------------------------------
vtkAMRCutPlane::~vtkAMRCutPlane()
{
  this->blocksToLoad.clear();
  if( this->Plane != NULL )
    {
    this->Plane->Delete();
    }
  this->Plane = NULL;
}

//------------------------------------------------------------------------------
void vtkAMRCutPlane::PrintSelf( std::ostream &oss, vtkIndent indent )
{
  this->Superclass::PrintSelf( oss, indent );
  oss << indent << "LevelOfResolution: "
      << this->LevelOfResolution << endl;
  oss << indent << "UseNativeCutter: "
      << this->UseNativeCutter << endl;
  oss << indent << "Controller: "
      << this->Controller << endl;
  oss << indent << "Center: ";
  for( int i=0; i < 3; ++i )
    {
    oss << this->Center[i ] << " ";
    }
  oss << endl;
  oss << indent << "Normal: ";
  for( int i=0; i < 3; ++i )
    {
    oss << this->Normal[i] << " ";
    }
  oss << endl;
}

//------------------------------------------------------------------------------
int vtkAMRCutPlane::FillInputPortInformation(
    int vtkNotUsed(port), vtkInformation *info )
{
  assert( "pre: information object is NULL!" && (info != NULL) );
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(),"vtkOverlappingAMR");
  return 1;
}

//------------------------------------------------------------------------------
int vtkAMRCutPlane::FillOutputPortInformation(
    int vtkNotUsed(port), vtkInformation *info )
{
  assert( "pre: information object is NULL!" && (info != NULL) );
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkMultiBlockDataSet" );
  return 1;
}

//------------------------------------------------------------------------------
int vtkAMRCutPlane::RequestInformation(
    vtkInformation* vtkNotUsed(rqst), vtkInformationVector** inputVector,
    vtkInformationVector* vtkNotUsed(outputVector) )
{
  this->blocksToLoad.clear();

  if( this->Plane != NULL )
    {
    this->Plane->Delete();
    }

  vtkInformation *input = inputVector[0]->GetInformationObject(0);
  assert( "pre: input information object is NULL" && (input != NULL) );

  if( input->Has(vtkCompositeDataPipeline::COMPOSITE_DATA_META_DATA() ) )
    {
    vtkOverlappingAMR *metadata =
        vtkOverlappingAMR::SafeDownCast(
          input->Get(vtkCompositeDataPipeline::COMPOSITE_DATA_META_DATA()));

    this->Plane = this->GetCutPlane( metadata );
    assert( "Cut plane is NULL" && (this->Plane != NULL) );

    this->ComputeAMRBlocksToLoad( this->Plane, metadata );
    }

  this->Modified();
  return 1;
}

//------------------------------------------------------------------------------
int vtkAMRCutPlane::RequestUpdateExtent(
    vtkInformation* vtkNotUsed(rqst), vtkInformationVector** inputVector,
    vtkInformationVector* vtkNotUsed(outputVector) )
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  assert( "pre: inInfo is NULL" && (inInfo != NULL) );

  inInfo->Set(
      vtkCompositeDataPipeline::UPDATE_COMPOSITE_INDICES(),
      &this->blocksToLoad[0], this->blocksToLoad.size() );
  return 1;
}

//------------------------------------------------------------------------------
int vtkAMRCutPlane::RequestData(
    vtkInformation* vtkNotUsed(rqst), vtkInformationVector** inputVector,
    vtkInformationVector* outputVector )
{
  // STEP 0: Get input object
  vtkInformation *input = inputVector[0]->GetInformationObject( 0 );
  assert( "pre: input information object is NULL" && (input != NULL)  );
  vtkOverlappingAMR *inputAMR=
      vtkOverlappingAMR::SafeDownCast(
          input->Get( vtkDataObject::DATA_OBJECT() ) );
  assert( "pre: input AMR dataset is NULL!" && (inputAMR != NULL) );

  // STEP 1: Get output object
  vtkInformation *output = outputVector->GetInformationObject( 0 );
  assert( "pre: output information is NULL" && (output != NULL) );
  vtkMultiBlockDataSet *mbds=
      vtkMultiBlockDataSet::SafeDownCast(
          output->Get( vtkDataObject::DATA_OBJECT() ) );
  assert( "pre: output multi-block dataset is NULL" && (mbds != NULL) );

  if( this->IsAMRData2D( inputAMR ) )
    {
    // Return an empty multi-block, we cannot cut a 2-D dataset
    return 1;
    }

  unsigned int blockIdx = 0;
  unsigned int level    = 0;
  for( ; level < inputAMR->GetNumberOfLevels(); ++level )
    {
    unsigned int dataIdx = 0;
    for( ; dataIdx < inputAMR->GetNumberOfDataSets( level ); ++dataIdx )
      {
      vtkUniformGrid *grid = inputAMR->GetDataSet( level, dataIdx );
      if( this->UseNativeCutter == 1 )
        {
        if( grid != NULL )
          {
          vtkCutter *myCutter = vtkCutter::New();
          myCutter->SetInputData( grid );
          myCutter->SetCutFunction( this->Plane );
          myCutter->Update();
          mbds->SetBlock( blockIdx, myCutter->GetOutput( ) );
          ++blockIdx;
          myCutter->Delete();
          }
        else
          {
          mbds->SetBlock(blockIdx,NULL);
          ++blockIdx;
          }
        }
      else
        {
        if( grid != NULL )
          {
          this->CutAMRBlock( blockIdx, grid, mbds );
          ++blockIdx;
          }
        else
          {
          mbds->SetBlock(blockIdx,NULL);
          ++blockIdx;
          }
        }
      } // END for all data
    } // END for all levels

  this->Modified();
  return 1;
}

//------------------------------------------------------------------------------
void vtkAMRCutPlane::CutAMRBlock(
    unsigned int blockIdx, vtkUniformGrid *grid, vtkMultiBlockDataSet *output )
{
  // Locator, used for detecting duplicate points
  vtkIncrementalOctreePointLocator *locator = vtkIncrementalOctreePointLocator::New();


  vtkPolyData *mesh       = vtkPolyData::New();
  vtkPoints *meshPts      = vtkPoints::New();
  vtkCellArray *meshVerts = vtkCellArray::New();
  vtkCellArray *cells     = vtkCellArray::New();

  // STEP 0: Initialize locator
  locator->InitPointInsertion(meshPts,grid->GetBounds());

  vtkIdType cellIdx = 0;
  for( ; cellIdx < grid->GetNumberOfCells(); ++cellIdx )
    {
    if( grid->IsCellVisible( cellIdx ) &&
        this->PlaneIntersectsCell( grid->GetCell(cellIdx) ) )
      {
      this->ExtractCellFromGrid(
          grid,grid->GetCell(cellIdx),locator,cells );
      } // END if
    } // END for all cells

  // Insert the points
  mesh->SetPoints( meshPts );
  meshPts->Delete();

  // Insert mesh vertices
  vtkIdType idx = 0;
  for( ;idx < meshPts->GetNumberOfPoints(); ++idx )
    {
    meshVerts->InsertNextCell(1);
    meshVerts->InsertCellPoint(idx);
    } // END for all points
  mesh->SetVerts( meshVerts );
  meshVerts->Delete();

  // Insert the cells
  mesh->SetPolys( cells );
  cells->Delete();

  locator->Delete();
  output->SetBlock( blockIdx, mesh );
  mesh->Delete();
}

//------------------------------------------------------------------------------
void vtkAMRCutPlane::ExtractCellFromGrid(
            vtkUniformGrid *grid,
            vtkCell* cell, vtkIncrementalOctreePointLocator *loc,
            vtkCellArray *cells)
{
  assert( "pre: grid is NULL"  && (grid != NULL)  );
  assert( "pre: cell is NULL"  && (cell != NULL)  );
  assert( "pre: loc is NULL"   && (loc != NULL)   );
  assert( "pre: cells is NULL" && (cells != NULL) );

  switch( cell->GetCellType() )
    {
    case VTK_PIXEL:
    case VTK_QUAD:
      cells->InsertNextCell(4);
      break;
    case VTK_VOXEL:
    case VTK_HEXAHEDRON:
      cells->InsertNextCell(8);
      break;
    default:
      vtkErrorMacro("Cell type must be either a quad(pixel) or a hex(voxel)!");
    }

  vtkIdType nodeIdx = 0;
  for( ; nodeIdx < cell->GetNumberOfPoints(); ++nodeIdx )
    {
    vtkIdType meshPntIdx = cell->GetPointId( nodeIdx );
    assert( "pre: mesh point ID should within grid range point ID" &&
            (meshPntIdx < grid->GetNumberOfPoints()));

    vtkIdType targetMeshPntIdx =
        loc->InsertNextPoint( grid->GetPoint(meshPntIdx) );
    cells->InsertCellPoint( targetMeshPntIdx );
    } // END for all nodes

}

//------------------------------------------------------------------------------
vtkPlane* vtkAMRCutPlane::GetCutPlane( vtkOverlappingAMR *metadata )
{
  assert( "pre: metadata is NULL" && (metadata != NULL) );

  vtkPlane *pl = vtkPlane::New();

  // Get global bounds
  double minBounds[3];
  double maxBounds[3];
  vtkAMRBox root;
  metadata->GetRootAMRBox( root );
  root.GetMinBounds( minBounds );
  root.GetMaxBounds( maxBounds );

  this->InitializeCenter( minBounds, maxBounds );

  pl->SetNormal( this->Normal );
  pl->SetOrigin( this->Center );
  return( pl );
}

//------------------------------------------------------------------------------
void vtkAMRCutPlane::ComputeAMRBlocksToLoad(
      vtkPlane* p, vtkOverlappingAMR *m)
{
  assert( "pre: Plane object is NULL" && (p != NULL) );
  assert( "pre: metadata is NULL" && (m != NULL) );

  // Store A,B,C,D from the plane equation
  double plane[4];
  plane[0] = p->GetNormal()[0];
  plane[1] = p->GetNormal()[1];
  plane[2] = p->GetNormal()[2];
  plane[3] = p->GetNormal()[0]*p->GetOrigin()[0] +
             p->GetNormal()[1]*p->GetOrigin()[1] +
             p->GetNormal()[2]*p->GetOrigin()[2];

  double bounds[6];

  int NumLevels = m->GetNumberOfLevels();
  int maxLevelToLoad =
     (this->LevelOfResolution < NumLevels )?
         this->LevelOfResolution : NumLevels;

  unsigned int level = 0;
  for( ; level <= static_cast<unsigned int>(maxLevelToLoad); ++level )
    {
    unsigned int dataIdx = 0;
    for( ; dataIdx < m->GetNumberOfDataSets( level ); ++dataIdx )
      {
      vtkAMRBox box;
      m->GetMetaData( level, dataIdx, box  );
      bounds[0] = box.GetMinX();
      bounds[1] = box.GetMaxX();
      bounds[2] = box.GetMinY();
      bounds[3] = box.GetMaxY();
      bounds[4] = box.GetMinZ();
      bounds[5] = box.GetMaxZ();

      if( this->PlaneIntersectsAMRBox( plane, bounds ) )
        {
        unsigned int amrGridIdx = m->GetCompositeIndex(level,dataIdx);
        this->blocksToLoad.push_back( amrGridIdx );
        }
      } // END for all data
    } // END for all levels

  std::sort( this->blocksToLoad.begin(), this->blocksToLoad.end() );
}

//------------------------------------------------------------------------------
void vtkAMRCutPlane::InitializeCenter( double min[3], double max[3] )
{
  if( !this->initialRequest )
    {
    return;
    }

  this->Center[0] = 0.5*( max[0]-min[0] );
  this->Center[1] = 0.5*( max[1]-min[1] );
  this->Center[2] = 0.5*( max[2]-min[2] );
  this->initialRequest = false;
}

//------------------------------------------------------------------------------
bool vtkAMRCutPlane::PlaneIntersectsCell( vtkCell *cell )
{
  assert( "pre: cell is NULL!" && (cell != NULL) );
  return( this->PlaneIntersectsAMRBox( cell->GetBounds() ) );
}
//------------------------------------------------------------------------------
bool vtkAMRCutPlane::PlaneIntersectsAMRBox( double bounds[6] )
{
  // Store A,B,C,D from the plane equation
  double plane[4];
  plane[0] = this->Plane->GetNormal()[0];
  plane[1] = this->Plane->GetNormal()[1];
  plane[2] = this->Plane->GetNormal()[2];
  plane[3] = this->Plane->GetNormal()[0]*this->Plane->GetOrigin()[0] +
             this->Plane->GetNormal()[1]*this->Plane->GetOrigin()[1] +
             this->Plane->GetNormal()[2]*this->Plane->GetOrigin()[2];

 return( this->PlaneIntersectsAMRBox( plane,bounds) );
}

//------------------------------------------------------------------------------
bool vtkAMRCutPlane::PlaneIntersectsAMRBox( double plane[4], double bounds[6] )
{
  bool lowPnt  = false;
  bool highPnt = false;

  for( int i=0; i < 8; ++i )
    {
    // Get box coordinates
    double x = ( i&1 ? bounds[1] : bounds[0] );
    double y = ( i&2 ? bounds[3] : bounds[2] );
    double z = ( i&3 ? bounds[5] : bounds[4] );

    // Plug-in coordinates to the plane equation
    double v = plane[3] - plane[0]*x - plane[1]*y - plane[2]*z;

    if( v == 0.0 ) // Point is on a plane
      {
      return true;
      }

    if( v < 0.0 )
      {
      lowPnt = true;
      }
    else
      {
      highPnt = true;
      }

    if( lowPnt && highPnt )
      {
      return true;
      }
    }

  return false;
}

//------------------------------------------------------------------------------
bool vtkAMRCutPlane::IsAMRData2D( vtkOverlappingAMR *input )
{
  assert( "pre: Input AMR dataset is NULL" && (input != NULL)  );

  vtkAMRBox box;
  input->GetMetaData( 0, 0, box );

  if( box.GetDimensionality() == 2 )
    {
    return true;
    }

 return false;
}
