vtk_module(vtkFiltersParallelStatistics
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonMath
    vtkCommonDataModel
    vtkCommonSystem
    vtkalglib
    vtkFiltersStatistics
    vtkParallelCore
  TEST_DEPENDS
    vtkParallelMPI
  )
