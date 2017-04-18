
Building the KEA ArcGIS Plugin for Arc 10 and 10.1
==================================================

Prerequisites
-------------

1. Visual Studio 2008 Express. 
2. CMake
3. Static libraries of HDF5 and zlib built with VS2008.
4. GDAL headers and libraries for the versions of GDAL Arc uses. These are available
     from the kealib downloads as 'arcgdal_for_compilation.zip'

We prefer to build KEA against static builds of HDF5 since this saves having to match whatever
version Arc itself was built against. If we where to compile against HDF5 dlls we would have to
match the exact version in Arc to avoid a crash.

It is assumed that the static HDF5 and zlib libraries are under c:\keastatic\vs2008 and arcgdal_for_compilation.zip
is extracted under c:\arcgdal.

For Arc 10, the following commands should be executed in the folder with the KEA source:

cmake -D HDF5_STATIC_LIBS=TRUE -D CMAKE_INSTALL_PREFIX="c:\kea\arc100" ^
-D GDAL_INCLUDE_DIR="c:\arcgdal\gdal16\include" -D GDAL_LIB_PATH="c:\arcgdal\gdal16\lib" ^
-D HDF5_INCLUDE_DIR="C:\keastatic\vs2008\include" -D HDF5_LIB_PATH="c:\keastatic\vs2008\lib" ^
-D CMAKE_BUILD_TYPE=Release -G "NMake Makefiles" .
nmake
nmake install

The gdal_KEA.dll should now be in c:\kea\arc100\gdalplugins and libkea.dll should be in c:\kea\arc100\lib.

For Arc 10.1-10.3 the procedure is only slightly different (note different version of gdal):

nmake clean
del CMakeCache.txt
cmake -D HDF5_STATIC_LIBS=TRUE -D CMAKE_INSTALL_PREFIX="c:\kea\arc101" ^
-D GDAL_INCLUDE_DIR="c:\arcgdal\gdal18\include" -D GDAL_LIB_PATH="c:\arcgdal\gdal18\lib" ^
-D HDF5_INCLUDE_DIR="C:\keastatic\vs2008\include" -D HDF5_LIB_PATH="c:\keastatic\vs2008\lib" ^
-D CMAKE_BUILD_TYPE=Release -G "NMake Makefiles" .
nmake
nmake install

The gdal_KEA.dll should now be in c:\kea\arc101\gdalplugins and libkea.dll should be in c:\kea\arc101\lib.



Installing KEA driver for ArcMap 10.0 and 10.1-10.3
---------------------------------------------------

1. Create a 'gdalplugins' directory under the 'bin' directory if it does not already exist.

2. For Arc 10.0 copy Arc100\libkea.dll into the bin directory of your Arc install and copy the contents of the 'Arc100\gdalplugins' subdirectory of this zip file into the newly created gdalplugins directory.

3. For Arc 10.1-10.3 copy Arc101\libkea.dll into the bin directory of your Arc install and copy the contents of the 'Arc101\gdalplugins' subdirectory of this zip file into the newly created gdalplugins directory.

4. Edit bin/RasterFormats.dat and add the line:

        <e on="y" nm="KEA" ex="*.kea" et="KEA" at="0x27" />
    
    at the bottom of the section that has a number of lines looking similar
    
5. Restart Arc and you should be all go

