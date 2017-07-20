

:: Comment out the DLLBUILD=1 and ODBC_SUPPORTED=1 lines in nmake.opt
:: add ws2_32.lib and Psapi.lib to LINKER_FLAGS
setlocal
cd gdal-2.2.1

call "C:\Users\sam\AppData\Local\Programs\Common\Microsoft\Visual C++ for Python\9.0\vcvarsall.bat" x86

::nmake /f makefile.vc clean
::if errorlevel 1 exit /B 1

:: make clean doesn't completely work
d::el /s /q *.obj

nmake /f makefile.vc MSVC_VER=1500 GDAL_HOME=c:\dev\gdalstatic_2008_x86 devinstall
if errorlevel 1 exit /B 1

exit /b 1

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

nmake /f makefile.vc clean
if errorlevel 1 exit /B 1

:: make clean doesn't completely work
del /s /q *.obj

nmake /f makefile.vc MSVC_VER=1800 GDAL_HOME=c:\dev\gdalstatic_2013_x86 devinstall
if errorlevel 1 exit /B 1

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64

nmake /f makefile.vc clean
if errorlevel 1 exit /B 1

:: make clean doesn't completely work
del /s /q *.obj

nmake /f makefile.vc MSVC_VER=1800 GDAL_HOME=c:\dev\gdalstatic_2013_x64 WIN64=1 devinstall
if errorlevel 1 exit /B 1

cd ..
