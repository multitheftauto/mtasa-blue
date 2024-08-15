@echo off

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" goto :FindVisualStudio

echo Unable to locate Visual Studio: vswhere not found
echo ^>^> %VSWHERE%
pause
exit /b 1

:FindVisualStudio
for /f "usebackq tokens=*" %%i in (`call "%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find "**\VsDevCmd.bat"`) do (
    set VSDEVCMD=%%i
)

if defined VSDEVCMD goto :SetupEnv
echo Visual Studio not found
echo Make sure you've installed the 'Desktop development with C++' workload
pause
exit /b 1

:SetupEnv
echo Found Visual Studio environment setup batch file:
echo ^>^> %VSDEVCMD%
start "VsDevCmd: arm64" "%comspec%" /k "%VSDEVCMD%" -arch=arm64 -host_arch=amd64
start "VsDevCmd: x64" "%comspec%" /k "%VSDEVCMD%" -arch=amd64 -host_arch=amd64
start "VsDevCmd: x86" "%comspec%" /k "%VSDEVCMD%" -arch=x86 -host_arch=amd64
