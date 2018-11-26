vtk_module(vtkIOWeb
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkRenderingOpenGL2
    vtkTestingRendering
    vtkInteractionStyle
  KIT
    vtkIO
  DEPENDS
    vtkCommonCore
    vtkIOCore
    vtkIOExport
  PRIVATE_DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonMisc
    vtksys
)
