
setlocal
set INSTALLDIR="C:\dev\keainstall"

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

nmake /f makefile2015_x86.vc clean
if errorlevel 1 exit /B 1

nmake /f makefile2015_x86.vc
if errorlevel 1 exit /B 1

mkdir %INSTALLDIR%\2015\x86
copy kea.dll %INSTALLDIR%\2015\x86
if errorlevel 1 exit /B 1

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64

nmake /f makefile2015_x64.vc clean
if errorlevel 1 exit /B 1

nmake /f makefile2015_x64.vc
if errorlevel 1 exit /B 1

mkdir %INSTALLDIR%\2015\x64
copy kea.dll %INSTALLDIR%\2015\x64
if errorlevel 1 exit /B 1

