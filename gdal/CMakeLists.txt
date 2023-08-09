cmake_minimum_required (VERSION 3.5)
###############################################################################
# set file locations
set(LIBKEA_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../src CACHE PATH "Path to KEA library")
set(LIBKEA_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include CACHE PATH "Path to KEA Headers")
option(KEAHDF5_STATIC_LIBS "Build against static KEA and HDF5" OFF)
option(KEA_PLUGIN_OUTOFTREE "GDAL plugin is being build separately from kealib (must set LIBKEA_LIB_PATH,LIBKEA_HEADERS_DIR)" OFF)

set(KEA_GDAL_H keadataset.h keaband.h keaoverview.h keacopy.h keamaskband.h kearat.h)

set(KEA_GDAL_CPP keadriver.cpp keadataset.cpp keaband.cpp keaoverview.cpp keacopy.cpp keamaskband.cpp kearat.cpp)
if(MSVC)
    # cmake ends up with some inconsistently slashed paths. If there are single
    # slashes in there they end up being interpreted as escape sequences
    # which result in a very strange looking string table. Create sanitised versions
    file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}" CMAKE_INSTALL_PREFIX_SAN)
    file(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}" CMAKE_PREFIX_PATH_SAN)
    file(TO_CMAKE_PATH "${GDAL_DIR}" GDAL_DIR_SAN)
    file(TO_CMAKE_PATH "${HDF5_ROOT}" HDF5_ROOT_SAN)
    file(TO_CMAKE_PATH "${LIBKEA_HEADERS_DIR}" LIBKEA_HEADERS_DIR_SAN)
    file(TO_CMAKE_PATH "${LIBKEA_LIB_PATH}" LIBKEA_LIB_PATH_SAN)
    file(TO_CMAKE_PATH "${CMAKE_CURRENT_BINARY_DIR}" CMAKE_CURRENT_BINARY_DIR_SAN)
    file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" CMAKE_CURRENT_SOURCE_DIR_SAN)
    configure_file(resource.rc.in resource.rc @ONLY)
    set(KEA_GDAL_RC ${CMAKE_CURRENT_BINARY_DIR}/resource.rc)
    
    # so the resource compiler can find kea-config.h
    set(CMAKE_RC_FLAGS "/I${LIBKEA_HEADERS_DIR}")
else()
    set(KEA_GDAL_RC)
endif()

###############################################################################
set(LIBKEA_GDAL_DRIVER gdal_KEA)

###############################################################################
# Group source files for IDE source explorers
source_group("CMake Files" FILES CMakeLists.txt)
source_group("gdal_src_kea" FILES ${KEA_GDAL_CPP} ${KEA_GDAL_RC})
source_group("gdal_include_kea" FILES ${KEA_GDAL_H})
###############################################################################

if(MSVC)
    # by default the compiler produces gratuitous warnings. Disable some of them
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4290 /wd4005 /wd4101 /wd4244 /wd4800 /wd4251 /wd4996")
endif()
###############################################################################
# Build and link library
option (BUILD_SHARED_LIBS "Build with shared library" ON)

add_library(${LIBKEA_GDAL_DRIVER} ${KEA_GDAL_CPP} ${KEA_GDAL_H} ${KEA_GDAL_RC})
# remove the leading "lib" as GDAL won't look for files with this prefix
set_target_properties(${LIBKEA_GDAL_DRIVER} PROPERTIES PREFIX "")

find_package(GDAL REQUIRED)
if(KEAHDF5_STATIC_LIBS)
    set(HDF5_USE_STATIC_LIBRARIES ON)
	# Need to link to HDF5 also
    find_package(HDF5 COMPONENTS C CXX REQUIRED)

	target_link_libraries(${LIBKEA_GDAL_DRIVER} PUBLIC GDAL::GDAL hdf5::hdf5 ${LIBKEA_LIB_PATH}/liblibkea.lib)
    
    # Bizarelly, something about this combination forces the MSVC Non-DLL flags
    # which then causes linking to fail (as everything else is build to be a DLL). Reset.
    set_target_properties(${LIBKEA_GDAL_DRIVER} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
elseif(KEA_PLUGIN_OUTOFTREE)
    if(MSVC)
        target_link_libraries(${LIBKEA_GDAL_DRIVER} PUBLIC GDAL::GDAL hdf5::hdf5 ${LIBKEA_LIB_PATH}/libkea.lib)
    else()
        target_link_libraries(${LIBKEA_GDAL_DRIVER} PUBLIC GDAL::GDAL hdf5::hdf5 -L${LIBKEA_LIB_PATH} -lkea)
    endif()
else()
    target_link_libraries(${LIBKEA_GDAL_DRIVER} PUBLIC GDAL::GDAL PRIVATE Kealib)
endif()
include_directories(${LIBKEA_HEADERS_DIR})
###############################################################################

###############################################################################
# Installation 
# Always install into the prefix
# to keep compatibility with previous versions
install (TARGETS ${LIBKEA_GDAL_DRIVER} DESTINATION lib/gdalplugins)

# Then look for existing plugins directories (only possible on Unix - gdal-config not really runable on Windows)
set(GDAL_PLUGIN_DIRS)
find_program(GDAL_CONFIG_EXECUTABLE "gdal-config" HINTS ${GDAL_CONFIG})
if(NOT MSVC AND EXISTS "${GDAL_CONFIG_EXECUTABLE}")
    # find gdal prefix and version
    execute_process(COMMAND ${GDAL_CONFIG_EXECUTABLE} --prefix OUTPUT_VARIABLE GDAL_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(STRIP ${GDAL_PREFIX} GDAL_PREFIX)
    execute_process(COMMAND ${GDAL_CONFIG_EXECUTABLE} --version OUTPUT_VARIABLE GDAL_VERSION_STRING)
    include(TransformVersion)
    transform_version(GDAL_VERSION_NUM GDAL_VERSION_MAJOR GDAL_VERSION_MINOR GDAL_VERSION_PATCH ${GDAL_VERSION_STRING})
    if(OSX_FRAMEWORK)
        list(APPEND GDAL_PLUGIN_DIRS "${GDAL_PREFIX}/PlugIns")
    endif()
    # debian/ubuntu/mint
    list(APPEND GDAL_PLUGIN_DIRS "${GDAL_PREFIX}/lib/gdalplugins/${GDAL_VERSION_MAJOR}.${GDAL_VERSION_MINOR}")
    # standard unix location such as /usr/local/lib/gdalplugins, /opt/gdal/lib/gdalplugins
    list(APPEND GDAL_PLUGIN_DIRS "${GDAL_PREFIX}/lib/gdalplugins")
endif()
# windows/osge4w
if(WIN32)
     list(APPEND GDAL_PLUGIN_DIRS "c:\\OSGEO4W\\bin\\gdalplugins")
endif()
if(UNIX)
    # default location for unix/mac
    list(APPEND GDAL_PLUGIN_DIRS "/usr/local/lib/gdalplugins/")
endif()

# detect plugin directory
set(GDAL_PLUGIN_DIR "NOTFOUND")
foreach(dir ${GDAL_PLUGIN_DIRS})
    # FIXME: we should check library existence in a proper location
    if(EXISTS "${dir}" AND IS_DIRECTORY "${dir}")
        set(GDAL_PLUGIN_DIR "${dir}")
        message(STATUS "GDAL plugin installation dir: ${GDAL_PLUGIN_DIR}")
        break()
    endif()
endforeach()
unset(GDAL_PLUGIN_DIRS)

# Installation
if(EXISTS "${GDAL_PLUGIN_DIR}")
    install (TARGETS ${LIBKEA_GDAL_DRIVER} DESTINATION ${GDAL_PLUGIN_DIR})
endif()
###############################################################################
