@echo on
setlocal

:: Update these 3 variables. 
:: OUTDIR is where the output files will be put
SET OUTDIR=c:\dev\arckea\dist
:: GDALDIR is where arcgdal_for_compilation.zip (from the bitbucket downloads) as been unzipped to
SET GDALDIR=C:\dev\arcgdalforcompilation
:: HDF5DIR is the same as INSTALLDIR in buildzlibhdf5.bat
SET HDF5DIR=c:\dev\arckea

:: Stop PATH getting too long by resetting it
set oldpath=%PATH%

SetLocal
set VCMACH=x86
set PATH=%oldpath%
call "C:\Users\sam\AppData\Local\Programs\Common\Microsoft\Visual C++ for Python\9.0\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc93
if errorlevel 1 exit /B 1
call :build_arc100
if errorlevel 1 exit /B 1
call :build_arc101
if errorlevel 1 exit /B 1
EndLocal

:: Now the vs2013 builds x86
SetLocal
set VCMACH=x86
set PATH=%oldpath%
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
set PATH=%oldpath%
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
@echo on
call :build_arc104
if errorlevel 1 exit /B 1
call :build_arc105
if errorlevel 1 exit /B 1
call :build_arc1051
if errorlevel 1 exit /B 1
EndLocal

:: Visual Studio 2015 x64 builds
SetLocal
set VCMACH=x64
set PATH=%oldpath%
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %VCMACH%
@echo on
call :build_arcpro14
if errorlevel 1 exit /B 1
call :build_arcpro20
if errorlevel 1 exit /B 1
EndLocal

:: Visual Studio 2017 x86 Builds
SetLocal
set VCMACH=x86
set PATH=%oldpath%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc106_arcpro21
if errorlevel 1 exit /B 1
EndLocal

:: Visual Studio 2017 x64 Builds
SetLocal
set VCMACH=x64
set PATH=%oldpath%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc106_arcpro21
if errorlevel 1 exit /B 1
EndLocal

EXIT /B %ERRORLEVEL%

:build_arc93

set ARCGDALDIR=%GDALDIR%\gdal14
set ARCHDF5DIR=%HDF5DIR%\VC2008_x86
mkdir build_arc93
cd build_arc93

cmake -D KEAHDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc93 ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
	  -D GDAL_DIR=%ARCGDALDIR% ^
	  -D HDF5_ROOT=%ARCHDF5DIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..\gdal
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc93

EXIT /B 0

:build_arc100

set ARCGDALDIR=%GDALDIR%\gdal16
set ARCHDF5DIR=%HDF5DIR%\VC2008_x86
mkdir build_arc100
cd build_arc100

cmake -D KEAHDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc100 ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
	  -D GDAL_DIR=%ARCGDALDIR% ^
	  -D HDF5_ROOT=%ARCHDF5DIR% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..\gdal
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc100

EXIT /B 0

:build_arc101

set ARCGDALDIR=%GDALDIR%\gdal18
set ARCHDF5DIR=%HDF5DIR%\VC2008_x86
mkdir build_arc101
cd build_arc101

cmake -D KEAHDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc101 ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
	  -D GDAL_DIR=%ARCGDALDIR% ^
	  -D HDF5_ROOT=%ARCHDF5DIR% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..\gdal
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc101

EXIT /B 0

:build_arc104

set ARCGDALDIR=%GDALDIR%\gdal18b\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2013_%VCMACH%
mkdir build_arc104
cd build_arc104

cmake -D KEAHDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc104\%VCMACH% ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D GDAL_DIR=%ARCGDALDIR% ^
	  -D HDF5_ROOT=%ARCHDF5DIR% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..\gdal
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc104

EXIT /B 0

:build_arc105

set ARCGDALDIR=%GDALDIR%\gdal201\vc12\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2013_%VCMACH%
mkdir build_arc105
cd build_arc105

cmake -D KEAHDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc105\%VCMACH% ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
	  -D GDAL_DIR=%ARCGDALDIR% ^
	  -D HDF5_ROOT=%ARCHDF5DIR% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..\gdal
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc105

EXIT /B 0

:build_arc1051

set ARCGDALDIR=%GDALDIR%\gdal211\vc12\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2013_%VCMACH%
mkdir build_arc1051
cd build_arc1051

cmake -D KEAHDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc1051\%VCMACH% ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D GDAL_DIR=%ARCGDALDIR% ^
	  -D HDF5_ROOT=%ARCHDF5DIR% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..\gdal
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc1051

EXIT /B 0

:build_arcpro14

set ARCGDALDIR=%GDALDIR%\gdal201\vc14\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2015_%VCMACH%
mkdir build_arcpro14
cd build_arcpro14

cmake -D KEAHDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arcpro14\%VCMACH% ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D GDAL_DIR=%ARCGDALDIR% ^
	  -D HDF5_ROOT=%ARCHDF5DIR% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..\gdal
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arcpro14

EXIT /B 0

:build_arcpro20

set ARCGDALDIR=%GDALDIR%\gdal211\vc14\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2015_%VCMACH%
mkdir build_arcpro20
cd build_arcpro20
  
cmake -D KEAHDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arcpro20\%VCMACH% ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D GDAL_DIR=%ARCGDALDIR% ^
	  -D HDF5_ROOT=%ARCHDF5DIR% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..\gdal
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arcpro20

EXIT /B 0

:build_arc106_arcpro21

set ARCGDALDIR=%GDALDIR%\gdal211b\%VCMACH%
set ARCHDF5DIR=%HDF5DIR%\VC2017_%VCMACH%
mkdir build_arc106_arcpro21
cd build_arc106_arcpro21
  
cmake -D KEAHDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc106_arcpro21\%VCMACH% ^
	  -D CMAKE_PREFIX_PATH=%ARCGDALDIR% ^
      -D LIBKEA_HEADERS_DIR=%ARCHDF5DIR%\include ^
      -D LIBKEA_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D GDAL_DIR=%ARCGDALDIR% ^
	  -D HDF5_ROOT=%ARCHDF5DIR% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..\gdal
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc106_arcpro21

EXIT /B 0
