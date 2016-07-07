/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpaquePass.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOpaquePass - Render the opaque geometry with property key
// filtering.
// .SECTION Description
// vtkOpaquePass renders the opaque geometry of all the props that have the
// keys contained in vtkRenderState.
//
// This pass expects an initialized depth buffer and color buffer.
// Initialized buffers means they have been cleared with farest z-value and
// background color/gradient/transparent color.
//
// .SECTION See Also
// vtkRenderPass vtkDefaultPass

#ifndef vtkOpaquePass_h
#define vtkOpaquePass_h

#include "vtkRenderingOpenGLModule.h" // For export macro
#include "vtkDefaultPass.h"

class VTKRENDERINGOPENGL_EXPORT vtkOpaquePass : public vtkDefaultPass
{
public:
  static vtkOpaquePass *New();
  vtkTypeMacro(vtkOpaquePass,vtkDefaultPass);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Perform rendering according to a render state \p s.
  // \pre s_exists: s!=0
  virtual void Render(const vtkRenderState *s);

 protected:
  // Description:
  // Default constructor.
  vtkOpaquePass();

  // Description:
  // Destructor.
  virtual ~vtkOpaquePass();

 private:
  vtkOpaquePass(const vtkOpaquePass&);  // Not implemented.
  void operator=(const vtkOpaquePass&);  // Not implemented.
};

#endif
