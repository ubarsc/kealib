@echo on
setlocal

:: This script builds the dependencies (zlib and hdf5) for kealib for ArcGIS
:: Download zlib and hdf5 sources and unzip them and set the ZLIBDIR and HDF5DIR
:: vars (below) to the locations. Also set INSTALLDIR to where you want the files to 
:: be installed to. 

:: You will also need Visual Studio 2008, 2013, 2015 and 2017. Express or Community
:: versions are fine.
:: I have assumed these are installed in the standard locations. If not you might need to tweak the code below.

set ZLIBDIR=C:\dev\arc\zlib-1.2.11
:: NB: HDF5 1.10.6 and later don't appear to compile with VS 2008
:: But earlier versions seem to have problems reporting threadsafety 
:: properly. So we build with latest where we can
set HDF5DIR_VS2008=C:\dev\arc\hdf5-1.10.5
set HDF5DIR_LATEST=C:\dev\arc\hdf5-1.10.6
set INSTALLDIR=c:\dev\arckea

:: Visual Studio 2013 x86 and x64
SetLocal
set VCYEAR=VC2013
set VCMACH=x86
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" %VCMACH%
@echo on
set HDF5DIR=%HDF5DIR_LATEST%
call :build
EndLocal

SetLocal
set VCYEAR=VC2013
set VCMACH=x64
:: Note VS2013 doesn't understand 'x64'...
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
@echo on
set HDF5DIR=%HDF5DIR_LATEST%
call :build
EndLocal

:: Visual Studio 2017 for ArcPro > 2.0, ArcGIS 10.6
SetLocal
set VCYEAR=VC2017
set VCMACH=x86
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -vcvars_ver=14.12 %VCMACH%
@echo on
set HDF5DIR=%HDF5DIR_LATEST%
call :build
EndLocal

SetLocal
set VCYEAR=VC2017
set VCMACH=x64
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" -vcvars_ver=14.12 %VCMACH%
@echo on
set HDF5DIR=%HDF5DIR_LATEST%
call :build
EndLocal

:: Visual Studio 2019 for ArcPro >= 2.5, ArcGIS 10.8
SetLocal
set VCYEAR=VC2019
set VCMACH=x86
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
set HDF5DIR=%HDF5DIR_LATEST%
call :build
EndLocal

SetLocal
set VCYEAR=VC2019
set VCMACH=x64
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %VCMACH%
@echo on
set HDF5DIR=%HDF5DIR_LATEST%
call :build
EndLocal

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

:: ZLIB
cd %ZLIBDIR%
mkdir %BUILDDIR%
cd %BUILDDIR%

:: Edit %ZLIBDIR%\CMakeLists.txt and change
:: set_target_properties(zlib PROPERTIES SUFFIX "1.dll")
:: to:
:: set_target_properties(zlib PROPERTIES SUFFIX "kea.dll")
cmake -G "NMake Makefiles" ^
    -D CMAKE_BUILD_TYPE=Release ^
    -D BUILD_SHARED_LIBS=ON ^
    -D CMAKE_INSTALL_PREFIX=%VCINSTALLDIR% ^
    ..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1

cd ..
rmdir /s /q %BUILDDIR%

::HDF5
cd %HDF5DIR%
mkdir %BUILDDIR%
cd %BUILDDIR%

set HDF5_EXT_ZLIB=zlib.lib
cmake -G "NMake Makefiles" ^
      -D HDF5_EXTERNAL_LIB_PREFIX=kea ^
      -D CMAKE_BUILD_TYPE=Release ^
      -D CMAKE_INSTALL_PREFIX=%VCINSTALLDIR% ^
      -D CMAKE_PREFIX_PATH=%VCINSTALLDIR% ^
      -D HDF5_BUILD_CPP_LIB=ON ^
      -D HDF5_ENABLE_Z_LIB_SUPPORT:BOOL=ON ^
      -D HDF5_BUILD_TOOLS=OFF ^
      -D HDF5_BUILD_EXAMPLES=OFF ^
      -D HDF5_ENABLE_THREADSAFE:BOOL=ON ^
      -D ALLOW_UNSUPPORTED:BOOL=ON ^
      -D BUILD_TESTING=OFF ^
      ..
if errorlevel 1 exit /B 1
nmake install
if errorlevel 1 exit /B 1      

cd ..
rmdir /s /q %BUILDDIR%

:: Even though we set HDF5_EXTERNAL_LIB_PREFIX, cmake won't pick up these files
:: Create copies without the prefix
for %%L IN (hdf5.lib,hdf5_cpp.lib,hdf5_hl.lib,hdf5_hl_cpp.lib) DO (
  COPY "%VCINSTALLDIR%\lib\kea%%L" "%VCINSTALLDIR%\lib\%%L"
  if errorlevel 1 exit /B 1      
)

     
EXIT /B 0
