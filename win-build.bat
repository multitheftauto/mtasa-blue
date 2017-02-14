@echo off

set MSBUILD_REG_KEY="HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\14.0"
set MSBUILD_REG_SUBKEY="MSBuildToolsPath"

rem Check if MSBuild is installed
reg query %MSBUILD_REG_KEY% /v %MSBUILD_REG_SUBKEY% > nul 2>&1
if %ERRORLEVEL%==1 (
	echo Could not find MSBuild. Make sure you have Visual Studio 2015 installed
	goto end
)

rem Find msbuild
for /f "skip=2 tokens=2,*" %%a in ('reg query %MSBUILD_REG_KEY% /v %MSBUILD_REG_SUBKEY%') do set MSBUILDPATH="%%bmsbuild.exe"
echo Found MSBuild at: %MSBUILDPATH%

rem Start compiling
%MSBUILDPATH% Build/MTASA.sln /property:Configuration=Release /m

:end
pause
