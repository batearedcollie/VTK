vtk_module(vtkIOLSDyna
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonExecutionModel
    vtkIOXML
  PRIVATE_DEPENDS
    vtksys
  TEST_DEPENDS
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkTestingRendering
    vtkInteractionStyle
  )
