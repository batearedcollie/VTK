vtk_module(vtkUtilitiesBenchmarks
  EXCLUDE_FROM_WRAPPING
  DEPENDS
    vtkCommonComputationalGeometry
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonSystem
    vtkCommonTransforms
    vtkDomainsChemistry
    vtkFiltersCore
    vtkFiltersSources
    vtkImagingCore
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkRenderingContext${VTK_RENDERING_BACKEND}
    vtkRenderingCore
    vtkRenderingVolume
    vtkRenderingVolume${VTK_RENDERING_BACKEND}
    vtksys
  PRIVATE_DEPENDS
    vtkChartsCore
    vtkIOCore
    vtkRenderingContext2D
    vtkViewsContext2D
)