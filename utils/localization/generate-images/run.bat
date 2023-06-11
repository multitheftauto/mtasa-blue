@SETLOCAL ENABLEEXTENSIONS
@CD /D "%~dp0"
@ECHO OFF

nvm use 18
TIMEOUT /T 2
call npm ci

START serve.bat
TIMEOUT /T 2

ROBOCOPY "../../../Shared/data/MTA San Andreas/MTA/locale" "locale" /S /E

node generate
IF %ERRORLEVEL% NEQ 0 (
   ECHO Got error level %errorlevel%
   TASKKILL /IM python.exe /F
   EXIT /B %errorlevel%
)
TASKKILL /IM python.exe /F

nvm use 16
TIMEOUT /T 2

node --no-experimental-fetch minify

RMDIR /S /Q locale uncompressed

PAUSE
