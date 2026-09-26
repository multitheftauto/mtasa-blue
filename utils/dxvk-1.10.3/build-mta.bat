@echo off

setlocal EnableExtensions EnableDelayedExpansion

set "TARGET=%~f1"
if "%TARGET%"=="" set "TARGET=%~dp0..\..\Bin\mta\dxvk"

if exist "%~dp0d3d9.dll" if exist "%~dp0dxgi.dll" (
    echo [DXVK] Using ready-made d3d9.dll + dxgi.dll from %~dp0
    call :publish "%~dp0d3d9.dll" "%~dp0dxgi.dll"
    if errorlevel 1 exit /b 1
    exit /b 0
)

where meson >nul 2>nul && where ninja >nul 2>nul && where i686-w64-mingw32-gcc >nul 2>nul && where glslangValidator >nul 2>nul && goto :detect_done

set "TOOL_PATH="
call :detect_tools
if defined TOOL_PATH set "PATH=!TOOL_PATH:~1!;!PATH!"

where meson >nul 2>nul && where ninja >nul 2>nul && where i686-w64-mingw32-gcc >nul 2>nul && where glslangValidator >nul 2>nul && goto :detect_done
set "TOOL_PATH="
call :deep_scan
if defined TOOL_PATH set "PATH=!TOOL_PATH:~1!;!PATH!"

:detect_done
where meson                     >nul 2>nul || goto :missing
where ninja                     >nul 2>nul || goto :missing
where i686-w64-mingw32-gcc      >nul 2>nul || goto :missing
where i686-w64-mingw32-g++      >nul 2>nul || goto :missing
where i686-w64-mingw32-windres  >nul 2>nul || goto :missing
where glslangValidator          >nul 2>nul || goto :missing

echo [DXVK] Building d3d9.dll + dxgi.dll from source ^(utils\dxvk-1.10.3^)...

set "SRC=%~dp0"
if "%SRC:~-1%"=="\" set "SRC=%SRC:~0,-1%"
set "BUILD=%SRC%\build.m32"
set "CROSS=%SRC%\build-win32.txt"

if not exist "%BUILD%\build.ninja" (
    meson setup "%BUILD%" "%SRC%" --cross-file "%CROSS%" --buildtype release ^
        -Denable_d3d9=true -Denable_dxgi=true -Denable_d3d11=true -Denable_d3d10=false -Denable_tests=false ^
        -Dcpp_args="-include cstdint"
    if errorlevel 1 goto :failed
)

ninja -C "%BUILD%" src/d3d9/d3d9.dll src/dxgi/dxgi.dll
if errorlevel 1 ninja -C "%BUILD%"
if errorlevel 1 goto :failed

i686-w64-mingw32-strip --strip-unneeded "%BUILD%\src\d3d9\d3d9.dll"
i686-w64-mingw32-strip --strip-unneeded "%BUILD%\src\dxgi\dxgi.dll"

call :publish "%BUILD%\src\d3d9\d3d9.dll" "%BUILD%\src\dxgi\dxgi.dll"
if errorlevel 1 goto :failed
exit /b 0

:missing
echo [DXVK] Skipped - DXVK build tools not found ^(Meson, Ninja, MinGW-w64, glslang^).
echo [DXVK] Install them to build utils\dxvk-1.10.3 from source ^(see build-mta.bat header^).
exit /b 0

:failed
echo [DXVK] ERROR: DXVK build failed - see output above.
exit /b 1

:publish
if not exist "%TARGET%" mkdir "%TARGET%"
copy /Y "%~1" "%TARGET%\" >nul
if errorlevel 1 exit /b 1
copy /Y "%~2" "%TARGET%\" >nul
if errorlevel 1 exit /b 1

echo [DXVK] d3d9.dll + dxgi.dll installed into %TARGET%
exit /b 0

:detect_tools
call :add_python_scripts "%APPDATA%\Python"
call :add_python_scripts "%LOCALAPPDATA%\Programs\Python"
call :add_python_scripts "%ProgramFiles%\Python"
call :add_python_scripts "%ProgramFiles(x86)%\Python"
if defined PYTHONHOME call :add_tool_dir "%PYTHONHOME%\Scripts"
if defined PYTHON_HOME call :add_tool_dir "%PYTHON_HOME%\Scripts"

call :add_tool_dir "%USERPROFILE%\scoop\shims"
call :add_tool_dir "%LOCALAPPDATA%\Microsoft\WinGet\Links"
call :add_tool_dir "%ProgramData%\chocolatey\bin"

call :add_tool_dir "C:\MinGW\bin"
call :add_tool_dir "C:\mingw\bin"
call :add_tool_dir "C:\mingw32\bin"

call :add_tool_dir "C:\msys64\mingw32\bin"
call :add_tool_dir "C:\msys64\usr\bin"
call :add_tool_dir "C:\msys32\mingw32\bin"
call :add_tool_dir "%ProgramFiles%\msys64\mingw32\bin"
call :add_tool_dir "%ProgramFiles%\msys64\usr\bin"

if defined VULKAN_SDK call :add_tool_dir "%VULKAN_SDK%\Bin"
if defined VULKAN_SDK call :add_tool_dir "%VULKAN_SDK%\bin"
call :add_sdk_versions "%ProgramFiles%\VulkanSDK"
call :add_sdk_versions "%ProgramFiles(x86)%\VulkanSDK"
call :add_sdk_versions "%LOCALAPPDATA%\VulkanSDK"
exit /b 0

:deep_scan
call :add_recursive "%USERPROFILE%\w64devkit"
call :add_recursive "C:\w64devkit"
call :add_recursive "C:\mingw-w64"
call :add_recursive "%USERPROFILE%\.local"
call :add_recursive "%USERPROFILE%\scoop\apps"
exit /b 0

:add_tool_dir
if not exist "%~1" exit /b 0
if exist "%~1\meson.exe"                goto :add_tool_dir_yes
if exist "%~1\ninja.exe"                goto :add_tool_dir_yes
if exist "%~1\i686-w64-mingw32-gcc.exe" goto :add_tool_dir_yes
if exist "%~1\glslangValidator.exe"     goto :add_tool_dir_yes
exit /b 0
:add_tool_dir_yes
set "TOOL_PATH=!TOOL_PATH!;%~1"
exit /b 0

:add_python_scripts
if not exist "%~1" exit /b 0
for /d %%V in ("%~1\Python*") do (
    if exist "%%V\Scripts\meson.exe" set "TOOL_PATH=!TOOL_PATH!;%%V\Scripts"
    if exist "%%V\Scripts\ninja.exe" set "TOOL_PATH=!TOOL_PATH!;%%V\Scripts"
)
exit /b 0

:add_sdk_versions
if not exist "%~1" exit /b 0
for /d %%V in ("%~1\*") do (
    if exist "%%V\Bin\glslangValidator.exe" set "TOOL_PATH=!TOOL_PATH!;%%V\Bin"
)
exit /b 0

:add_recursive
if not exist "%~1" exit /b 0
for /r "%~1" %%F in (meson.exe ninja.exe i686-w64-mingw32-gcc.exe glslangValidator.exe) do (
    if exist "%%F" set "TOOL_PATH=!TOOL_PATH!;%%~dpF"
)
exit /b 0