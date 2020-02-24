@echo on
setlocal

:: Update these 3 variables. 
:: OUTDIR is where the output files will be put
SET OUTDIR=c:\dev\arckea\dist
:: GDALDIR is where arcgdal_for_compilation.zip (from the github downloads) as been unzipped to
SET GDALDIR=C:\dev\arcgdalforcompilation
:: HDF5DIR is the same as INSTALLDIR in buildzlibhdf5.bat
SET HDF5DIR=c:\dev\arckea

REM SetLocal
REM set VCMACH=x86
REM call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" %VCMACH%
REM @echo on
REM call :build_arc93
REM if errorlevel 1 exit /B 1
REM call :build_arc100
REM if errorlevel 1 exit /B 1
REM call :build_arc101
REM if errorlevel 1 exit /B 1
REM EndLocal

REM :: Now the vs2013 builds x86
REM SetLocal
REM set VCMACH=x86
REM call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" %VCMACH%
REM @echo on
REM call :build_arc104
REM if errorlevel 1 exit /B 1
REM call :build_arc105
REM if errorlevel 1 exit /B 1
REM call :build_arc1051
REM if errorlevel 1 exit /B 1
REM EndLocal

REM :: Now x64
REM SetLocal
REM set VCMACH=x64
REM :: Note VS2013 doesn't understand 'x64'...
REM call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
REM @echo on
REM call :build_arc104
REM if errorlevel 1 exit /B 1
REM call :build_arc105
REM if errorlevel 1 exit /B 1
REM call :build_arc1051
REM if errorlevel 1 exit /B 1
REM EndLocal

REM :: Visual Studio 2015 x64 builds - note 8.1 on the end of vcvarsall.bat - selects Windows SDK 8.1
REM :: which has a working rc.exe - removed in 10.1
REM SetLocal
REM set VCMACH=x64
REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %VCMACH% 8.1
REM @echo on
REM call :build_arcpro14
REM if errorlevel 1 exit /B 1
REM call :build_arcpro20
REM if errorlevel 1 exit /B 1
REM EndLocal

REM :: Visual Studio 2017 x86 Builds
REM :: ESRI Says "Visual Studio 2017 Update 2" which is 14.09(?) but this is the earliest 
REM :: I have installed.
REM SetLocal
REM set VCMACH=x86
REM call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -vcvars_ver=14.12 %VCMACH%
REM @echo on
REM call :build_arc106_arcpro21
REM if errorlevel 1 exit /B 1
REM EndLocal

REM :: Visual Studio 2017 x64 Builds
REM SetLocal
REM set VCMACH=x64
REM call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -vcvars_ver=14.12 %VCMACH%
REM @echo on
REM call :build_arc106_arcpro21
REM if errorlevel 1 exit /B 1
REM EndLocal

REM :: Visual Studio 2017 x86 Builds
REM :: ESRI Says "Visual Studio 2017 version 15.5.6" which is 14.11
REM SetLocal
REM set VCMACH=x86
REM call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -vcvars_ver=14.12 %VCMACH%
REM @echo on
REM call :build_arc1061_arcpro22
REM call :build_arc1071_arcpro24
REM if errorlevel 1 exit /B 1
REM EndLocal

REM :: Visual Studio 2017 x64 Builds
REM SetLocal
REM set VCMACH=x64
REM call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -vcvars_ver=14.12 %VCMACH%
REM @echo on
REM call :build_arc1061_arcpro22
REM call :build_arc1071_arcpro24
REM if errorlevel 1 exit /B 1
REM EndLocal

:: Visual Studio 2019 x86 Builds
SetLocal
set VCMACH=x86
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc108_arcpro25
if errorlevel 1 exit /B 1
EndLocal

:: Visual Studio 2019 x64 Builds
SetLocal
set VCMACH=x64
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc108_arcpro25
if errorlevel 1 exit /B 1
EndLocal

EXIT /B %ERRORLEVEL%

:build_arc93

set ARCGDALDIR=%GDALDIR%\gdal14
set ARCHDF5DIR=%HDF5DIR%\VC2008_x86
set ARCHINSTALL=%OUTDIR%\arc93
mkdir build_arc93
cd build_arc93

cmake -D KEA_PLUGIN_OUTOFTREE=ON ^
      -D CMAKE_INSTALL_PREFIX=%ARCHINSTALL% ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
	  -D GDAL_DIR=%ARCGDALDIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
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
rmdir /s /q build_arc93

EXIT /B 0

:build_arc100

set ARCGDALDIR=%GDALDIR%\gdal16
set ARCHDF5DIR=%HDF5DIR%\VC2008_x86
set ARCHINSTALL=%OUTDIR%\arc100
mkdir build_arc100
cd build_arc100

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
rmdir /s /q build_arc100

EXIT /B 0

:build_arc101

set ARCGDALDIR=%GDALDIR%\gdal18
set ARCHDF5DIR=%HDF5DIR%\VC2008_x86
set ARCHINSTALL=%OUTDIR%\arc101
mkdir build_arc101
cd build_arc101

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
rmdir /s /q build_arc101

EXIT /B 0

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
