vtk_module(vtkFiltersParallelMPI
  GROUPS
    MPI
  DEPENDS
    vtkFiltersExtraction
    vtkFiltersGeneral
    vtkFiltersParallel
    vtkParallelCore
    vtkParallelMPI
  TEST_DEPENDS
    vtkIOXML
    vtkIOParallelXML
    vtkCommonDataModel
    vtkTestingCore
  )
