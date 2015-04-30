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

Only tested with Imagine 2011 and 2015.

Prerequisites:
--------------

1. Visual Studio C++ of the same version that Imagine itself was build with.
   For Imagine 2011 this was Visual Studio C++ 2008.
   For Imagine 2015 this was Visual Studio C++ Desktop 2013.
2. A static build of ZLIB/HDF5/kealib with the same version of Visual Studio.

Use makefile2011.vc for Imagine 2011, makefile2015_x86.vc for 32bit Imagine 2015
and makefile2015_x64.vc for 64 bit Imagine 2015 ('<makefile name>').

It is advisable to compile and install both 32bit and 64bit KEA when running 64bit
Imagine 2015 since some Imagine tools may still be 32bit.

Update paths in the <makefile name> you are to use and start the Visual Studio Command Prompt and
navigate to the location of it and type 'nmake -f <makefile name>'. 

The run 'nmake -f <makefile name> install' and 'nmake -f <makefile name> configure' and 
restart Imagine.

