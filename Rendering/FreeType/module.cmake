# The tests can be built without matplotlib, but we add some additional tests
# if it is enabled.
set(optional_test_depends)
if(${Module_vtkRenderingMatplotlib})
  set(optional_test_depends "vtkRenderingMatplotlib")
endif()
vtk_module(vtkRenderingFreeType
  IMPLEMENTS
    vtkRenderingCore
  GROUPS
    Rendering
  TEST_DEPENDS
    ${optional_test_depends}
    vtkTestingRendering
    vtkViewsContext2D
    vtkRenderingOpenGL2
    vtkRenderingContextOpenGL2
  KIT
    vtkRendering
  DEPENDS
    vtkCommonCore
    vtkCommonExecutionModel
    vtkRenderingCore
    vtkfreetype
  PRIVATE_DEPENDS
    vtkCommonDataModel
    vtkFiltersGeneral
  )
