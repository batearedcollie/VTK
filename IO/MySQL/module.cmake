vtk_module(vtkIOMySQL
  DEPENDS
    vtkCommonDataModel
  IMPLEMENTS
    vtkIOSQL
  PRIVATE_DEPENDS
    vtksys
  TEST_DEPENDS
    vtkTestingCore
    vtkTestingIOSQL
  )
