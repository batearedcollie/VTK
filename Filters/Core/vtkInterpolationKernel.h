/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInterpolationKernel.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkInterpolationKernel - base class for interpolation kernels

// .SECTION Description
// vtkInterpolationKernel specifies an abstract interface for interpolation
// kernels. An interpolation kernel is used to produce an interpolated data
// value from the data in a local neighborhood surounding a point X. For
// example, given the N nearest points surrounding X, the interpolation
// kernel provides N weights, which when combined with the N data values
// associated with the nearest points, produces a data value at point X.
//
// Note that various kernel initialization methods are provided. The basic
// method requires providing a point locator to accelerate neigborhood
// queries. Some kernels may refer back to the original dataset, or the point
// attribute data associated with the dataset.

// .SECTION Caveats
// The ComputeWeights() method is thread safe.

// .SECTION See Also
// vtkPointInterpolator vtkGaussianKernel vtkSPHKernel vtkShepardKernel
// vtkVoronoiKernel


#ifndef vtkInterpolationKernel_h
#define vtkInterpolationKernel_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkObject.h"

class vtkAbstractPointLocator;
class vtkIdList;
class vtkDoubleArray;
class vtkDataSet;
class vtkPointData;


class VTKFILTERSCORE_EXPORT vtkInterpolationKernel : public vtkObject
{
public:
  // Description:
  // Standard methods for type and printing.
  vtkTypeMacro(vtkInterpolationKernel,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Initialize the kernel. Pass information into the kernel that is
  // necessary to subsequently perform evaluation. The locator refers to the
  // points that are to be interpolated from; these points (ds) and the
  // associated point data (pd) are provided as well. Note that some kernels
  // may require manual setup / initialization, in which case set
  // RequiresInitialization to false, do not call Initialize(), and of course
  // manually initialize the kernel.
  virtual void Initialize(vtkAbstractPointLocator *loc, vtkDataSet *ds,
                          vtkPointData *pd);

  // Description:
  // Indicate whether the kernel needs initialization. By default this data
  // member is true, and using classes will invoke Initialize() on the
  // kernel. However, if the user takes over initialization manually, then
  // set RequiresInitialization to false (0).
  vtkSetMacro(RequiresInitialization, int);
  vtkGetMacro(RequiresInitialization, int);

  // Description:
  // Given a point x, compute interpolation weights associated with nearby
  // points. The method returns the number of nearby points N (i.e., the
  // neighborhood). Note that both the nearby points list pIds and the
  // weights array are of length N, are provided by the caller of the method,
  // and may be dynamically resized as necessary.
  virtual vtkIdType ComputeWeights(double x[3], vtkIdList *pIds,
                                   vtkDoubleArray *weights) = 0;

protected:
  vtkInterpolationKernel();
  ~vtkInterpolationKernel();

  int RequiresInitialization;
  vtkAbstractPointLocator *Locator;
  vtkDataSet *DataSet;
  vtkPointData *PointData;

  // Just clear out the data. Can be overloaded by subclasses as necessary.
  virtual void FreeStructures();

private:
  vtkInterpolationKernel(const vtkInterpolationKernel&);  // Not implemented.
  void operator=(const vtkInterpolationKernel&);  // Not implemented.
};

#endif
