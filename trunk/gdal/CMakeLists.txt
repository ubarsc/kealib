###############################################################################
# set file locations
set(LIBKEA_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(LIBKEA_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../src)
option(KEAHDF5_STATIC_LIBS "Build against static KEA and HDF5" OFF)

set(KEA_GDAL_H keadataset.h keaband.h keaoverview.h keacopy.h keamaskband.h kearat.h)

set(KEA_GDAL_CPP keadriver.cpp keadataset.cpp keaband.cpp keaoverview.cpp keacopy.cpp keamaskband.cpp kearat.cpp)

###############################################################################
set(LIBKEA_GDAL_DRIVER gdal_KEA)

###############################################################################
# Group source files for IDE source explorers
source_group("CMake Files" FILES CMakeLists.txt)
source_group("gdal_src_kea" FILES ${KEA_GDAL_CPP})
source_group("gdal_include_kea" FILES ${KEA_GDAL_H})
###############################################################################

if(MSVC)
    # by default the compiler produces gratuitous warnings. Disable some of them
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4290 /wd4005 /wd4101 /wd4244 /wd4800 /wd4251 /wd4996")
endif()
###############################################################################
# Build and link library
option (BUILD_SHARED_LIBS "Build with shared library" ON)

add_library(${LIBKEA_GDAL_DRIVER} ${KEA_GDAL_CPP} ${KEA_GDAL_H} )
# remove the leading "lib" as GDAL won't look for files with this prefix
set_target_properties(${LIBKEA_GDAL_DRIVER} PROPERTIES PREFIX "")

find_package(GDAL REQUIRED)
if (WIN32)
  target_link_libraries(${LIBKEA_GDAL_DRIVER} ${GDAL_LIBRARY} -LIBPATH:${LIBKEA_LIB_PATH} libkea.lib)
else()
  target_link_libraries(${LIBKEA_GDAL_DRIVER} ${GDAL_LIBRARY} "-L${LIBKEA_LIB_PATH}" -lkea)
endif(WIN32)
include_directories(${GDAL_INCLUDE_DIR})
include_directories(${LIBKEA_HEADERS_DIR})
###############################################################################

###############################################################################

# Set target properties
#SET_TARGET_PROPERTIES(${LIBKEA_GDAL_DRIVER}
#PROPERTIES
#        SOVERSION ${LIBKEA_VERSION}
#        VERSION ${LIBKEA_VERSION}
#)

###############################################################################

###############################################################################
set(GDAL_PLUGIN_DIRS)
find_program(GDAL_CONFIG_EXECUTABLE "gdal-config" HINSTS ${GDAL_CONFIG})
if(EXISTS "${GDAL_CONFIG_EXECUTABLE}")
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
if(WINDOWS)
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