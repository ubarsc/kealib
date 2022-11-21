@echo on
setlocal

:: Update these 3 variables. 
:: OUTDIR is where the output files will be put
SET OUTDIR=c:\dev\arckea\dist
:: GDALDIR is where arcgdal_for_compilation.zip (from the github downloads) as been unzipped to
SET GDALDIR=C:\dev\arcgdalforcompilation
:: HDF5DIR is the same as INSTALLDIR in buildzlibhdf5.bat
SET HDF5DIR=c:\dev\arckea

:: Visual Studio 2022 for Arc Pro 3.0 and ArcGIS 11
SetLocal
set VCMACH=x64
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc11_arcpro30
if errorlevel 1 exit /B 1
EndLocal

EXIT /B %ERRORLEVEL%

:build_arc11_arcpro30
set ARCGDALDIR=%GDALDIR%\gdal34\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2022_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arc11_arcpro30\%VCMACH%
mkdir build_arc11_arcpro30
cd build_arc11_arcpro30
  
cmake -D KEA_PLUGIN_OUTOFTREE=ON ^
      -D CMAKE_INSTALL_PREFIX=%ARCHINSTALL% ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D GDAL_DIR=%ARCGDALDIR% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..\gdal
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

:: Copy the necessary dlls over so the installer can find them 
for %%L IN (keahdf5.dll,keahdf5_cpp.dll,libkea.dll,zlibkea.dll) DO (
  COPY "%ARCHDF5DIR%\bin\%%L" "%ARCHINSTALL%\lib\%%L"
  if errorlevel 1 exit /B 1      
)

cd ..
rmdir /s /q build_arc11_arcpro30
EXIT /B 0
