/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHyperTreeGridGeometry.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkHyperTreeGridGeometry.h"

#include "vtkBitArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCharArray.h"
#include "vtkHyperTreeGrid.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"

vtkStandardNewMacro(vtkHyperTreeGridGeometry);

static int vtkSuperCursorFaceIndices[] = { -1, 1, -3, 3, -9, 9 };

static int vtkHTGo[] = { 0, 1, 0, 1, 0, 1 };
static int vtkHTG0[] = { 0, 0, 1, 1, 2, 2 };
static int vtkHTG1[] = { 1, 1, 0, 0, 0, 0 };
static int vtkHTG2[] = { 2, 2, 2, 2, 1, 1 };
//-----------------------------------------------------------------------------
vtkHyperTreeGridGeometry::vtkHyperTreeGridGeometry()
{
  this->Points = 0;
  this->Cells = 0;
  this->Input = 0;
  this->Output = 0;
}

//-----------------------------------------------------------------------------
vtkHyperTreeGridGeometry::~vtkHyperTreeGridGeometry()
{
  if ( this->Points )
    {
    this->Points->Delete();
    this->Points = 0;
    }
  if ( this->Cells )
    {
    this->Cells->Delete();
    this->Cells = 0;
    }
  if ( this->Filling )
    {
    this->Filling->Delete();
    this->Filling = 0;
    }
}

//----------------------------------------------------------------------------
void vtkHyperTreeGridGeometry::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );

  if ( this->Input )
    {
    os << indent << "Input:\n";
    this->Input->PrintSelf( os, indent.GetNextIndent() );
    }
  else
    {
    os << indent << "Input: (none)\n";
    }
  if ( this->Output )
    {
    os << indent << "Output:\n";
    this->Output->PrintSelf( os, indent.GetNextIndent() );
    }
  else
    {
    os << indent << "Output: (none)\n";
    }
  if ( this->Points )
    {
    os << indent << "Points:\n";
    this->Points->PrintSelf( os, indent.GetNextIndent() );
    }
  else
    {
    os << indent << "Points: (none)\n";
    }
  if ( this->Cells )
    {
    os << indent << "Cells:\n";
    this->Cells->PrintSelf( os, indent.GetNextIndent() );
    }
  else
    {
    os << indent << "Filling: (none)\n";
    }
  if ( this->Filling )
    {
    os << indent << "Filling:\n";
    this->Filling->PrintSelf( os, indent.GetNextIndent() );
    }
  else
    {
    os << indent << "Filling: (none)\n";
    }
}

//-----------------------------------------------------------------------------
int vtkHyperTreeGridGeometry::FillInputPortInformation( int, vtkInformation *info )
{
  info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkHyperTreeGrid" );
  return 1;
}

//----------------------------------------------------------------------------
int vtkHyperTreeGridGeometry::RequestData( vtkInformation*,
                                           vtkInformationVector** inputVector,
                                           vtkInformationVector* outputVector )
{
  // Get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject( 0 );
  vtkInformation *outInfo = outputVector->GetInformationObject( 0 );

  // Retrieve input and output
  this->Input = vtkHyperTreeGrid::SafeDownCast( inInfo->Get( vtkDataObject::DATA_OBJECT() ) );
  this->Output= vtkPolyData::SafeDownCast( outInfo->Get( vtkDataObject::DATA_OBJECT() ) );

  // Ensure that primal grid API is used for hyper trees
  int inputDualFlagIsOn = this->Input->GetUseDualGrid();
  if ( inputDualFlagIsOn )
    {
    this->Input->SetUseDualGrid( 0 );
    }

  // Initialize output cell data
  vtkCellData* inCD = this->Input->GetCellData();
  vtkCellData* outCD = this->Output->GetCellData();
  outCD->CopyAllocate( inCD );

  // Extract geometry from hyper tree grid
  this->ProcessTrees();

  // Return duality flag of input to its original state
  if ( inputDualFlagIsOn )
    {
    this->Input->SetUseDualGrid( 1 );
    }

  // Clean up
  this->Input = 0;
  this->Output = 0;

  this->UpdateProgress ( 1. );
  return 1;
}

//-----------------------------------------------------------------------------
void vtkHyperTreeGridGeometry::ProcessTrees()
{
  // TODO: MTime on generation of this table.
  this->Input->GenerateSuperCursorTraversalTable();

  // Primal corner points
  this->Points = vtkPoints::New();
  this->Cells = vtkCellArray::New();

  // Filling of voids
  this->Filling = vtkCharArray::New();
  this->Filling->SetNumberOfTuples( this->Input->GetNumberOfLeaves() );

  // Iterate over all hyper trees
  unsigned int* gridSize = this->Input->GetGridSize();
  for ( unsigned int k = 0; k < gridSize[2]; ++ k )
    {
    for ( unsigned int j = 0; j < gridSize[1]; ++ j )
      {
        for ( unsigned int i = 0; i < gridSize[0]; ++ i )
        {
        // Storage for super cursors
        vtkHyperTreeGridSuperCursor superCursor;

        // Initialize center cursor
        this->Input->InitializeSuperCursor( &superCursor, i, j, k );

        // Traverse and populate dual recursively
        this->RecursiveProcessTree( &superCursor );
        } // i
      } // j
    } // k

  // Set output geometry and topology
  this->Output->SetPoints( this->Points );
  if ( this->Input->GetDimension() == 1  )
    {
    this->Output->SetLines( this->Cells );
    }
  else
    {
    this->Output->SetPolys( this->Cells );
    }
}


//----------------------------------------------------------------------------
void vtkHyperTreeGridGeometry::AddFace( vtkIdType inId, double* origin,
                                        double* size, int offset0,
                                        int axis0, int axis1, int axis2 )
{
  vtkIdType ids[4];
  double pt[3];
  pt[0] = origin[0];
  pt[1] = origin[1];
  pt[2] = origin[2];
  if ( offset0 )
    {
    pt[axis0] += size[axis0];
    }
  ids[0] = this->Points->InsertNextPoint( pt );
  pt[axis1] += size[axis1];
  ids[1] = this->Points->InsertNextPoint( pt );
  pt[axis2] += size[axis2];
  ids[2] = this->Points->InsertNextPoint( pt );
  pt[axis1] = origin[axis1];
  ids[3] = this->Points->InsertNextPoint( pt );

  vtkIdType outId = this->Cells->InsertNextCell( 4, ids );
  this->Output->GetCellData()->CopyData( this->Input->GetCellData(), inId, outId );
}

//----------------------------------------------------------------------------
void vtkHyperTreeGridGeometry::RecursiveProcessTree( vtkHyperTreeGridSuperCursor* superCursor )
{
  // Get cursor at super cursor center
  vtkHyperTreeSimpleCursor* cursor = superCursor->GetCursor( 0 );

  // If cursor is not at leaf, recurse to all children
  if ( ! cursor->IsLeaf() )
    {
    int numChildren = this->Input->GetNumberOfChildren();
    for ( int child = 0; child < numChildren; ++ child )
      {
      vtkHyperTreeGridSuperCursor newSuperCursor;
      this->Input->InitializeSuperCursorChild( superCursor, &newSuperCursor, child );
      this->RecursiveProcessTree( &newSuperCursor );
      } // child
    return;
    } // if ( ! cursor->IsLeaf() )

  // Cursor is a leaf, retrieve its global index
  vtkIdType inId = cursor->GetGlobalLeafIndex();

  // Retrive dimension, as 3D has extra complications
  int dim = this->Input->GetDimension();

  // If leaf is masked, its void will have to be patched
  if ( this->Input->GetLeafMaterialMask()->GetTuple1( inId ) )
    {
    if ( dim == 3 )
      {
      for ( int f = 0; f < 6; ++ f )
        {
        cursor = superCursor->GetCursor( vtkSuperCursorFaceIndices[f] );
        if ( cursor->GetTree() )
          {
          if ( cursor->IsLeaf() )
            {
            int id = cursor->GetGlobalLeafIndex();
            if ( ! this->Input->GetLeafMaterialMask()->GetTuple1( id ) )
              {
              char mask = this->Filling->GetTuple1( inId );
              this->Filling->InsertTuple1( inId, mask + ( 1 << f ) );
              this->AddFace( id, superCursor->Origin, superCursor->Size, 
                             vtkHTGo[f],
                             vtkHTG0[f],
                             vtkHTG1[f],
                             vtkHTG2[f] );
              }
            } // if ( cursor->IsLeaf() )
          else
            {
            cerr << "Cell " << inId << " face " << f << " cannot fill !\n";
            } // else
          } // if ( cursor->GetTree() )
        } // i
      } // if ( dim == 3 )

    return;
    } // if ( this->Input->GetLeafMaterialMask()->GetTuple1( inId ) )

  // In 3D case, terminate if the middle cell is not on the boundary.
  if ( dim == 3
       && superCursor->GetCursor( -1 )->GetTree()
       && superCursor->GetCursor( 1 )->GetTree()
       && superCursor->GetCursor( -3 )->GetTree()
       && superCursor->GetCursor( 3 )->GetTree()
       && superCursor->GetCursor( -9 )->GetTree()
       && superCursor->GetCursor( 9 )->GetTree() )
    {
    return;
    }
  
  // Create the outer geometry, depending on the dimension of the grid
  switch (  this->Input->GetDimension() )
    {
    case 1:
      // In 1D the geometry is composed of edges
      vtkIdType ids[2];
      ids[0] = this->Points->InsertNextPoint( superCursor->Origin );
      double pt[3];
      pt[0] = superCursor->Origin[0] + superCursor->Size[0];
      pt[1] = superCursor->Origin[1];
      pt[2] = superCursor->Origin[2];
      ids[1] = this->Points->InsertNextPoint( pt );
      this->Cells->InsertNextCell( 2, ids );
      break;
    case 2:
      // In 2D all faces are generated
      this->AddFace( inId, superCursor->Origin, superCursor->Size, 0, 2, 0, 1 );
      break;
    case 3:
      // 3D cells have internal faces to skip, check the 6 faces for boundaries
      if ( ! superCursor->GetCursor( -1 )->GetTree() )
        {
        this->AddFace( inId, superCursor->Origin, superCursor->Size, 0, 0, 1, 2 );
        }
      if ( ! superCursor->GetCursor( 1 )->GetTree() )
        {
        this->AddFace( inId, superCursor->Origin, superCursor->Size, 1, 0, 1, 2 );
        }
      if ( ! superCursor->GetCursor( -3 )->GetTree() )
        {
        this->AddFace( inId, superCursor->Origin, superCursor->Size, 0, 1, 0, 2 );
        }
      if ( ! superCursor->GetCursor( 3 )->GetTree() )
        {
        this->AddFace( inId, superCursor->Origin, superCursor->Size, 1, 1, 0, 2 );
        }
      if ( ! superCursor->GetCursor( -9 )->GetTree() )
        {
        this->AddFace( inId, superCursor->Origin, superCursor->Size, 0, 2, 0, 1 );
        }
      if ( ! superCursor->GetCursor( 9 )->GetTree() )
        {
        this->AddFace( inId, superCursor->Origin, superCursor->Size, 1, 2, 0, 1 );
        }
      break;
    } // switch (  this->Input->GetDimension() )
}
