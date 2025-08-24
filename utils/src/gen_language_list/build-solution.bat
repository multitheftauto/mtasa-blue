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
call "%VSDEVCMD%" -arch=arm64 -host_arch=amd64 >NUL

where MSBuild.exe >NUL 2>&1
if %ERRORLEVEL% == 0 goto :Build
echo MSBuild not found
echo Make sure you've installed the 'Desktop development with C++' workload
pause
exit /b 1

:Build
call create-solution.bat < NUL

MSBuild.exe build\utility.sln ^
    -target:gen_language_list ^
    -property:Configuration=Release ^
    -property:Platform=x64 ^
    -maxCpuCount

copy build\bin\Release\gen_language_list.exe ..\..\ >NUL

if %0 == "%~0" pause
