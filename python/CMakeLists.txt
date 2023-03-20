#  Permission is hereby granted, free of charge, to any person 
#  obtaining a copy of this software and associated documentation 
#  files (the "Software"), to deal in the Software without restriction, 
#  including without limitation the rights to use, copy, modify, 
#  merge, publish, distribute, sublicense, and/or sell copies of the 
#  Software, and to permit persons to whom the Software is furnished 
#  to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be 
#  included in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
#  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
#  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
#  ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
#  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
#  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.17)
project(extrat LANGUAGES CXX)

if(MSVC)
  set(LIBKEA_NAME "libkea")
  set(LIBKEA_CONFIG "kea-config.bat")
else()
  set(LIBKEA_NAME "kea")
  set(LIBKEA_CONFIG "kea-config")
endif()

# No GDAL since we only depend on it at run time
find_package(Python 3.7 COMPONENTS Interpreter Development NumPy)

# Find version of awkward we have installed
EXECUTE_PROCESS(COMMAND ${Python_EXECUTABLE} -c "import awkward; print(awkward.__version__)" OUTPUT_VARIABLE AWKWARD_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Using awkward version ${AWKWARD_VERSION}")

# get awkward headers
include(FetchContent)
FetchContent_Declare(
  awkward
  URL      https://github.com/scikit-hep/awkward/releases/download/v${AWKWARD_VERSION}/header-only.zip
)
# don't use FetchContent_MakeAvailable(awkward) - it will install - see https://stackoverflow.com/questions/65527126/disable-install-for-fetchcontent
FetchContent_GetProperties(awkward)
if(NOT awkward_POPULATED)
  FetchContent_Populate(awkward)
  add_subdirectory(${awkward_SOURCE_DIR} ${awkward_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
set(AWKWARD_INCLUDE1 "${awkward_SOURCE_DIR}/layout-builder")
set(AWKWARD_INCLUDE2 "${awkward_SOURCE_DIR}/builder-options")
set(AWKWARD_INCLUDE3 "${awkward_SOURCE_DIR}/growable-buffer")

# get kealib info
execute_process(COMMAND ${LIBKEA_CONFIG} --includes OUTPUT_VARIABLE KEALIB_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LIBKEA_CONFIG} --ldflags OUTPUT_VARIABLE KEALIB_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "Includes: ${AWKWARD_INCLUDE1} ${AWKWARD_INCLUDE2} ${AWKWARD_INCLUDE3} ${KEALIB_INCLUDE} ${Python_NumPy_INCLUDE_DIRS}")

find_package(pybind11 CONFIG REQUIRED)

find_library(LIBKEA ${LIBKEA_NAME} REQUIRED HINTS ${KEALIB_LIBRARIES})

find_package(HDF5 COMPONENTS CXX HL REQUIRED)

pybind11_add_module(extrat extrat.cpp)
target_include_directories(extrat BEFORE PRIVATE "${AWKWARD_INCLUDE1}" PRIVATE "${AWKWARD_INCLUDE2}" PRIVATE "${AWKWARD_INCLUDE3}" PRIVATE "${KEALIB_INCLUDE}" PRIVATE "${Python_NumPy_INCLUDE_DIRS}")
target_compile_definitions(extrat PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
set_target_properties(extrat PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_link_libraries(extrat PRIVATE ${LIBKEA})

install(FILES kea_build_neighbours DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
set(PYKEA_INSTALL_DIR "${Python_SITELIB}/kealib" CACHE PATH "Install Dir for Python Bindings")
install(TARGETS extrat DESTINATION ${PYKEA_INSTALL_DIR})
install(FILES __init__.py build_neighbours.py DESTINATION ${PYKEA_INSTALL_DIR})

Python_add_library(fakegdal MODULE WITH_SOABI fakegdal.cpp)
get_target_property(FAKEGDAL_SUFFIX fakegdal SUFFIX)
set(FAKEGDAL_NAME "fakegdal${FAKEGDAL_SUFFIX}")
target_include_directories(fakegdal BEFORE PRIVATE "${HDF5_INCLUDE_DIRS}" PRIVATE "${KEALIB_INCLUDE}")
target_link_libraries(fakegdal PRIVATE PRIVATE ${LIBKEA})
install(TARGETS fakegdal DESTINATION ${PYKEA_INSTALL_DIR})

add_executable(testseg testseg.cpp)
target_include_directories(testseg BEFORE PRIVATE "${KEALIB_INCLUDE}")
target_link_libraries(testseg PRIVATE ${LIBKEA})

# custom install step to create .pyc files
install(CODE "execute_process(COMMAND ${Python_EXECUTABLE} -m compileall ${PYKEA_INSTALL_DIR})")

enable_testing()
add_test(NAME testpython COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testpython.py)
set_property(TEST testpython PROPERTY ENVIRONMENT "KEALIB_LIBGDAL=${PYKEA_INSTALL_DIR}/${FAKEGDAL_NAME}")
