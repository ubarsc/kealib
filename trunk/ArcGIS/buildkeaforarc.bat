@echo on
setlocal

:: Update these 3 variables. 
:: OUTDIR is where the output files will be put
SET OUTDIR=c:\dev\arckea\dist
:: GDALDIR is where arcgdal_for_compilation.zip (from the bitbucket downloads) as been unzipped to
SET GDALDIR=C:\dev\arcgdalforcompilation
:: HDF5DIR is the same as INSTALLDIR in buildzlibhdf5.bat
SET HDF5DIR=c:\dev\arckea

set VCMACH=x86
call "C:\Users\sam\AppData\Local\Programs\Common\Microsoft\Visual C++ for Python\9.0\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc93
if errorlevel 1 exit /B 1
call :build_arc100
if errorlevel 1 exit /B 1
call :build_arc101
if errorlevel 1 exit /B 1

:: Now the vs2013 builds x86
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" %VCMACH%
@echo on
call :build_arc104
if errorlevel 1 exit /B 1
call :build_arc105
if errorlevel 1 exit /B 1
call :build_arc1051

:: Now x64
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

:: Visual Studio 2015 x64 builds
set VCMACH=x64
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %VCMACH%
@echo on
call :build_arcpro14
if errorlevel 1 exit /B 1
call :build_arcpro20
if errorlevel 1 exit /B 1

EXIT /B %ERRORLEVEL%

:build_arc93

set ARCGDALDIR=%GDALDIR%\gdal14
set ARCHDF5DIR=%HDF5DIR%\VC2008_x86
mkdir build_arc93
cd build_arc93

cmake -D HDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc93 ^
      -D GDAL_INCLUDE_DIR=%ARCGDALDIR%\include ^
      -D GDAL_LIB_PATH=%ARCGDALDIR%\lib ^
      -D HDF5_INCLUDE_DIR=%ARCHDF5DIR%\include ^
      -D HDF5_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..
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

cmake -D HDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc100 ^
      -D GDAL_INCLUDE_DIR=%ARCGDALDIR%\include ^
      -D GDAL_LIB_PATH=%ARCGDALDIR%\lib ^
      -D HDF5_INCLUDE_DIR=%ARCHDF5DIR%\include ^
      -D HDF5_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..
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

cmake -D HDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc101 ^
      -D GDAL_INCLUDE_DIR=%ARCGDALDIR%\include ^
      -D GDAL_LIB_PATH=%ARCGDALDIR%\lib ^
      -D HDF5_INCLUDE_DIR=%ARCHDF5DIR%\include ^
      -D HDF5_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc101

EXIT /B 0

:build_arc104

set ARCGDALDIR=%GDALDIR%\gdal18b
set ARCHDF5DIR=%HDF5DIR%\VC2013_%VCMACH%
mkdir build_arc104
cd build_arc104

IF "%VCMACH%" == "x86" (
  SET GDALLIBPATH=%ARCGDALDIR%\lib 
) ELSE (
  SET GDALLIBPATH=%ARCGDALDIR%\lib\amd64 
)
  
cmake -D HDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc104\%VCMACH% ^
      -D GDAL_INCLUDE_DIR=%ARCGDALDIR%\include ^
      -D GDAL_LIB_PATH=%GDALLIBPATH% ^
      -D HDF5_INCLUDE_DIR=%ARCHDF5DIR%\include ^
      -D HDF5_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc104

EXIT /B 0

:build_arc105

set ARCGDALDIR=%GDALDIR%\gdal201
set ARCHDF5DIR=%HDF5DIR%\VC2013_%VCMACH%
mkdir build_arc105
cd build_arc105

IF "%VCMACH%" == "x86" (
  SET GDALLIBPATH=%ARCGDALDIR%\lib\vc12 
) ELSE (
  SET GDALLIBPATH=%ARCGDALDIR%\lib\vc12\x64 
)
  
cmake -D HDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc105\%VCMACH% ^
      -D GDAL_INCLUDE_DIR=%ARCGDALDIR%\include ^
      -D GDAL_LIB_PATH=%GDALLIBPATH% ^
      -D HDF5_INCLUDE_DIR=%ARCHDF5DIR%\include ^
      -D HDF5_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc105

EXIT /B 0

:build_arc1051

set ARCGDALDIR=%GDALDIR%\gdal211
set ARCHDF5DIR=%HDF5DIR%\VC2013_%VCMACH%
mkdir build_arc1051
cd build_arc1051

IF "%VCMACH%" == "x86" (
  SET GDALLIBPATH=%ARCGDALDIR%\lib\vc12 
) ELSE (
  SET GDALLIBPATH=%ARCGDALDIR%\lib\vc12\x64 
)
  
cmake -D HDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arc1051\%VCMACH% ^
      -D GDAL_INCLUDE_DIR=%ARCGDALDIR%\include ^
      -D GDAL_LIB_PATH=%GDALLIBPATH% ^
      -D HDF5_INCLUDE_DIR=%ARCHDF5DIR%\include ^
      -D HDF5_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arc1051

EXIT /B 0

:build_arcpro14

set ARCGDALDIR=%GDALDIR%\gdal201
set ARCHDF5DIR=%HDF5DIR%\VC2015_%VCMACH%
mkdir build_arcpro14
cd build_arcpro14

IF "%VCMACH%" == "x86" (
  SET GDALLIBPATH=%ARCGDALDIR%\lib\vc14 
) ELSE (
  SET GDALLIBPATH=%ARCGDALDIR%\lib\vc14\x64 
)
  
cmake -D HDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arcpro14\%VCMACH% ^
      -D GDAL_INCLUDE_DIR=%ARCGDALDIR%\include ^
      -D GDAL_LIB_PATH=%GDALLIBPATH% ^
      -D HDF5_INCLUDE_DIR=%ARCHDF5DIR%\include ^
      -D HDF5_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arcpro14

EXIT /B 0

:build_arcpro20

set ARCGDALDIR=%GDALDIR%\gdal211
set ARCHDF5DIR=%HDF5DIR%\VC2015_%VCMACH%
mkdir build_arcpro20
cd build_arcpro20

IF "%VCMACH%" == "x86" (
  SET GDALLIBPATH=%ARCGDALDIR%\lib\vc14 
) ELSE (
  SET GDALLIBPATH=%ARCGDALDIR%\lib\vc14\x64 
)
  
cmake -D HDF5_STATIC_LIBS=TRUE ^
      -D CMAKE_INSTALL_PREFIX=%OUTDIR%\arcpro20\%VCMACH% ^
      -D GDAL_INCLUDE_DIR=%ARCGDALDIR%\include ^
      -D GDAL_LIB_PATH=%GDALLIBPATH% ^
      -D HDF5_INCLUDE_DIR=%ARCHDF5DIR%\include ^
      -D HDF5_LIB_PATH=%ARCHDF5DIR%\lib ^
      -D CMAKE_BUILD_TYPE=Release ^
      -G "NMake Makefiles" ^
      ..\..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1 

cd ..
rmdir /s /q build_arcpro20

EXIT /B 0
