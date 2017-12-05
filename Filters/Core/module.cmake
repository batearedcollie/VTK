vtk_module(vtkFiltersCore
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkTestingRendering
    vtkInteractionStyle
    vtkIOLegacy
    vtkIOXML
    vtkIOExodus
    vtkImagingCore
    vtkFiltersGeneral
    vtkRenderingOpenGL2
    vtkRenderingVolumeOpenGL2
  KIT
    vtkFilters
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkCommonMisc
  PRIVATE_DEPENDS
    vtkCommonMath
    vtkCommonSystem
    vtkCommonTransforms
  )
