
:: Builds kealib. Run this after buildzlibhdf5.bat
:: Ensure INSTALLDIR is set the same as buildzlibhdf5.bat - hdf5 etc read from here 
:: and kealib installed there.

set INSTALLDIR=c:\dev\arckea
:: GDALDIR is where arcgdalforcompilation_XXX.zip (from the gitbub downloads) as been unzipped to
SET GDALDIR=C:\dev\arcgdalforcompilation

:: Visual Studio 2022 for Arc Pro 3.0 and ArcGIS 11
SetLocal
set VCYEAR=VC2022
set VCMACH=x64
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
set HDF5DIR=%HDF5DIR_LATEST%
call :build
EndLocal

EXIT /B %ERRORLEVEL%

:build

set BUILDDIR=build_%VCYEAR%_%VCMACH%
set VCINSTALLDIR=%INSTALLDIR%\%VCYEAR%_%VCMACH%

mkdir %BUILDDIR%
cd %BUILDDIR%

del ..\..\CMakeCache.txt
cmake -D BUILD_SHARED_LIBS=ON ^
      -D CMAKE_INSTALL_PREFIX=%VCINSTALLDIR% ^
	  -D CMAKE_PREFIX_PATH=%VCINSTALLDIR% ^
	  -D HDF5_ROOT=%VCINSTALLDIR% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1      

cd ..
rmdir /s /q %BUILDDIR%

EXIT /B 0
