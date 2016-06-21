/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkLightsPass.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkLightsPass - Implement the lights render pass.
// .SECTION Description
// Render the lights.
//
// This pass expects an initialized camera.
// It disables all the lights, apply transformations for lights following the
// camera, and turn on the enables lights.
//
// .SECTION See Also
// vtkRenderPass

#ifndef vtkLightsPass_h
#define vtkLightsPass_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkRenderPass.h"

class vtkOpenGLRenderWindow;

class VTKRENDERINGOPENGL2_EXPORT vtkLightsPass : public vtkRenderPass
{
public:
  static vtkLightsPass *New();
  vtkTypeMacro(vtkLightsPass,vtkRenderPass);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Perform rendering according to a render state \p s.
  // \pre s_exists: s!=0
  virtual void Render(const vtkRenderState *s);

 protected:
  // Description:
  // Default constructor.
  vtkLightsPass();

  // Description:
  // Destructor.
  virtual ~vtkLightsPass();

 private:
  vtkLightsPass(const vtkLightsPass&);  // Not implemented.
  void operator=(const vtkLightsPass&) VTK_DELETE_FUNCTION;
};

#endif
