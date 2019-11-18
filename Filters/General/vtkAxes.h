/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAxes.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkAxes
 * @brief   create an x-y-z axes
 *
 * vtkAxes creates three lines that form an x-y-z axes. The origin of the
 * axes is user specified (0,0,0 is default), and the size is specified with
 * a scale factor. Three scalar values are generated for the three lines and
 * can be used (via color map) to indicate a particular coordinate axis.
 */

#ifndef vtkAxes_h
#define vtkAxes_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class VTKFILTERSGENERAL_EXPORT vtkAxes : public vtkPolyDataAlgorithm
{
public:
  static vtkAxes* New();

  vtkTypeMacro(vtkAxes, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set the origin of the axes.
   */
  vtkSetVector3Macro(Origin, double);
  vtkGetVectorMacro(Origin, double, 3);
  //@}

  //@{
  /**
   * Set the scale factor of the axes. Used to control size.
   */
  vtkSetMacro(ScaleFactor, double);
  vtkGetMacro(ScaleFactor, double);
  //@}

  //@{
  /**
   * If Symmetric is on, the axis continue to negative values.
   */
  vtkSetMacro(Symmetric, vtkTypeBool);
  vtkGetMacro(Symmetric, vtkTypeBool);
  vtkBooleanMacro(Symmetric, vtkTypeBool);
  //@}

  //@{
  /**
   * Option for computing normals.  By default they are computed.
   */
  vtkSetMacro(ComputeNormals, vtkTypeBool);
  vtkGetMacro(ComputeNormals, vtkTypeBool);
  vtkBooleanMacro(ComputeNormals, vtkTypeBool);
  //@}

protected:
  vtkAxes();
  ~vtkAxes() override {}

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  // This source does not know how to generate pieces yet.
  int ComputeDivisionExtents(vtkDataObject* output, int idx, int numDivisions);

  double Origin[3];
  double ScaleFactor;

  vtkTypeBool Symmetric;
  vtkTypeBool ComputeNormals;

private:
  vtkAxes(const vtkAxes&) = delete;
  void operator=(const vtkAxes&) = delete;
};

#endif
