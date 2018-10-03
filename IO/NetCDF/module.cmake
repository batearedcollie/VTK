vtk_module(vtkIONetCDF
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkCommonExecutionModel
    vtkRenderingOpenGL2
    vtkTestingRendering
    vtkInteractionStyle
  KIT
    vtkIO
  DEPENDS
    vtkCommonCore
    vtkCommonExecutionModel
  PRIVATE_DEPENDS
    vtkCommonDataModel
    vtknetcdf
    vtksys
  )
