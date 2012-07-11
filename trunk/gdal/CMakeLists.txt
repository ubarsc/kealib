
###############################################################################
# set file locations
set(LIBKEA_HEADERS_DIR ../include/libkea)

set(KEA_GDAL_H KEADataset.h KEABand.h)

set(KEA_GDAL_CPP KEADriver.cpp KEADataset.cpp KEABand.cpp)

###############################################################################
set(LIBKEA_GDAL_DRIVER gdal_KEA)

###############################################################################
# Group source files for IDE source explorers
source_group("CMake Files" FILES CMakeLists.txt)
source_group("gdal_src_kea" FILES ${KEA_GDAL_CPP})
source_group("gdal_include_kea" FILES ${KEA_GDAL_H})
###############################################################################

###############################################################################
# Build and link library

add_library(${LIBKEA_GDAL_DRIVER} ${KEA_GDAL_CPP} ${KEA_GDAL_H} )
# remove the leading "lib" as GDAL won't look for files with this prefix
set_target_properties(${LIBKEA_GDAL_DRIVER} PROPERTIES PREFIX "")

target_link_libraries(${LIBKEA_GDAL_DRIVER} ${GDAL_LIBRARIES} -L../src -lkea)
include_directories(${GDAL_INCLUDE_DIR})
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
install (TARGETS ${LIBKEA_GDAL_DRIVER} DESTINATION gdalplugins)
###############################################################################