@echo off
set VSWHERE=utils\vswhere.exe

rem Download vswhere (command line utility to find MSBuild path)
if not exist %VSWHERE% (
	echo vswhere utility doesn't exist. Downloading...
	powershell -Command "Invoke-WebRequest https://mirror.mtasa.com/bdata/vswhere.exe -OutFile %VSWHERE%"
)

rem Create solution (ignoring pause)
call win-create-projects.bat < nul
echo.

rem Find MSBuild installation path
for /f "usebackq tokens=1* delims=: " %%i in (`%VSWHERE% -latest -requires Microsoft.Component.MSBuild`) do (
	if /i "%%i"=="installationPath" set INSTALLDIR=%%j
)

rem Output an error if not exists
set MSBUILDPATH="%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe"
if not exist %MSBUILDPATH% (
	echo Could not find MSBuild. Make sure you have Visual Studio 2017 installed
	goto end
)
echo Found MSBuild at: %MSBUILDPATH%

rem Start compiling
%MSBUILDPATH% Build/MTASA.sln /property:Configuration=Release /m

:end
pause
