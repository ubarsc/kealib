
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
target_link_libraries(${LIBKEA_LIB_NAME} PRIVATE hdf5::hdf5 hdf5::hdf5_cpp)

include(GenerateExportHeader)
generate_export_header(${LIBKEA_LIB_NAME}
  EXPORT_MACRO_NAME KEA_EXPORT
  EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/libkea/kea_export.h
)
target_include_directories(${LIBKEA_LIB_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(${LIBKEA_LIB_NAME}
  PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 1
    PREFIX "lib"
    IMPORT_PREFIX "lib"
    SOVERSION ${LIBKEA_VERSION_MAJOR}.${LIBKEA_VERSION_MINOR}
    VERSION ${LIBKEA_VERSION}
)

add_library(Kealib INTERFACE)
target_include_directories(Kealib INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
# TODO: a better way??
if(MSVC)
    target_link_libraries(Kealib INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/libkea${CMAKE_IMPORT_LIBRARY_SUFFIX}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/lib/libkea${CMAKE_IMPORT_LIBRARY_SUFFIX}>)
else()
    target_link_libraries(Kealib INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/libkea${CMAKE_SHARED_LIBRARY_SUFFIX}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/lib/libkea${CMAKE_SHARED_LIBRARY_SUFFIX}>)
endif(MSVC)
###############################################################################

###############################################################################
# Testing
# exe needs to be in 'src' otherwise it doesn't work
add_executable (test1 ${PROJECT_SOURCE_DIR}/src/tests/test1.cpp)
target_link_libraries (test1 ${LIBKEA_LIB_NAME})
###############################################################################

###############################################################################
# Package
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/libkeaConfig.cmake"
  INSTALL_DESTINATION lib/cmake/libkea
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/libkeaConfigVersion.cmake"
  VERSION "${LIBKEA_VERSION}"
  COMPATIBILITY AnyNewerVersion
)
###############################################################################
# Install
install (TARGETS ${LIBKEA_LIB_NAME} Kealib
          EXPORT libkeaTargets
          RUNTIME DESTINATION bin
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)
install(FILES ${LIBKEA_H} ${CMAKE_CURRENT_BINARY_DIR}/include/libkea/kea_export.h
        DESTINATION include/libkea)
install(EXPORT libkeaTargets
        FILE libkeaTargets.cmake
        NAMESPACE libkea::
        DESTINATION lib/cmake/libkea)
install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/libkeaConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/libkeaConfigVersion.cmake"
        DESTINATION lib/cmake/libkea)
###############################################################################
