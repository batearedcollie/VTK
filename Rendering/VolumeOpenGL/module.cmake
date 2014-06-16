vtk_module(vtkRenderingVolumeOpenGL
  IMPLEMENTS
    vtkRenderingVolume
  BACKEND
    OpenGL
  DEPENDS
    vtkRenderingOpenGL
  PRIVATE_DEPENDS
    vtksys
    vtkFiltersGeneral
    vtkFiltersSources
  TEST_DEPENDS
    vtkTestingCore
    vtkTestingRendering
    vtkRenderingFreeType
    vtkImagingSources
    vtkImagingGeneral
    vtkInteractionStyle
  )
