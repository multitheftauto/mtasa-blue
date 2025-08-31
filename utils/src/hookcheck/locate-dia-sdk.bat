@echo off

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" goto :FindVisualStudio
exit /b 1

:FindVisualStudio
for /f "usebackq tokens=*" %%i in (`call "%VSWHERE%" -products * -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set VSDIR=%%i
)

if defined VSDIR goto :CheckPath
exit /b 2

:CheckPath
if exist "%VSDIR%\DIA SDK\" goto :PrintPath
exit /b 3

:PrintPath
echo %VSDIR%\DIA SDK\
exit /b 0
