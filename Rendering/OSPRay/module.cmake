set(optional_test_depends)
if("${Module_vtkRenderingParallel}" AND "${Module_vtkParallelMPI}")
  set(optional_test_depends "vtkRenderingParallel;vtkParallelMPI")
endif()
vtk_module(vtkRenderingOSPRay
  IMPLEMENTS
    vtkRenderingVolume
  TEST_DEPENDS
    vtkFiltersTexture
    vtkInteractionStyle
    vtkIOGeometry
    vtkIOPLY
    vtkRenderingAnnotation
    vtkTestingCore
    vtkTestingRendering
    ${optional_test_depends}
    # Dependencies for volume rendering tests
    vtkRenderingVolume
    vtkRenderingVolume${VTK_RENDERING_BACKEND}
    vtkFiltersCore
    vtkFiltersHybrid
    vtkFiltersModeling
    vtkImagingSources
    vtkImagingGeneral
    vtkImagingHybrid
    vtkInteractionStyle
    vtkIOLegacy
    vtkRenderingFreeType
  EXCLUDE_FROM_ALL
  DEPENDS
    vtkCommonDataModel
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkRenderingCore
    vtkRenderingSceneGraph
  PRIVATE_DEPENDS
    vtkCommonCore
    vtkIOImage
    vtkIOLegacy
    vtkIOXML
    vtkjsoncpp
    vtksys
  )
