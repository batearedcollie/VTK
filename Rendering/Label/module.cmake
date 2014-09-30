vtk_module(vtkRenderingLabel
  GROUPS
    Rendering
  DEPENDS
    vtkRenderingFreeType
  PRIVATE_DEPENDS
    vtkFiltersExtraction
  TEST_DEPENDS
    vtkIOXML
    vtkTestingCore
    vtkTestingRendering
    vtkRenderingFreeType${VTK_RENDERING_BACKEND}
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkInteractionStyle
  KIT
    vtkRendering
  )
