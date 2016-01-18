@echo off
SET SOLUTIONPATH=%~1
SET OUTPATH=%~2

echo Maybe copying CEF libraries...
rem echo "%SOLUTIONPATH%data\MTA San Andreas\MTA\cef\cef.pak"
rem echo "%OUTPATH%\cef\cef.pak"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\cef\cef.pak" "%OUTPATH%\cef\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\cef\cef_100_percent.pak" "%OUTPATH%\cef\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\cef\cef_200_percent.pak" "%OUTPATH%\cef\"
rem xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\cef\cef_extensions.pak" "%OUTPATH%\cef\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\cef\devtools_resources.pak" "%OUTPATH%\cef\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\cef\locales\en-US.pak" "%OUTPATH%\cef\locales\"

xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\d3dcompiler_43.dll" "%OUTPATH%\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\d3dcompiler_47.dll" "%OUTPATH%\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\icudtl.dat" "%OUTPATH%\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\libcef.dll" "%OUTPATH%\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\libEGL.dll" "%OUTPATH%\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\libGLESv2.dll" "%OUTPATH%\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\natives_blob.bin" "%OUTPATH%\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\snapshot_blob.bin" "%OUTPATH%\"
xcopy /Y /D "%SOLUTIONPATH%\data\MTA San Andreas\MTA\wow_helper.exe" "%OUTPATH%\"