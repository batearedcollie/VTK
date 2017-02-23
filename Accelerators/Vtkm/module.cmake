
vtk_module(vtkAcceleratorsVTKm
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkFiltersGeneral
    vtkFiltersGeometry
  TEST_DEPENDS
    vtkTestingCore
    vtkTestingRendering
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkRenderingVolume${VTK_RENDERING_BACKEND}
    vtkIOLegacy
    vtkIOXML
    vtkImagingHybrid
    vtkImagingSources
  EXCLUDE_FROM_ALL
  )
