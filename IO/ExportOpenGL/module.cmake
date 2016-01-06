vtk_module(vtkIOExportOpenGL
  IMPLEMENTS
    vtkIOExport
  BACKEND
    OpenGL
  DEPENDS
    vtkCommonCore
    vtkIOExport
    vtkRenderingAnnotation
    vtkRenderingContext2D
    vtkRenderingCore
    vtkRenderingFreeType
    vtkRenderingGL2PS
    vtkRenderingLabel
    vtkRendering${VTK_RENDERING_BACKEND}
  PRIVATE_DEPENDS
    vtkgl2ps
  EXCLUDE_FROM_WRAPPING
)
