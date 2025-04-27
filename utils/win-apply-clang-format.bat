@echo off
:: What this batch script does:
::  * Fix tabs
::  * Fix line endings
::  * Remove DEVELOPERS in file header
::  * Remove inline from class method declarations
::  * Apply clang-format (ignoring _asm blocks and preprocessor directives)
::
:: Get clang-format.exe from https://prereleases.llvm.org/win-snapshots/
:: Get fnr.exe from https://findandreplace.io/download
::
set CLANGFORMAT="%~dp0\clang-format-r325576.exe"
set FNR="%~dp0\fnr.exe"

if not exist %CLANGFORMAT% (
    echo Cannot find %CLANGFORMAT%
    pause
    goto:eof
)
if not exist %FNR% (
    echo Cannot find %FNR%
    pause
    goto:eof
)

set FILEMASK=*.c,*.cc,*.cpp,*.h,*.hh,*.hpp
set FNROUTPUT=fnr_output.txt
set FINDANDREPLACE=%FNR% --cl --fileMask "%FILEMASK%" --alwaysUseEncoding "Windows-1252" --includeSubDirectories --logFile %FNROUTPUT%

call:doformat ..\Client
call:doformat ..\Server
call:doformat ..\Shared

if exist %FNROUTPUT% del %FNROUTPUT%
pause
goto:eof

::--------------------------------------------------------
::-- doformat
::--------------------------------------------------------
:doformat
echo Processing directory "%1"

:: Use 0A end-of-line marker for the duration of formatting
set EOL=\n
echo Conform line endings
call:searchInReplaced %FINDANDREPLACE% --dir "%1" --useRegEx --useEscapeChars --find "\r\n" --replace "%EOL%"
call:searchInReplaced %FINDANDREPLACE% --dir "%1" --useRegEx --useEscapeChars --find "\r" --replace "%EOL%"

echo Tabs to spaces
call:searchInReplaced %FINDANDREPLACE% --dir "%1" --useRegEx --find "\t" --replace "    "

echo Remove trailing spaces
call:searchInReplaced %FINDANDREPLACE% --dir "%1" --useRegEx --useEscapeChars --find " +%EOL%" --replace "%EOL%"

echo Remove inline from class methods
call:searchInReplaced %FINDANDREPLACE% --dir "%1" --useRegEx --find "^(class(.*%EOL%)*)( +)inline +((.*%EOL%)*})" --replace "$1$3$4"

echo Exclude _asm blocks
%FINDANDREPLACE% --dir "%1" --useRegEx --useEscapeChars --find "(^.*_asm *(%EOL%)? *{(.*%EOL%)*?.*}.*)" --replace "// clang-format off%EOL%//TEMP%EOL%$1%EOL%//TEMP%EOL%// clang-format on"

echo Exclude preprocessor directives
%FINDANDREPLACE% --dir "%1" --useRegEx --useEscapeChars --find "^( *?#(.*?\\%EOL%)*(.*?%EOL%))" --replace "#dummy%EOL%// clang-format off%EOL%//TEMP%EOL%$1//TEMP%EOL%// clang-format on%EOL%#dummy%EOL%"

echo Apply .clang-format
pushd %1
for /R %%f in (%FILEMASK%) do (
    echo "%%f"
    :: Run twice as some files (e.g. Client\game_sa\CCameraSA.h) require it (bug?)
    %CLANGFORMAT% -i %%f
    %CLANGFORMAT% -i %%f
)
popd

echo Undo excludes
call:searchInReplaced %FINDANDREPLACE% --dir "%1" --useRegEx --find "^( *#dummy%EOL%)* *// *clang-format off%EOL% *// *TEMP%EOL%" --replace ""
call:searchInReplaced %FINDANDREPLACE% --dir "%1" --useRegEx --find "%EOL% *// *TEMP%EOL% *// *clang-format on(%EOL% *#dummy)*" --replace ""

echo Restore line endings
%FINDANDREPLACE% --dir "%1" --useRegEx --useEscapeChars --find "%EOL%" --replace "\r\n"

goto:eof

::--------------------------------------------------------
::-- searchInReplaced
::--------------------------------------------------------
:searchInReplaced
:again
if exist %FNROUTPUT% del %FNROUTPUT%
%*
for /f "tokens=4 delims= " %%i in ('type %FNROUTPUT% ^| FINDSTR /c:"With Matches:"') do SET /a WithMatches=%%i
:: Repeat until no matches found
if %WithMatches% GTR 0 goto :again
goto:eof
