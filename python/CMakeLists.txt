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
project(pykealib LANGUAGES CXX)

# get libawkward info
execute_process(COMMAND python -m awkward.config --incdir OUTPUT_VARIABLE AWKWARD_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND python -m awkward.config --libdir OUTPUT_VARIABLE AWKWARD_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
# get kealib info
execute_process(COMMAND kea-config --includes OUTPUT_VARIABLE KEALIB_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND kea-config --ldflags OUTPUT_VARIABLE KEALIB_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)

find_package(GDAL REQUIRED)
find_package(Python3 COMPONENTS NumPy)

include_directories(BEFORE "${AWKWARD_INCLUDE}" "${KEALIB_INCLUDE}" "${GDAL_INCLUDE_DIRS}" "${Python3_NumPy_INCLUDE_DIRS}")

add_subdirectory(pybind11)

find_library(CPU-KERNELS awkward-cpu-kernels REQUIRED HINTS ${AWKWARD_LIBRARIES})
find_library(LIBAWKWARD awkward REQUIRED HINTS ${AWKWARD_LIBRARIES})
find_library(LIBKEA kea REQUIRED HINTS ${KEALIB_LIBRARIES})
message(STATUS "Libraries: ${CPU-KERNELS} ${LIBAWKWARD} ${LIBKEA} ${GDAL_LIBRARIES}")

pybind11_add_module(pykealib pykealib.cpp)
target_compile_definitions(pykealib PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
set_target_properties(pykealib PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_link_libraries(pykealib PRIVATE ${CPU-KERNELS} ${LIBAWKWARD} ${LIBKEA} ${GDAL_LIBRARIES})
