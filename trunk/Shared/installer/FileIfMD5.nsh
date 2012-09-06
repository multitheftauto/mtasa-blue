;=====================================================================================
; MD5 File compare replace macro
;=====================================================================================
!include "LogicLib.nsh"
!include "FileFunc.nsh"

!macro FileIfMD5 FileOutPath MD5
	${GetFileName} "${FileOutPath}" $R0
	md5dll::GetMD5File "$OUTDIR\$R0"
	Pop $0
	${If} $0 == ${MD5}
		File "${FileOutPath}"
	${EndIf}
!macroend

!macro GetMD5 FullPath MD5
	md5dll::GetMD5File "${FullPath}"
	Pop $0
	StrCpy ${MD5} $0
!macroend
