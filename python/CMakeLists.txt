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

cmake_minimum_required(VERSION 3.22)
cmake_policy(VERSION 3.22)
project(extrat LANGUAGES CXX)

# No GDAL since we only depend on it at run time
find_package(Python 3.7 COMPONENTS Interpreter Development)

set(AWKWARD_VERSION "" CACHE STRING "Set version of awkward being used. Determined via Python if not set")
if(AWKWARD_VERSION STREQUAL "")
    # Find version of awkward we have installed
    message(STATUS "Finding awkward version via Python")
    # don't import all of awkward - has some C libraries that won't work for cross compilation
    EXECUTE_PROCESS(COMMAND ${Python_EXECUTABLE} -c "import awkward; print(awkward.__version__)" OUTPUT_VARIABLE AWKWARD_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(AWKWARD_VERSION STREQUAL "")
        message(FATAL_ERROR "Unable to import awkward")
    endif()
endif()
message(STATUS "Using awkward version ${AWKWARD_VERSION}")

# get awkward headers
include(FetchContent)
FetchContent_Declare(
  awkward
  DOWNLOAD_EXTRACT_TIMESTAMP true
  URL      https://github.com/scikit-hep/awkward/releases/download/v${AWKWARD_VERSION}/header-only.zip
)
# don't use FetchContent_MakeAvailable(awkward) - it will install - see https://stackoverflow.com/questions/65527126/disable-install-for-fetchcontent
FetchContent_GetProperties(awkward)
if(NOT awkward_POPULATED)
  FetchContent_Populate(awkward)
  add_subdirectory(${awkward_SOURCE_DIR} ${awkward_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

find_package(pybind11 CONFIG REQUIRED)
find_package(Kealib CONFIG REQUIRED)

pybind11_add_module(extrat extrat.cpp)
target_include_directories(extrat BEFORE PRIVATE "${KEALIB_INCLUDE}")
set_target_properties(extrat PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_link_libraries(extrat PRIVATE Kealib::Kealib awkward::layout-builder)

install(FILES kea_build_neighbours DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
set(PYKEA_INSTALL_DIR "${Python_SITELIB}/kealib" CACHE PATH "Install Dir for Python Bindings")
install(TARGETS extrat DESTINATION ${PYKEA_INSTALL_DIR})
install(FILES __init__.py build_neighbours.py DESTINATION ${PYKEA_INSTALL_DIR})

# Our 'fake' GDAL - don't install
Python_add_library(fakegdal MODULE WITH_SOABI fakegdal.cpp)
get_target_property(FAKEGDAL_SUFFIX fakegdal SUFFIX)
set(FAKEGDAL_NAME "fakegdal${FAKEGDAL_SUFFIX}")
target_link_libraries(fakegdal PRIVATE Kealib::Kealib)

add_executable(testseg testseg.cpp)
target_link_libraries(testseg PRIVATE libkea::Kealib)

# Remove in release 1.6.0
find_package(libkea CONFIG REQUIRED)
add_executable(testseg_libkea testseg.cpp)
target_link_libraries(testseg_libkea PRIVATE libkea::Kealib)

# custom install step to create .pyc files
install(CODE "execute_process(COMMAND ${Python_EXECUTABLE} -m compileall ${PYKEA_INSTALL_DIR})")

enable_testing()
add_test(NAME testpython COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testpython.py)
# ensure we can import fakegdal and the load the same lib as GDAL from the build location
set_property(TEST testpython PROPERTY ENVIRONMENT_MODIFICATION "PYTHONPATH=path_list_prepend:${CMAKE_CURRENT_BINARY_DIR};KEALIB_LIBGDAL=set:${CMAKE_CURRENT_BINARY_DIR}/${FAKEGDAL_NAME}")
