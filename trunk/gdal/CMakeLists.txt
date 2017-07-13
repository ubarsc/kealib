###############################################################################
# CMake settings
cmake_minimum_required(VERSION 2.8.10)

###############################################################################
# set file locations
set(LIBKEA_HEADERS_DIR ../include CACHE PATH "Path to KEA headers")
SET(LIBKEA_LIB_PATH ../src CACHE PATH "Path to KEA library")
option(KEAHDF5_STATIC_LIBS "Build against static KEA and HDF5")

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

# If we are not being called from the main kealib build set libs etc
IF(NOT GDAL_LIBRARIES)
  IF(WIN32)
    IF(KEAHDF5_STATIC_LIBS)
        set(GDAL_LIBRARIES -LIBPATH:${GDAL_LIB_PATH} gdal_i.lib -LIBPATH:"${HDF5_LIB_PATH}" libhdf5_cpp.lib libhdf5.lib zlibstatic.lib)
    else()
        set(GDAL_LIBRARIES -LIBPATH:${GDAL_LIB_PATH} gdal_i.lib -LIBPATH:"${HDF5_LIB_PATH}" libhdf5_cpp.lib libhdf5.lib)
    endif(KEAHDF5_STATIC_LIBS)
  else()
    set(GDAL_LIBRARIES "-L${GDAL_LIB_PATH} -lgdal -L${HDF5_LIB_PATH} -lhdf5 -lhdf5_hl -lhdf5_cpp")
  endif()
endif(NOT GDAL_LIBRARIES)

if (WIN32)
  target_link_libraries(${LIBKEA_GDAL_DRIVER} ${GDAL_LIBRARIES} -LIBPATH:${LIBKEA_LIB_PATH} libkea.lib)
else()
  target_link_libraries(${LIBKEA_GDAL_DRIVER} ${GDAL_LIBRARIES} "-L${LIBKEA_LIB_PATH}" -lkea)
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
# Installation 
install (TARGETS ${LIBKEA_GDAL_DRIVER} DESTINATION lib/gdalplugins)
###############################################################################