/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenGLRenderWindow.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkOpenGLRenderWindow
 * @brief   OpenGL rendering window
 *
 * vtkOpenGLRenderWindow is a concrete implementation of the abstract class
 * vtkRenderWindow. vtkOpenGLRenderer interfaces to the OpenGL graphics
 * library. Application programmers should normally use vtkRenderWindow
 * instead of the OpenGL specific version.
*/

#ifndef vtkOpenGLRenderWindow_h
#define vtkOpenGLRenderWindow_h

#include "vtkRenderingOpenGLModule.h" // For export macro
#include "vtkRenderWindow.h"
#include "vtkOpenGL.h" // Needed for GLuint.

class vtkIdList;
class vtkOpenGLExtensionManager;
class vtkOpenGLHardwareSupport;
class vtkTextureUnitManager;
class vtkStdString;

class VTKRENDERINGOPENGL_EXPORT vtkOpenGLRenderWindow : public vtkRenderWindow
{
public:
  vtkTypeMacro(vtkOpenGLRenderWindow, vtkRenderWindow);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set/Get the maximum number of multisamples
   */
  static void SetGlobalMaximumNumberOfMultiSamples(int val);
  static int  GetGlobalMaximumNumberOfMultiSamples();
  //@}

  /**
   * What rendering backend has the user requested
   */
  const char *GetRenderingBackend() override;

  //@{
  /**
   * Set/Get the pixel data of an image, transmitted as RGBRGB...
   */
  unsigned char *GetPixelData(int x,int y,int x2,int y2,int front,int right=0)
                             override;
  int GetPixelData(int x,int y,int x2,int y2, int front,
                   vtkUnsignedCharArray *data, int right=0) override;
  int SetPixelData(int x,int y,int x2,int y2,unsigned char *data,
                   int front, int right=0) override;
  int SetPixelData(int x,int y,int x2,int y2,
                   vtkUnsignedCharArray *data, int front, int right=0)
                  override;
  //@}

  //@{
  /**
   * Set/Get the pixel data of an image, transmitted as RGBARGBA...
   */
  float *GetRGBAPixelData(int x,int y,int x2,int y2,int front,int right=0)
                         override;
  int GetRGBAPixelData(int x,int y,int x2,int y2, int front,
                       vtkFloatArray* data, int right=0) override;
  int SetRGBAPixelData(int x,int y,int x2,int y2, float *data,
                       int front, int blend=0, int right=0) override;
  int SetRGBAPixelData(int x,int y,int x2,int y2, vtkFloatArray *data,
                       int front, int blend=0,int right=0) override;
  void ReleaseRGBAPixelData(float *data) override;
  unsigned char *GetRGBACharPixelData(int x,int y,int x2,int y2,
                                      int front, int right=0) override;
  int GetRGBACharPixelData(int x,int y,int x2,int y2, int front,
                           vtkUnsignedCharArray *data,int right=0) override;
  int SetRGBACharPixelData(int x, int y, int x2, int y2,
                           unsigned char *data, int front,
                           int blend=0,int right=0) override;
  int SetRGBACharPixelData(int x,int y,int x2,int y2,
                           vtkUnsignedCharArray *data, int front,
                           int blend=0, int right=0) override;
  //@}

  //@{
  /**
   * Set/Get the zbuffer data from an image
   */
  float *GetZbufferData( int x1, int y1, int x2, int y2 ) override;
  int GetZbufferData( int x1, int y1, int x2, int y2, float* z ) override;
  int GetZbufferData( int x1, int y1, int x2, int y2,
                              vtkFloatArray* z ) override;
  int SetZbufferData( int x1, int y1, int x2, int y2, float *buffer ) override;
  int SetZbufferData( int x1, int y1, int x2, int y2,
                              vtkFloatArray *buffer ) override;
  //@}

  /**
   * Register a texture name with this render window.
   */
  void RegisterTextureResource (GLuint id);

  /**
   * Get the size of the depth buffer.
   */
  int GetDepthBufferSize() override;

  /**
   * Get the size of the color buffer.
   * Returns 0 if not able to determine otherwise sets R G B and A into buffer.
   */
  int GetColorBufferSizes(int *rgba) override;

  //@{
  /**
   * Set the size of the window in screen coordinates in pixels.
   */
  void SetSize(int a[2]) override;
  void SetSize(int,int) override;
  //@}

  /**
   * Initialize OpenGL for this window.
   */
  virtual void OpenGLInit();

  // Initialize the state of OpenGL that VTK wants for this window
  virtual void OpenGLInitState();

  // Initialize VTK for rendering in a new OpenGL context
  virtual void OpenGLInitContext();

  /**
   * Return the OpenGL name of the back left buffer.
   * It is GL_BACK_LEFT if GL is bound to the window-system-provided
   * framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT if GL is bound to an
   * application-created framebuffer object (GPU-based offscreen rendering)
   * It is used by vtkOpenGLCamera.
   */
  unsigned int GetBackLeftBuffer();

  /**
   * Return the OpenGL name of the back right buffer.
   * It is GL_BACK_RIGHT if GL is bound to the window-system-provided
   * framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT+1 if GL is bound to an
   * application-created framebuffer object (GPU-based offscreen rendering)
   * It is used by vtkOpenGLCamera.
   */
  unsigned int GetBackRightBuffer();

  /**
   * Return the OpenGL name of the front left buffer.
   * It is GL_FRONT_LEFT if GL is bound to the window-system-provided
   * framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT if GL is bound to an
   * application-created framebuffer object (GPU-based offscreen rendering)
   * It is used by vtkOpenGLCamera.
   */
  unsigned int GetFrontLeftBuffer();

  /**
   * Return the OpenGL name of the front right buffer.
   * It is GL_FRONT_RIGHT if GL is bound to the window-system-provided
   * framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT+1 if GL is bound to an
   * application-created framebuffer object (GPU-based offscreen rendering)
   * It is used by vtkOpenGLCamera.
   */
  unsigned int GetFrontRightBuffer();

  /**
   * Return the OpenGL name of the back left buffer.
   * It is GL_BACK if GL is bound to the window-system-provided
   * framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT if GL is bound to an
   * application-created framebuffer object (GPU-based offscreen rendering)
   * It is used by vtkOpenGLCamera.
   */
  unsigned int GetBackBuffer();

  /**
   * Return the OpenGL name of the front left buffer.
   * It is GL_FRONT if GL is bound to the window-system-provided
   * framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT if GL is bound to an
   * application-created framebuffer object (GPU-based offscreen rendering)
   * It is used by vtkOpenGLCamera.
   */
  unsigned int GetFrontBuffer();

  /**
   * Get the time when the OpenGL context was created.
   */
  virtual vtkMTimeType GetContextCreationTime();

  /**
   * Returns the extension manager. A new one will be created if one hasn't
   * already been set up.
   */
  vtkOpenGLExtensionManager* GetExtensionManager();

  /**
   * Returns an Hardware Support object. A new one will be created if one
   * hasn't already been set up.
   */
  vtkOpenGLHardwareSupport* GetHardwareSupport();

  /**
   * Returns its texture unit manager object. A new one will be created if one
   * hasn't already been set up.
   */
  vtkTextureUnitManager *GetTextureUnitManager();

  /**
   * Block the thread until the actual rendering is finished().
   * Useful for measurement only.
   */
  void WaitForCompletion() override;

  //@{
  /**
   * Create and bind offscreen rendering buffers without destroying the current
   * OpenGL context. This allows to temporary switch to offscreen rendering
   * (ie. to make a screenshot even if the window is hidden).
   * Return if the creation was successful (1) or not (0).
   * Note: This function requires that the device supports OpenGL framebuffer extension.
   * The function has no effect if OffScreenRendering is ON.
   */
  int SetUseOffScreenBuffers(bool offScreen) override;
  bool GetUseOffScreenBuffers() override;
  //@}

protected:
  vtkOpenGLRenderWindow();
  ~vtkOpenGLRenderWindow() override;

  long OldMonitorSetting;
  vtkIdList *TextureResourceIds;

  int GetPixelData(int x, int y, int x2, int y2, int front, unsigned char* data, int right=0);
  int GetRGBAPixelData(int x, int y, int x2, int y2, int front, float* data, int right=0);
  int GetRGBACharPixelData(int x, int y, int x2, int y2, int front,
                           unsigned char* data, int right=0);

  /**
   * Create an offScreen window based on OpenGL framebuffer extension.
   * Return if the creation was successful or not.
   * \pre positive_width: width>0
   * \pre positive_height: height>0
   * \pre not_initialized: !OffScreenUseFrameBuffer
   * \post valid_result: (result==0 || result==1)
   * && (result implies OffScreenUseFrameBuffer)
   */
  int CreateHardwareOffScreenWindow(int width, int height);

  int CreateHardwareOffScreenBuffers(int width, int height, bool bind = false);
  void BindHardwareOffScreenBuffers();

  /**
   * Destroy an offscreen window based on OpenGL framebuffer extension.
   * \pre initialized: OffScreenUseFrameBuffer
   * \post destroyed: !OffScreenUseFrameBuffer
   */
  void DestroyHardwareOffScreenWindow();

  void UnbindHardwareOffScreenBuffers();
  void DestroyHardwareOffScreenBuffers();

  /**
   * Flag telling if a framebuffer-based offscreen is currently in use.
   */
  int OffScreenUseFrameBuffer;

  //@{
  /**
   * Variables used by the framebuffer-based offscreen method.
   */
  int NumberOfFrameBuffers;
  unsigned int TextureObjects[4]; // really GLuint
  unsigned int FrameBufferObject; // really GLuint
  unsigned int DepthRenderBufferObject; // really GLuint
  int HardwareBufferSize[2];
  bool HardwareOffScreenBuffersBind;
  //@}

  /**
   * Create a not-off-screen window.
   */
  virtual void CreateAWindow() = 0;

  /**
   * Destroy a not-off-screen window.
   */
  virtual void DestroyWindow() = 0;

  /**
   * Set the texture unit manager.
   */
  void SetTextureUnitManager(vtkTextureUnitManager *textureUnitManager);

  unsigned int BackLeftBuffer;
  unsigned int BackRightBuffer;
  unsigned int FrontLeftBuffer;
  unsigned int FrontRightBuffer;
  unsigned int FrontBuffer;
  unsigned int BackBuffer;

  /**
   * Flag telling if the context has been created here or was inherited.
   */
  int OwnContext;

  vtkTimeStamp ContextCreationTime;

  vtkTextureUnitManager *TextureUnitManager;

private:
  vtkOpenGLRenderWindow(const vtkOpenGLRenderWindow&) = delete;
  void operator=(const vtkOpenGLRenderWindow&) = delete;

  void SetExtensionManager(vtkOpenGLExtensionManager*);
  void SetHardwareSupport(vtkOpenGLHardwareSupport * renderWindow);

  vtkOpenGLExtensionManager* ExtensionManager;
  vtkOpenGLHardwareSupport* HardwareSupport;
};

#endif
