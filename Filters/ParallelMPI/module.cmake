vtk_module(vtkFiltersParallelMPI
  GROUPS
    MPI
  DEPENDS
    vtkFiltersExtraction
    vtkFiltersGeneral
    vtkFiltersParallel
    vtkImagingCore
    vtkParallelCore
    vtkParallelMPI
  TEST_DEPENDS
    vtkIOXML
    vtkIOParallelXML
    vtkCommonDataModel
    vtkTestingCore
  KIT
    vtkParallel
  )
