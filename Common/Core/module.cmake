vtk_module(vtkCommonCore
  GROUPS
    StandAlone
  DEPENDS
    vtkkwiml
  PRIVATE_DEPENDS
    vtksys
  TEST_DEPENDS
    vtkTestingCore
    vtkCommonSystem
    vtkCommonTransforms
    vtksys
  KIT
    vtkCommon
  )
