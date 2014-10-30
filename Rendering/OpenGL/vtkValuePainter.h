/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkValuePainter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkValuePainter - painter that renders arrays encoded into
// pixel colors.
// .SECTION Description
// vtkValuePainter renders polygonal geometry where data values
// are encoded into 24 bit integers and send to the screen.
// The pixel colors can then be read and converted back into values later.
//
// .SECTION See Also
// vtkValuePass, vtkValuePasses

#ifndef __vtkValuePainter_h
#define __vtkValuePainter_h

#include "vtkRenderingOpenGLModule.h" // For export macro
#include "vtkStandardPolyDataPainter.h"

class vtkInformationDoubleVectorKey;
class vtkInformationIntegerKey;
class vtkInformationStringKey;

class VTKRENDERINGOPENGL_EXPORT vtkValuePainter :
  public vtkStandardPolyDataPainter
{
public:
  static vtkValuePainter* New();
  vtkTypeMacro(vtkValuePainter, vtkStandardPolyDataPainter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the array to be drawn. By default point scalars are
  // used. Arguments are same as those passed to
  // vtkAlgorithm::SetInputArrayToProcess except the first 3 arguments i.e. idx,
  // port, connection.
  void SetInputArrayToProcess(int fieldAssociation, const char *name);
  void SetInputArrayToProcess(int fieldAssociation, int fieldAttributeType);

  // Description:
  // Set the component (0..numcomponents-1) of the specified array to be drawn.
  // If input array is not a multicomponent array or component is otherwise out
  // of range the painter uses the 0'th component instead.
  void SetInputComponentToProcess(int comp);

  // Description:
  // Use the provided scalar range instead of the range of the input data
  // array.
  void SetScalarRange(double min, double max);

  // Description:
  // Optionally passed down from RenderPass pipeline to
  // controls what data array to draw.
  static vtkInformationIntegerKey* SCALAR_MODE();
  static vtkInformationDoubleVectorKey* SCALAR_RANGE();
  static vtkInformationIntegerKey* ARRAY_ID();
  static vtkInformationStringKey* ARRAY_NAME();
  static vtkInformationIntegerKey* ARRAY_COMPONENT();

  // Description:
  // Internal convenience method to convert a value to a color
  // TODO: make this templated and programmable
  static void ValueToColor(double value, double min, double scale,
                           unsigned char *color);

  static void ColorToValue(unsigned char *color, double min, double scale,
                           double &value);

protected:
  vtkValuePainter();
  ~vtkValuePainter();

  // Description:
  // overridden to look for informationkeys that specify what array to draw
  virtual void ProcessInformation(vtkInformation*);

  // Description:
  // overridden to draw the chosen array value directly as color
  virtual void RenderInternal(vtkRenderer* renderer, vtkActor* actor,
    unsigned long typeflags, bool forceCompileOnly);

  // Description:
  // overridden
  // implement drawlines, points, strips
  void DrawCells(int mode, vtkCellArray *connectivity,
    vtkIdType startCellId, vtkRenderer *renderer);

private:
  vtkValuePainter(const vtkValuePainter&); // Not implemented.
  void operator=(const vtkValuePainter&); // Not implemented.

  class vtkInternals;
  vtkInternals *Internals;
};

#endif
