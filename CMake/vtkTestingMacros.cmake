# -----------------------------------------------------------------------------
# Usage: vtk_add_test_mpi(name [TESTING_DATA])
macro (vtk_add_test_mpi name)
  get_filename_component(TName ${name} NAME_WE)
  set(argn "${ARGN}")
  set(data_dir "")
  set(test_extra "")
  foreach(a IN LISTS argn)
    if("[${a}]" STREQUAL "[TESTING_DATA]")
      set(data_dir ${VTK_TEST_DATA_DIR})
    elseif("x${a}" MATCHES "\\.cxx$")
      list(APPEND test_extra ${a})
    else()
      message(FATAL_ERROR "Unknown argument \"${a}\"")
    endif()
  endforeach()

  if(data_dir)
    set(_D -D ${data_dir})
    set(_T -T ${VTK_BINARY_DIR}/Testing/Temporary)
    set(_V -V "DATA{${${vtk-module}_SOURCE_DIR}/Testing/Data/Baseline/${TName}.png,:}")
  else()
    set(_D "")
    set(_T "")
    set(_V "")
  endif()

  ExternalData_add_test(VTKData
    NAME ${vtk-module}Cxx-MPI-${TName}
    COMMAND ${VTK_MPIRUN_EXE}
    ${VTK_MPI_PRENUMPROC_FLAGS} ${VTK_MPI_NUMPROC_FLAG} ${VTK_MPI_MAX_NUMPROCS}
    ${VTK_MPI_PREFLAGS}
    $<TARGET_FILE:${TName}>
    ${_D} ${_T} ${_V}
    ${VTK_MPI_POSTFLAGS})

  vtk_module_test_executable(${TName} ${TName}.cxx ${test_extra})
endmacro()

# -----------------------------------------------------------------------------
# Usage: vtk_add_test_cxx([name.cxx[,-E<n>][,NO_VALID]]...
#          [NO_DATA] [NO_VALID])
# NO_DATA indicates that this test doesn't require any input data file
# NO_VALID indicates that this test doesn't compare results against baseline images
# NO_OUTPUT indicates that the test doesn't produce any output files
function(vtk_add_test_cxx)
  # Parse Command line args
  set(names "")
  set(no_data 0)
  set(no_valid 0)
  set(no_output 0)
  foreach(a IN LISTS ARGN)
    if("[${a}]" STREQUAL "[NO_DATA]")
      set(no_data 1)
    elseif("[${a}]" STREQUAL "[NO_VALID]")
      set(no_valid 1)
    elseif("[${a}]" STREQUAL "[NO_OUTPUT]")
      set(no_output 1)
    elseif("x${a}" MATCHES "^x([^.]*)\\.cxx,?(.*)$")
      set(name "${CMAKE_MATCH_1}")
      string(REPLACE "," ";" _${name}_OPTIONS "${CMAKE_MATCH_2}")
      list(APPEND names ${name})
    else()
      message(FATAL_ERROR "Unknown argument \"${a}\"")
    endif()
  endforeach()

  if(NOT no_data)
    set(_D -D ${VTK_TEST_DATA_DIR})
  else()
    set(_D "")
  endif()

  set(_T "")
  if(NOT no_output)
    set(_T -T ${VTK_TEST_OUTPUT_DIR})
  endif()

  if(vtk-module)
    set(prefix ${vtk-module})
    set(base_dir ${${vtk-module}_SOURCE_DIR}/Testing/Data/Baseline)
  elseif(vtk-example)
    set(prefix ${vtk-example})
    set(base_dir ${CMAKE_CURRENT_SOURCE_DIR}/Baseline)
  else()
    message(FATAL_ERROR "Neither vtk-module nor vtk-example is set!")
  endif()

  foreach(name ${names})
    set(_V "")
    set(_E "")
    set(tmp_no_valid "${no_valid}")
    foreach(opt IN LISTS _${name}_OPTIONS)
      if("x${opt}" MATCHES "^x-E([0-9]+)$")
        set(_E -E ${CMAKE_MATCH_1})
      elseif("[${opt}]" STREQUAL "[NO_VALID]")
        set(tmp_no_valid 1)
      else()
        message(FATAL_ERROR "Test ${name} has unknown option \"${opt}\"")
      endif()
    endforeach()
    if(NOT tmp_no_valid)
      set(_V -V "DATA{${base_dir}/${name}.png,:}")
    endif()
    ExternalData_add_test(VTKData
      NAME ${prefix}Cxx-${name}
      COMMAND ${prefix}CxxTests ${name} ${${name}_ARGS}
      ${_D} ${_T} ${_V} ${_E})
    set_property(DIRECTORY APPEND PROPERTY VTK_TEST_CXX_SOURCES ${name}.cxx)
  endforeach()
endfunction()

macro(vtk_test_cxx_executable exe_name)
  set(argn "${ARGN}")
  set(test_driver vtkTestDriver.h)
  set(test_extra "")
  foreach(a IN LISTS argn)
    if("[${a}]" STREQUAL "[RENDERING_FACTORY]")
      include(vtkTestingRenderingDriver)
      set(test_driver ${vtkTestingRendering_SOURCE_DIR}/vtkTestingObjectFactory.h)
    elseif("x${a}" MATCHES "\\.cxx$")
      list(APPEND test_extra ${a})
    else()
      message(FATAL_ERROR "Unknown argument \"${a}\"")
    endif()
  endforeach()
  get_property(vtk_test_cxx_sources DIRECTORY PROPERTY VTK_TEST_CXX_SOURCES)
  if(vtk-module)
    set(tmp_before_tm ${CMAKE_TESTDRIVER_BEFORE_TESTMAIN})
    set(CMAKE_TESTDRIVER_BEFORE_TESTMAIN
      "    vtksys::SystemInformation::SetStackTraceOnError(1);\n ${tmp_before_tm}")
  endif()
  create_test_sourcelist(Tests ${exe_name}.cxx ${vtk_test_cxx_sources}
    EXTRA_INCLUDE ${test_driver})
  if(vtk-module)
    set(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "${tmp_before_tm}")
    vtk_module_test_executable(${exe_name} ${Tests} ${test_extra})
  elseif(vtk-example)
    add_executable(${exe_name} ${Tests} ${test_extra})
    target_link_libraries(${exe_name} ${VTK_LIBRARIES})
  else()
    message(FATAL_ERROR "Neither vtk-module nor vtk-example is set!")
  endif()
endmacro()

# -----------------------------------------------------------------------------
# Usage: vtk_add_test_python(name [NO_RT] [NO_DATA] [NO_VALID])
# NO_RT means that your test won't use the image comparison helpers from vtk.test.testing
# NO_DATA indicates that this test doesn't require any input data file
# NO_VALID indicates that this test doesn't compare results against baseline images
# NO_OUTPUT indicates that the test doesn't produce any output files
function(vtk_add_test_python name)
  if(NOT VTK_PYTHON_EXE)
    message(FATAL_ERROR "VTK_PYTHON_EXE not set")
  endif()
  # Parse Command line args
  get_filename_component(TName ${name} NAME_WE)
  set(no_data 0)
  set(no_valid 0)
  set(no_output 0)
  set(no_rt 0)
  foreach(a IN LISTS ARGN)
    if("[${a}]" STREQUAL "[NO_DATA]")
      set(no_data 1)
    elseif("[${a}]" STREQUAL "[NO_VALID]")
      set(no_valid 1)
    elseif("[${a}]" STREQUAL "[NO_OUTPUT]")
      set(no_output 1)
    elseif("[${a}]" STREQUAL "[NO_RT]")
      set(no_rt 1)
    else()
      message(FATAL_ERROR "Unknown argument \"${a}\"")
    endif()
  endforeach()

  if(NOT no_data)
    set(_D -D ${VTK_TEST_DATA_DIR})
  else()
    set(_D "")
  endif()

  set(rtImageTest "")
  set(_B "")
  set(_V "")
  set(_T "")
  set(_A "")
  if(NOT no_valid)
    if(no_rt)
      set(_B -B "DATA{${${vtk-module}_SOURCE_DIR}/Testing/Data/Baseline/,REGEX:${TName}(_[0-9]+)?.png}")
    else()
      set(rtImageTest ${VTK_BINARY_DIR}/Utilities/vtkTclTest2Py/rtImageTest.py)
      set(_V -V "DATA{${${vtk-module}_SOURCE_DIR}/Testing/Data/Baseline/${TName}.png,:}")
      set(_A -A ${VTK_BINARY_DIR}/Utilities/vtkTclTest2Py)
    endif()
  endif()
  if(NOT no_output)
    set(_T -T ${VTK_TEST_OUTPUT_DIR})
  endif()

  ExternalData_add_test(VTKData
    NAME ${vtk-module}Python-${TName}
    COMMAND ${VTK_PYTHON_EXE} --enable-bt ${rtImageTest}
    ${CMAKE_CURRENT_SOURCE_DIR}/${TName}.py ${${TName}_ARGS}
    ${_D} ${_B} ${_T} ${_V} ${_A})
endfunction()

# -----------------------------------------------------------------------------
# Usage: vtk_add_test_tcl(name [NO_DATA] [NO_VALID] [NO_OUTPUT] [NO_RT])
# NO_RT means that your test won't use the image comparison helpers from rtImageTest.tcl
# NO_DATA indicates that this test doesn't require any input data file
# NO_VALID indicates that this test doesn't compare results against baseline images
# NO_OUTPUT indicates that the test doesn't produce any output files
function(vtk_add_test_tcl name)
  if(NOT VTK_TCL_EXE)
    message(FATAL_ERROR "VTK_TCL_EXE not set")
  endif()
  # Parse Command line args
  get_filename_component(TName ${name} NAME_WE)
  set(no_data 0)
  set(no_valid 0)
  set(no_output 0)
  set(no_rt 0)
  foreach(a IN LISTS ARGN)
    if("[${a}]" STREQUAL "[NO_DATA]")
      set(no_data 1)
    elseif("[${a}]" STREQUAL "[NO_VALID]")
      set(no_valid 1)
    elseif("[${a}]" STREQUAL "[NO_OUTPUT]")
      set(no_output 1)
    elseif("[${a}]" STREQUAL "[NO_RT]")
      set(no_rt 1)
    else()
      message(FATAL_ERROR "Unknown argument \"${a}\"")
    endif()
  endforeach()

  if(NOT no_data)
    set(_D -D ${VTK_TEST_DATA_DIR})
  elseif(no_rt)
    set(_D "")
  else()
    set(_D -D VTK_DATA_ROOT-NOTFOUND)
  endif()

  set(rtImageTest "")
  set(_V "")
  set(_T "")
  if(NOT no_rt)
    set(rtImageTest ${vtkTestingRendering_SOURCE_DIR}/rtImageTest.tcl)
    if(NOT no_valid)
      set(_V -V "DATA{${${vtk-module}_SOURCE_DIR}/Testing/Data/Baseline/${TName}.png,:}")
    endif()
    if(NOT no_output)
      set(_T -T ${VTK_TEST_OUTPUT_DIR})
    endif()
  endif()
  set(_A -A ${VTK_SOURCE_DIR}/Wrapping/Tcl)

  ExternalData_add_test(VTKData
    NAME ${vtk-module}Tcl-${TName}
    COMMAND ${VTK_TCL_EXE} ${rtImageTest}
    ${CMAKE_CURRENT_SOURCE_DIR}/${TName}.tcl ${${TName}_ARGS}
    ${_D} ${_T} ${_V} ${_A})
endfunction()
