vtk_module(vtkWebPython
  GROUPS
    Web
  EXCLUDE_FROM_WRAPPING
  DEPENDS
    AutobahnPython
  COMPILE_DEPENDS
    vtkPython
  OPTIONAL_PYTHON_LINK
  EXCLUDE_FROM_ALL)