vtk_module(vtkIOParallel
  GROUPS
    StandAlone
  DEPENDS
    vtkParallelCore
    vtkFiltersParallel
    vtkIONetCDF
    vtkIOXML
    vtkIOImage
  PRIVATE_DEPENDS
    vtkexodusII
    vtknetcdf
  TEST_DEPENDS
    vtkParallelMPI
    vtkRenderingParallel
    vtkTestingCore
    vtkTestingRendering
  KIT
    vtkParallel
  )
