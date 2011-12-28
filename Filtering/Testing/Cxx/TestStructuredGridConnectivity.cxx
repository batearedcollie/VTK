/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestStructuredGridConnectivity.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME TestStructuredGridConnectivity.cxx --Test vtkStructuredGridConnectivity
//
// .SECTION Description
// Serial tests for structured grid connectivity

// C++ includes
#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>
#include <set>

// VTK includes
#include "vtkDataSet.h"
#include "vtkUniformGrid.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkMultiPieceDataSet.h"
#include "vtkMPIController.h"
#include "vtkDataObject.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkStructuredGridConnectivity.h"
#include "vtkStructuredNeighbor.h"
#include "vtkUnsignedCharArray.h"
#include "vtkIntArray.h"
#include "vtkGhostArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkCell.h"
#include "vtkUniformGridPartitioner.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkXMLMultiBlockDataWriter.h"

#define ENABLE_IO

//------------------------------------------------------------------------------
// Description:
// Writes the grid to a file
void WriteGrid( vtkUniformGrid *grid, std::string prefix )
{

#ifdef ENABLE_IO
  assert( "pre: input grid is NULL" && (grid != NULL) );

  vtkXMLImageDataWriter *writer = vtkXMLImageDataWriter::New();

  std::ostringstream oss;
  oss << prefix << "." << writer->GetDefaultFileExtension();
  writer->SetFileName( oss.str().c_str() );
  writer->SetInput( grid );
  writer->Write();

  writer->Delete();
#endif

}

//------------------------------------------------------------------------------
// Description:
// This method attaches a point array to the given grid that will label the
// the points by color -- 0(off) or 1(on) -- to indicate whether or not a
// particular flag is "ON"
void AttachPointFlagsArray(
    vtkUniformGrid *grid, const int flag, std::string label )
{
  assert( "pre: grid should not be NULL!" && (grid != NULL) );

  vtkUnsignedIntArray *flags = vtkUnsignedIntArray::New();
  flags->SetName( label.c_str() ) ;
  flags->SetNumberOfComponents( 1 );
  flags->SetNumberOfTuples( grid->GetNumberOfPoints() );

  vtkIdType pidx = 0;
  for( ; pidx < grid->GetNumberOfPoints(); ++pidx )
    {
    unsigned char nodeProperty =
         *(grid->GetPointVisibilityArray()->GetPointer( pidx ));
    if( vtkGhostArray::IsPropertySet(nodeProperty,flag) )
      {
      flags->SetValue( pidx, 1);
      }
    else
      {
      flags->SetValue( pidx, 0);
      }
    } // END for all points

  grid->GetPointData()->AddArray( flags );
  flags->Delete();
}

//------------------------------------------------------------------------------
// Description:
// This method attaches a cell array to the given grid that will label the
// the points by color -- 0(off) or 1(on) -- to indicate whether or not a
// particular flag is "ON"
void AttachCellFlagsArray(
    vtkUniformGrid *grid, const int flag, std::string label )
{
  assert( "pre: grid should not be NULL" && (grid != NULL) );

  vtkUnsignedIntArray *flags = vtkUnsignedIntArray::New();
  flags->SetName( label.c_str() );
  flags->SetNumberOfComponents( 1 );
  flags->SetNumberOfTuples( grid->GetNumberOfCells() );

  vtkIdType cellIdx = 0;
  for( ; cellIdx < grid->GetNumberOfCells(); ++cellIdx )
    {
    unsigned char cellProperty =
        *(grid->GetCellVisibilityArray()->GetPointer(cellIdx));
    if( vtkGhostArray::IsPropertySet(cellProperty,flag) )
      {
      flags->SetValue( cellIdx, 1 );
      }
    else
      {
      flags->SetValue( cellIdx, 0 );
      }
    } // END for all cells

  grid->GetCellData()->AddArray( flags );
  flags->Delete();
}

//------------------------------------------------------------------------------
// Description:
// Applies an XYZ field to the nodes and cells of the grid whose value is
// corresponding to the XYZ coordinates at that location
void ApplyXYZFieldToGrid( vtkUniformGrid *grd )
{
  assert( "pre: grd should not be NULL" && (grd != NULL)  );

  // Get the grid's point-data and cell-data data-structures
  vtkCellData  *CD = grd->GetCellData();
  vtkPointData *PD = grd->GetPointData();
  assert( "pre: Cell data is NULL" && (CD != NULL) );
  assert( "pre: Point data is NULL" && (PD != NULL)  );

  // Allocate arrays
  vtkDoubleArray *cellXYZArray = vtkDoubleArray::New();
  cellXYZArray->SetName( "CellXYZ" );
  cellXYZArray->SetNumberOfComponents( 3 );
  cellXYZArray->SetNumberOfTuples( grd->GetNumberOfCells() );


  vtkDoubleArray *nodeXYZArray = vtkDoubleArray::New();
  nodeXYZArray->SetName( "NodeXYZ" );
  nodeXYZArray->SetNumberOfComponents( 3 );
  nodeXYZArray->SetNumberOfTuples( grd->GetNumberOfPoints() );

  // Compute field arrays
  std::set< vtkIdType > visited;
  for( vtkIdType cellIdx=0; cellIdx < grd->GetNumberOfCells(); ++cellIdx )
    {
    vtkCell *c = grd->GetCell( cellIdx );
    assert( "pre: cell is not NULL" && (c != NULL) );

    double centroid[3];
    double xsum = 0.0;
    double ysum = 0.0;
    double zsum = 0.0;

    for( vtkIdType node=0; node < c->GetNumberOfPoints(); ++node )
      {
      double xyz[3];

      vtkIdType meshPntIdx = c->GetPointId( node );
      if( visited.find( meshPntIdx ) == visited.end() )
        {
        visited.insert( meshPntIdx );
        grd->GetPoint(  meshPntIdx, xyz );

        xsum += xyz[0];
        ysum += xyz[1];
        zsum += xyz[2];

        nodeXYZArray->SetComponent( meshPntIdx, 0, xyz[0] );
        nodeXYZArray->SetComponent( meshPntIdx, 1, xyz[1] );
        nodeXYZArray->SetComponent( meshPntIdx, 2, xyz[2] );
        } // END if
      } // END for all nodes

    centroid[0] = xsum / c->GetNumberOfPoints();
    centroid[1] = ysum / c->GetNumberOfPoints();
    centroid[2] = zsum / c->GetNumberOfPoints();

    cellXYZArray->SetComponent( cellIdx, 0, centroid[0] );
    cellXYZArray->SetComponent( cellIdx, 1, centroid[1] );
    cellXYZArray->SetComponent( cellIdx, 2, centroid[2] );
    } // END for all cells

  // Insert field arrays to grid point/cell data
  CD->AddArray( cellXYZArray );
  cellXYZArray->Delete();

  PD->AddArray( nodeXYZArray );
  nodeXYZArray->Delete();
}

//------------------------------------------------------------------------------
// Description:
// Get Grid whole extent and dimensions
void GetGlobalGrid( const int dimension, int wholeExtent[6], int dims[3] )
{
  for( int i=0; i < 3; ++i )
    {
    wholeExtent[ i*2   ] = 0;
    wholeExtent[ i*2+1 ] = 0;
    dims[ i ]            = 1;
    }

  switch( dimension )
    {
    case 2:
      wholeExtent[0] = 0;
      wholeExtent[1] = 99;
      wholeExtent[2] = 0;
      wholeExtent[3] = 99;

      dims[0] = wholeExtent[1] - wholeExtent[0] + 1;
      dims[1] = wholeExtent[3] - wholeExtent[2] + 1;
      break;
    case 3:
      wholeExtent[0] = 0;
      wholeExtent[1] = 99;
      wholeExtent[2] = 0;
      wholeExtent[3] = 99;
      wholeExtent[4] = 0;
      wholeExtent[5] = 99;

      dims[0] = wholeExtent[1] - wholeExtent[0] + 1;
      dims[1] = wholeExtent[3] - wholeExtent[2] + 1;
      dims[2] = wholeExtent[5] - wholeExtent[4] + 1;
      break;
    default:
      assert( "Cannot create grid of invalid dimension" && false );
    }
}

//------------------------------------------------------------------------------
// Description:
// Generates a multi-block dataset
vtkMultiBlockDataSet* GetDataSet(
    const int dimension, const int numPartitions, const int numGhosts)
{
  int wholeExtent[6];
  int dims[3];
  GetGlobalGrid( dimension, wholeExtent, dims );


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
  gridPartitioner->SetNumberOfGhostLayers( numGhosts );
  gridPartitioner->Update();

// THIS DOES NOT COPY THE INFORMATION KEYS!
//  vtkMultiBlockDataSet *mbds = vtkMultiBlockDataSet::New();
//  mbds->ShallowCopy( gridPartitioner->GetOutput() );

  vtkMultiBlockDataSet *mbds =
      vtkMultiBlockDataSet::SafeDownCast( gridPartitioner->GetOutput() );
  mbds->SetReferenceCount( mbds->GetReferenceCount()+1 );

  wholeGrid->Delete();
  gridPartitioner->Delete();

  assert( "pre: mbds is NULL" && (mbds != NULL) );
  return( mbds );
}

//------------------------------------------------------------------------------
// Description:
// Computes the total number of nodes in the multi-block dataset.
int GetTotalNumberOfNodes( vtkMultiBlockDataSet *multiblock )
{
  assert( "multi-block grid is NULL" && (multiblock != NULL) );

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
        if( !vtkGhostArray::IsPropertySet(
            nodeProperty,vtkGhostArray::IGNORE ) )
          {
          ++numNodes;
          }
        } // END for all nodes
      } // END if grid != NULL

    } // END for all blocks

  return( numNodes );
}

//------------------------------------------------------------------------------
void RegisterGrids(
    vtkMultiBlockDataSet *mbds, vtkStructuredGridConnectivity *connectivity )
{
  assert( "pre: Multi-block is NULL!" && (mbds != NULL) );
  assert( "pre: connectivity is NULL!" && (connectivity != NULL) );

  for( unsigned int block=0; block < mbds->GetNumberOfBlocks(); ++block )
    {
    vtkUniformGrid *grid = vtkUniformGrid::SafeDownCast(mbds->GetBlock(block));
    assert( "pre: grid should not be NULL!" && (grid != NULL) );

    vtkInformation *info = mbds->GetMetaData( block );
    assert( "pre: metadata should not be NULL" && (info != NULL) );
    assert( "pre: must have piece extent!" &&
            info->Has(vtkDataObject::PIECE_EXTENT() ) );

    connectivity->RegisterGrid(
        block,info->Get(vtkDataObject::PIECE_EXTENT()),
        grid->GetPointVisibilityArray(),
        grid->GetCellVisibilityArray(),
        grid->GetPointData(),
        grid->GetCellData(),
        NULL);
    } // END for all blocks
}

//------------------------------------------------------------------------------
void WriteMultiBlock( vtkMultiBlockDataSet *mbds )
{

#ifdef ENABLE_IO
  assert( "pre: Multi-block is NULL!" && (mbds != NULL) );


  vtkXMLMultiBlockDataWriter *writer = vtkXMLMultiBlockDataWriter::New();
  assert( "pre: Cannot allocate writer" && (writer != NULL) );

  std::ostringstream oss;
  oss.str("");
  oss << "MyMultiBlock_" << mbds->GetNumberOfBlocks() << "."
      << writer->GetDefaultFileExtension();
  writer->SetFileName( oss.str().c_str() );
  writer->SetInput( mbds );
  writer->Write();

  writer->Delete();
#endif

}

//------------------------------------------------------------------------------
// Program main
int TestStructuredGridConnectivity( int argc, char *argv[] )
{
  // Silence compiler wanrings for unused vars argc and argv
  static_cast<void>( argc );
  static_cast<void>( argv );

  int expected = 100*100*100;
  int rc = 0;
  int numberOfPartitions[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
  int numGhostLayers[]     = { 0, 1, 2, 3 };

  for( int i=0; i < 8; ++i )
    {
    for( int j=0; j < 4; ++j )
      {
      // STEP 0: Construct the dataset
      std::cout << "===\n";
      std::cout << "i: " << i << " j:" << j << std::endl;
      std::cout << "-- Acquire dataset with N=" << numberOfPartitions[ i ];
      std::cout << " BLOCKS and NG=" << numGhostLayers[ j ] << "...";
      std::cout.flush();

      vtkMultiBlockDataSet *mbds =
          GetDataSet(3,numberOfPartitions[ i ], numGhostLayers[ j ] );
      assert( "pre: multi-block is NULL" && (mbds != NULL) );

      std::cout << "[DONE]\n";
      std::cout.flush();
      std::cout << "NUMBLOCKS: " << mbds->GetNumberOfBlocks() << std::endl;
      std::cout.flush();
      assert( "pre: NumBlocks mismatch!" &&
       (numberOfPartitions[i] ==static_cast<int>(mbds->GetNumberOfBlocks()) ) );
      WriteMultiBlock( mbds );

      // STEP 1: Construct the grid connectivity
      std::cout << "-- Allocating grid connectivity data-structures...";
      std::cout.flush();
      vtkStructuredGridConnectivity *gridConnectivity=
          vtkStructuredGridConnectivity::New();
      gridConnectivity->SetNumberOfGrids( mbds->GetNumberOfBlocks() );
      gridConnectivity->SetNumberOfGhostLayers( numGhostLayers[j] );
      gridConnectivity->SetWholeExtent( mbds->GetWholeExtent() );
      std::cout << "[DONE]\n";
      std::cout.flush();

      // STEP 2: Registers the grids
      std::cout << "-- Registering grid blocks...";
      std::cout.flush();
      RegisterGrids( mbds, gridConnectivity );
      std::cout << "[DONE]\n";
      std::cout.flush();

      // STEP 3: Compute neighbors
      std::cout << "-- Computing neighbors...";
      std::cout.flush();
      gridConnectivity->ComputeNeighbors();
      std::cout << "[DONE]\n";
      std::cout.flush();

      gridConnectivity->Print( std::cout );

      // STEP 5: Compute total number of nodes compare to expected
      std::cout << "-- Computing the total number of nodes...";
      std::cout.flush();
      int NumNodes = GetTotalNumberOfNodes( mbds );
      std::cout << "[DONE]\n";
      std::cout.flush();

      std::cout << "NUMNODES=" << NumNodes << " EXPECTED=" << expected << "...";
      if( NumNodes != expected )
        {
        ++rc;
        std::cout << "[ERROR]\n";
        std::cout.flush();
        mbds->Delete();
        gridConnectivity->Delete();
        return( rc );
        }
      else
        {
        std::cout << "[OK]\n";
        std::cout.flush();
        }

      // STEP 6: De-allocated data-structures
      mbds->Delete();
      gridConnectivity->Delete();
      }// END for all ghost layer tests
    } // END for all numPartition tests

  return( rc );
}

//------------------------------------------------------------------------------
int Simple2DTest( int argc, char **argv )
{
  assert( "pre: argument counter must equal 4" && (argc==4) );

  int np = atoi( argv[2] );
  int ng = atoi( argv[3] );
  std::cout << "Running Simple 2-D Test..." << std::endl;
  std::cout << "Number of partitions: "   << np << std::endl;
  std::cout << "Number of ghost-layers: " << ng << std::endl;
  std::cout.flush();

  int expected = 100*100;

  vtkMultiBlockDataSet *mbds = GetDataSet(2, np, ng);
//  WriteMultiBlock( mbds );

  vtkStructuredGridConnectivity *gridConnectivity =
      vtkStructuredGridConnectivity::New();
  gridConnectivity->SetNumberOfGhostLayers( ng );
  gridConnectivity->SetNumberOfGrids( mbds->GetNumberOfBlocks() );
  gridConnectivity->SetWholeExtent( mbds->GetWholeExtent() );

  RegisterGrids( mbds, gridConnectivity );

  gridConnectivity->ComputeNeighbors();
  gridConnectivity->Print( std::cout );
  std::cout.flush();

  unsigned int block=0;
  for( ; block < mbds->GetNumberOfBlocks(); ++block )
    {
    vtkUniformGrid *myGrid =
        vtkUniformGrid::SafeDownCast(mbds->GetBlock( block ) );
    if( myGrid != NULL )
      {
      AttachPointFlagsArray( myGrid, vtkGhostArray::IGNORE, "IGNORE" );
      AttachPointFlagsArray( myGrid, vtkGhostArray::SHARED, "SHARED" );
      AttachPointFlagsArray( myGrid, vtkGhostArray::GHOST, "GHOST" );
      AttachPointFlagsArray( myGrid, vtkGhostArray::BOUNDARY, "BOUNDARY" );
      AttachCellFlagsArray( myGrid, vtkGhostArray::DUPLICATE, "DUPLICATE" );
      AttachCellFlagsArray( myGrid, vtkGhostArray::INTERIOR, "INTERIOR" );
      }
    } // END for all blocks
  WriteMultiBlock( mbds );

  int NumNodes = GetTotalNumberOfNodes( mbds );
  std::cout << "[DONE]\n";
  std::cout.flush();

  std::cout << "NUMNODES=" << NumNodes << " EXPECTED=" << expected << "...";
  if( NumNodes != expected )
   {
   std::cout << "[ERROR]\n";
   std::cout.flush();
   }
  else
   {
   std::cout << "[OK]\n";
   std::cout.flush();
   }
  mbds->Delete();
  gridConnectivity->Delete();
  return 0;
}

//------------------------------------------------------------------------------
int Simple3DTest( int argc, char **argv )
{
  assert( "pre: argument counter must equal 4" && (argc==4) );

  int np = atoi( argv[2] );
  int ng = atoi( argv[3] );
  std::cout << "Running Simple 3-D Test..." << std::endl;
  std::cout << "Number of partitions: "   << np << std::endl;
  std::cout << "Number of ghost-layers: " << ng << std::endl;
  std::cout.flush();

  int expected = 100*100*100;

  vtkMultiBlockDataSet *mbds = GetDataSet(3, np, ng);
  WriteMultiBlock( mbds );

  vtkStructuredGridConnectivity *gridConnectivity =
      vtkStructuredGridConnectivity::New();
  gridConnectivity->SetNumberOfGhostLayers( ng );
  gridConnectivity->SetNumberOfGrids( mbds->GetNumberOfBlocks() );
  gridConnectivity->SetWholeExtent( mbds->GetWholeExtent() );

  RegisterGrids( mbds, gridConnectivity );

  gridConnectivity->ComputeNeighbors();
  gridConnectivity->Print( std::cout );
  std::cout.flush();

  int NumNodes = GetTotalNumberOfNodes( mbds );
  std::cout << "[DONE]\n";
  std::cout.flush();

  std::cout << "NUMNODES=" << NumNodes << " EXPECTED=" << expected << "...";
  if( NumNodes != expected )
   {
   std::cout << "[ERROR]\n";
   std::cout.flush();
   }
  else
   {
   std::cout << "[OK]\n";
   std::cout.flush();
   }
  mbds->Delete();
  gridConnectivity->Delete();
  return 0;
}
//------------------------------------------------------------------------------
// Program main
int main( int argc, char **argv )
{
  int rc = 0;

  if( argc > 1 )
    {
    int testNumber = atoi( argv[1] );
    switch( testNumber )
      {
      case 1:
        rc = Simple2DTest( argc, argv );
        break;
      case 2:
        rc = Simple3DTest( argc, argv );
        break;
      default:
        std::cerr << "Undefined test: " << testNumber << " ";
        std::cerr << "No tests will run!\n";
        rc = 0;
      }
    }
  else
    {
    rc = TestStructuredGridConnectivity( argc, argv );
    }

  return( rc );
}
