vtk_module(vtkFiltersCore
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonExecutionModel
    vtkCommonSystem
    vtkCommonMisc
    vtkCommonTransforms
    vtkCommonMath
  TEST_DEPENDS
    vtkTestingRendering
    vtkInteractionStyle
    vtkIOLegacy
    vtkIOXML
    vtkImagingCore
    vtkFiltersGeneral
    vtkRenderingOpenGL2
  )
