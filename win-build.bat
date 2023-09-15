@echo off
set VSWHERE=utils\vswhere.exe

rem Download vswhere (command line utility to find MSBuild path)
if not exist %VSWHERE% (
	echo vswhere utility doesn't exist. Downloading...
	powershell -Command "Invoke-WebRequest https://mirror-cdn.multitheftauto.com/bdata/vswhere.exe -OutFile %VSWHERE%"
)

rem Find MSBuild installation path
for /f "usebackq tokens=1* delims=: " %%i in (`%VSWHERE% -latest -requires Microsoft.Component.MSBuild`) do (
	if /i "%%i"=="installationPath" set INSTALLDIR=%%j
)

rem Output an error if not exists
set MSBUILDPATH="%InstallDir%\MSBuild\Current\Bin\MSBuild.exe"
if not exist %MSBUILDPATH% (
	echo Could not find MSBuild. Make sure you have Visual Studio 2022 installed
	goto end
)
echo Found MSBuild at: %MSBUILDPATH%

set BUILD_CONFIGURATION=Release
set BUILD_PLATFORM=Win32

rem Read configuration (1st parameter)
IF /i [%1] == [Debug] (
    set BUILD_CONFIGURATION=Debug
) ELSE IF /i [%1] == [Release] (
    set BUILD_CONFIGURATION=Release
) ELSE (
    IF not [%1] == [] (
        echo Invalid first argument %1. Using default configuration %BUILD_CONFIGURATION%.
    )
)

rem Read platform (2nd parameter)
IF /i [%2] == [Win32] (
    set BUILD_PLATFORM=Win32
) ELSE IF /i [%2] == [x64] (
    set BUILD_PLATFORM=x64
) ELSE IF /i [%2] == [ARM] (
    set BUILD_PLATFORM=ARM
) ELSE IF /i [%2] == [ARM64] (
    set BUILD_PLATFORM=ARM64
) ELSE (
    IF not [%2] == [] (
        echo Invalid first argument %2. Using default platform %BUILD_PLATFORM%.
    )
)

echo Build configuration:
echo   BUILD_CONFIGURATION = %BUILD_CONFIGURATION%
echo   BUILD_PLATFORM = %BUILD_PLATFORM%

rem Create solution (ignoring pause)
call win-create-projects.bat < nul
echo.

rem Start compiling
%MSBUILDPATH% Build/MTASA.sln ^
    -property:Configuration="%BUILD_CONFIGURATION%" ^
    -property:Platform="%BUILD_PLATFORM%" ^
    -maxCpuCount

:end
pause
