/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestPStructuredGridConnectivity.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME TestPStructuredGridConnectivity.cxx -- Parallel structured connectivity
//
// .SECTION Description
//  A test for parallel structured grid connectivity.

// C++ includes
#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>

// MPI include
#include <mpi.h>

// VTK includes
#include "vtkDataSet.h"
#include "vtkUniformGrid.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkMPIController.h"
#include "vtkDataObject.h"
#include "vtkInformation.h"
#include "vtkPStructuredGridConnectivity.h"
#include "vtkStructuredGridConnectivity.h"
#include "vtkStructuredNeighbor.h"
#include "vtkMeshPropertyEncoder.h"
#include "vtkMeshProperty.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUniformGridPartitioner.h"
#include "vtkUnsignedCharArray.h"

//------------------------------------------------------------------------------
//      G L O B A  L   D A T A
//------------------------------------------------------------------------------
vtkMultiProcessController *Controller;
int Rank;
int NumberOfProcessors;

//------------------------------------------------------------------------------
void LogMessage( const std::string &msg)
{
  if( Rank == 0 )
    {
    std::cout << msg << std::endl;
    std::cout.flush();
    }
}

//------------------------------------------------------------------------------
int GetTotalNumberOfNodes( vtkMultiBlockDataSet *multiblock )
{
  assert( "pre: Controller should not be NULL" && (Controller != NULL) );
  assert( "pre: multi-block grid is NULL" && (multiblock != NULL) );

  // STEP 0: Count local number of nodes
  int numNodes = 0;
  for(unsigned int block=0; block < multiblock->GetNumberOfBlocks(); ++block )
    {
    vtkUniformGrid *grid =
        vtkUniformGrid::SafeDownCast( multiblock->GetBlock( block ) );

    if( grid != NULL )
      {
      vtkIdType pntIdx = 0;
      for( ; pntIdx < grid->GetNumberOfPoints(); ++pntIdx )
        {
        unsigned char nodeProperty =
            *(grid->GetPointVisibilityArray()->GetPointer( pntIdx ));
        if( !vtkMeshPropertyEncoder::IsPropertySet(
            nodeProperty,VTKNodeProperties::IGNORE ) )
          {
          ++numNodes;
          }
        } // END for all nodes
      } // END if grid != NULL

    } // END for all blocks

  // STEP 2: Synchronize processes
  Controller->Barrier();

  // STEP 3: Get a global sum
  int totalSum = 0;
  Controller->AllReduce(&numNodes,&totalSum,1,vtkCommunicator::SUM_OP);

  return( totalSum );
}


//------------------------------------------------------------------------------
void FillVisibilityArrays(
    vtkMultiBlockDataSet *mbds, vtkPStructuredGridConnectivity *connectivity )
{
  assert( "pre: Multi-block is NULL!" && (mbds != NULL) );
  assert( "pre: connectivity is NULL!" && (connectivity != NULL) );

  for( unsigned int block=0; block < mbds->GetNumberOfBlocks(); ++block )
    {
    vtkUniformGrid *grid = vtkUniformGrid::SafeDownCast(mbds->GetBlock(block));
    if( grid != NULL )
      {
      vtkUnsignedCharArray *nodes = vtkUnsignedCharArray::New();
      nodes->SetNumberOfValues( grid->GetNumberOfPoints() );

      vtkUnsignedCharArray *cells = vtkUnsignedCharArray::New();
      cells->SetNumberOfValues( grid->GetNumberOfCells() );

      connectivity->FillMeshPropertyArrays(
          block, nodes->GetPointer(0), cells->GetPointer(0)  );

      grid->SetPointVisibilityArray( nodes );
      nodes->Delete();
      grid->SetCellVisibilityArray( cells );
      cells->Delete();
      } // END if grid != NULL
    } // END for all blocks
}

//------------------------------------------------------------------------------
// Description:
// Generates a distributed multi-block dataset, each grid is added using
// round-robin assignment.
vtkMultiBlockDataSet* GetDataSet( const int numPartitions )
{
  int wholeExtent[6];
  wholeExtent[0] = 0;
  wholeExtent[1] = 99;
  wholeExtent[2] = 0;
  wholeExtent[3] = 99;
  wholeExtent[4] = 0;
  wholeExtent[5] = 99;

  int dims[3];
  dims[0] = wholeExtent[1] - wholeExtent[0] + 1;
  dims[1] = wholeExtent[3] - wholeExtent[2] + 1;
  dims[2] = wholeExtent[5] - wholeExtent[4] + 1;

  // Generate grid for the entire domain
  vtkUniformGrid *wholeGrid = vtkUniformGrid::New();
  wholeGrid->SetOrigin( 0.0, 0.0, 0.0  );
  wholeGrid->SetSpacing( 0.5, 0.5, 0.5 );
  wholeGrid->SetDimensions( dims );

  // partition the grid, the grid partitioner will generate the whole extent and
  // node extent information.
  vtkUniformGridPartitioner *gridPartitioner = vtkUniformGridPartitioner::New();
  gridPartitioner->SetInput( wholeGrid  );
  gridPartitioner->SetNumberOfPartitions( numPartitions );
  gridPartitioner->Update();
  vtkMultiBlockDataSet *partitionedGrid =
      vtkMultiBlockDataSet::SafeDownCast( gridPartitioner->GetOutput() );
  assert( "pre: partitionedGrid != NULL" && (partitionedGrid != NULL) );

  // Each process has the same number of blocks, i.e., the same structure,
  // however some block entries are NULL indicating that the data lives on
  // some other process
  vtkMultiBlockDataSet *mbds = vtkMultiBlockDataSet::New();
  mbds->SetNumberOfBlocks( numPartitions );
  mbds->SetWholeExtent( partitionedGrid->GetWholeExtent( ) );

  // Populate blocks for this process
  unsigned int block=0;
  for( ; block < partitionedGrid->GetNumberOfBlocks(); ++block )
    {
    if( Rank == block%NumberOfProcessors )
      {
      // Copy the uniform grid
      vtkUniformGrid *grid = vtkUniformGrid::New();
      grid->DeepCopy( partitionedGrid->GetBlock(block) );

      mbds->SetBlock( block, grid );
      grid->Delete();

      // Copy the global extent for the blockinformation
      vtkInformation *info = partitionedGrid->GetMetaData( block );
      assert( "pre: null metadata!" && (info != NULL) );
      assert( "pre: must have a piece extent!" &&
              (info->Has(vtkDataObject::PIECE_EXTENT() ) ) );

      vtkInformation *metadata = mbds->GetMetaData( block );
      assert( "pre: null metadata!" && (metadata != NULL) );
      metadata->Set(
        vtkDataObject::PIECE_EXTENT(),
        info->Get( vtkDataObject::PIECE_EXTENT() ),
        6 );
      } // END if we own the block
    else
      {
      mbds->SetBlock( block, NULL );
      } // END else we don't own the block
    } // END for all blocks

  wholeGrid->Delete();
  gridPartitioner->Delete();

  assert( "pre: mbds is NULL" && (mbds != NULL) );
  return( mbds );
}

//------------------------------------------------------------------------------
void RegisterGrids(
    vtkMultiBlockDataSet *mbds, vtkPStructuredGridConnectivity *connectivity )
{
  assert( "pre: Multi-block is NULL!" && (mbds != NULL) );
  assert( "pre: connectivity is NULL!" && (connectivity != NULL) );

  for( unsigned int block=0; block < mbds->GetNumberOfBlocks(); ++block )
    {
    if( mbds->GetBlock( block ) != NULL )
      {
      vtkInformation *info = mbds->GetMetaData( block );
      assert( "pre: metadata should not be NULL" && (info != NULL) );
      assert( "pre: must have piece extent!" &&
              info->Has(vtkDataObject::PIECE_EXTENT() ) );
      connectivity->RegisterGrid(
          block,info->Get(vtkDataObject::PIECE_EXTENT()));
      } // END if block belongs to this process
    } // END for all blocks
}

//------------------------------------------------------------------------------
// Tests StructuredGridConnectivity on a distributed data-set
int TestPStructuredGridConnectivity( const int factor )
{
  assert( "pre: MPI Controller is NULL!" && (Controller != NULL) );

  int expected = 100*100*100;

  // STEP 0: Calculation number of partitions as factor of the number of
  // processes.
  assert( "pre: factor >= 1" && (factor >= 1) );
  int numPartitions = factor * NumberOfProcessors;

  // STEP 1: Acquire the distributed structured grid for this process.
  // Each process has the same number of blocks, but not all entries are
  // poplulated. A NULL entry indicates that the block belongs to a different
  // process.
  vtkMultiBlockDataSet *mbds = GetDataSet( numPartitions );
  assert( "pre: mbds != NULL" && (mbds != NULL) );
  assert( "pre: numBlocks mismatch" &&
           (mbds->GetNumberOfBlocks()==numPartitions) );

  // STEP 2: Setup the grid connectivity
  vtkPStructuredGridConnectivity *gridConnectivity =
      vtkPStructuredGridConnectivity::New();
  gridConnectivity->SetController( Controller );
  gridConnectivity->SetNumberOfGrids( mbds->GetNumberOfBlocks() );
  gridConnectivity->SetWholeExtent( mbds->GetWholeExtent() );

  // STEP 3: Register the grids
  RegisterGrids( mbds, gridConnectivity );
  Controller->Barrier();

  // STEP 4: Compute neighbors
  gridConnectivity->ComputeNeighbors();
  Controller->Barrier();

  // STEP 5: Fill Visibility arrays
  FillVisibilityArrays( mbds, gridConnectivity );
  Controller->Barrier();

  // STEP 6: Total global count of the nodes
  int count = GetTotalNumberOfNodes( mbds );
  Controller->Barrier();

  if( count != expected )
    return 1;
  return 0;
}

//------------------------------------------------------------------------------
// Program main
int main( int argc, char **argv )
{
  int rc       = 0;

  // STEP 0: Initialize
  Controller = vtkMPIController::New();
  Controller->Initialize( &argc, &argv, 0 );
  assert("pre: Controller should not be NULL" && (Controller != NULL) );
  vtkMultiProcessController::SetGlobalController( Controller );
  LogMessage( "Finished MPI Initialization!" );

  LogMessage( "Getting Rank ID and NumberOfProcessors..." );
  Rank               = Controller->GetLocalProcessId();
  NumberOfProcessors = Controller->GetNumberOfProcesses();
  assert( "pre: NumberOfProcessors >= 1" && (NumberOfProcessors >= 1) );
  assert( "pre: Rank is out-of-bounds" && (Rank >= 0) );

  // STEP 1: Run test where the number of partitions is equal to the number of
  // processes
  Controller->Barrier();
  LogMessage( "Testing with same number of partitions as processes..." );
  rc += TestPStructuredGridConnectivity( 1 );
  Controller->Barrier();

  // STEP 2: Run test where the number of partitions is double the number of
  // processes
  Controller->Barrier();
  LogMessage( "Testing with double the number of partitions as processes..." );
  rc += TestPStructuredGridConnectivity( 2 );
  Controller->Barrier();

  // STEP 3: Deallocate controller and exit
  Controller->Finalize();
  Controller->Delete();

  return( rc );
}
