###############################################################################
#
# CMake build scripts for LibKEA
# 
# Created 2012/07/02 by Peter Bunting
#
# These scripts were initial based on those used for spdlib (http://www.spdlib.org) 
# which were derived from those used by libLAS (http://liblas.org/) 
#
# History
# 2012/07/02 - Created by Peter Bunting
#
###############################################################################

###############################################################################
# Set Project name and version
project (LIBKEA)

if (WIN32)
  set (WINDOWS ${WIN32})
endif()

set (PROJECT_BINARY_DIR bin)
set (PROJECT_LIBRARY_DIR lib)
set (PROJECT_SOURCE_DIR src)
set (PROJECT_TEST_DIR tests)
set (PROJECT_HEADER_DIR include)
set (PROJECT_TOOLS_DIR tools)
set (PROJECT_GDAL_DIR gdal)

# The version number.
set (LIBKEA_HG_VERSION 268)
set (LIBKEA_VERSION_MAJOR 1)
set (LIBKEA_VERSION_MINOR 4)
set (LIBKEA_VERSION_PATCH 6)
set (LIBKEA_VERSION "${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_VERSION "${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_STRING "LibKEA ${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_BUGREPORT "petebunting@mac.com")
set (LIBKEA_PACKAGE "LibKEA")
set (LIBKEA_COPYRIGHT_YEAR 2013)

if(MSVC)
    # Name of C++ library
    set(LIBKEA_LIB_NAME libkea)
else()
    # Name of C++ library
    set(LIBKEA_LIB_NAME kea)
endif()

option (BUILD_SHARED_LIBS "Build with shared library" ON)

set(HDF5_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for HDF5")
set(HDF5_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for HDF5")

set(GDAL_INCLUDE_DIR "NOTFOUND" CACHE PATH "Include PATH for GDAL")
set(GDAL_LIB_PATH "NOTFOUND" CACHE PATH "Library PATH for GDAL")

set(HDF5_STATIC_LIBS FALSE CACHE BOOL "On Windows, link against static HDF5 libs")

IF(GDAL_INCLUDE_DIR AND GDAL_LIB_PATH)
    # by default, only build if paths are set
    set(LIBKEA_WITH_GDAL TRUE CACHE BOOL "Choose if .kea GDAL driver should be built")
ELSE()
    set(LIBKEA_WITH_GDAL FALSE CACHE BOOL "Choose if .kea GDAL driver should be built")
ENDIF()

###############################################################################

###############################################################################
# CMake settings
cmake_minimum_required(VERSION 2.8.10)

IF(NOT CMAKE_BUILD_TYPE)
  #SET(CMAKE_BUILD_TYPE "DEBUG")
  SET(CMAKE_BUILD_TYPE "RELEASE")
  #SET(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
  #SET(CMAKE_BUILD_TYPE "MINSIZEREL")
ENDIF()

set(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_MACOSX_RPATH 1)

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CMAKE_VERBOSE_MAKEFILE)

# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ./cmake/modules/)
###############################################################################

###############################################################################
# Platform and compiler specific settings

if(MSVC)
        add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        add_definitions(-D_CRT_NONSTDC_NO_WARNING)
        add_definitions(-D_SCL_SECURE_NO_WARNINGS)
        
        # by default the compiler produces gratuitous warnings. Disable some of them
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4290 /wd4005 /wd4101 /wd4244 /wd4800 /wd4251 /wd4996")

        # Generate dot-user file with user-specific settings for Visual Studio project
        set(MSVC_ENVIRONMENT_PATH "" CACHE STRING
            "Custom PATH for Environment property in Visual Studio project configuration")
        mark_as_advanced(MSVC_ENVIRONMENT_PATH)
        set(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG "${ENVIRONMENT_PATH}")
        mark_as_advanced(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG)

else()
  # Recommended C++ compilation flags
  # -Weffc++
  set(LIBKEA_COMMON_CXX_FLAGS
	"-Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wredundant-decls -Wno-long-long -DNDEBUG")
	#"-pedantic -ansi -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wfloat-equal -Wredundant-decls -Wno-long-long")

  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${LIBKEA_COMMON_CXX_FLAGS}")
    if (CMAKE_COMPILER_IS_GNUCXX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98")
    endif()

  elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER}" MATCHES "clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBKEA_COMMON_CXX_FLAGS}")
  endif()
endif(MSVC)

if (APPLE)
  set(SO_EXT dylib)
 # set(CMAKE_FIND_FRAMEWORK "LAST")
elseif(WIN32)
  set(SO_EXT dll)
else()
  set(SO_EXT so)
endif(APPLE)
###############################################################################

###############################################################################
# Check the required libraries are present

include_directories(${HDF5_INCLUDE_DIR})
if (MSVC)
  if(HDF5_STATIC_LIBS)
    set(HDF5_LIBRARIES -LIBPATH:"${HDF5_LIB_PATH}" hdf5_cpp.lib hdf5.lib zlibstatic.lib)
  else()
    set(HDF5_LIBRARIES -LIBPATH:"${HDF5_LIB_PATH}" hdf5_cpp.lib hdf5.lib)
    set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS H5_BUILT_AS_DYNAMIC_LIB)
  endif()
else()
  set(HDF5_LIBRARIES "-L${HDF5_LIB_PATH} -lhdf5 -lhdf5_hl -lhdf5_cpp")
endif(MSVC)

if (MSVC)
  set(GDAL_LIBRARIES -LIBPATH:"${GDAL_LIB_PATH}" gdal_i.lib)
else()
  set(GDAL_LIBRARIES -L${GDAL_LIB_PATH} -lgdal)
endif(MSVC)
###############################################################################

###############################################################################
# Setup configure file
configure_file ( "${PROJECT_HEADER_DIR}/kea-config.h.in" "${PROJECT_HEADER_DIR}/libkea/kea-config.h" )
configure_file ( "${PROJECT_TOOLS_DIR}/kea-config.in" "${PROJECT_BINARY_DIR}/kea-config" )
###############################################################################

###############################################################################
# Documentation
file(READ "doc/index.txt" README )
file(WRITE "README.txt" "${README}")
###############################################################################

###############################################################################
# Build library

include_directories ("${PROJECT_HEADER_DIR}")
add_subdirectory ("${PROJECT_SOURCE_DIR}")
if (LIBKEA_WITH_GDAL)
	add_subdirectory ("${PROJECT_GDAL_DIR}")
endif(LIBKEA_WITH_GDAL)
###############################################################################

###############################################################################
# Tests
enable_testing()
add_test(NAME test1 COMMAND src/test1)

###############################################################################

###############################################################################
# Installation

install (FILES "${PROJECT_BINARY_DIR}/kea-config" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
###############################################################################

###############################################################################
# Create Distribution
SET(CPACK_SOURCE_GENERATOR "TGZ;ZIP;TBZ2")
SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")

set(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")

# Set files to ignore
list(APPEND CPACK_SOURCE_IGNORE_FILES "_CPack_Packages")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".gz")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".bz2")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".zip")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".svn")
list(APPEND CPACK_SOURCE_IGNORE_FILES "README")
list(APPEND CPACK_SOURCE_IGNORE_FILES "HOWTORELEASE.txt")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeCache.txt")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CPackConfig.cmake")
list(APPEND CPACK_SOURCE_IGNORE_FILES "schemas")

include(CPack)

add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
###############################################################################
