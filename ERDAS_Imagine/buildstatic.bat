@echo on
:: Use this to build a static GDAL so the kea stuff can link against it and 
:: use the WKT stuff....

:: Open nmake.opt and comment out DLLBUILD line (no way to override this)
:: AND ODBC_SUPPORTED line

set BLD_OPTS=EXTRA_LINKER_FLAGS="ws2_32.lib Psapi.lib"

:: GDAL 2.2.4 is last version with VS 2008 support....
cd gdal-2.2.4\gdal
setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86

:: make clean doesn't completely work
del /s /q *.obj *.lib
del port\cpl_config.h

nmake /f makefile.vc MSVC_VER=1500 GDAL_HOME=c:\dev\arckea\gdalstatic_2008_x86 %BLD_OPTS% devinstall
if errorlevel 1 exit /B 1

endlocal
setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

:: make clean doesn't completely work
del /s /q *.obj *.lib
del port\cpl_config.h

nmake /f makefile.vc MSVC_VER=1800 GDAL_HOME=c:\dev\arckea\gdalstatic_2013_x86 %BLD_OPTS% devinstall
if errorlevel 1 exit /B 1

endlocal
setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64

:: make clean doesn't completely work
del /s /q *.obj *.lib
del port\cpl_config.h

nmake /f makefile.vc MSVC_VER=1800 GDAL_HOME=c:\dev\arckea\gdalstatic_2013_x64 WIN64=1 %BLD_OPTS% devinstall
if errorlevel 1 exit /B 1

endlocal
cd ..\..
