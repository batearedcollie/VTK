vtk_module(vtkFiltersParallelImaging
  GROUPS
    StandAlone
  KIT
    vtkParallel
  DEPENDS
    vtkCommonExecutionModel
    vtkFiltersImaging
    vtkFiltersParallel
    vtkImagingCore
  PRIVATE_DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonSystem
    vtkFiltersExtraction
    vtkFiltersStatistics
    vtkImagingGeneral
    vtkParallelCore
  )