/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenGLPolyDataMapper2D.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOpenGLPolyDataMapper2D - 2D PolyData support for OpenGL
// .SECTION Description
// vtkOpenGLPolyDataMapper2D provides 2D PolyData annotation support for
// vtk under OpenGL.  Normally the user should use vtkPolyDataMapper2D
// which in turn will use this class.

// .SECTION See Also
// vtkPolyDataMapper2D

#ifndef vtkOpenGLPolyDataMapper2D_h
#define vtkOpenGLPolyDataMapper2D_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkPolyDataMapper2D.h"
#include "vtkOpenGLHelper.h" // used for ivars
#include <string> // For API.

class vtkOpenGLBufferObject;
class vtkOpenGLHelper;
class vtkOpenGLVertexBufferObject;
class vtkPoints;
class vtkRenderer;
class vtkTextureObject;

class VTKRENDERINGOPENGL2_EXPORT vtkOpenGLPolyDataMapper2D : public vtkPolyDataMapper2D
{
public:
  vtkTypeMacro(vtkOpenGLPolyDataMapper2D, vtkPolyDataMapper2D);
  static vtkOpenGLPolyDataMapper2D *New();
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Actually draw the poly data.
  void RenderOverlay(vtkViewport* viewport, vtkActor2D* actor);

  // Description:
  // Release any graphics resources that are being consumed by this mapper.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

protected:
  vtkOpenGLPolyDataMapper2D();
  ~vtkOpenGLPolyDataMapper2D();

  // Description:
  // Does the shader source need to be recomputed
  virtual bool GetNeedToRebuildShader(vtkOpenGLHelper &cellBO, vtkViewport *ren, vtkActor2D *act);

  // Description:
  // Build the shader source code
  virtual void BuildShader(std::string &VertexCode,
                           std::string &fragmentCode,
                           std::string &geometryCode,
                           vtkViewport *ren, vtkActor2D *act);

  // Description:
  // Determine what shader to use and compile/link it
  virtual void UpdateShader(vtkOpenGLHelper &cellBO, vtkViewport *viewport, vtkActor2D *act);

  // Description:
  // Set the shader parameteres related to the mapper/input data, called by UpdateShader
  virtual void SetMapperShaderParameters(vtkOpenGLHelper &cellBO, vtkViewport *ren, vtkActor2D *act);


    // Description:
  // Set the shader parameteres related to the Camera
  void SetCameraShaderParameters(vtkOpenGLHelper &cellBO, vtkViewport *viewport, vtkActor2D *act);

  // Description:
  // Set the shader parameteres related to the property
  void SetPropertyShaderParameters(vtkOpenGLHelper &cellBO, vtkViewport *viewport, vtkActor2D *act);

  // Description:
  // Update the scene when necessary.
  void UpdateVBO(vtkActor2D *act, vtkViewport *viewport);

  // The VBO and its layout.
  vtkOpenGLVertexBufferObject *VBO;

  // Structures for the various cell types we render.
  vtkOpenGLHelper Points;
  vtkOpenGLHelper Lines;
  vtkOpenGLHelper Tris;
  vtkOpenGLHelper TriStrips;

  vtkTextureObject *CellScalarTexture;
  vtkOpenGLBufferObject *CellScalarBuffer;
  bool HaveCellScalars;
  int PrimitiveIDOffset;

  vtkTimeStamp VBOUpdateTime; // When was the VBO updated?
  vtkPoints *TransformedPoints;

private:
  vtkOpenGLPolyDataMapper2D(const vtkOpenGLPolyDataMapper2D&);  // Not implemented.
  void operator=(const vtkOpenGLPolyDataMapper2D&);  // Not implemented.
};

#endif
