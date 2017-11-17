vtk_module(vtkFiltersProgrammable
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkRenderingOpenGL2
    vtkTestingRendering
    vtkInteractionStyle
  KIT
    vtkFilters
  DEPENDS
    vtkCommonExecutionModel
  PRIVATE_DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonTransforms
  )
