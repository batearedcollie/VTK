vtk_module(vtkIOGeometry
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonDataModel
    vtkCommonSystem
    vtkCommonMisc
    vtkIOCore
    vtkzlib
  COMPILE_DEPENDS
    vtkUtilitiesMaterialLibrary
  TEST_DEPENDS
    vtkIOAMR
    vtkFiltersGeometry
    vtkRenderingOpenGL
    vtkTestingRendering
  )
