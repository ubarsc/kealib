
Building the KEA ArcGIS Plugin for Arc 9.3 - 10.5.1
===================================================

Prerequisites
-------------

1. Visual Studio 2008 Express Or Visual C++ for Python 2.7.
2. Visual Studio 2013 Community Edition.
3. CMake
4. GDAL headers and libraries for the versions of GDAL Arc uses. These are available
     from the kealib downloads as 'arcgdal_for_compilationX.zip' where X is the highest number available.

We prefer to build KEA against static builds of HDF5 since this saves having to match whatever
version Arc itself was built against. If we where to compile against HDF5 dlls we would have to
match the exact version in Arc to avoid a crash.

Refer to the file "buildzlibhdf5.bat" in this directory. You should be able to build static
zlib and hdf5 libraries with this file. Note that you will probably have to set the first
three variables to more sensible locations.

After you have run this, refer to the file "buildkeaforarc.bat". You will need to edit the 
first three variables again. Running this should output the KEA plugin for the various
Arc versions to the directory specified. 

Installing KEA driver for ArcMap Arc 9.3 - 10.5.1
-------------------------------------------------

1. Create a 'gdalplugins' directory under the 'bin' directory of your Arc install if it does not already exist.

2. Select the appropriate subfolder for your version of Arc. 

Arc 9.3 = arc93
Arc 10.0 = arc100
Arc 10.1-10.3 = arc101
Arc 10.4 = arc104
Arc 10.5 = arc105
Arc 10.5.1 = arc1051

2. Once you have found the appropriate subdirectory for your version of Arc, copy libkea.dll from the 'lib' subdirectory
  into the 'bin' directory of your Arc Install.
  
3. Copy gdal_KEA.dll from the 'lib\gdalplugins' subdirectory into the newly created 'gdalplugins' directory of your
  Arc install (see step 1 above).

4. Edit bin/RasterFormats.dat and add the line:

        <e on="y" nm="KEA" ex="*.kea" et="KEA" at="0x27" />
    
    at the bottom of the section that has a number of lines looking similar
    
5. Restart Arc and you should be all go

