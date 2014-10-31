/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkRenderStepsPass.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkRenderStepsPass - Execute render passes sequentially.
// .SECTION Description
// vtkRenderStepsPass executes a standard list of render passes sequentially.
// This class allows to define a sequence of render passes at run time.
// You can set a step to NULL in order to skip that step. Likewise you
// can replace any of the default steps with your own step. Typically in
// such a case you would get the current step, replace it with your own
// and likely have your step call the current step as a delegate. For example
// to replace the translucent step with a depthpeeling step you would get the
// current tranlucent step and set it as a delegate on the depthpeeling step.
// Then set this classes translparent step to the depthpeelnig step.
//
// .SECTION See Also
// vtkRenderPass

#ifndef __vtkRenderStepsPass_h
#define __vtkRenderStepsPass_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkRenderPass.h"

class vtkSequencePass;
class vtkCameraPass;

class VTKRENDERINGOPENGL2_EXPORT vtkRenderStepsPass : public vtkRenderPass
{
public:
  static vtkRenderStepsPass *New();
  vtkTypeMacro(vtkRenderStepsPass,vtkRenderPass);
  void PrintSelf(ostream& os, vtkIndent indent);

  //BTX
  // Description:
  // Perform rendering according to a render state \p s.
  // \pre s_exists: s!=0
  virtual void Render(const vtkRenderState *s);
  //ETX

  // Description:
  // Release graphics resources and ask components to release their own
  // resources.
  // \pre w_exists: w!=0
  virtual void ReleaseGraphicsResources(vtkWindow *w);

  // Description:
  // Get the RenderPass used for the Camera Step
  vtkGetObjectMacro(CameraPass, vtkCameraPass);
  void SetCameraPass(vtkCameraPass *);

  // Description:
  // Get the RenderPass used for the Lights Step
  vtkGetObjectMacro(LightsPass, vtkRenderPass);
  void SetLightsPass(vtkRenderPass *);

  // Description:
  // Get the RenderPass used for the Opaque Step
  vtkGetObjectMacro(OpaquePass, vtkRenderPass);
  void SetOpaquePass(vtkRenderPass *);

  // Description:
  // Get the RenderPass used for the translucent Step
  vtkGetObjectMacro(TranslucentPass, vtkRenderPass);
  void SetTranslucentPass(vtkRenderPass *);

  // Description:
  // Get the RenderPass used for the Volume Step
  vtkGetObjectMacro(VolumetricPass, vtkRenderPass);
  void SetVolumetricPass(vtkRenderPass *);

  // Description:
  // Get the RenderPass used for the Overlay Step
  vtkGetObjectMacro(OverlayPass, vtkRenderPass);
  void SetOverlayPass(vtkRenderPass *);

  // Description:
  // Get the RenderPass used for the PostProcess Step
  vtkGetObjectMacro(PostProcessPass, vtkRenderPass);
  void SetPostProcessPass(vtkRenderPass *);

protected:
  vtkRenderStepsPass();
  virtual ~vtkRenderStepsPass();

  vtkCameraPass *CameraPass;
  vtkRenderPass *LightsPass;
  vtkRenderPass *OpaquePass;
  vtkRenderPass *TranslucentPass;
  vtkRenderPass *VolumetricPass;
  vtkRenderPass *OverlayPass;
  vtkRenderPass *PostProcessPass;
  vtkSequencePass *SequencePass;

private:
  vtkRenderStepsPass(const vtkRenderStepsPass&);  // Not implemented.
  void operator=(const vtkRenderStepsPass&);  // Not implemented.
};

#endif
