vtk_module(vtkCommonComputationalGeometry
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkTestingCore
    vtkRenderingOpenGL2
  KIT
    vtkCommon
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
  )
