
#####################################################################################################
# From http://nsis.sourceforge.net/GetTimeStamp
!ifndef TimeStamp
!define TimeStamp "!insertmacro _TimeStamp"
!macro _TimeStamp FormatedString
    !ifdef __UNINSTALL__
        Call un.__TimeStamp
    !else
        Call __TimeStamp
    !endif
    Pop ${FormatedString}
!macroend
 
!macro __TimeStamp UN
Function ${UN}__TimeStamp
    ClearErrors
    ## Store the needed Registers on the stack
    Push $0 ; Stack $0
    Push $1 ; Stack $1 $0
    Push $2 ; Stack $2 $1 $0
    Push $3 ; Stack $3 $2 $1 $0
    Push $4 ; Stack $4 $3 $2 $1 $0
    Push $5 ; Stack $5 $4 $3 $2 $1 $0
    Push $6 ; Stack $6 $5 $4 $3 $2 $1 $0
    Push $7 ; Stack $7 $6 $5 $4 $3 $2 $1 $0
    ;Push $8 ; Stack $8 $7 $6 $5 $4 $3 $2 $1 $0

    ## Call System API to get the current system Time
    System::Alloc 16
    Pop $0
    System::Call 'kernel32::GetLocalTime(i) i(r0)'
    System::Call '*$0(&i2, &i2, &i2, &i2, &i2, &i2, &i2, &i2)i (.r1, .r2, n, .r3, .r4, .r5, .r6, .r7)'
    System::Free $0

    IntFmt $2 "%02i" $2
    IntFmt $3 "%02i" $3
    IntFmt $4 "%02i" $4
    IntFmt $5 "%02i" $5
    IntFmt $6 "%02i" $6

    ## Generate Timestamp
    ;StrCpy $0 "YEAR=$1$\nMONTH=$2$\nDAY=$3$\nHOUR=$4$\nMINUITES=$5$\nSECONDS=$6$\nMS$7"
    StrCpy $0 "$1-$2-$3 $4:$5:$6"

    ## Restore the Registers and add Timestamp to the Stack
    ;Pop $8  ; Stack $7 $6 $5 $4 $3 $2 $1 $0
    Pop $7  ; Stack $6 $5 $4 $3 $2 $1 $0
    Pop $6  ; Stack $5 $4 $3 $2 $1 $0
    Pop $5  ; Stack $4 $3 $2 $1 $0
    Pop $4  ; Stack $3 $2 $1 $0
    Pop $3  ; Stack $2 $1 $0
    Pop $2  ; Stack $1 $0
    Pop $1  ; Stack $0
    Exch $0 ; Stack ${TimeStamp}
 
FunctionEnd
!macroend
!insertmacro __TimeStamp ""
!insertmacro __TimeStamp "un."
!endif


#####################################################################################################
# LogText
# Based on http://nsis.sourceforge.net/Logging:_Simple_Text_File_Logging_Functions_and_Macros
# Written on by Mike Schinkel [http://www.mikeschinkel.com/blog/]
 
Var /GLOBAL __TextLog_FileName
 
!define LogText '!insertmacro LogTextCall'
!macro LogTextCall _text
    Push "${_text}"
    Call LogText
!macroend
 
Function LogText
    Exch $0     ; pABC -> 0ABC
    Push $1
    Push $2
    ${If} $__TextLog_FileName != ""
        ${TimeStamp} $1
        ${IfNot} ${UAC_IsInnerInstance}
            StrCpy $1 "$1 [Outer]"
        ${Else}
            StrCpy $1 "$1 [Inner]"
        ${EndIf}
        StrCpy $0 "$1 $0"
        FileOpen $2  "$__TextLog_FileName"  a
        FileSeek $2 0 END
        FileWrite $2 "$0$\r$\n"
        FileClose $2
    ${EndIf}
    Pop $2
    Pop $1
    Pop $0      ; 0ABC -> ABC
FunctionEnd
 

!define LogSetFileName '!insertmacro LogSetFileNameCall'
!macro LogSetFileNameCall _path _filename
    Push "${_path}"
    Push "${_filename}"
    Call LogSetFileName
!macroend
 
Function LogSetFileName
    Exch $1     ; filename
    Exch
    Exch $0     ; path
    Exch
    CreateDirectory "$0"
    StrCpy $__TextLog_FileName "$0\$1"
    ${IfNot} ${UAC_IsInnerInstance}
        # Cycle log if over 15KB
        FileOpen $1 $__TextLog_FileName "r"
        FileSeek $1 0 END $0
        FileClose $1
        IntCmp $0 15000 toosmall toosmall
            StrCpy $1 "$__TextLog_FileName_old"
            Delete $1
            Rename $__TextLog_FileName $1
    toosmall:
        # Begin new outer
        ${LogText} "-------------------------------------"
    ${Else}
        # Switch from outer to inner
        ${LogText} ">>>>"
    ${EndIf}
    Pop $1
    Pop $0      ; 0ABC -> ABC
FunctionEnd
