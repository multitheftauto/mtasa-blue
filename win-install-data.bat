@echo off

rem Optionally install resources
echo.
set ANSWER=n
set /P ANSWER="Install resources? [y/N] "
if /I "%ANSWER%"=="y" (
    utils\premake5 install_resources
)

if %0 == "%~0" pause
