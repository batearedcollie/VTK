/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vtkAMRDualMeshExtractor.cxx

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include <sstream>
#include <cassert>

#include "vtkAMRDualMeshExtractor.h"
#include "vtkObjectFactory.h"
#include "vtkHierarchicalBoxDataSet.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkMultiPieceDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUniformGrid.h"
#include "vtkIdList.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkXMLMultiBlockDataWriter.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIntArray.h"

//
// Standard methods
//
vtkStandardNewMacro( vtkAMRDualMeshExtractor );


vtkAMRDualMeshExtractor::vtkAMRDualMeshExtractor()
{
  this->SetNumberOfOutputPorts( 1 );
}

//------------------------------------------------------------------------------
vtkAMRDualMeshExtractor::~vtkAMRDualMeshExtractor()
{
}

//------------------------------------------------------------------------------
void vtkAMRDualMeshExtractor::PrintSelf( std::ostream& oss, vtkIndent indent )
{
 this->Superclass::PrintSelf( oss, indent );
}

//------------------------------------------------------------------------------
int vtkAMRDualMeshExtractor::FillInputPortInformation(
              int vtkNotUsed(port), vtkInformation *info )
{
  info->Set(
   vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(),"vtkHierarchicalBoxDataSet");
  return 1;
}

//------------------------------------------------------------------------------
int vtkAMRDualMeshExtractor::FillOutputPortInformation(
              int vtkNotUsed(port), vtkInformation *info )
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(),"vtkMultiBlockDataSet");
  return 1;
}

//------------------------------------------------------------------------------
int vtkAMRDualMeshExtractor::RequestData( vtkInformation *request,
    vtkInformationVector **inputVector, vtkInformationVector *outputVector )
{
  // STEP 0: Get input object
  vtkInformation *input = inputVector[0]->GetInformationObject( 0 );
  assert( "pre: Null input information object!" && (input != NULL) );

  vtkHierarchicalBoxDataSet *amrds=
   vtkHierarchicalBoxDataSet::SafeDownCast(
    input->Get(vtkDataObject::DATA_OBJECT()));
  assert( "pre: input AMR dataset is NULL" && (amrds != NULL) );

  // STEP 1: Get output object
  vtkInformation *output= outputVector->GetInformationObject(0);
  assert( "pre: Null output information object!" && (output != NULL) );

  vtkMultiBlockDataSet *mbds=
   vtkMultiBlockDataSet::SafeDownCast(
    output->Get(vtkDataObject::DATA_OBJECT()));
  assert( "pre: ouput multi-block dataset is NULL" && (mbds != NULL) );

  // STEP 2: Allocate output multi-block data-structure
  mbds->SetNumberOfBlocks( amrds->GetNumberOfLevels() );

  unsigned int level=0;
  for( ; level < amrds->GetNumberOfLevels(); ++level )
    {
      vtkMultiPieceDataSet *mpds = vtkMultiPieceDataSet::New();
      mpds->SetNumberOfPieces( amrds->GetNumberOfDataSets( level ) );
      mbds->SetBlock( level, mpds );
      mpds->Delete();
    } // END for all levels

  std::cout << "Extract dual mesh...";
  std::cout.flush();

  this->ExtractDualMesh( amrds, mbds );
  std::cout << "[DONE]\n";
  std::cout.flush();

  return 1;
}

//------------------------------------------------------------------------------
void vtkAMRDualMeshExtractor::WriteMultiBlockData( vtkMultiBlockDataSet *mbds )
{
  // Sanity Check
  assert( "pre: Multi-block dataset is NULL" && (mbds != NULL) );
  vtkXMLMultiBlockDataWriter *writer = vtkXMLMultiBlockDataWriter::New();

  std::ostringstream oss;
  oss.str(""); oss.clear();
  oss << "DUALMESH." << writer->GetDefaultFileExtension();
  writer->SetFileName( oss.str( ).c_str( ) );
  writer->SetInput( mbds );
  writer->Write();
  writer->Delete();
}

//------------------------------------------------------------------------------
void vtkAMRDualMeshExtractor::ComputeCellCenter(
    vtkUniformGrid *ug, int cellIdx, double centroid[3] )
{

  // Sanity checsk
  assert( "input grid is NULL" && (ug != NULL)  );
  assert( "center array is NULL" && (centroid != NULL) );

  vtkCell *myCell = ug->GetCell( cellIdx );
  assert( "post: cell is NULL" && (myCell != NULL) );

  double pCenter[3];
  double *weights = new double[ myCell->GetNumberOfPoints() ];
  int subId       = myCell->GetParametricCenter( pCenter );
  myCell->EvaluateLocation( subId,pCenter,centroid,weights );
  delete [] weights;
}

//------------------------------------------------------------------------------
bool vtkAMRDualMeshExtractor::GetCellIds(
     vtkUniformGrid *ug, int ijk[3], int dims[3],
     vtkIdList *pntIdList, int numNodesPerCell )
{
  // Sanity checks
  assert( "pre: Input uniform grid is NULL" && (ug != NULL) );
  assert( "pre: pntIdList is NULL" && (pntIdList != NULL) );

  int ijkpnt[3];
  ijkpnt[0]  = ijk[0];
  ijkpnt[1]  = ijk[1];
  ijkpnt[2]  = ijk[2];

  int count  = 0;
  int pntIdx = -1;

  switch( numNodesPerCell )
    {
    case 4: /* Form Quad */
      if( (ijk[0] == dims[0]-1) || (ijk[1] == dims[1]-1) )
        return false;

      pntIdx       = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 0, pntIdx );

      ijkpnt[0]++;
      pntIdx       = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 1, pntIdx );

      ijkpnt[1]++;
      pntIdx       = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 2, pntIdx );

      ijkpnt[0]--;
      pntIdx       = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 3, pntIdx );
      break;
    case 8: /* Form Hex */
      if( (ijk[0]==dims[0]-1) || (ijk[1]==dims[1]-1) || (ijk[2]==dims[2]-1) )
       return false;

      /* Hex base */
      pntIdx        = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 0, pntIdx );

      ijkpnt[0]++;
      pntIdx       = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 1, pntIdx );

      ijkpnt[1]++;
      pntIdx       = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 2, pntIdx );

      ijkpnt[0]--;
      pntIdx       = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 3, pntIdx );

      /* Hex top */
      ijkpnt[2]++;
      ijkpnt[0] = ijk[0];
      ijkpnt[1] = ijk[1];

      pntIdx       = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 4, pntIdx );

      ijkpnt[0]++;
      pntIdx = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 5, pntIdx );

      ijkpnt[1]++;
      pntIdx = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 6, pntIdx );

      ijkpnt[0]--;
      pntIdx = vtkStructuredData::ComputePointId( dims, ijkpnt );
      if( !ug->IsCellVisible( pntIdx ) )
        ++count;
      pntIdList->InsertId( 7, pntIdx );
      break;
    default:
      vtkErrorMacro( "Undefined number of nodes per cell!" );
    }

  // If all the dual nodes correspond to cells that are not visible
  // on input grid, then reject the formation of this dual by returning
  // false, otherwise, accept the formation of the cell
  if( count == numNodesPerCell )
    return false;
  return true;

}

//------------------------------------------------------------------------------
bool vtkAMRDualMeshExtractor::ProcessCellDual(
    vtkUniformGrid *ug, const int cellIdx,
    const int cellijk[3], const int celldims[3] )
{
  assert( "pre: input uniform grid is NULL" && (ug != NULL) );
  assert( "pre: input grid must have point data" &&
           (ug->GetPointData() != NULL) );
  assert( "pre: input grid must have PointOwnership information" &&
           ug->GetPointData()->HasArray("PointOwnership") );
  assert( "pre: cell index out-of-bounds!" &&
          ( (cellIdx >= 0) && (cellIdx < ug->GetNumberOfCells() ) ) );

  if( ug->IsCellVisible( cellIdx ) )
    return true;
  else
    {
      vtkPointData *PD = ug->GetPointData();
      vtkIntArray  *PO =
       vtkIntArray::SafeDownCast( PD->GetArray("PointOwnership") );
      vtkIdList *ptIds = vtkIdList::New();
      ug->GetCellPoints( cellIdx, ptIds );
      for( int i=0; i < ptIds->GetNumberOfIds(); ++i )
        {
          if( PO->GetValue( ptIds->GetId(i) ) == 1 )
            {
              ptIds->Delete();
              return true;
            }
        } // END for all nodes
      ptIds->Delete();
    }

  return false;
}

//------------------------------------------------------------------------------
vtkUnstructuredGrid* vtkAMRDualMeshExtractor::GetDualMesh( vtkUniformGrid *ug )
{
  assert( "pre: Input uniform grid is NULL!" && (ug!=NULL) );

  vtkUnstructuredGrid  *mesh = vtkUnstructuredGrid::New();
  vtkCellArray *meshElements = vtkCellArray::New();
  vtkPoints    *nodes        = vtkPoints::New();

  // STEP 0: Get the cell dimensions of the grid. Note, the cell dimensions
  // of the grid are the node dimensions on the dual!
  int celldims[3];
  ug->GetDimensions(celldims);
  --celldims[0]; --celldims[1]; --celldims[2];
  celldims[0] = (celldims[0] < 1)? 1 : celldims[0];
  celldims[1] = (celldims[1] < 1)? 1 : celldims[1];
  celldims[2] = (celldims[2] < 1)? 1 : celldims[2];

  // STEP 2: Compute the dual cell dimensions on the dual grid & total number
  // of cells in the final dual mesh of the given input grid.
  int dualCellDims[3];
  dualCellDims[0] = (celldims[0]==1)? 1 : (celldims[0]-1);
  dualCellDims[1] = (celldims[1]==1)? 1 : (celldims[1]-1);
  dualCellDims[2] = (celldims[2]==1)? 1 : (celldims[2]-1);

  // Note: this is the max number of cells that the dual can have
  int numCellsInDual  = dualCellDims[0]*dualCellDims[1]*dualCellDims[2];

  // STEP 3: Determine the total number of nodes per cell. Note, we use
  // quad cells for 2D or hex cells for 3D.
  int numNodesPerCell = (celldims[2] == 1)?4:8;
  int cellType        = (numNodesPerCell==4)? VTK_QUAD : VTK_HEXAHEDRON;

  // STEP 4: Allocate Dual mesh data-structures
  nodes->SetNumberOfPoints( ug->GetNumberOfCells() );
  meshElements->EstimateSize( numCellsInDual, numNodesPerCell );

  // STEP 5: Construct mesh topology & copy solution
  vtkIdList *pntIdList = vtkIdList::New();
  pntIdList->SetNumberOfIds( numNodesPerCell );
  int cellCounter = 0;
  for( int i=0; i < celldims[0]; ++i )
    {
      for( int j=0; j < celldims[1]; ++j )
        {
          for( int k=0; k < celldims[2]; ++k )
            {
              int ijk[3];
              ijk[0]=i; ijk[1]=j; ijk[2]=k;

              // Since celldims consists of the cell dimensions, ComputePointId
              // is sufficient to get the corresponding linear cell index!
              int cellIdx = vtkStructuredData::ComputePointId( celldims, ijk );
              assert( "Cell Index Out-of-range" &&
               (cellIdx >= 0) && (cellIdx < ug->GetNumberOfCells()));

              double centroid[3];
              this->ComputeCellCenter( ug, cellIdx, centroid );
              nodes->InsertPoint( cellIdx, centroid );

              if( this->ProcessCellDual( ug, cellIdx, ijk, celldims ) &&
                  this->GetCellIds(ug,ijk,celldims,pntIdList,numNodesPerCell) )
                {
                  meshElements->InsertNextCell( pntIdList );
                  ++cellCounter;
                }

              mesh->GetPointData()->ShallowCopy( ug->GetCellData() );

            } // END for all k
        } // END for all j
    } // END for all i
  pntIdList->Delete();

// These asserts are not valid since dual cells are not formed for the entire
//  input grid
//  assert("post: cellCounter==numCellsInDual" && (cellCounter==numCellsInDual));
//  assert( meshElements->GetNumberOfCells() == cellCounter );

  // Release unused memory
  meshElements->Squeeze();

  mesh->SetPoints( nodes );
  nodes->Delete();
  mesh->SetCells( cellType,meshElements );
  meshElements->Delete();

  return mesh;
}

//------------------------------------------------------------------------------
void vtkAMRDualMeshExtractor::ExtractDualMesh(
    vtkHierarchicalBoxDataSet *amrds, vtkMultiBlockDataSet* mbds )
{
  // Sanity checks
  assert( "pre: input AMR dataset is NULL" && (amrds != NULL) );
  assert( "pre: ouput multi-block dataset iS NULL" && (mbds != NULL) );
  assert( "pre: numLevels in must equal numBlocks out" &&
          (amrds->GetNumberOfLevels() == mbds->GetNumberOfBlocks() ) );

  unsigned int level = 0;
  for( ;level < amrds->GetNumberOfLevels(); ++level )
    {
      vtkMultiPieceDataSet *mpds =
        vtkMultiPieceDataSet::SafeDownCast( mbds->GetBlock( level ) );
      assert( "pre: Multi-piece data-structure is NULL" && (mpds!=NULL) );
      assert( "pre: numData in must equal numPieces out" &&
              (amrds->GetNumberOfDataSets(level)==mpds->GetNumberOfPieces()));

      unsigned int dataIdx = 0;
      for( ;dataIdx < amrds->GetNumberOfDataSets( level ); ++dataIdx )
        {
          vtkUniformGrid *ug = amrds->GetDataSet( level, dataIdx );
          if( ug != NULL )
            {
              vtkUnstructuredGrid *dualMesh = this->GetDualMesh( ug );
              assert( "post: dual-mesh is NULL" && (dualMesh != NULL) );
              mpds->SetPiece( dataIdx, dualMesh );
              dualMesh->Delete();
            }

        } // END for all data
    } // END for all levels

}
