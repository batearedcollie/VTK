vtk_module(vtkRenderingOpenGL2
  DEPENDS
    vtkCommonExecutionModel
    vtkRenderingOpenGL
  IMPLEMENTS
    vtkRenderingCore
  PRIVATE_DEPENDS
    vtksys
    vtkRenderingOpenGL
  COMPILE_DEPENDS
    vtkUtilitiesEncodeString
  TEST_DEPENDS
    vtkTestingCore
    vtkTestingRendering
    vtkInteractionStyle
    vtkIOPLY
  EXCLUDE_FROM_WRAPPING
  )
