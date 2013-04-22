vtk_module(vtkFiltersGeneric
  GROUPS
    StandAlone
  DEPENDS
    vtkFiltersCore
    vtkFiltersSources
  TEST_DEPENDS
    vtkIOXML
    vtkIOLegacy
    vtkRenderingOpenGL
    vtkRenderingFreeTypeOpenGL
    vtkFiltersModeling
    vtkRenderingLabel
    vtkTestingRendering
    vtkInteractionStyle
    vtkTestingGenericBridge)
