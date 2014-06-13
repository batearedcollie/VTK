vtk_module(vtkFiltersHybrid
  GROUPS
    StandAlone
  DEPENDS
    vtkImagingSources
    vtkFiltersGeneral
    vtkRenderingCore # For vtkCamera in vtkDepthSortPolyData
  TEST_DEPENDS
    vtkIOXML
    vtkRenderingOpenGL2
    vtkImagingCore
    vtkImagingStencil
    vtkTestingRendering
    vtkInteractionStyle
    vtkIOLegacy
  )
