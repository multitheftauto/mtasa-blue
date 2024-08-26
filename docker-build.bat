@echo off

set NAME=mtasa

docker build . -t "%NAME%"
for /f "delims=" %%i in ('docker images -q %NAME%') do set ID=%%i

echo.
echo ID: %ID%