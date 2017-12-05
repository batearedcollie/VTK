vtk_module(vtkRenderingVolume
  GROUPS
    Rendering
  TEST_DEPENDS
    vtkFiltersModeling
    vtkTestingCore
    vtkTestingRendering
    vtkRenderingVolumeOpenGL2
    vtkRenderingFreeType
    vtkIOXML
    vtkImagingSources
    vtkImagingGeneral
    vtkImagingHybrid
    vtkInteractionStyle
    vtkIOLegacy
    vtkIOImage
  KIT
    vtkRendering
  DEPENDS
    vtkCommonCore
    vtkCommonExecutionModel
    vtkRenderingCore
  PRIVATE_DEPENDS
    vtkCommonDataModel
    vtkCommonMath
    vtkCommonMisc
    vtkCommonSystem
    vtkCommonTransforms
    vtkIOXML
    vtkImagingCore
  )
