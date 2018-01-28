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
project (LIBKEA)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
set (LIBKEA_HG_VERSION 289)
set (LIBKEA_VERSION_MAJOR 1)
set (LIBKEA_VERSION_MINOR 4)
set (LIBKEA_VERSION_PATCH 7)
set (LIBKEA_VERSION "${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_VERSION "${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_STRING "LibKEA ${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_BUGREPORT "petebunting@mac.com")
set (LIBKEA_PACKAGE "LibKEA")
set (LIBKEA_COPYRIGHT_YEAR 2013)

# set Name of C++ library
if(MSVC)
    set(LIBKEA_LIB_NAME libkea)
else()
    set(LIBKEA_LIB_NAME kea)
endif()

include(CMakeDependentOption)
option (BUILD_SHARED_LIBS "Build with shared library" ON)
cmake_dependent_option(HDF5_USE_STATIC_LIBRARIES "On Windows, allow linking against static HDF5 libs" OFF "BUILD_SHARED_LIBS" ON)
find_package(HDF5 COMPONENTS CXX HL REQUIRED)
find_package(GDAL)
cmake_dependent_option(LIBKEA_WITH_GDAL  "Choose if .kea GDAL driver should be built" ON "GDAL_FOUND" OFF)

###############################################################################

###############################################################################
# CMake settings

IF(NOT CMAKE_BUILD_TYPE)
  #SET(CMAKE_BUILD_TYPE "DEBUG")
  SET(CMAKE_BUILD_TYPE "RELEASE")
  #SET(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
  #SET(CMAKE_BUILD_TYPE "MINSIZEREL")
ENDIF()

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
        
        # by default the compiler produces gratuitous warnings. Disable some of them
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4290 /wd4005 /wd4101 /wd4244 /wd4800 /wd4251 /wd4996")
else()
  set(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wredundant-decls -Wno-long-long -DNDEBUG")
endif(MSVC)

###############################################################################
# Setup configure file
configure_file ( "${PROJECT_HEADER_DIR}/kea-config.h.in" "${CMAKE_BINARY_DIR}/${PROJECT_HEADER_DIR}/libkea/kea-config.h" )
configure_file ( "${PROJECT_TOOLS_DIR}/kea-config.in" "${CMAKE_BINARY_DIR}/${PROJECT_BINARY_DIR}/kea-config" )
###############################################################################

###############################################################################
# Documentation
file(READ "doc/index.txt" README )
file(WRITE "README.txt" "${README}")
###############################################################################

###############################################################################
# Build library

include_directories ("${PROJECT_HEADER_DIR}")
include_directories ("${CMAKE_BINARY_DIR}/${PROJECT_HEADER_DIR}")
include_directories(${HDF5_INCLUDE_DIRS})
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
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_BINARY_DIR}/kea-config" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
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
