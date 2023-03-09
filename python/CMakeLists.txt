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

cmake_minimum_required(VERSION 3.16)
project(extrat LANGUAGES CXX)

if(MSVC)
  set(LIBKEA_NAME "libkea")
  set(LIBKEA_CONFIG "kea-config.bat")
else()
  set(LIBKEA_NAME "kea")
  set(LIBKEA_CONFIG "kea-config")
endif()

# No GDAL since we only depend on it at run time
find_package(Python3 COMPONENTS Interpreter Development NumPy)

# Find version of awkward we have installed
#EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} -c "import awkward; print(awkward.__version__)" OUTPUT_VARIABLE AWKWARD_VERSION)
SET(AWKWARD_VERSION "2.0.9")

# get awkward headers
include(FetchContent)
FetchContent_Declare(
  awkward
  URL      https://github.com/scikit-hep/awkward/releases/download/v${AWKWARD_VERSION}/header-only.tar.gz
)
FetchContent_MakeAvailable(awkward)
set(AWKWARD_INCLUDE "${awkward_SOURCE_DIR}/include")

# get kealib info
execute_process(COMMAND ${LIBKEA_CONFIG} --includes OUTPUT_VARIABLE KEALIB_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LIBKEA_CONFIG} --ldflags OUTPUT_VARIABLE KEALIB_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "Includes: ${AWKWARD_INCLUDE} ${KEALIB_INCLUDE} ${Python3_NumPy_INCLUDE_DIRS}")

find_package(pybind11 CONFIG REQUIRED)

find_library(LIBKEA ${LIBKEA_NAME} REQUIRED HINTS ${KEALIB_LIBRARIES})

find_package(HDF5 COMPONENTS CXX HL REQUIRED)

pybind11_add_module(extrat extrat.cpp)
target_include_directories(extrat BEFORE PRIVATE "${AWKWARD_INCLUDE}" PRIVATE "${KEALIB_INCLUDE}" PRIVATE "${Python3_NumPy_INCLUDE_DIRS}")
target_compile_definitions(extrat PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
set_target_properties(extrat PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_link_libraries(extrat PRIVATE ${LIBKEA})

install(FILES kea_build_neighbours.py DESTINATION bin)
set(PYKEA_INSTALL_DIR "${Python3_SITELIB}/kealib" CACHE PATH "Install Dir for Python Bindings")
install(TARGETS extrat DESTINATION ${PYKEA_INSTALL_DIR})
install(FILES __init__.py DESTINATION ${PYKEA_INSTALL_DIR})

add_library(fakegdal SHARED fakegdal.cpp)
set_target_properties(fakegdal
    PROPERTIES
        PREFIX ""
        OUTPUT_NAME "fakegdal"
    )
target_include_directories(fakegdal BEFORE PRIVATE "${Python3_INCLUDE_DIRS}" PRIVATE "${HDF5_INCLUDE_DIRS}")
target_link_libraries(fakegdal PRIVATE ${Python3_LIBRARIES} PRIVATE ${LIBKEA})
