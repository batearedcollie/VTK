vtk_module(vtkFiltersExtraction
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkFiltersCore
    vtkFiltersGeneral
    vtkFiltersStatistics
  TEST_DEPENDS
    vtkIOLegacy
    vtkIOXML
    vtkIOExodus
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkTestingRendering
    vtkInteractionStyle
  KIT
    vtkFilters
  )
