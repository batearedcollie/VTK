vtk_module(vtkRenderingImage
  GROUPS
    Imaging
    Rendering
  TEST_DEPENDS
    vtkTestingCore
    vtkTestingRendering
    vtkRenderingOpenGL2
    vtkInteractionStyle
    vtkImagingSources
    vtkFiltersModeling
  KIT
    vtkRendering
  DEPENDS
    vtkCommonExecutionModel
    vtkRenderingCore
  PRIVATE_DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonMath
    vtkCommonTransforms
    vtkImagingCore
  )
