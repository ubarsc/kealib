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

#  Get the Subversion revision number for the project's source code.  The
#  VersionControl.cmake file tests the variable WINDOWS for the Windows
#  platform, so it needs to be set before including the file.
if (WIN32)
  set (WINDOWS ${WIN32})
endif()

set (PROJECT_BINARY_DIR bin)
set (PROJECT_LIBRARY_DIR lib)
set (PROJECT_SOURCE_DIR src)
set (PROJECT_HEADER_DIR include)
set (PROJECT_TOOLS_DIR tools)
set (PROJECT_GDAL_DIR gdal)

# The version number.
set (LIBKEA_HG_VERSION 1)
set (LIBKEA_VERSION_MAJOR 1)
set (LIBKEA_VERSION_MINOR 0)
set (LIBKEA_VERSION_PATCH 0)
set (LIBKEA_VERSION "${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_VERSION "${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_STRING "LibKEA ${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}.${LIBKEA_VERSION_PATCH}")
set (LIBKEA_PACKAGE_BUGREPORT "petebunting@mac.com")
set (LIBKEA_PACKAGE "LibKEA")
set (LIBKEA_COPYRIGHT_YEAR 2012)

if(WIN32)
    # Name of C++ library
    set(LIBKEA_LIB_NAME libkea)
else()
    # Name of C++ library
    set(LIBKEA_LIB_NAME kea)
endif()

option (BUILD_SHARED_LIBS "Build with shared library" ON)
set(LIBKEA_WITH_UTILITIES TRUE CACHE BOOL "Choose if KeaLib utilities should be built")
set(LIBKEA_WITH_GDAL TRUE CACHE BOOL "Choose if .kea GDAL driver should be built")

set(HDF5_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for HDF5")
set(HDF5_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for HDF5")

set(GDAL_INCLUDE_DIR /usr/local/include CACHE PATH "Include PATH for GDAL")
set(GDAL_LIB_PATH /usr/local/lib CACHE PATH "Library PATH for GDAL")

###############################################################################

###############################################################################
# CMake settings
cmake_minimum_required(VERSION 2.6.0)

IF(NOT CMAKE_BUILD_TYPE)
  #SET(CMAKE_BUILD_TYPE "DEBUG")
  SET(CMAKE_BUILD_TYPE "RELEASE")
  #SET(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
  #SET(CMAKE_BUILD_TYPE "MINSIZEREL")
ENDIF()

set(CMAKE_COLOR_MAKEFILE ON)

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CMAKE_VERBOSE_MAKEFILE)

# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ./cmake/modules/)
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
# Platform and compiler specific settings

if(WIN32)
    if (MSVC)
        if (MSVC80 OR MSVC90 OR MSVC10)
            add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            add_definitions(-D_CRT_NONSTDC_NO_WARNING)
            add_definitions(-D_SCL_SECURE_NO_WARNINGS)
        endif()
        
        if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W4"
                   CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        endif()

        # Generate dot-user file with user-specific settings for Visual Studio project
        set(MSVC_ENVIRONMENT_PATH "" CACHE STRING
            "Custom PATH for Environment property in Visual Studio project configuration")
        mark_as_advanced(MSVC_ENVIRONMENT_PATH)
        set(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG "${ENVIRONMENT_PATH}")
        mark_as_advanced(VCPROJ_USER_ENVIRONMENT_PATH_DEBUG)

    endif()

	set(CMAKE_INCLUDE_PATH c:/osgeo4w/include;$ENV{CMAKE_INCLUDE_PATH})
    set(CMAKE_LIBRARY_PATH c:/osgeo4w/lib;$ENV{CMAKE_LIBRARY_PATH})
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
endif(WIN32)

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
set(HDF5_LIBRARIES -L${HDF5_LIB_PATH} -lhdf5 -lhdf5_hl -lhdf5_cpp)

set(GDAL_LIBRARIES -L${GDAL_LIB_PATH} -lgdal)
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
# Build executables
if (LIBKEA_WITH_UTILITIES)
	add_executable(createkeaimg ${PROJECT_SOURCE_DIR}/exe/createkeaimg/main.cpp)
	target_link_libraries (createkeaimg ${LIBKEA_LIB_NAME} )
	add_executable(gdal2kea ${PROJECT_SOURCE_DIR}/exe/gdal2kea/main.cpp)
	target_link_libraries (gdal2kea ${LIBKEA_LIB_NAME} ${GDAL_LIBRARIES} )
endif(LIBKEA_WITH_UTILITIES)
###############################################################################

###############################################################################
# Installation
if (LIBKEA_WITH_UTILITIES)
	install (TARGETS createkeaimg DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (TARGETS gdal2kea DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif(LIBKEA_WITH_UTILITIES)

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
