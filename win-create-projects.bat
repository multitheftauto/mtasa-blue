@echo off

rem Update CEF eventually
utils\premake5.exe install_cef

rem Update Unifont
utils\premake5.exe install_unifont

rem Update discord-rpc
utils\premake5.exe install_discord

rem Generate solutions
utils\premake5.exe vs2026

rem Create symlink from utils\settings.VisualStudio.json to Build\settings.VisualStudio.json
if exist "%~dp0\Build\settings.VisualStudio.json" del "%~dp0\Build\settings.VisualStudio.json"
mklink /H "%~dp0\Build\settings.VisualStudio.json" "%~dp0\utils\settings.VisualStudio.json"

rem Create a shortcut to the solution - https://superuser.com/questions/392061/how-to-make-a-shortcut-from-cmd
set SCRIPTFILE="%TEMP%\CreateMyShortcut.vbs"
(
  echo Set oWS = WScript.CreateObject^("WScript.Shell"^)
  echo sLinkFile = oWS.ExpandEnvironmentStrings^("MTASA.sln - Shortcut.lnk"^)
  echo Set oLink = oWS.CreateShortcut^(sLinkFile^)
  echo oLink.TargetPath = oWS.ExpandEnvironmentStrings^("%~dp0\Build\MTASA.sln"^)
  echo oLink.Save
) 1>%SCRIPTFILE%
cscript //nologo %SCRIPTFILE%
del /f /q %SCRIPTFILE%

if %0 == "%~0" pause
