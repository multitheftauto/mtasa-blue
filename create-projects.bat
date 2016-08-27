@echo off

rem Update CEF eventually
utils\premake5 install_cef

rem Generate solutions
utils\premake5 vs2015
pause
