@echo on
setlocal

:: This script builds the dependencies (zlib and hdf5) for kealib for ArcGIS
:: Download zlib and hdf5 sources and unzip them and set the ZLIBDIR and HDF5DIR
:: vars (below) to the locations. Also set INSTALLDIR to where you want the files to 
:: be installed to. 

:: You will also need Visual Studio 2008, 2013, 2015 and 2017. For 2008 I used MS C++ for Python 2.7
:: (https://www.microsoft.com/en-au/download/details.aspx?id=44266) which is the same thing.
:: VS 2013 Community Edition is available here (https://www.visualstudio.com/en-us/news/releasenotes/vs2013-community-vs).
:: I have assumed these are installed in the standard locations. If not you might need to tweak the code below.

set ZLIBDIR=C:\dev\arc\zlib-1.2.11
set HDF5DIR=C:\dev\arc\hdf5-1.10.1
set INSTALLDIR=c:\dev\arckea
:: Stop PATH getting too long by resetting it
set oldpath=%PATH%

:: Visual Studio 2008 x86
SetLocal
set VCYEAR=VC2008
set VCMACH=x86
set PATH=%oldpath%
call "C:\Users\sam\AppData\Local\Programs\Common\Microsoft\Visual C++ for Python\9.0\vcvarsall.bat" %VCMACH%
@echo on
call :build
EndLocal

:: Visual Studio 2013 x86 and x64
SetLocal
set VCYEAR=VC2013
set VCMACH=x86
set PATH=%oldpath%
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" %VCMACH%
@echo on
call :build
EndLocal

SetLocal
set VCYEAR=VC2013
set VCMACH=x64
:: Note VS2013 doesn't understand 'x64'...
set PATH=%oldpath%
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
@echo on
call :build
EndLocal

:: Visual Studio 2015 for ArcPro <= 2.0
SetLocal
set VCYEAR=VC2015
set VCMACH=x64
set PATH=%oldpath%
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %VCMACH%
@echo on
call :build
EndLocal

:: Visual Studio 2017 for ArcPro > 2.0, ArcGIS 10.6
SetLocal
set VCYEAR=VC2017
set VCMACH=x86
set PATH=%oldpath%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
call :build
EndLocal

SetLocal
set VCYEAR=VC2017
set VCMACH=x64
set PATH=%oldpath%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
call :build
EndLocal

EXIT /B %ERRORLEVEL%

:build

set BUILDDIR=build_%VCYEAR%_%VCMACH%
set VCINSTALLDIR=%INSTALLDIR%\%VCYEAR%_%VCMACH%

:: ZLIB
cd %ZLIBDIR%
mkdir %BUILDDIR%
cd %BUILDDIR%

cmake -G "NMake Makefiles" ^
    -D CMAKE_BUILD_TYPE=Release ^
    -D CMAKE_INSTALL_PREFIX=%VCINSTALLDIR% ^
    -D BUILD_SHARED_LIBS=OFF ^
    ..
if errorlevel 1 exit /B 1
:: can't seem to just build 'zlibstatic' so do the lot - note BUILD_SHARED_LIBS=OFF doesn't do anything
nmake install
if errorlevel 1 exit /B 1

:: delete the dll and implib
del %VCINSTALLDIR%\bin\zlib.dll
del %VCINSTALLDIR%\lib\zlib.lib

cd ..
rmdir /s /q %BUILDDIR%

::HDF5
cd %HDF5DIR%
mkdir %BUILDDIR%
cd %BUILDDIR%

:: ZLIB_USE_EXTERNAL needs to be set to OFF for some reason...
cmake -G "NMake Makefiles" ^
      -D CMAKE_BUILD_TYPE=Release ^
      -D CMAKE_INSTALL_PREFIX=%VCINSTALLDIR% ^
      -D CMAKE_PREFIX_PATH=%VCINSTALLDIR% ^
      -D HDF5_BUILD_CPP_LIB=ON ^
      -D HDF5_ENABLE_Z_LIB_SUPPORT:BOOL=ON ^
      -D ZLIB_ROOT=%VCINSTALLDIR% ^
      -D BUILD_SHARED_LIBS=OFF ^
      -D HDF5_BUILD_TOOLS=OFF ^
      -D HDF5_BUILD_EXAMPLES=OFF ^
      -D ZLIB_LIBRARY=%VCINSTALLDIR%\lib\zlibstatic.lib ^
      -D ZLIB_INCLUDE_DIR=%VCINSTALLDIR%\include ^
      -D ZLIB_USE_EXTERNAL=OFF ^
      -D BUILD_TESTING=OFF ^
      ..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1      

cd ..
rmdir /s /q %BUILDDIR%
     
EXIT /B 0
