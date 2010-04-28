@echo off
REM This builds all the libraries of the folder for 1 uname

call tecmake %1 %2 %3 %4 %5 %6 %7 %8

if "%1"==""         goto luaexe
if "%1"=="vc6"      goto luaexe
if "%1"=="vc8_64"   goto luaexe_64
if "%1"=="dll8_64"   goto luadll8_64
if "%1"=="dll"      goto luadll
if "%1"=="all"  goto luaexe
goto end

:luaexe
call tecmake vc6 "MF=lua" %2 %3 %4 %5 %6 %7
call tecmake vc6 "MF=luac" %2 %3 %4 %5 %6 %7
call tecmake vc6 "MF=bin2c" %2 %3 %4 %5 %6 %7
if "%1"=="all"  goto luaexe_64
goto end

:luaexe_64
call tecmake vc8_64 "MF=lua" %2 %3 %4 %5 %6 %7
call tecmake vc8_64 "MF=luac" %2 %3 %4 %5 %6 %7
call tecmake vc8_64 "MF=bin2c" %2 %3 %4 %5 %6 %7
if "%1"=="all"  goto luadll8_64
goto end

:luadll8_64
copy /Y ..\lib\dll8_64\*.dll ..\bin\Win64
if "%1"=="all"  goto luadll
goto end

:luadll
copy /Y ..\lib\dll\*.dll ..\bin\Win32
call tecmake mingw3 mingw3-dll
call tecmake bc56 bc56-dll
goto end

:end
