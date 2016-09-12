vtk_module(vtkRenderingOpenGL
  IMPLEMENTS
    vtkRenderingCore
  BACKEND
    OpenGL
  IMPLEMENTATION_REQUIRED_BY_BACKEND
  COMPILE_DEPENDS
    vtkParseOGLExt
    vtkUtilitiesEncodeString
  TEST_DEPENDS
    vtkInteractionStyle
    vtkTestingRendering
    vtkIOExport
    vtkIOLegacy
    vtkIOXML
    vtkRenderingImage
    vtkRenderingLOD
    vtkImagingGeneral
    vtkImagingSources
    vtkFiltersProgrammable
    vtkFiltersSources
    vtkRenderingAnnotation
  KIT
    vtkOpenGL
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkFiltersCore
    vtkRenderingCore
  PRIVATE_DEPENDS
    vtkCommonExecutionModel
    vtkCommonMath
    vtkCommonSystem
    vtkCommonTransforms
    vtkIOImage
    vtkImagingCore
    vtkImagingHybrid
    vtksys
  )