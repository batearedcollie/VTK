if(NOT VTK_LEGACY_REMOVE)
  vtk_module(vtkViewsGeovis
    GROUPS
      Views
    DEPENDS
      vtkViewsInfovis
    PRIVATE_DEPENDS
      vtkCommonCore
      vtkGeovisCore
      vtkInfovisLayout
      vtkInteractionStyle
      vtkRenderingCore
      vtkViewsCore
    )
endif()
