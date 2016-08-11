vtk_module(vtkFiltersFlowPaths
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkFiltersAMR
    vtkFiltersImaging
    vtkTestingCore
    vtkTestingRendering
    vtkInteractionStyle
    vtkRendering${VTK_RENDERING_BACKEND}
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkCommonMath
  PRIVATE_DEPENDS
    vtkFiltersCore
    vtkFiltersSources
    vtkIOCore
  )