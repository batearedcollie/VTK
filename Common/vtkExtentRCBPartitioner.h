/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vtkExtentRCBPartitioner.h

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/
// .NAME vtkExtentRCBPartitioner.h -- Partitions a global structured extent.
//
// .SECTION Description
//  This method partitions a global extent to N partitions where N is a user
//  supplied parameter.

#ifndef VTKEXTENTRCBPARTITIONER_H_
#define VTKEXTENTRCBPARTITIONER_H_

#include "vtkObject.h"
#include <vector> // For STL vector
#include <cassert>  // For assert

class VTK_COMMON_EXPORT vtkExtentRCBPartitioner : public vtkObject
{
  public:
    static vtkExtentRCBPartitioner *New();
    vtkTypeMacro(vtkExtentRCBPartitioner,vtkObject);
    void PrintSelf(ostream &oss, vtkIndent indent );

    // Description:
    // Set/Get the number of requested partitions
    void SetNumberOfPartitions( const int N )
    {
      assert( "pre: Number of partitions requested must be > 0" && (N >= 0) );
      this->Reset();
      this->NumberOfPartitions = N;
    }

    // Description:
    // Set/Get the global extent array to be partitioned.
    // The global extent is packed as follows:
    // [imin,imax,jmin,jmax,kmin,kmax]
    void SetGlobalExtent(int imin,int imax,int jmin,int jmax,int kmin,int kmax)
      {
      this->Reset();
      this->GlobalExtent[0]     = imin;
      this->GlobalExtent[1]     = imax;
      this->GlobalExtent[2]     = jmin;
      this->GlobalExtent[3]     = jmax;
      this->GlobalExtent[4]     = kmin;
      this->GlobalExtent[5]     = kmax;
      }
    void SetGlobalExtent( int ext[6] )
    {
      this->SetGlobalExtent( ext[0], ext[1], ext[2], ext[3], ext[4], ext[5] );
    }


    // Description:
    // Returns the number of extents.
    vtkGetMacro(NumExtents,int);

    // Description:
    // Partitions the extent
    void Partition();

    // Description:
    // Returns the extent of the partition corresponding to the given ID.
    void GetPartitionExtent( const int idx, int ext[6] );

  protected:
    vtkExtentRCBPartitioner();
   ~vtkExtentRCBPartitioner();

    // Description:
    // Resets the partitioner to the initial state, all previous partition
    // extents are cleared.
    void Reset()
     {
     this->PartitionExtents.clear();
     this->NumExtents          = 0;
     this->ExtentIsPartitioned = false;
     }

     // Description:
     // Returns the extent at the position corresponding to idx.
     void GetExtent( const int idx, int ext[6] );

     // Description:
     // Adds the extent to the end of the list of partitioned extents
     void AddExtent(int ext[6]);

     // Description:
     // Replaces the extent at the position indicated by idx with the provided
     // extent.
     void ReplaceExtent(const int idx, int ext[6]);

     // Description:
     // Splits the extent along the given dimension.
     void SplitExtent(int parent[6],int s1[6],int s2[6],int splitDimension);

     // Description:
     // Returns the total number of extents. It's always the 2^N where
     // N is the number of subdivisions.
     int GetNumberOfTotalExtents();

     // Description:
     // Computes the total number of nodes for the given structured grid extent
     int GetNumberOfNodes( int ext[6] );

     // Description:
     // Computes the total number of cells for the given structured grid extent
     int GetNumberOfCells( int ext[6] );

     // Description:
     // Returns the length of the longest dimension
     int GetLongestDimensionLength( int ext[6] );

     // Description:
     // Returns the longest edge
     int GetLongestDimension( int ext[6] );

     // Description:
     // A convenience method for debugging purposes.
     void PrintExtent( std::string name, int ext[6] );

     int GlobalExtent[6];
     int NumberOfPartitions;
     int NumExtents;
     bool ExtentIsPartitioned;

     // BTX
     std::vector<int> pextents;
     // ETX

  private:
    vtkExtentRCBPartitioner( const vtkExtentRCBPartitioner& );// Not implemented
    void operator=( const vtkExtentRCBPartitioner& );// Not implemented
};

#endif /* VTKEXTENTRCBPARTITIONER_H_ */
