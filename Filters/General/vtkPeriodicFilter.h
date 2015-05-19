/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPeriodicFiler.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkPeriodicFilter - A filter to produce mapped  periodic
// multiblock dataset from a single block
//
// .SECTION Description:
// Generate periodic dataset by transforming points, vectors, tensors
// data arrays from an original data array.
// The generated dataset is of the same type than the input (float or double).
// This is an abstract class wich do not implement the actual transformation.
// Point coordinates are transformed, as well as all vectors (3-components) and
// tensors (9 components) in points and cell data arrays.
// The generated multiblock will have the same tree architecture than the input,
// except transformed leaves are replaced by a vtkMultipieceDataSet.
// Supported input leaf dataset type are: vtkPolyData, vtkStructuredGrid
// and vtkUnstructuredGrid. Other data objects are transformed using the
// transform filter (at a high cost!).

#ifndef vtkPeriodicFilter_h
#define vtkPeriodicFilter_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkMultiBlockDataSetAlgorithm.h"

#include <set> // For block selection

class vtkCompositeDataIterator;
class vtkCompositeDataSet;
class vtkDataObjectTreeIterator;
class vtkMultiPieceDataSet;

#define VTK_ITERATION_MODE_DIRECT_NB 0    // Generate a user-provided number of periods
#define VTK_ITERATION_MODE_MAX       1    // Generate a maximum of periods, i.e. a full period.

class VTKFILTERSGENERAL_EXPORT vtkPeriodicFilter : public vtkMultiBlockDataSetAlgorithm
{
public:
  vtkTypeMacro(vtkPeriodicFilter, vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get Iteration mode.
  // VTK_ITERATION_MODE_DIRECT_NB to specify the number of periods,
  // VTK_ITERATION_MODE_MAX to generate a full period (default).
  vtkSetClampMacro(IterationMode, int,
                   VTK_ITERATION_MODE_DIRECT_NB,
                   VTK_ITERATION_MODE_MAX);
  vtkGetMacro(IterationMode, int);
  void SetIterationModeToDirectNb()
    { this->SetIterationMode(VTK_ITERATION_MODE_DIRECT_NB); }
  void SetIterationModeToMax()
    { this->SetIterationMode(VTK_ITERATION_MODE_MAX); }

  // Description:
  // Set/Get Number of periods.
  // Used only with ITERATION_MODE_DIRECT_NB.
  vtkSetMacro(NumberOfPeriods, int);
  vtkGetMacro(NumberOfPeriods, int);

  // Description:
  // Select the periodic pieces indices.
  // Each node in the multi - block tree is identified by an \c index. The index can
  // be obtained by performing a preorder traversal of the tree (including empty
  // nodes). eg. A(B (D, E), C(F, G)).
  // Inorder traversal yields: A, B, D, E, C, F, G
  // Index of A is 0, while index of C is 4.
  virtual void AddIndex(unsigned int index);

  // Description:
  // Remove an index from selected indices tress
  virtual void RemoveIndex(unsigned int index);

  // Description:
  // Clear selected indices tree
  virtual void RemoveAllIndices();

protected:
  vtkPeriodicFilter();
  ~vtkPeriodicFilter();

  virtual int RequestData(vtkInformation *,
                          vtkInformationVector **,
                          vtkInformationVector *);

  // Description:
  // Create the periodic sub tree
  virtual void CreatePeriodicSubTree(vtkDataObjectTreeIterator* loc,
                                     vtkMultiBlockDataSet* output,
                                     vtkMultiBlockDataSet* input);

  // Description:
  // Create a periodic data, leaf of the tree
  virtual void CreatePeriodicDataSet(vtkCompositeDataIterator* loc,
                                     vtkCompositeDataSet* output,
                                     vtkCompositeDataSet* input) = 0;

  // Description:
  // Generate a name for a piece in the periodic dataset from the input dataset
  virtual void GeneratePieceName(vtkCompositeDataSet* input,
                                 vtkCompositeDataIterator* inputLoc,
                                 vtkMultiPieceDataSet* output,
                                 vtkIdType outputId);

private:
  vtkPeriodicFilter(const vtkPeriodicFilter&); // Not implemented.
  void operator=(const vtkPeriodicFilter&); // Not implemented.

  int IterationMode;
  int NumberOfPeriods;      // User provided number of periods

  std::set<vtkIdType> Indices;          // All the tree indices
  std::set<vtkIdType> ActiveIndices;    // Selected indices only
};

#endif
