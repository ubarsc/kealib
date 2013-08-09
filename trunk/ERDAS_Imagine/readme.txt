This is a basic read-only KEA driver for ERDAS Imagine.

Solaris
=======

Prerequisites:
--------------

1. Solaris Studio compilers. For versions of Imagine after 8.4 these MUST be 
   the same version as Imagine itself was compiled with due to C++ 
   incompatibilites.
2. A static build of ZLIB/HDF5/kealib with -fPIC with the same version
   of Solaris Studio.

Update the paths in makefile, and type 'make'. 'make install' copies them to 
the location that Imagine checks in your home directory. Reconfigure raster
formats to get the new driver and restart Imagine. 
Note that you may need to be root to do this. 

Note: 8.4 and earlier will not open files larger that 2Gb due to limitations
with the Imagine file open dialog.

Windows
=======

Only tested with Imagine 2011.

Prerequisites:
--------------

1. Visual Studio C++ of the same version that Imagine itself was build with.
   For Imagine 2011 this was Visual Studio C++ 2008.
2. A static build of ZLIB/HDF5/kealib with the same version of VS.

Update paths in makefile.vc and start the Visual Studio Command Prompt and
navigate to the location of it and type 'nmake -f makefile.vc'. Copy the resulting kea.dll
to C:\Program Files\ERDAS\<Imagine Version>\usr\lib\Win32Release\rasterformats
and run C:\Program Files\ERDAS\<Imagine Version>\bin\Win32Release\configure_rf.exe
and restart Imagine.

