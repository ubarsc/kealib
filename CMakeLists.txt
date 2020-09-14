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
# 2018/01/25 - updated by Hiroshi Miura
# 2012/07/02 - Created by Peter Bunting
#
###############################################################################

###############################################################################
# Set Project name and version
cmake_minimum_required (VERSION 3.5)
if (POLICY CMP0018)
  # position independent code policy
  cmake_policy(SET CMP0018 NEW)
endif ()
if (POLICY CMP0074)
  # _ROOT vars policy
  cmake_policy(SET CMP0074 NEW)
endif ()
project (LIBKEA)

IF(NOT CMAKE_BUILD_TYPE)
  #SET(CMAKE_BUILD_TYPE "DEBUG")
  SET(CMAKE_BUILD_TYPE "RELEASE")
  #SET(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
  #SET(CMAKE_BUILD_TYPE "MINSIZEREL")
ENDIF()

set (PROJECT_BINARY_DIR bin)
set (PROJECT_LIBRARY_DIR lib)
set (PROJECT_TEST_DIR tests)
set (PROJECT_HEADER_DIR include)
set (PROJECT_TOOLS_DIR tools)
set (PROJECT_GDAL_DIR gdal)

# The version number.
set (LIBKEA_VERSION_MAJOR 1)
set (LIBKEA_VERSION_MINOR 4)
set (LIBKEA_VERSION_PATCH 13)
set (LIBKEA_VERSION "${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_VERSION "${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_STRING "LibKEA ${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_BUGREPORT "petebunting@mac.com")
set (LIBKEA_PACKAGE "LibKEA")
set (LIBKEA_COPYRIGHT_YEAR 2013)

set(LIBKEA_LIB_NAME kea)

include(CMakeDependentOption)
# CMake global option valiable
option (BUILD_SHARED_LIBS "Build shared library" ON)

# FindHDF5 feature
# -------------------
# On UNIX systems, this module will read the variable
# HDF5_USE_STATIC_LIBRARIES to determine whether or not to prefer a
# static link to a dynamic link for HDF5 and all of it's dependencies.
# To use this feature, make sure that the HDF5_USE_STATIC_LIBRARIES
# variable is set before the call to find_package.
option(HDF5_USE_STATIC_LIBRARIES "Force to use HDF5 static libraries." OFF)
# Both the serial and parallel HDF5 wrappers are considered and the first
# directory to contain either one will be used.  In the event that both appear
# in the same directory the serial version is preferentially selected. This
# behavior can be reversed by setting the variable HDF5_PREFER_PARALLEL to
# true.
option(HDF5_PREFER_PARALLEL "Prefer HDF5 parallel wrapper instead of serial one." OFF)
# To provide the module with a hint about where to find your HDF5
# installation, you can set the environment variable HDF5_ROOT.  The
# Find module will then look in this path when searching for HDF5
# executables, paths, and libraries.
if(NOT "x$ENV{HDF5_ROOT}" STREQUAL "x")
    message(STATUS "HDF5_ROOT environment variable specified: $ENV{HDF5_ROOT}")
endif()
find_package(HDF5 COMPONENTS CXX HL REQUIRED)

# https://support.hdfgroup.org/HDF5/doc/TechNotes/ThreadSafeLibrary.html
include(CheckSymbolExists)
SET(CMAKE_REQUIRED_INCLUDES ${HDF5_INCLUDE_DIRS})
if(MSVC AND NOT HDF5_USE_STATIC_LIBRARIES)
	# Needed so that correct result of threadsafe test is obtained
	set(CMAKE_REQUIRED_DEFINITIONS -DH5_BUILT_AS_DYNAMIC_LIB)
endif(MSVC AND NOT HDF5_USE_STATIC_LIBRARIES)

check_symbol_exists(H5_HAVE_THREADSAFE "H5pubconf.h" HDF5_THREADSAFE)
if( NOT HDF5_THREADSAFE )
    message(NOTICE "") 
    message(NOTICE "*** HDF5 not built with HDF5_ENABLE_THREADSAFE=ON ***")
    message(NOTICE "*** KEALib WILL NOT be threadsafe ***") 
    message(NOTICE "")
endif()

# required to get compilation on Windows
find_package(Threads)
# Needed for dependent option below
find_package(GDAL)
cmake_dependent_option(LIBKEA_WITH_GDAL  "Choose if .kea GDAL driver should be built" OFF "GDAL_FOUND" OFF)
###############################################################################
# Code to change HDF5_LIBRARIES (from FindHDF5.cmake) into a form
# that can be understood by libtool and put into kea-config

SET(HDF5_LIBRARIES_PATHS)
SET(HDF5_LIBRARIES_NAMES)
foreach(lib_name ${HDF5_LIBRARIES})
    get_filename_component(DIRNAME ${lib_name} DIRECTORY)
    get_filename_component(LIBNAME ${lib_name} NAME_WE)
    list(APPEND HDF5_LIBRARIES_PATHS ${DIRNAME})

    # remove the 'lib' bit if it has one
    string(FIND ${LIBNAME} "lib" lib_index)
    if(${lib_index} EQUAL "0")
        string(SUBSTRING ${LIBNAME} 3 -1 LIBNAME)
    endif()

    list(APPEND HDF5_LIBRARIES_NAMES ${LIBNAME})
endforeach(lib_name)

# Remove duplicates if stuff was actually in there
if(HDF5_LIBRARIES_PATHS)
    list(REMOVE_DUPLICATES HDF5_LIBRARIES_PATHS)
endif()
if(HDF5_LIBRARIES_NAMES)
    list(REMOVE_DUPLICATES HDF5_LIBRARIES_NAMES)
endif()

# Code below changes from arrays returned by FindHDF5.cmake 
# into strings as expected by kea-config.in and used by libtool
# (otherwise things end up separated by semicolons etc)

# variables "fixed" in this way have the _LIBTOOL suffix
set(HDF5_LIBRARIES_LIBTOOL) 
foreach(dir_name ${HDF5_LIBRARIES_PATHS})
    string(CONCAT HDF5_LIBRARIES_LIBTOOL ${HDF5_LIBRARIES_LIBTOOL} " -L" ${dir_name})
endforeach(dir_name)

foreach(lib_name ${HDF5_LIBRARIES_NAMES})
    string(CONCAT HDF5_LIBRARIES_LIBTOOL ${HDF5_LIBRARIES_LIBTOOL} " -l" ${lib_name})
endforeach(lib_name)
message(STATUS "HDF5_LIBRARIES_LIBTOOL ${HDF5_LIBRARIES_LIBTOOL}")

set(HDF5_INCLUDE_LIBTOOL)
set(HDF5_CFLAGS_LIBTOOL)
foreach(dir_name ${HDF5_INCLUDE_DIRS})
    message(STATUS "dir_name ${dir_name}")
    string(CONCAT HDF5_INCLUDE_LIBTOOL ${HDF5_INCLUDE_LIBTOOL} " " ${dir_name})
    string(CONCAT HDF5_CFLAGS_LIBTOOL ${HDF5_CFLAGS_LIBTOOL} " -I" ${dir_name})
endforeach(dir_name)
message(STATUS "HDF5_INCLUDE_LIBTOOL ${HDF5_INCLUDE_LIBTOOL}")
message(STATUS "HDF5_CFLAGS_LIBTOOL ${HDF5_CFLAGS_LIBTOOL}")

###############################################################################
# CMake settings

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MACOSX_RPATH 1)

# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
###############################################################################

###############################################################################
# Platform and compiler specific settings

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_WARNING)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)

    if(NOT HDF5_USE_STATIC_LIBRARIES)
        # for some reason FindHDF5.cmake doesn't always set this
        add_definitions(-DH5_BUILT_AS_DYNAMIC_LIB)
    endif(NOT HDF5_USE_STATIC_LIBRARIES)
    
    # by default the compiler produces gratuitous warnings. Disable some of them
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4290 /wd4005 /wd4101 /wd4244 /wd4800 /wd4251 /wd4996")
else()
    set(CMAKE_CXX_FLAGS
	      "${CMAKE_CXX_FLAGS} -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wredundant-decls -Wno-long-long -DNDEBUG")
endif(MSVC)

###############################################################################
# Setup configure file
configure_file ( "${PROJECT_HEADER_DIR}/kea-config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_HEADER_DIR}/libkea/kea-config.h" )
if(MSVC)
    configure_file ( "${PROJECT_TOOLS_DIR}/kea-config.bat.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_BINARY_DIR}/kea-config.bat" )
else()
    configure_file ( "${PROJECT_TOOLS_DIR}/kea-config.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_BINARY_DIR}/kea-config" )
endif(MSVC)
###############################################################################

###############################################################################
# Documentation
#file(READ "doc/index.txt" README )
#file(WRITE "README.txt" "${README}")
###############################################################################

###############################################################################
# Build library

include_directories ("${PROJECT_HEADER_DIR}")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_HEADER_DIR}")
include_directories(${HDF5_INCLUDE_DIRS})
add_subdirectory (src)
if (LIBKEA_WITH_GDAL)
	add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_GDAL_DIR}")
endif(LIBKEA_WITH_GDAL)
###############################################################################

###############################################################################
# Tests
enable_testing()
add_test(NAME test1 COMMAND src/test1)
###############################################################################

###############################################################################
# Installation
if(MSVC)
    install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_BINARY_DIR}/kea-config.bat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
else()
    install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_BINARY_DIR}/kea-config" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif(MSVC)    
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_HEADER_DIR}/libkea/kea-config.h" DESTINATION include/libkea)
###############################################################################

###############################################################################
# Create Distribution
SET(CPACK_SOURCE_GENERATOR "TGZ;ZIP;TBZ2")
SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")

set(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")

# Set files to ignore
list(APPEND CPACK_SOURCE_IGNORE_FILES "_CPack_Packages")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".bak")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".bz2")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".gz")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".hg")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".hgtags")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".idea")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".orig")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".svn")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".zip")
list(APPEND CPACK_SOURCE_IGNORE_FILES "README")
list(APPEND CPACK_SOURCE_IGNORE_FILES "HOWTORELEASE.txt")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeCache.txt")
list(APPEND CPACK_SOURCE_IGNORE_FILES "cmake-build-*")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CPackConfig.cmake")
list(APPEND CPACK_SOURCE_IGNORE_FILES "schemas")

include(CPack)

add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
###############################################################################
