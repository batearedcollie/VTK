vtk_module(vtkInfovisLayout
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkRenderingLabel
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkTestingRendering
    vtkInteractionStyle
    vtkIOInfovis
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonExecutionModel
  PRIVATE_DEPENDS
    vtkCommonComputationalGeometry
    vtkCommonSystem
    vtkCommonTransforms
    vtkFiltersCore
    vtkFiltersGeneral
    vtkFiltersModeling
    vtkFiltersSources
    vtkImagingHybrid
    vtkInfovisCore
  )