vtk_module(vtkWebGLExporter
  GROUPS
    Web
  DEPENDS
    vtkIOExport
    vtkInteractionWidgets
    vtkRenderingCore
    vtksys
  COMPILE_DEPENDS
    vtkUtilitiesEncodeString
)
