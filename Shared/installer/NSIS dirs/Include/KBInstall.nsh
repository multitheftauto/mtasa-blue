;
; Download and install KB hotfix (if not already installed)
;

;----------------------------------------
; In: ID = KB number
;     URL = Download URL
;     TEMPFILE = Temp filename to use
;----------------------------------------
!macro __InstallKB ID URL TEMPFILE
    Push $0
    ${IsKBInstalled} ${ID} $0
    ${If} $0 == 0
        DetailPrint "Installing ${ID} ..."
        NSISdl::download ${URL} ${TEMPFILE}
        Pop $0
        ${If} $0 != "success"
            DetailPrint "* Download of ${ID} failed:"
            DetailPrint "* $0"
            DetailPrint "* Installation continuing anyway"
        ${Else}
            ${ExecInstallKB} ${ID} ${TEMPFILE}
            ${IsKBInstalled} ${ID} $0
            ${If} $0 == 0
                DetailPrint "* Some error occured installing ${ID}"
                DetailPrint "* It is required in order to run Multi Theft Auto : San Andreas"
                DetailPrint "* Installation continuing anyway"
            ${EndIf}
        ${EndIf}
    ${EndIf}
    Pop $0
!macroend
!macro _InstallKB ID URL
    !insertmacro __InstallKB ${ID} ${URL} "$TEMP\${ID}.msu"
!macroend
!define InstallKB `!insertmacro _InstallKB`


;----------------------------------------
; Run KB installer
; In: ID = KB number
;     FILENAME = KB file
;----------------------------------------
!macro _ExecInstallKB ID FILENAME
    Push $0
    Push $1
    Push $R1
    StrCpy $1 "$TEMP\RunMSU.bat"
    FileOpen $0 $1 w
    FileWrite $0 "@echo off$\r$\n"
    FileWrite $0 "start $\"$\" wusa /quiet /norestart $\"${FILENAME}$\""
    FileClose $0
    ExecWait $1

    ; Wait for wusa.exe to start
    ${ProcessWait} wusa.exe 5000 $0

    ; Wait for slui.exe (license warning) to start and terminate it
    ${ProcessWait} slui.exe 4000 $0
    ${TerminateProcess} slui.exe $0
    Sleep 500
    ${TerminateProcess} slui.exe $0

    ; Show progress while wusa.exe is running
    DetailPrint ""
    StrCpy $R1 0
    ${Do}
        IntOp $R1 $R1 + 1
        IntOp $0 $R1 & 0x0f
        ${If} $0 == 1
            ${DetailUpdate} "Please wait..."
            DetailPrint "Working..."
        ${EndIf}
        ${ProcessWaitClose} wusa.exe 1000 $0
        ${If} $0 > 0
            StrCpy $R1 100
        ${EndIf}
        ${DetailUpdate} "Installing ${ID}: $R1% done..."
    ${LoopUntil} $0 > 0
    Pop $R1
    Pop $1
    Pop $0
!macroend
!define ExecInstallKB `!insertmacro _ExecInstallKB`


;----------------------------------------
; Determine if a KB hotfix is already installed
; In: ID = KB number
; Out: RESULT = (0 - no, 1 - yes)
;----------------------------------------
!macro _IsKBInstalled ID RESULT
    nsExec::ExecToStack 'cmd /Q /C "wmic qfe get hotfixid | findstr "^${ID}""'
    Pop ${RESULT} ; return value (it always 0 even if an error occured)
    Pop ${RESULT} ; command output
    ${If} ${RESULT} == ""
        StrCpy ${RESULT} 0
    ${Else}
        StrCpy ${RESULT} 1
    ${EndIf}
!macroend
!define IsKBInstalled `!insertmacro _IsKBInstalled`


