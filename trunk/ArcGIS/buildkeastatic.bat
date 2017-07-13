
:: Builds static kealib. Run this after buildzlibhdf5.bat
:: Ensure INSTALLDIR is set the same as buildzlibhdf5.bat - hdf5 etc read from here 
:: and static kealib installed there.

set INSTALLDIR=c:\dev\arckea

:: Visual Studio 2008 x86
set VCYEAR=VC2008
set VCMACH=x86
call "C:\Users\sam\AppData\Local\Programs\Common\Microsoft\Visual C++ for Python\9.0\vcvarsall.bat" %VCMACH%
@echo on
call :build

:: Visual Studio 2013 x86 and x64
set VCYEAR=VC2013
set VCMACH=x86
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" %VCMACH%
@echo on
call :build

set VCMACH=x64
:: Note VS2013 doesn't understand 'x64'...
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
@echo on
call :build

:: Visual Studio 2015 for ArcPro
set VCYEAR=VC2015
set VCMACH=x64
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %VCMACH%
@echo on
call :build

EXIT /B %ERRORLEVEL%

:build

set BUILDDIR=build_%VCYEAR%_%VCMACH%
set VCINSTALLDIR=%INSTALLDIR%\%VCYEAR%_%VCMACH%

mkdir %BUILDDIR%
cd %BUILDDIR%

cmake -D HDF5_STATIC_LIBS=TRUE ^
      -D BUILD_SHARED_LIBS=OFF ^
      -D CMAKE_INSTALL_PREFIX=%VCINSTALLDIR% ^
      -D HDF5_INCLUDE_DIR=%VCINSTALLDIR%\include ^
      -D HDF5_LIB_PATH=%VCINSTALLDIR%\lib ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1      

cd ..
rmdir /s /q %BUILDDIR%

EXIT /B 0
