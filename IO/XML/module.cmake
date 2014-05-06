vtk_module(vtkIOXML
  GROUPS
    StandAlone
  DEPENDS
    vtkIOXMLParser
    vtkIOGeometry
  PRIVATE_DEPENDS
    vtksys
  TEST_DEPENDS
    vtkFiltersAMR
    vtkFiltersCore
    vtkFiltersHyperTree
    vtkFiltersSources
    vtkImagingSources
    vtkInfovisCore
    vtkIOLegacy
    vtkRenderingOpenGL2
    vtkTestingCore
    vtkTestingRendering
    vtkInteractionStyle
  )
