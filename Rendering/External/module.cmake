vtk_module(vtkRenderingExternal
  DEPENDS
    vtkRenderingCore
    vtkRendering${VTK_RENDERING_BACKEND}
  TEST_DEPENDS
    vtkglew
    vtkTestingRendering
  EXCLUDE_FROM_ALL
  )
