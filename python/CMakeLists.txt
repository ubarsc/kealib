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

cmake_minimum_required(VERSION 3.1...3.16)
project(extrat LANGUAGES CXX)

if(MSVC)
  set(LIBKEA_NAME "libkea")
  set(LIBKEA_CONFIG "kea-config.bat")
else()
  set(LIBKEA_NAME "kea")
  set(LIBKEA_CONFIG "kea-config")
endif()

# get libawkward info
execute_process(COMMAND python -m awkward.config --incdir OUTPUT_VARIABLE AWKWARD_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND python -m awkward.config --libdir OUTPUT_VARIABLE AWKWARD_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
# get kealib info
execute_process(COMMAND ${LIBKEA_CONFIG} --includes OUTPUT_VARIABLE KEALIB_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LIBKEA_CONFIG} --ldflags OUTPUT_VARIABLE KEALIB_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)

find_package(GDAL REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development NumPy)

message(STATUS "Includes: ${AWKWARD_INCLUDE} ${KEALIB_INCLUDE} ${GDAL_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS}")
include_directories(BEFORE "${AWKWARD_INCLUDE}" "${KEALIB_INCLUDE}" "${GDAL_INCLUDE_DIRS}" "${Python3_NumPy_INCLUDE_DIRS}")

add_subdirectory(pybind11)

find_library(CPU-KERNELS awkward-cpu-kernels REQUIRED HINTS ${AWKWARD_LIBRARIES})
find_library(LIBAWKWARD awkward REQUIRED HINTS ${AWKWARD_LIBRARIES})
find_library(LIBKEA ${LIBKEA_NAME} REQUIRED HINTS ${KEALIB_LIBRARIES})
message(STATUS "Libraries: ${CPU-KERNELS} ${LIBAWKWARD} ${LIBKEA} ${GDAL_LIBRARIES}")

pybind11_add_module(extrat extrat.cpp)
target_compile_definitions(extrat PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
set_target_properties(extrat PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_link_libraries(extrat PRIVATE ${CPU-KERNELS} ${LIBAWKWARD} ${LIBKEA} ${GDAL_LIBRARIES})

install(FILES kea_build_neighbours.py DESTINATION bin)
set(PYKEA_INSTALL_DIR "${Python3_SITELIB}/kealib" CACHE PATH "Install Dir for Python Bindings")
install(TARGETS extrat DESTINATION ${PYKEA_INSTALL_DIR})
install(FILES __init__.py DESTINATION ${PYKEA_INSTALL_DIR})
