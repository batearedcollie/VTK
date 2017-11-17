vtk_module(vtkImagingOpenGL2
  TCL_NAME vtkImagingOpenGLII
  BACKEND
    OpenGL2
  TEST_DEPENDS
    vtkTestingRendering
    vtkInteractionStyle
  DEPENDS
    vtkImagingGeneral
    vtkRenderingOpenGL2
  PRIVATE_DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkRenderingOpenGL2
)
