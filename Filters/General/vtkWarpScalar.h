/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWarpScalar.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkWarpScalar
 * @brief   deform geometry with scalar data
 *
 * vtkWarpScalar is a filter that modifies point coordinates by moving
 * points along point normals by the scalar amount times the scale factor.
 * Useful for creating carpet or x-y-z plots.
 *
 * If normals are not present in data, the Normal instance variable will
 * be used as the direction along which to warp the geometry. If normals are
 * present but you would like to use the Normal instance variable, set the
 * UseNormal boolean to true.
 *
 * If XYPlane boolean is set true, then the z-value is considered to be
 * a scalar value (still scaled by scale factor), and the displacement is
 * along the z-axis. If scalars are also present, these are copied through
 * and can be used to color the surface.
 *
 * Note that the filter passes both its point data and cell data to
 * its output, except for normals, since these are distorted by the
 * warping.
*/

#ifndef vtkWarpScalar_h
#define vtkWarpScalar_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPointSetAlgorithm.h"

class vtkDataArray;

class VTKFILTERSGENERAL_EXPORT vtkWarpScalar : public vtkPointSetAlgorithm
{
public:
  static vtkWarpScalar *New();
  vtkTypeMacro(vtkWarpScalar,vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Specify value to scale displacement.
   */
  vtkSetMacro(ScaleFactor,double);
  vtkGetMacro(ScaleFactor,double);
  //@}

  //@{
  /**
   * Turn on/off use of user specified normal. If on, data normals
   * will be ignored and instance variable Normal will be used instead.
   */
  vtkSetMacro(UseNormal,vtkTypeBool);
  vtkGetMacro(UseNormal,vtkTypeBool);
  vtkBooleanMacro(UseNormal,vtkTypeBool);
  //@}

  //@{
  /**
   * Normal (i.e., direction) along which to warp geometry. Only used
   * if UseNormal boolean set to true or no normals available in data.
   */
  vtkSetVector3Macro(Normal,double);
  vtkGetVectorMacro(Normal,double,3);
  //@}

  //@{
  /**
   * Turn on/off flag specifying that input data is x-y plane. If x-y plane,
   * then the z value is used to warp the surface in the z-axis direction
   * (times the scale factor) and scalars are used to color the surface.
   */
  vtkSetMacro(XYPlane,vtkTypeBool);
  vtkGetMacro(XYPlane,vtkTypeBool);
  vtkBooleanMacro(XYPlane,vtkTypeBool);
  //@}

  int FillInputPortInformation(int port, vtkInformation *info) override;

protected:
  vtkWarpScalar();
  ~vtkWarpScalar() override;

  int RequestDataObject(vtkInformation *request,
                        vtkInformationVector **inputVector,
                        vtkInformationVector *outputVector) override;
  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *) override;

  double ScaleFactor;
  vtkTypeBool UseNormal;
  double Normal[3];
  vtkTypeBool XYPlane;

  double *(vtkWarpScalar::*PointNormal)(vtkIdType id, vtkDataArray *normals);
  double *DataNormal(vtkIdType id, vtkDataArray *normals=nullptr);
  double *InstanceNormal(vtkIdType id, vtkDataArray *normals=nullptr);
  double *ZNormal(vtkIdType id, vtkDataArray *normals=nullptr);

private:
  vtkWarpScalar(const vtkWarpScalar&) = delete;
  void operator=(const vtkWarpScalar&) = delete;
};

#endif
