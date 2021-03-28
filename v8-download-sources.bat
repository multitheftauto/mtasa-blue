@echo off

rem Update v8
utils\premake5.exe install_v8

if %0 == "%~0" pause
