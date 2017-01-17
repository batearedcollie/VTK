vtk_module(vtkRenderingContext2D
  TCL_NAME vtkRenderingContextIID
  GROUPS
    Rendering
  TEST_DEPENDS
    vtkRenderingContext${VTK_RENDERING_BACKEND}
    vtkTestingRendering
    vtkViewsContext2D
    vtkChartsCore
  KIT
    vtkRendering
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkRenderingCore
  PRIVATE_DEPENDS
    vtkCommonMath
    vtkCommonTransforms
    vtkFiltersGeneral
    vtkRenderingFreeType
  )