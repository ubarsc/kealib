
###############################################################################
# set file locations
set(LIBKEA_SRC_DIR libkea)
set(LIBKEA_HEADERS_DIR ../include/libkea)

set(LIBKEA_H
	${LIBKEA_HEADERS_DIR}/KEACommon.h
	${LIBKEA_HEADERS_DIR}/KEAException.h
	${LIBKEA_HEADERS_DIR}/KEAImageIO.h
	${LIBKEA_HEADERS_DIR}/KEAAttributeTable.h
	${LIBKEA_HEADERS_DIR}/KEAAttributeTableInMem.h 
	${LIBKEA_HEADERS_DIR}/KEAAttributeTableFile.h )

set(LIBKEA_CPP
	${LIBKEA_SRC_DIR}/KEAImageIO.cpp
	${LIBKEA_SRC_DIR}/KEAAttributeTable.cpp
	${LIBKEA_SRC_DIR}/KEAAttributeTableInMem.cpp 
	${LIBKEA_SRC_DIR}/KEAAttributeTableFile.cpp )

###############################################################################

###############################################################################
# Group source files for IDE source explorers
source_group("CMake Files" FILES CMakeLists.txt)
source_group("src_kea" FILES ${LIBKEA_CPP})
source_group("include_kea" FILES ${LIBKEA_H})
###############################################################################

###############################################################################
# Build, link and install library
add_library(${LIBKEA_LIB_NAME} ${LIBKEA_CPP} ${LIBKEA_H} )
target_link_libraries(${LIBKEA_LIB_NAME} ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES})

if(BUILD_SHARED_LIBS)
    SET_TARGET_PROPERTIES(${LIBKEA_LIB_NAME}
            PROPERTIES
            SOVERSION ${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}
            VERSION ${LIBKEA_VERSION}
            CLEAN_DIRECT_OUTPUT 1)
else()
    SET_TARGET_PROPERTIES(${LIBKEA_LIB_NAME}
            PROPERTIES
            OUTPUT_NAME "${LIBKEA_LIB_NAME}"
            PREFIX "lib"
            VERSION ${LIBKEA_VERSION}
            CLEAN_DIRECT_OUTPUT 1)
endif()
add_library(Kealib INTERFACE)
target_link_libraries(Kealib INTERFACE "${LIBKEA_LIB_NAME}")
target_include_directories(Kealib INTERFACE
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                             $<INSTALL_INTERFACE:include/kealib>)
###############################################################################

###############################################################################
# Testing
add_executable (test1 ${PROJECT_TEST_DIR}/test1.cpp)
target_link_libraries (test1 Kealib)
###############################################################################

###############################################################################
# Install
install (TARGETS ${LIBKEA_LIB_NAME}
          RUNTIME DESTINATION bin
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)
install (FILES ${LIBKEA_H} DESTINATION include/libkea)
###############################################################################
