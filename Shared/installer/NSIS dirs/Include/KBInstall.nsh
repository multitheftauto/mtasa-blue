;
; Download and install KB hotfix (if not already installed)
;

;----------------------------------------
; In: ID = KB number
;     BASENAME = KB filename without path or extention
;     URL = Download URL
;     TEMPFILE = Download filename to use
;----------------------------------------
!macro __InstallKB ID BASENAME URL TEMPFILE
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
            ${ExecInstallKB} ${ID} ${BASENAME} ${TEMPFILE}
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
!macro _InstallKB ID BASENAME URL
    !insertmacro __InstallKB ${ID} ${BASENAME} ${URL} "$TEMP\${BASENAME}.msu"
!macroend
!define InstallKB `!insertmacro _InstallKB`


;----------------------------------------
; Run KB installer
; Uses technique described at https://dennisspan.com/slow-installation-of-msu-files-using-the-wusa-exe/
; In: ID = KB number
;     BASENAME = KB filename without path or extention
;     MSUFILE = Full .msu filename
;     BATFILE = Filename to use for temp .bat file
;----------------------------------------
!macro __ExecInstallKB ID BASENAME MSUFILE BATFILE
    Push $0
    Push $R1
    Delete "$TEMP\${BASENAME}.cab"
    ExecWait 'expand -f:* "${MSUFILE}" "$TEMP"'
    Sleep 500

    FileOpen $0 ${BATFILE} w
    FileWrite $0 "@echo off$\r$\n"
    FileWrite $0 'start "" /WAIT /B "$WINDIR\system32\dism.exe" /online /norestart /add-package /packagepath:"$TEMP\${BASENAME}.cab"'
    FileClose $0
    ${DisableX64FSRedirection}
    Exec ${BATFILE}
    ${ProcessWait} dism.exe 5000 $0         ; Wait for dism.exe to start (5 sec timout)
    BringToFront

    ; Show progress while dism.exe is running
    DetailPrint ""
    StrCpy $R1 0
    ${Do}
        IntOp $R1 $R1 + 1
        IntOp $0 $R1 & 0x0f
        ${If} $0 == 1
            ${DetailUpdate} "Please wait..."
            DetailPrint "Working..."
        ${EndIf}
        ${ProcessWaitClose} dism.exe 1500 $0
        ${If} $0 > 0
            StrCpy $R1 100
        ${EndIf}
        ${DetailUpdate} "Installing ${ID}: $R1% done..."
    ${LoopUntil} $R1 == 100

    ${EnableX64FSRedirection}
    Pop $R1
    Pop $0
!macroend
!macro _ExecInstallKB ID BASENAME MSUFILE
    !insertmacro __ExecInstallKB ${ID} ${BASENAME} ${MSUFILE} "$TEMP\RunMSU.bat"
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


