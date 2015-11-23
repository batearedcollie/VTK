/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAbstractCellLinks.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkAbstractCellLinks - an abstract base class for classes that build
// topological links from points to cells
// .SECTION Description
// vtkAbstractCellLinks is a family of supplemental objects to vtkCellArray and
// vtkCellTypes, enabling fast access from points to the cells using the
// points. vtkStaticCellLinksTemplate is an array of links, each link represents a
// list of cell id's using a particular point. The information provided by
// this object can be used to determine neighbors and construct other local
// topological information.

//
// .SECTION See Also
// vtkCellLinks vtkStaticCellLinks vtkStaticCellLinksTemplate

#ifndef vtkAbstractCellLinks_h
#define vtkAbstractCellLinks_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkObject.h"

class vtkDataSet;
class vtkCellArray;


class VTKCOMMONDATAMODEL_EXPORT vtkAbstractCellLinks : public vtkObject
{
public:
  // Description:
  // Standard type and print methods.
  vtkTypeMacro(vtkAbstractCellLinks,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Build the link list array. All subclasses must implement this method.
  virtual void BuildLinks(vtkDataSet *data) = 0;

  // Description:
  // Based on the input (i.e., number of points, number of cells, and length
  // of connectivity array) this helper method returns the integral type to
  // use when instantiating the class in order to properly represent the data.
  // The return value is one of the types (VTK_ID_TYPE,VTK_INT,VTK_SHORT) defined
  // in the file vtkType.h. Subclasses may choose to instantiate themselves with
  // different integral types for performance and/or memory reasons.
  static int GetIdType(vtkIdType maxPtId, vtkIdType maxCellId, vtkCellArray *ca);

protected:
  vtkAbstractCellLinks();
  virtual ~vtkAbstractCellLinks();

private:
  vtkAbstractCellLinks(const vtkAbstractCellLinks&);  // Not implemented.
  void operator=(const vtkAbstractCellLinks&);  // Not implemented.
};

#endif
