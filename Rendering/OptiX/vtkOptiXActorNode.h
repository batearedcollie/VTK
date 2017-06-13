/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOptiXActorNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkOptiXActorNode
 * @brief   links vtkActor and vtkMapper to OptiX
 *
 * Translates vtkActor/Mapper state into OptiX rendering calls
*/

#ifndef vtkOptiXActorNode_h
#define vtkOptiXActorNode_h

#include "vtkRenderingOptiXModule.h" // For export macro
#include "vtkActorNode.h"

class vtkActor;
class vtkCompositeDataDisplayAttributes;
class vtkDataArray;
class vtkInformationIntegerKey;
class vtkInformationObjectBaseKey;
class vtkInformationStringKey;
class vtkPiecewiseFunction;
class vtkPolyData;
class vtkMapper;

class VTKRENDERINGOPTIX_EXPORT vtkOptiXActorNode :
  public vtkActorNode
{
public:
  static vtkOptiXActorNode* New();
  vtkTypeMacro(vtkOptiXActorNode, vtkActorNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
   * Overridden to take into account my renderables time, including
   * mapper and data into mapper inclusive of composite input.
   */
  virtual vtkMTimeType GetMTime();

  /**
   * When added to the mapper, enables scale array and scale function.
   */
  static vtkInformationIntegerKey* ENABLE_SCALING();

  //@{
  /**
   * Convenience method to set enabled scaling on my renderable.
   */
  static void SetEnableScaling(int value, vtkActor *);
  static int GetEnableScaling(vtkActor *);
  //@}

  /**
   * Name of a point aligned, single component wide, double valued array that,
   * when added to the mapper, will be used to scale each element in the
   * sphere and cylinder representations individually.
   * When not supplied the radius is constant across all elements and
   * s a function of the Mapper's PointSize and LineWidth.
   */
  static vtkInformationStringKey* SCALE_ARRAY_NAME();

  /**
   * Convenience method to set a scale array on my renderable.
   */
  static void SetScaleArrayName(const char *scaleArrayName, vtkActor *);

  /**
   * A piecewise function for values from the scale array that alters the
   * resulting radii arbitrarily.
   */
  static vtkInformationObjectBaseKey* SCALE_FUNCTION();

  /**
   * Convenience method to set a scale function on my renderable.
   */
  static void SetScaleFunction(vtkPiecewiseFunction *scaleFunction, vtkActor *);

protected:
  vtkOptiXActorNode();
  ~vtkOptiXActorNode();

private:
  vtkOptiXActorNode(const vtkOptiXActorNode&) VTK_DELETE_FUNCTION;
  void operator=(const vtkOptiXActorNode&) VTK_DELETE_FUNCTION;

  vtkMapper* LastUsedMapper;
};
#endif
