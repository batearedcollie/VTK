vtk_module(vtkIOMPIParallel
  IMPLEMENTS
    vtkIOGeometry
  GROUPS
    MPI
  DEPENDS
    vtkIOGeometry
    vtkParallelMPI
  PRIVATE_DEPENDS
    vtksys
  TEST_DEPENDS
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkTestingRendering
    vtkInteractionStyle
  )
