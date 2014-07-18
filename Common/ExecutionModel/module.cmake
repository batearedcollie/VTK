vtk_module(vtkCommonExecutionModel
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonDataModel
  COMPILE_DEPENDS
    vtkCommonMisc
  TEST_DEPENDS
    vtkTestingCore
    vtkFiltersCore
    vtkIOCore
    vtkIOLegacy
  KIT
    vtkCommon
  )
