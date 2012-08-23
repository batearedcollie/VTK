vtk_module(vtkInfovisLayout
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonExecutionModel
    vtkImagingHybrid
    vtkFiltersModeling
    vtkInfovisCore
  TEST_DEPENDS
    vtkRenderingLabel
    vtkRenderingOpenGL
    vtkTestingRendering
    vtkIOInfovis
    vtkViewsContext2D
  )
