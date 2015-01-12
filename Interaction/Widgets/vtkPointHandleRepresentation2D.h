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
// .NAME vtkPointHandleRepresentation2D - represent the position of a point in display coordinates
// .SECTION Description
// This class is used to represent a vtkHandleWidget. It represents a
// position in 2D world coordinates using a x-y cursor (the cursor defined by
// an instance of vtkPolyData and generated by a vtkPolyDataAlgorithm).

// .SECTION See Also
// vtkHandleRepresentation vtkHandleWidget


#ifndef vtkPointHandleRepresentation2D_h
#define vtkPointHandleRepresentation2D_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkHandleRepresentation.h"

class vtkProperty2D;
class vtkActor2D;
class vtkPolyDataMapper2D;
class vtkPolyData;
class vtkGlyph2D;
class vtkPoints;
class vtkPolyDataAlgorithm;
class vtkPointPlacer;

class VTKINTERACTIONWIDGETS_EXPORT vtkPointHandleRepresentation2D : public vtkHandleRepresentation
{
public:
  // Description:
  // Instantiate this class.
  static vtkPointHandleRepresentation2D *New();

  // Description:
  // Standard methods for instances of this class.
  vtkTypeMacro(vtkPointHandleRepresentation2D,vtkHandleRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the cursor shape with an instance of vtkPolyData. Note that
  // shape is assumed to be defined in the display coordinate system. By
  // default a vtkCursor2D shape is used.
  void SetCursorShape(vtkPolyData *cursorShape);
  vtkPolyData *GetCursorShape();

  // Description:
  // Set/Get the position of the point in display coordinates.  This overloads
  // the superclasses SetDisplayPosition in order to set the focal point
  // of the cursor.
  virtual void SetDisplayPosition(double xyz[3]);

  // Description:
  // Set/Get the handle properties when unselected and selected.
  void SetProperty(vtkProperty2D*);
  void SetSelectedProperty(vtkProperty2D*);
  vtkGetObjectMacro(Property,vtkProperty2D);
  vtkGetObjectMacro(SelectedProperty,vtkProperty2D);

  // Description:
  // Subclasses of vtkPointHandleRepresentation2D must implement these
  // methods. These are the methods that the widget and its representation
  // use to communicate with each other.
  virtual double *GetBounds();
  virtual void BuildRepresentation();
  virtual void StartWidgetInteraction(double eventPos[2]);
  virtual void WidgetInteraction(double eventPos[2]);
  virtual int ComputeInteractionState(int X, int Y, int modify=0);

  // Description:
  // Methods to make this class behave as a vtkProp.
  virtual void ShallowCopy(vtkProp *prop);
  virtual void DeepCopy(vtkProp *prop);
  virtual void GetActors2D(vtkPropCollection *);
  virtual void ReleaseGraphicsResources(vtkWindow *);
  virtual int RenderOverlay(vtkViewport *viewport);

  void Highlight(int highlight);

  // Description:
  // Override the superclass implementation. This class does not use a
  // point placer. Point placers dictate the placement of points in 3D
  // space. Since this class constrains points to lie in an overlay
  // plane anyway, we don't care. Just returns.
  virtual void SetPointPlacer ( vtkPointPlacer * );

protected:
  vtkPointHandleRepresentation2D();
  ~vtkPointHandleRepresentation2D();

  // Render the cursor
  vtkActor2D           *Actor;
  vtkPolyDataMapper2D  *Mapper;
  vtkGlyph2D           *Glypher;
  vtkPolyData          *CursorShape;
  vtkPolyData          *FocalData;
  vtkPoints            *FocalPoint;

  // Support picking
  double LastPickPosition[3];
  double LastEventPosition[2];

  // Methods to manipulate the cursor
  int  ConstraintAxis;
  void Translate(double eventPos[2]);
  void Scale(double eventPos[2]);




  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty2D *Property;
  vtkProperty2D *SelectedProperty;
  void           CreateDefaultProperties();

  // The size of the hot spot.
  int    DetermineConstraintAxis(int constraint, double eventPos[2]);
  int    WaitingForMotion;
  int    WaitCount;

private:
  vtkPointHandleRepresentation2D(const vtkPointHandleRepresentation2D&);  //Not implemented
  void operator=(const vtkPointHandleRepresentation2D&);  //Not implemented
};

#endif
