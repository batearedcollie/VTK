vtk_module(vtkIOImport
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkTestingRendering
    vtkImagingSources
    vtkInteractionStyle
    vtkIOXML
  DEPENDS
    vtkCommonCore
    vtkCommonExecutionModel
    vtkCommonMisc
    vtkRenderingCore
    vtksys
  PRIVATE_DEPENDS
    vtkCommonDataModel
    vtkCommonTransforms
    vtkFiltersCore
    vtkFiltersSources
    vtkIOImage
  )