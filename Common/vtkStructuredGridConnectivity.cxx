/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vtkStructuredGridConnectivity.cxx

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/
#include "vtkStructuredGridConnectivity.h"
#include "vtkObjectFactory.h"
#include "vtkMeshProperty.h"
#include "vtkMeshPropertyEncoder.h"
#include "vtkStructuredData.h"
#include "vtkStructuredExtent.h"
#include "vtkIdList.h"
#include "vtkStructuredNeighbor.h"

#include <set>
#include <vector>
#include <algorithm>

#define NO_OVERLAP   0
#define NODE_OVERLAP 1
#define EDGE_OVERLAP 2

vtkStandardNewMacro( vtkStructuredGridConnectivity );

//------------------------------------------------------------------------------
vtkStructuredGridConnectivity::vtkStructuredGridConnectivity()
{
  this->DataDescription = -1;
  this->NumberOfGrids   = 0;
  this->WholeExtent[0]  = this->WholeExtent[1] = this->WholeExtent[2] =
  this->WholeExtent[3]  = this->WholeExtent[4] = this->WholeExtent[5] = -1;
  this->GridExtents.clear();
}

//------------------------------------------------------------------------------
vtkStructuredGridConnectivity::~vtkStructuredGridConnectivity()
{
  this->GridExtents.clear();
}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::PrintSelf(std::ostream& os,vtkIndent indent)
{
  this->Superclass::PrintSelf( os, indent );
}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::RegisterGrid(const int gridID,int ext[6])
{
  assert( "pre: gridID out-of-bounds!" &&
           (gridID >= 0  && gridID < this->NumberOfGrids) );
  for( int i=0; i < 6; ++i )
    this->GridExtents[ gridID*6+i ] = ext[i];
}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::GetGridExtent(const int gridID, int ext[6])
{
  assert( "pre: gridID out-of-bounds!" &&
          (gridID >= 0  && gridID < this->NumberOfGrids) );
  for( int i=0; i < 6; ++i )
    ext[i] = this->GridExtents[ gridID*6+i ];
}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::AcquireDataDescription()
{
  if( this->DataDescription != -1 )
    return;

  int dims[3];
  vtkStructuredExtent::GetDimensions( this->WholeExtent, dims );
  this->DataDescription = vtkStructuredData::GetDataDescription( dims );
  assert( "pre: Error acquiring data description" &&
           (this->DataDescription >= 0) );
}

//------------------------------------------------------------------------------
vtkIdList* vtkStructuredGridConnectivity::GetNeighbors(
    const int gridID,int *extents )
{
  assert( "pre: input extents array is NULL" && (extents != NULL) );

  int N = this->GetNumberOfNeighbors( gridID );
  if( N < 1 ) return NULL;

  vtkIdList *neiList = vtkIdList::New();
  neiList->SetNumberOfIds( N );

  unsigned int nei=0;
  for( ; nei < this->Neighbors[ gridID ].size(); ++nei )
    {
    vtkIdType neiId = this->Neighbors[ gridID ][ nei ].NeighborID;
    neiList->SetId( nei, neiId );
    for( int i=0; i < 6; ++i  )
      extents[ nei*6+i ] = this->Neighbors[ gridID ][ nei ].OverlapExtent[ i ];
    } // END for all neighbors

  assert( "post: N==neiList.size()" &&
          (N == neiList->GetNumberOfIds()) );
  return( neiList );
}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::ComputeNeighbors()
{
  this->AcquireDataDescription( );
  if( this->DataDescription == VTK_EMPTY ||
      this->DataDescription == VTK_SINGLE_POINT )
      return;

  for( int i=0; i < this->NumberOfGrids; ++i )
    {
    for( int j=i+1; j < this->NumberOfGrids; ++j )
      {
      this->EstablishNeighbors(i,j);
      } // END for all j
    } // END for all i
}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::MarkNodeProperty(
    const int i, const int j, const int k, const int idx,
    int ext[6], unsigned char &p )
{
  vtkMeshPropertyEncoder::Reset( p );

  if( this->IsNodeInterior( i, j, k, ext ) )
    {
    vtkMeshPropertyEncoder::SetProperty( p,VTKNodeProperties::INTERNAL );
    }
  else
    {
    if( this->IsNodeOnBoundary(i,j,k) )
      {
      vtkMeshPropertyEncoder::SetProperty( p,VTKNodeProperties::BOUNDARY );
      }
    else
      {
      vtkMeshPropertyEncoder::SetProperty( p,VTKNodeProperties::SHARED );
      }
    }
}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::FillMeshPropertyArrays(
    const int gridID, unsigned char *nodesArray, unsigned char *cellsArray )
{
  assert( "pre: Nodes array is not NULL" && (nodesArray != NULL) );
  assert( "pre: Cell array is not NULL" && (cellsArray != NULL) );

  int GridExtent[6];
  this->GetGridExtent( gridID, GridExtent );

  int ijkmin[3];
  ijkmin[0] = GridExtent[0];
  ijkmin[1] = GridExtent[2];
  ijkmin[2] = GridExtent[4];

  int dims[3];
  vtkStructuredExtent::GetDimensions( GridExtent, dims );
//  std::cout << "\n ============ \n";
//  std::cout << "DIMS:" << dims[0] << " " << dims[1] << " " << dims[2] << "\n";
//  std::cout.flush();

  int *overlapExtents = NULL;
  vtkIdList *neiList  = NULL;
  int N = this->GetNumberOfNeighbors( gridID );
  if( N > 0 )
    {
    overlapExtents = new int[ 6*N ];
    neiList  = this->GetNeighbors( gridID, overlapExtents );
    assert("pre: neiList should not be NULL!" && (neiList != NULL) );
    assert("pre: Number of neighbors must equal size of neiList" &&
           (N == neiList->GetNumberOfIds() ) );
    }

  int ext[6];
  for( int i=GridExtent[0]; i <= GridExtent[1]; ++i )
    {
    for( int j=GridExtent[2]; j <= GridExtent[3]; ++j )
      {
      for( int k=GridExtent[4]; k <= GridExtent[5]; ++k )
        {
        // Convert global indices to local indices
        int li = i - ijkmin[0];
        int lj = j - ijkmin[1];
        int lk = k - ijkmin[2];

        int idx = vtkStructuredData::ComputePointId( dims, li, lj, lk );

//        std::cout << "IJK: " << i << " " << j << " " <<  k << " -- ";
//        std::cout << li << " " << lj << " " << lk << " ==> ";
//        std::cout << idx << std::endl;
//        std::cout.flush();
        this->MarkNodeProperty( i, j, k, idx, GridExtent, nodesArray[idx] );
        } // END for all k
      } // END for all j
    } // END for all i

  if( overlapExtents != NULL )
    delete [] overlapExtents;
  neiList->Delete();
}

//------------------------------------------------------------------------------
bool vtkStructuredGridConnectivity::IsNodeOnBoundary(
    const int i, const int j, const int k )
{
  // TODO: Add support for 2-D and 1-D cases
  if( i==this->WholeExtent[0] || i==this->WholeExtent[1] ||
      j==this->WholeExtent[2] || j==this->WholeExtent[3] ||
      k==this->WholeExtent[4] || k==this->WholeExtent[5] )
      return true;
  return false;
}

//------------------------------------------------------------------------------
bool vtkStructuredGridConnectivity::IsNodeInterior(
    const int i, const int j, const int k,
    int GridExtent[6] )
{
  // TODO: Add support for 2-D and 1-D cases
  if( (GridExtent[0] < i) && (i < GridExtent[1]) &&
      (GridExtent[2] < j) && (j < GridExtent[3]) &&
      (GridExtent[4] < k) && (k < GridExtent[5])    )
      return true;
  return false;
}

//------------------------------------------------------------------------------
bool vtkStructuredGridConnectivity::IsNodeWithinExtent(
    const int i, const int j, const int k,
    int GridExtent[6] )
{
  // TODO: Add support for 2-D and 1-D cases
  if( (GridExtent[0] <= i) && (GridExtent[1] <= i) &&
      (GridExtent[2] <= j) && (GridExtent[3] <= j) &&
      (GridExtent[4] <= k) && (GridExtent[5] <= k) )
      return true;
  return false;
}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::EstablishNeighbors(const int i,const int j)
{
  assert( "pre: i < j" && (i < j) );

  int iGridExtent[6];
  int jGridExtent[6];
  this->GetGridExtent( i, iGridExtent );
  this->GetGridExtent( j, jGridExtent );

  // A 3-tuple that defines the grid orientation of the form {i,j,k} where
  // i=0;, j=1, k=2. For example, let's say that we want to define the
  // orientation to be in the XZ plane, then, orientation  array would be
  // constructed as follows: {0,2 -1}, where -1 indicates a NIL value.
  int orientation[3];

  // A place holder for setting up ndim, which store the dimensionality of
  // the data.
  int ndim = 3;

  switch( this->DataDescription )
    {
    case VTK_X_LINE:
      ndim           =  1;
      orientation[0] =  0;
      orientation[1] = -1;
      orientation[2] = -1;
      break;
    case VTK_Y_LINE:
      ndim           =  1;
      orientation[0] =  1;
      orientation[1] = -1;
      orientation[2] = -1;
      break;
    case VTK_Z_LINE:
      ndim           =  1;
      orientation[0] =  2;
      orientation[1] = -1;
      orientation[2] = -1;
      break;
    case VTK_XY_PLANE:
      ndim           =  2;
      orientation[0] =  0;
      orientation[1] =  1;
      orientation[2] = -1;
      break;
    case VTK_YZ_PLANE:
      ndim           =  2;
      orientation[0] =  1;
      orientation[1] =  2;
      orientation[2] = -1;
      break;
    case VTK_XZ_PLANE:
      ndim           =  2;
      orientation[0] =  0;
      orientation[1] =  2;
      orientation[2] = -1;
      break;
    case VTK_XYZ_GRID:
      ndim           =  3;
      orientation[0] =  0;
      orientation[1] =  1;
      orientation[2] =  2;
      break;
    default:
      assert( "pre: Undefined data-description!" && false );
    } // END switch

  this->DetectNeighbors( i, j, iGridExtent, jGridExtent, orientation, ndim );
}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::DetectNeighbors(
    const int i, const int j,
    int ex1[6], int ex2[6], int orientation[3], int ndim )
{
  std::vector< int > status;
  status.resize( ndim );

  int A[2];
  int B[2];
  int overlap[2];

  int overlapExtent[6];
  for( int ii=0; ii < 6; ++ii )
    overlapExtent[ ii ] = 0;


  int dim = 0;
  int idx = 0;
  for( dim=0; dim < ndim; ++dim )
    {
    idx           = orientation[dim];
    A[0]          = ex1[ idx*2 ];
    A[1]          = ex1[ idx*2+1 ];
    B[0]          = ex2[ idx*2 ];
    B[1]          = ex2[ idx*2+1 ];
    status[ idx ] = this->IntervalOverlap( A, B, overlap );
    if( status[idx] == NO_OVERLAP )
      return; /* No neighbors */
    overlapExtent[ idx*2 ]   = overlap[0];
    overlapExtent[ idx*2+1 ] = overlap[1];
    } // END for all dimensions

  this->SetNeighbors( i, j, overlapExtent );
}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::SetNeighbors(
            const int i, const int j, int overlapExtent[6] )
{
  vtkStructuredNeighbor Ni2j( j, overlapExtent );
  vtkStructuredNeighbor Nj2i( i, overlapExtent );

  this->Neighbors[ i ].push_back( Ni2j );
  this->Neighbors[ j ].push_back( Nj2i );

// BEGIN DEBUG
  int iGridExtent[6];
  int jGridExtent[6];
  this->GetGridExtent( i, iGridExtent );
  this->GetGridExtent( j, jGridExtent );

  std::cout << "===\n";
  this->PrintExtent( iGridExtent );
  this->PrintExtent( jGridExtent );
  std::cout << "\n\n";
  this->PrintExtent( overlapExtent );
// END DEBUG

}

//------------------------------------------------------------------------------
void vtkStructuredGridConnectivity::PrintExtent( int ex[6] )
{
  for( int i=0; i < 3; ++i )
    std::cout << " [" << ex[i*2] << ", " << ex[i*2+1] << "] ";
  std::cout << std::endl;
  std::cout.flush();
}

//------------------------------------------------------------------------------
int vtkStructuredGridConnectivity::IntervalOverlap(
                    int A[2], int B[2], int overlap[2] )
{
  int rc = NO_OVERLAP;

  // STEP 0: Initialize overlap
  for( int i=0; i < 2; i++ )
    overlap[i] = -1;

  // STEP 1: Allocate internal intersection vector. Note, since the cardinality
  // of A,B is 2, the intersection vector can be at most of size 2.
  std::vector< int > intersection;
  intersection.resize( 2 );

  // STEP 2: Compute intersection
  std::vector< int >::iterator it;
  it = std::set_intersection( A, A+2, B, B+2, intersection.begin() );

  // STEP 3: Find number of intersections and overlap extent
  int N = static_cast< int >( it-intersection.begin() );

  switch( N )
    {
    case 0:
      rc = NO_OVERLAP;
      overlap[ 0 ] = overlap[ 1 ] = -1;
      break;
    case 1:
      rc = NODE_OVERLAP;
      overlap[0] = overlap[1] = intersection[0];
      break;
    case 2:
      rc = EDGE_OVERLAP;
      overlap[0] = intersection[0];
      overlap[1] = intersection[1];
      break;
    default:
      rc = NO_OVERLAP;
      overlap[ 0 ] = overlap[ 1 ] = -1;
      vtkErrorMacro( "Code should not reach here!" )
    }

  return( rc );
}

