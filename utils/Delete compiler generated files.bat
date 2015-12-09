::
:: SUPER CLEAN 3000
::
@echo off
cd ..
set RMDIR_RECURSIVE=rmdir /s /q

:: Ask for confirmation
set /P c=Do you want to clean all compiler generated directories and files from the source tree? (Y/N)
if /I "%c%" neq "Y" (
    exit
)

:: Location safety check
if not exist "MTA10_Server" (
    echo Error: MTA10_Server does not exist
    pause
    exit
)

@echo on
%RMDIR_RECURSIVE% symbols
%RMDIR_RECURSIVE% obj
%RMDIR_RECURSIVE% output
%RMDIR_RECURSIVE% MTA10\ceflauncher\x86
%RMDIR_RECURSIVE% MTA10\core\x86
%RMDIR_RECURSIVE% MTA10\game_sa\x86
%RMDIR_RECURSIVE% MTA10\gui\x86
%RMDIR_RECURSIVE% MTA10\launch\x86
%RMDIR_RECURSIVE% MTA10\loader\x86
%RMDIR_RECURSIVE% MTA10\multiplayer_sa\x86
%RMDIR_RECURSIVE% MTA10\mods\deathmatch\x86
%RMDIR_RECURSIVE% MTA10_server\core\x86
%RMDIR_RECURSIVE% MTA10_server\core\x64
%RMDIR_RECURSIVE% MTA10_server\dbconmy\x86
%RMDIR_RECURSIVE% MTA10_server\dbconmy\x64
%RMDIR_RECURSIVE% MTA10_server\launcher\x86
%RMDIR_RECURSIVE% MTA10_server\launcher\x64
%RMDIR_RECURSIVE% MTA10_server\mods\deathmatch\x86
%RMDIR_RECURSIVE% MTA10_server\mods\deathmatch\x64
%RMDIR_RECURSIVE% Shared\ipch
%RMDIR_RECURSIVE% Shared\XML\x86
%RMDIR_RECURSIVE% Shared\XML\x64
%RMDIR_RECURSIVE% vendor\cef3\out
%RMDIR_RECURSIVE% vendor\cegui-0.4.0-custom\x86
%RMDIR_RECURSIVE% vendor\cegui-0.4.0-custom\makefiles\win32\VC++7.1\BaseSystem\x86
%RMDIR_RECURSIVE% vendor\cegui-0.4.0-custom\makefiles\win32\VC++7.1\Renderers\DirectX9Renderer\x86
%RMDIR_RECURSIVE% vendor\cegui-0.4.0-custom\makefiles\win32\VC++7.1\WidgetSets\Falagard\x86
%RMDIR_RECURSIVE% vendor\cryptopp\x86
%RMDIR_RECURSIVE% vendor\cryptopp\x64
%RMDIR_RECURSIVE% vendor\curl\x86
%RMDIR_RECURSIVE% vendor\curl\x64
%RMDIR_RECURSIVE% vendor\curl\lib\DLL-Debug
%RMDIR_RECURSIVE% vendor\curl\lib\DLL-Release
%RMDIR_RECURSIVE% vendor\ehs\x86
%RMDIR_RECURSIVE% vendor\ehs\x64
%RMDIR_RECURSIVE% vendor\ehs\vs2008\Debug
%RMDIR_RECURSIVE% vendor\ehs\vs2008\Release
%RMDIR_RECURSIVE% vendor\jpeg\x86
%RMDIR_RECURSIVE% vendor\json-c\x86
%RMDIR_RECURSIVE% vendor\json-c\x64
%RMDIR_RECURSIVE% vendor\libpng\x86
%RMDIR_RECURSIVE% vendor\lua\x86
%RMDIR_RECURSIVE% vendor\lua\x64
%RMDIR_RECURSIVE% vendor\pcre\x86
%RMDIR_RECURSIVE% vendor\pcre\x64
%RMDIR_RECURSIVE% vendor\pme\x86
%RMDIR_RECURSIVE% vendor\pme\x64
%RMDIR_RECURSIVE% vendor\pme\vs2008\Debug
%RMDIR_RECURSIVE% vendor\pme\vs2008\Release
%RMDIR_RECURSIVE% vendor\pthreads\x86
%RMDIR_RECURSIVE% vendor\pthreads\x64
%RMDIR_RECURSIVE% vendor\unrar\build
%RMDIR_RECURSIVE% vendor\zlib\Win32_LIB_Debug
%RMDIR_RECURSIVE% vendor\zlib\Win32_LIB_Release
%RMDIR_RECURSIVE% vendor\zlib\x64
%RMDIR_RECURSIVE% vendor\zlib\x86
pause
