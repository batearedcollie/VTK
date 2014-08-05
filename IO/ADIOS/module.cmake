vtk_module(vtkIOADIOS
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkParallelCore
  PRIVATE_DEPENDS
    vtkParallelMPI
  TEST_DEPENDS
    vtkTestingCore
    vtkTestingRendering
  EXCLUDE_FROM_ALL
  )
