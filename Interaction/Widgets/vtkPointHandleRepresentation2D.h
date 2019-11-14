/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointHandleRepresentation2D.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkPointHandleRepresentation2D
 * @brief   represent the position of a point in display coordinates
 *
 * This class is used to represent a vtkHandleWidget. It represents a
 * position in 2D world coordinates using a x-y cursor (the cursor defined by
 * an instance of vtkPolyData and generated by a vtkPolyDataAlgorithm).
 *
 * @sa
 * vtkHandleRepresentation vtkHandleWidget
*/

#ifndef vtkPointHandleRepresentation2D_h
#define vtkPointHandleRepresentation2D_h

#include "vtkHandleRepresentation.h"
#include "vtkInteractionWidgetsModule.h" // For export macro

class vtkProperty2D;
class vtkActor2D;
class vtkCoordinate;
class vtkPolyDataMapper2D;
class vtkPolyData;
class vtkGlyph2D;
class vtkPoints;
class vtkPolyDataAlgorithm;
class vtkPointPlacer;

class VTKINTERACTIONWIDGETS_EXPORT vtkPointHandleRepresentation2D : public vtkHandleRepresentation
{
public:
  /**
   * Instantiate this class.
   */
  static vtkPointHandleRepresentation2D *New();

  //@{
  /**
   * Standard methods for instances of this class.
   */
  vtkTypeMacro(vtkPointHandleRepresentation2D,vtkHandleRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  using vtkHandleRepresentation::Translate;

  //@{
  /**
   * Specify the cursor shape with an instance of vtkPolyData. Note that
   * shape is assumed to be defined in the display coordinate system. By
   * default a vtkCursor2D shape is used.
   */
  void SetCursorShape(vtkPolyData *cursorShape);
  vtkPolyData *GetCursorShape();
  //@}

  /**
   * Set/Get the position of the point in display coordinates.  This overloads
   * the superclasses SetDisplayPosition in order to set the focal point
   * of the cursor.
   */
  void SetDisplayPosition(double xyz[3]) override;

  //@{
  /**
   * Set/Get the handle properties when unselected and selected.
   */
  void SetProperty(vtkProperty2D*);
  void SetSelectedProperty(vtkProperty2D*);
  vtkGetObjectMacro(Property,vtkProperty2D);
  vtkGetObjectMacro(SelectedProperty,vtkProperty2D);
  //@}

  //@{
  /**
   * Subclasses of vtkPointHandleRepresentation2D must implement these
   * methods. These are the methods that the widget and its representation
   * use to communicate with each other.
   */
  double *GetBounds() VTK_SIZEHINT(6) override;
  void BuildRepresentation() override;
  void StartWidgetInteraction(double eventPos[2]) override;
  void WidgetInteraction(double eventPos[2]) override;
  int ComputeInteractionState(int X, int Y, int modify=0) override;
  //@}

  //@{
  /**
   * Methods to make this class behave as a vtkProp.
   */
  void ShallowCopy(vtkProp *prop) override;
  void DeepCopy(vtkProp *prop) override;
  void GetActors2D(vtkPropCollection *) override;
  void ReleaseGraphicsResources(vtkWindow *) override;
  int RenderOverlay(vtkViewport *viewport) override;
  //@}

  void Highlight(int highlight) override;

  /**
   * Override the superclass implementation. This class does not use a
   * point placer. Point placers dictate the placement of points in 3D
   * space. Since this class constrains points to lie in an overlay
   * plane anyway, we don't care. Just returns.
   */
  void SetPointPlacer ( vtkPointPlacer * ) override;

  /**
   * Override to ensure that the internal actor's visibility is consistent with
   * this representation's visibility. Inconsistency between the two would cause
   * issues in picking logic which relies on individual view prop visibility to
   * determine whether the prop is pickable.
   */
  void SetVisibility(vtkTypeBool visible) override;

protected:
  vtkPointHandleRepresentation2D();
  ~vtkPointHandleRepresentation2D() override;

  // Render the cursor
  vtkActor2D           *Actor;
  vtkCoordinate        *MapperCoordinate;
  vtkPolyDataMapper2D  *Mapper;
  vtkGlyph2D           *Glypher;
  vtkPolyData          *CursorShape;
  vtkPolyData          *FocalData;
  vtkPoints            *FocalPoint;

  // Support picking
  double LastPickPosition[3];
  double LastEventPosition[2];

  // Methods to manipulate the cursor
  virtual void Translate(const double* eventPos) override;
  void Scale(const double eventPos[2]);

  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty2D *Property;
  vtkProperty2D *SelectedProperty;
  void           CreateDefaultProperties();

  // The size of the hot spot.
  int    WaitingForMotion;
  int    WaitCount;

private:
  vtkPointHandleRepresentation2D(const vtkPointHandleRepresentation2D&) = delete;
  void operator=(const vtkPointHandleRepresentation2D&) = delete;
};

#endif
