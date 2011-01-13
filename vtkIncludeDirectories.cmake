#-----------------------------------------------------------------------------
# Include directories for other projects installed on the system.
SET(VTK_INCLUDE_DIRS_SYSTEM "")
IF(VTK_USE_RENDERING)
  # OpenGL include directories.
  IF(APPLE)
    IF(VTK_USE_X)
       SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM}
           ${OPENGL_INCLUDE_DIR})
    ENDIF(VTK_USE_X)
  ELSE(APPLE)
       SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM}
           ${OPENGL_INCLUDE_DIR})
  ENDIF(APPLE)

  IF(VTK_USE_X)
    # X include directories.
    SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM} ${X11_INCLUDE_DIR})
  ENDIF(VTK_USE_X)

  IF(VTK_USE_MANGLED_MESA)
    # Mangled Mesa include directory.
    SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM}
        ${MESA_INCLUDE_PATH})
  ELSE(VTK_USE_MANGLED_MESA)
    # Off-Screen Mesa include directory.
    IF(VTK_OPENGL_HAS_OSMESA)
      IF(OSMESA_INCLUDE_DIR)
        SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM}
            ${OSMESA_INCLUDE_DIR})
      ENDIF(OSMESA_INCLUDE_DIR)
    ENDIF(VTK_OPENGL_HAS_OSMESA)
  ENDIF(VTK_USE_MANGLED_MESA)
ENDIF(VTK_USE_RENDERING)

IF(VTK_INCLUDE_NEED_TCL)
  SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM} ${TCL_INCLUDE_PATH})
ENDIF(VTK_INCLUDE_NEED_TCL)

# VTK_INCLUDE_NEED_TK is set in toplevel CMakeLists.txt file.
IF(VTK_INCLUDE_NEED_TK)
  # Tcl/Tk include directories.
  SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM} ${TK_INCLUDE_PATH})
  IF (WIN32)
    SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM} ${TK_XLIB_PATH})
  ENDIF (WIN32)
  
  # Need Tk internal headers for Tk initialization.
  SET (try_file "tkInt.h")
  SET (try_paths)
  IF (WIN32)
    SET (try_file "tkWinPort.h")
  ENDIF (WIN32)
  IF (APPLE)
    SET (try_file "tkMacOSXDefault.h")
    GET_FILENAME_COMPONENT(TK_INCLUDE_PATH_PARENT "${TK_INCLUDE_PATH}" PATH)
    SET (try_paths "${TK_INCLUDE_PATH_PARENT}/PrivateHeaders")
  ENDIF (APPLE)
  IF (try_file)
    VTK_GET_TCL_TK_VERSION ("TCL_TK_MAJOR_VERSION" "TCL_TK_MINOR_VERSION")
    SET (TCL_TK_VERSIOND "${TCL_TK_MAJOR_VERSION}.${TCL_TK_MINOR_VERSION}")
    SET (try_paths ${try_paths} 
         "${VTK_SOURCE_DIR}/Utilities/TclTk/internals/tk${TCL_TK_VERSIOND}")
    FIND_PATH(
       TK_INTERNAL_PATH 
       ${try_file} 
       PATHS ${try_paths}
       DOC "The path to the Tk internal headers (${try_file}).")
    MARK_AS_ADVANCED(TK_INTERNAL_PATH)
  ENDIF (try_file)
ENDIF(VTK_INCLUDE_NEED_TK)

IF(VTK_WRAP_JAVA)
  # Java include directories.
  SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM}
      ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} ${JAVA_AWT_INCLUDE_PATH})
ENDIF(VTK_WRAP_JAVA)

IF(VTK_USE_BOOST)
  # Boost include directories.
  SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM} ${Boost_INCLUDE_DIR})
ENDIF(VTK_USE_BOOST)

IF(VTK_USE_GNU_R)
  # Boost include directories.
  SET(VTK_INCLUDE_DIRS_SYSTEM ${VTK_INCLUDE_DIRS_SYSTEM} ${R_INCLUDE_DIR})
ENDIF(VTK_USE_GNU_R)

#-----------------------------------------------------------------------------
# Include directories from the build tree.
# ${VTK_BINARY_DIR}/Utilities is needed to access vtksys, which is prefixed
# (i.e. include "vtksys/DynamicLoader.hxx")
SET(VTK_INCLUDE_DIRS_BUILD_TREE
  ${VTK_BINARY_DIR}
  ${VTK_BINARY_DIR}/Common
  ${VTK_BINARY_DIR}/Utilities
  )

#-----------------------------------------------------------------------------
# Include directories from the source tree.
SET(VTK_INCLUDE_DIRS_SOURCE_TREE "")

IF(VTK_WRAP_PYTHON)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
    ${VTK_SOURCE_DIR}/Wrapping/Python)
ENDIF(VTK_WRAP_PYTHON)

IF(VTK_USE_INFOVIS)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE} ${VTK_SOURCE_DIR}/Infovis)
ENDIF(VTK_USE_INFOVIS)

IF(VTK_USE_TEXT_ANALYSIS)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE} ${VTK_SOURCE_DIR}/TextAnalysis)
ENDIF(VTK_USE_TEXT_ANALYSIS)

IF(VTK_USE_GEOVIS)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE} ${VTK_SOURCE_DIR}/Geovis)
ENDIF(VTK_USE_GEOVIS)

IF(VTK_USE_VIEWS)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE} ${VTK_SOURCE_DIR}/Views)
ENDIF(VTK_USE_VIEWS)

IF(VTK_USE_PARALLEL)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE} ${VTK_SOURCE_DIR}/Parallel)
ENDIF(VTK_USE_PARALLEL)

IF(VTK_USE_RENDERING)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE} ${VTK_SOURCE_DIR}/VolumeRendering)
  SET(VTK_INCLUDE_DIRS_BUILD_TREE ${VTK_INCLUDE_DIRS_BUILD_TREE} ${VTK_BINARY_DIR}/VolumeRendering)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE} ${VTK_SOURCE_DIR}/Hybrid)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE} ${VTK_SOURCE_DIR}/Widgets)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE} ${VTK_SOURCE_DIR}/Rendering)
  SET(VTK_INCLUDE_DIRS_BUILD_TREE ${VTK_INCLUDE_DIRS_BUILD_TREE} ${VTK_BINARY_DIR}/Rendering)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE} ${VTK_SOURCE_DIR}/Charts)
  SET(VTK_INCLUDE_DIRS_BUILD_TREE ${VTK_INCLUDE_DIRS_BUILD_TREE} ${VTK_BINARY_DIR}/Charts)
# Access to vtkRegressionTestImage.h.
SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
  ${VTK_SOURCE_DIR}/Rendering/Testing/Cxx
)
ENDIF(VTK_USE_RENDERING)

# These directories are always needed.
SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
  ${VTK_SOURCE_DIR}/IO
  ${VTK_SOURCE_DIR}/Imaging
  ${VTK_SOURCE_DIR}/Graphics
  ${VTK_SOURCE_DIR}/GenericFiltering
  ${VTK_SOURCE_DIR}/Filtering
  ${VTK_SOURCE_DIR}/Common
  ${VTK_SOURCE_DIR}/Utilities
)



# Access to vtkTestUtilities.h.
SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
  ${VTK_SOURCE_DIR}/Common/Testing/Cxx
)

#-----------------------------------------------------------------------------
# Include directories needed for .cxx files in VTK.  These include
# directories will NOT be available to user projects.
SET(VTK_INCLUDE_DIRS_BUILD_TREE_CXX
  ${VTK_SOURCE_DIR}/Utilities/ftgl/src
  ${VTK_BINARY_DIR}/Utilities/ftgl
)

IF(VTK_USE_TK)
  # Need access to internal Tk headers for the vtkTk... widget .cxx files.
  SET(VTK_INCLUDE_DIRS_BUILD_TREE_CXX ${VTK_INCLUDE_DIRS_BUILD_TREE_CXX}
      ${TK_INTERNAL_PATH})
      
  #-----------------------------------------------------------------------------
  # Configure the Tk library for vtkRendering.
  INCLUDE(${VTK_SOURCE_DIR}/Wrapping/Tcl/vtkDetermineTkResources.cmake)
    
ENDIF(VTK_USE_TK)

IF (VTK_USE_MATROX_IMAGING)
  # Need access to mil.h include file for vtkMILVideoSource.cxx.
  SET(VTK_INCLUDE_DIRS_BUILD_TREE_CXX ${VTK_INCLUDE_DIRS_BUILD_TREE_CXX}
      ${MIL_INCLUDE_PATH})
ENDIF (VTK_USE_MATROX_IMAGING)

#-----------------------------------------------------------------------------
# Include directories for 3rd-party utilities provided by VTK.
VTK_THIRD_PARTY_INCLUDE2(ZLIB)
VTK_THIRD_PARTY_INCLUDE2(JPEG)
VTK_THIRD_PARTY_INCLUDE2(PNG)
VTK_THIRD_PARTY_INCLUDE2(TIFF)
VTK_THIRD_PARTY_INCLUDE2(EXPAT)
VTK_THIRD_PARTY_INCLUDE2(LIBXML2)
IF(VTK_USE_GL2PS)
  VTK_THIRD_PARTY_INCLUDE2(GL2PS)
ENDIF(VTK_USE_GL2PS)
VTK_THIRD_PARTY_INCLUDE(LIBPROJ4 vtklibproj4)
VTK_THIRD_PARTY_INCLUDE(DICOMParser DICOMParser)
VTK_THIRD_PARTY_INCLUDE(FREETYPE vtkfreetype/include)
VTK_THIRD_PARTY_INCLUDE(NetCDF vtknetcdf)
VTK_THIRD_PARTY_INCLUDE(Exodus2 vtkexodus2/include)
VTK_THIRD_PARTY_INCLUDE(MATERIALLIBRARY MaterialLibrary)
VTK_THIRD_PARTY_INCLUDE(VERDICT verdict)
VTK_THIRD_PARTY_INCLUDE(HDF5 vtkhdf5)
VTK_THIRD_PARTY_INCLUDE(HDF5 vtkhdf5/src)
IF(VTK_USE_PARALLEL)
  VTK_THIRD_PARTY_INCLUDE(Cosmo Cosmo)
  VTK_THIRD_PARTY_INCLUDE(VPIC VPIC)
ENDIF(VTK_USE_PARALLEL)
IF(VTK_USE_OGGTHEORA_ENCODER)
  VTK_THIRD_PARTY_INCLUDE(OGGTHEORA vtkoggtheora/include)
ENDIF(VTK_USE_OGGTHEORA_ENCODER)

# Include UTF-8 support
SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
  ${VTK_SOURCE_DIR}/Utilities/utf8/source)

# Include GUI support
IF(VTK_USE_GUISUPPORT)
  IF(VTK_USE_QVTK)
    SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
      ${VTK_SOURCE_DIR}/GUISupport/Qt)
    SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
      ${VTK_BINARY_DIR}/GUISupport/Qt)
    SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
      ${VTK_SOURCE_DIR}/GUISupport/Qt/Chart)
    SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
      ${VTK_BINARY_DIR}/GUISupport/Qt/Chart)
  ENDIF(VTK_USE_QVTK)
  IF(VTK_USE_MFC)
    SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
      ${VTK_SOURCE_DIR}/GUISupport/MFC)
    SET(VTK_INCLUDE_DIRS_BUILD_TREE ${VTK_INCLUDE_DIRS_BUILD_TREE}
      ${VTK_BINARY_DIR}/GUISupport/MFC)
  ENDIF(VTK_USE_MFC)
ENDIF(VTK_USE_GUISUPPORT)

# Infovis support
IF(VTK_USE_INFOVIS)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
    ${VTK_SOURCE_DIR}/Infovis
    ${VTK_SOURCE_DIR}/Utilities/vtkalglib
    )
  SET(VTK_INCLUDE_DIRS_BUILD_TREE ${VTK_INCLUDE_DIRS_BUILD_TREE}
    ${VTK_BINARY_DIR}/Utilities/vtkalglib
    )
ENDIF(VTK_USE_INFOVIS)

# TextAnalysis support
IF(VTK_USE_TEXT_ANALYSIS)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
    ${VTK_SOURCE_DIR}/TextAnalysis
    )
ENDIF(VTK_USE_TEXT_ANALYSIS)

# Geovis support
IF(VTK_USE_GEOVIS)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
    ${VTK_SOURCE_DIR}/Geovis
    )
ENDIF(VTK_USE_GEOVIS)

# Views support
IF(VTK_USE_VIEWS)
  SET(VTK_INCLUDE_DIRS_SOURCE_TREE ${VTK_INCLUDE_DIRS_SOURCE_TREE}
    ${VTK_SOURCE_DIR}/Views
    )
ENDIF(VTK_USE_VIEWS)
