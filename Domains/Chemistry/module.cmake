vtk_module(vtkDomainsChemistry
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonDataModel
    vtkRenderingCore
  PRIVATE_DEPENDS
    vtkIOXML
    vtkFiltersSources
  TEST_DEPENDS
    vtkTestingCore
    vtkTestingRendering
    vtkInteractionStyle
    vtkRenderingOpenGL2
  )
