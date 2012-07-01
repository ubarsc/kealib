
###############################################################################
# set file locations
set(LIBKEA_SRC_DIR libkea)
set(LIBKEA_HEADERS_DIR ../include/libkea)


set(LIBKEA_H
	${LIBKEA_HEADERS_DIR}/KEAWriter.h
	${LIBKEA_HEADERS_DIR}/KEAReader.h )

set(LIBKEA_CPP
	${LIBKEA_SRC_DIR}/KEAWriter.cpp
	${LIBKEA_SRC_DIR}/KEAReader.cpp )

###############################################################################

###############################################################################
# Group source files for IDE source explorers
source_group("CMake Files" FILES CMakeLists.txt)
source_group("src_kea" FILES ${LIBKEA_CPP})
source_group("include_kea" FILES ${LIBKEA_H})
###############################################################################

###############################################################################
# Build and link library

add_library(${LIBKEA_LIB_NAME} ${LIBKEA_CPP} ${LIBKEA_H} )
target_link_libraries(${LIBKEA_LIB_NAME} ${HDF5_LIBRARIES} )
###############################################################################

###############################################################################
# Set target properties

SET_TARGET_PROPERTIES(${LIBKEA_LIB_NAME}
PROPERTIES
        SOVERSION ${LIBKEA_VERSION}
        VERSION ${LIBKEA_VERSION}
)

###############################################################################

###############################################################################
# Installation 
install (TARGETS ${LIBKEA_LIB_NAME} DESTINATION lib)
install (FILES ${LIBKEA_H} DESTINATION include/libkea)
###############################################################################