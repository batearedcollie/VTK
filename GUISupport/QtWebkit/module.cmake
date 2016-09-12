vtk_module(vtkGUISupportQtWebkit
  TEST_DEPENDS
    vtkTestingCore
  EXCLUDE_FROM_WRAPPING
  DEPENDS
    vtkViewsQt
  PRIVATE_DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkFiltersExtraction
    vtkFiltersGeneral
    vtkFiltersSources
    vtkInfovisCore
    vtkViewsCore
  )