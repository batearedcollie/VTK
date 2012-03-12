/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMultiBlockMergeFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMultiBlockMergeFilter - merges multiblock inputs into a single multiblock output
// .SECTION Description
// vtkMultiBlockMergeFilter is an M to 1 filter similar to 
// vtkMultiBlockDataGroupFilter. However where as that class creates N groups 
// in the output for N inputs, this creates 1 group in the output with N 
// datasets inside it. In actuality if the inputs have M blocks, this will 
// produce M blocks, each of which has N datasets. Inside the merged group, 
// the i'th data set comes from the i'th data set in the i'th input.

#ifndef __vtkMultiBlockMergeFilter_h
#define __vtkMultiBlockMergeFilter_h

#include "vtkMultiBlockDataSetAlgorithm.h"

class VTK_GRAPHICS_EXPORT vtkMultiBlockMergeFilter 
: public vtkMultiBlockDataSetAlgorithm
{
public:
  vtkTypeMacro(vtkMultiBlockMergeFilter,vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object with PointIds and CellIds on; and ids being generated
  // as scalars.
  static vtkMultiBlockMergeFilter *New();

  // Description:
  // Assign a data object as input. Note that this method does not
  // establish a pipeline connection. Use AddInputConnection() to
  // setup a pipeline connection.
  void AddInputData(vtkDataObject *);
  void AddInputData(int, vtkDataObject*);

protected:
  vtkMultiBlockMergeFilter();
  ~vtkMultiBlockMergeFilter();

  int RequestData(vtkInformation *, 
                  vtkInformationVector **, 
                  vtkInformationVector *);

  virtual int FillInputPortInformation(int port, vtkInformation *info);

  int IsMultiPiece(vtkMultiBlockDataSet*);

  int Merge(unsigned int numPieces, unsigned int pieceNo, 
    vtkMultiBlockDataSet* output, 
    vtkMultiBlockDataSet* input);
private:
  vtkMultiBlockMergeFilter(const vtkMultiBlockMergeFilter&);  // Not implemented.
  void operator=(const vtkMultiBlockMergeFilter&);  // Not implemented.
};

#endif


