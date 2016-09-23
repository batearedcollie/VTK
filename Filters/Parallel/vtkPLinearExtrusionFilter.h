/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPLinearExtrusionFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkPLinearExtrusionFilter
 * @brief   Subclass that handles piece invariance.
 *
 * vtkPLinearExtrusionFilter is a parallel version of vtkLinearExtrusionFilter.
 *
 * @sa
 * vtkLinearExtrusionFilter
*/

#ifndef vtkPLinearExtrusionFilter_h
#define vtkPLinearExtrusionFilter_h

#include "vtkFiltersParallelModule.h" // For export macro
#include "vtkLinearExtrusionFilter.h"

class VTKFILTERSPARALLEL_EXPORT vtkPLinearExtrusionFilter : public vtkLinearExtrusionFilter
{
public:
  vtkTypeMacro(vtkPLinearExtrusionFilter,vtkLinearExtrusionFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
   * Create an object with PieceInvariant off.
   */
  static vtkPLinearExtrusionFilter *New();

  // To get piece invariance, this filter has to request an
  // extra ghost level.  Since piece invariance is not very
  // important for this filter,  it is optional.  Without invariance,
  // Internal surfaces will be generated.  These surface
  // Are hidden by the normal surface generated by this filter.
  // By default, PieceInvariance is off.
  vtkSetMacro(PieceInvariant, int);
  vtkGetMacro(PieceInvariant, int);
  vtkBooleanMacro(PieceInvariant, int);

protected:
  vtkPLinearExtrusionFilter();
  ~vtkPLinearExtrusionFilter() {}

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int RequestUpdateExtent(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  int PieceInvariant;
private:
  vtkPLinearExtrusionFilter(const vtkPLinearExtrusionFilter&) VTK_DELETE_FUNCTION;
  void operator=(const vtkPLinearExtrusionFilter&) VTK_DELETE_FUNCTION;
};

#endif
