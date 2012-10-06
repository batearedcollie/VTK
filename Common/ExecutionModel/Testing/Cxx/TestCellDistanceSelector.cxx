/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestCellDistanceSelector.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .SECTION Thanks
// This test was written by Philippe Pebay, Kitware SAS 2012

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkExtractSelection.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkCellDistanceSelector.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkPointData.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkSmartPointer.h"
#include "vtkTestUtilities.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"

#include <vtksys/ios/sstream>

// Reference values
vtkIdType cardCellDistanceSelection[] =
{
  125,
  54,
  108,
  45,
};

// ------------------------------------------------------------------------------------------------
static int CheckExtractedUGrid( vtkExtractSelection* extract,
                                const char* tag,
                                int testIdx,
                                bool writeGrid )
{
  // Output must be a multiblock dataset
  vtkMultiBlockDataSet* outputMB = vtkMultiBlockDataSet::SafeDownCast( extract->GetOutput() );
  if ( ! outputMB )
    {
    vtkGenericWarningMacro("Cannot downcast extracted selection to multiblock dataset.");

    return 1;
    }

  // First block must be an unstructured grid
  vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast( outputMB->GetBlock( 0 ) );
  if ( ! ugrid )
    {
    vtkGenericWarningMacro("Cannot downcast extracted selection to unstructured grid.");

    return 1;
    }

  // Initialize test status
  int testStatus = 0;
  cerr << endl;

  // Verify selection cardinality
  vtkIdType nCells = ugrid->GetNumberOfCells();
  cout << tag
       << " contains "
       << nCells
       << " cells."
       << endl;

  if ( nCells != cardCellDistanceSelection[testIdx] )
    {
    vtkGenericWarningMacro( "Incorrect cardinality: "
                           << nCells
                           << " != "
                           << cardCellDistanceSelection[testIdx] );
    testStatus = 1;
    }

  // Verify selection cells
  cerr << "Original cell Ids (types): ";
  ugrid->GetCellData()->SetActiveScalars( "vtkOriginalCellIds" );
  vtkDataArray* oCellIds = ugrid->GetCellData()->GetScalars();
  for ( vtkIdType i = 0; i < oCellIds->GetNumberOfTuples(); ++ i )
    {
    cerr << oCellIds->GetTuple1( i )
         << " ";
    }
  cerr << endl;

  // If requested, write mesh
  if ( writeGrid )
    {
    vtksys_ios::ostringstream fileNameSS;
    fileNameSS << "./CellDistanceExtraction-"
               << testIdx
               << ".vtk";
    vtkSmartPointer<vtkUnstructuredGridWriter> writer = vtkSmartPointer<vtkUnstructuredGridWriter>::New();
    writer->SetFileName( fileNameSS.str().c_str() );
    writer->SetInputData( ugrid );
    writer->Write();
    cerr << "Wrote file "
         << fileNameSS.str()
         << endl;
    }

  return testStatus;
}

//----------------------------------------------------------------------------
int TestCellDistanceSelector( int argc, char * argv [] )
{
  // Initialize test value
  int testIntValue = 0;

  // Read 3D unstructured input mesh
  char* fileName = vtkTestUtilities::ExpandDataFileName( argc, argv, "Data/AngularSector.vtk");
  vtkSmartPointer<vtkUnstructuredGridReader> reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
  reader->SetFileName( fileName );
  reader->Update();
  delete [] fileName;

  // Create multi-block mesh for linear selector
  vtkSmartPointer<vtkMultiBlockDataSet> mesh = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mesh->SetNumberOfBlocks( 1 );
  mesh->GetMetaData( static_cast<unsigned>( 0 ) )->Set( vtkCompositeDataSet::NAME(), "Mesh" );
  mesh->SetBlock( 0, reader->GetOutput() );

  // *****************************************************************************
  // 0. Selection with distance of 2 from cell 7000
  // *****************************************************************************

  // Create a selection, sel0, of cell with index 7000
  vtkSmartPointer<vtkIdTypeArray> selArr0 = vtkSmartPointer<vtkIdTypeArray>::New();
  selArr0->InsertNextValue( 7000 );
  vtkSmartPointer<vtkSelectionNode> selNode0 = vtkSmartPointer<vtkSelectionNode>::New();
  selNode0->SetContentType( vtkSelectionNode::INDICES );
  selNode0->SetFieldType( vtkSelectionNode::CELL );
  selNode0->GetProperties()->Set( vtkSelectionNode::COMPOSITE_INDEX(), 1 );
  selNode0->SetSelectionList( selArr0 );
  vtkSmartPointer<vtkSelection> sel0 = vtkSmartPointer<vtkSelection>::New();
  sel0->AddNode( selNode0 );

  // Create selection up to topological distance of 2
  vtkSmartPointer<vtkCellDistanceSelector> ls0 = vtkSmartPointer<vtkCellDistanceSelector>::New();
  ls0->SetInputData( 0, sel0 );
  ls0->SetInputData( 1, mesh );
  ls0->SetDistance( 2 );

  // Extract selection from mesh
  vtkSmartPointer<vtkExtractSelection> es0 =  vtkSmartPointer<vtkExtractSelection>::New();
  es0->SetInputData( 0, mesh );
  es0->SetInputConnection( 1, ls0->GetOutputPort() );
  es0->Update();
  testIntValue += CheckExtractedUGrid( es0, "Selection d({7000})<3", 0, true );

  return testIntValue;
}
