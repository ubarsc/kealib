@echo on
setlocal

:: Update these 3 variables. 
:: OUTDIR is where the output files will be put
SET OUTDIR=c:\dev\arckea\dist
:: GDALDIR is where arcgdal_for_compilation.zip (from the github downloads) as been unzipped to
SET GDALDIR=C:\dev\arcgdalforcompilation
:: HDF5DIR is the same as INSTALLDIR in buildzlibhdf5.bat
SET HDF5DIR=c:\dev\arckea

:: Now the vs2013 builds x86
SetLocal
set VCMACH=x86
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc104
if errorlevel 1 exit /B 1
call :build_arc105
if errorlevel 1 exit /B 1
call :build_arc1051
if errorlevel 1 exit /B 1
EndLocal

:: Now x64
SetLocal
set VCMACH=x64
:: Note VS2013 doesn't understand 'x64'...
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
@echo on
call :build_arc104
if errorlevel 1 exit /B 1
call :build_arc105
if errorlevel 1 exit /B 1
call :build_arc1051
if errorlevel 1 exit /B 1
EndLocal

:: Visual Studio 2017 x86 Builds
:: ESRI Says "Visual Studio 2017 Update 2" which is 14.09(?) but this is the earliest 
:: I have installed.
SetLocal
set VCMACH=x86
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -vcvars_ver=14.12 %VCMACH%
@echo on
call :build_arc106_arcpro21
if errorlevel 1 exit /B 1
EndLocal

:: Visual Studio 2017 x64 Builds
SetLocal
set VCMACH=x64
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -vcvars_ver=14.12 %VCMACH%
@echo on
call :build_arc106_arcpro21
if errorlevel 1 exit /B 1
EndLocal

:: Visual Studio 2017 x86 Builds
:: ESRI Says "Visual Studio 2017 version 15.5.6" which is 14.11
SetLocal
set VCMACH=x86
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -vcvars_ver=14.12 %VCMACH%
@echo on
call :build_arc1061_arcpro22
if errorlevel 1 exit /B 1
call :build_arc1071_arcpro24
if errorlevel 1 exit /B 1
EndLocal

:: Visual Studio 2017 x64 Builds
SetLocal
set VCMACH=x64
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -vcvars_ver=14.12 %VCMACH%
@echo on
call :build_arc1061_arcpro22
if errorlevel 1 exit /B 1
call :build_arc1071_arcpro24
if errorlevel 1 exit /B 1
EndLocal

REM :: Visual Studio 2019 x86 Builds
SetLocal
set VCMACH=x86
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc108_arcpro25
if errorlevel 1 exit /B 1
call :build_arc1081_arcpro26
if errorlevel 1 exit /B 1
call :build_arc109
if errorlevel 1 exit /B 1
EndLocal

:: Visual Studio 2019 x64 Builds
SetLocal
set VCMACH=x64
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc108_arcpro25
if errorlevel 1 exit /B 1
call :build_arc1081_arcpro26
if errorlevel 1 exit /B 1
call :build_arcpro27
if errorlevel 1 exit /B 1
call :build_arcpro28
if errorlevel 1 exit /B 1
call :build_arc109
if errorlevel 1 exit /B 1
EndLocal

:: Visual Studio 2022 for Arc Pro 3.0 and ArcGIS 11
SetLocal
set VCMACH=x64
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc11_arcpro30
if errorlevel 1 exit /B 1
EndLocal

EXIT /B %ERRORLEVEL%

:build_arc104

set ARCGDALDIR=%GDALDIR%\gdal18b\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2013_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arc104\%VCMACH%
mkdir build_arc104
cd build_arc104

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
rmdir /s /q build_arc104

EXIT /B 0

:build_arc105

set ARCGDALDIR=%GDALDIR%\gdal201\vc12\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2013_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arc105\%VCMACH%
mkdir build_arc105
cd build_arc105

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
rmdir /s /q build_arc105

EXIT /B 0

:build_arc1051

set ARCGDALDIR=%GDALDIR%\gdal211\vc12\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2013_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arc1051\%VCMACH%
mkdir build_arc1051
cd build_arc1051

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
rmdir /s /q build_arc1051

EXIT /B 0

:build_arcpro14

set ARCGDALDIR=%GDALDIR%\gdal201\vc14\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2015_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arcpro14\%VCMACH%
mkdir build_arcpro14
cd build_arcpro14

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
rmdir /s /q build_arcpro14

EXIT /B 0

:build_arcpro20

set ARCGDALDIR=%GDALDIR%\gdal211\vc14\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2015_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arcpro20\%VCMACH%
mkdir build_arcpro20
cd build_arcpro20
  
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
rmdir /s /q build_arcpro20

EXIT /B 0

:build_arc106_arcpro21

set ARCGDALDIR=%GDALDIR%\gdal211b\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2017_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arc106_arcpro21\%VCMACH%
mkdir build_arc106_arcpro21
cd build_arc106_arcpro21
  
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
rmdir /s /q build_arc106_arcpro21
EXIT /B 0

:build_arc1061_arcpro22

set ARCGDALDIR=%GDALDIR%\gdal211c\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2017_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arc1061_arcpro22\%VCMACH%
mkdir build_arc1061_arcpro22
cd build_arc1061_arcpro22
  
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
rmdir /s /q build_arc1061_arcpro22
EXIT /B 0

:build_arc1071_arcpro24

set ARCGDALDIR=%GDALDIR%\gdal211e\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2017_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arc1071_arcpro24\%VCMACH%
mkdir build_arc1071_arcpro24
cd build_arc1071_arcpro24
  
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
rmdir /s /q build_arc1071_arcpro24
EXIT /B 0

:build_arc108_arcpro25

set ARCGDALDIR=%GDALDIR%\gdal233e\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2019_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arc108_arcpro25\%VCMACH%
mkdir build_arc108_arcpro25
cd build_arc108_arcpro25
  
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
rmdir /s /q build_arc108_arcpro25
EXIT /B 0

:build_arc1081_arcpro26
set ARCGDALDIR=%GDALDIR%\gdal233e_2\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2019_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arc1081_arcpro26\%VCMACH%
mkdir build_arc1081_arcpro26
cd build_arc1081_arcpro26
  
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
rmdir /s /q build_arc1081_arcpro26
EXIT /B 0

:build_arcpro27
set ARCGDALDIR=%GDALDIR%\gdal233e_3\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2019_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arcpro27\%VCMACH%
mkdir build_arcpro27
cd build_arcpro27
  
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
rmdir /s /q build_arcpro27
EXIT /B 0

:build_arcpro28
set ARCGDALDIR=%GDALDIR%\gdal233e_4\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2019_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arcpro28\%VCMACH%
mkdir build_arcpro28
cd build_arcpro28
  
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
rmdir /s /q build_arcpro28
EXIT /B 0

:build_arc109
set ARCGDALDIR=%GDALDIR%\gdal233e_5\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2019_%VCMACH%
set ARCHINSTALL=%OUTDIR%\arc109\%VCMACH%
mkdir build_arc109
cd build_arc109
  
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
rmdir /s /q build_arc109
EXIT /B 0

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
