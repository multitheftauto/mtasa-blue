!include LogicLib.nsh

!macro GetTextExtent String Width # Seems to be accurate and return big values
	System::Alloc 16
	Pop $0
	System::Call "*$0(i 0, i 0, i 0, i 0)"

	System::Call "User32::GetDC(i $9) i .r1"
	System::Call "User32::DrawText(i r1, t '${String}', i -1, i r0, i 1024) i .r1"

	System::Call "*$0(i .r1,i .r2,i .r3,i .r4)"

	StrCpy ${Width} $3
	# StrCpy ${Height} $4 #$4 is Height if need be in future
	
	System::Free $0
!macroend

!macro Max Num1 Num2
	${If} ${Num1} > ${Num2}
		StrCpy ${Num2} ${Num1}
	${Else}
		StrCpy ${Num1} ${Num2}
	${EndIf}
!macroend


;----------------------------------------
; In: FILENAME = filename
; Out: MAJOR.MINOR.RELEASE.BUILD
;----------------------------------------
!macro _GetDLLVersionNumbers FILENAME MAJOR MINOR RELEASE BUILD
    Push $R8
    Push $R9
    GetDLLVersion "${FILENAME}" $R8 $R9
    IntOp ${MAJOR} $R8 / 0x00010000
    IntOp ${MINOR} $R8 & 0x0000FFFF
    IntOp ${RELEASE} $R9 / 0x00010000
    IntOp ${BUILD} $R9 & 0x0000FFFF
    Pop $R9
    Pop $R8
!macroend
!define GetDLLVersionNumbers `!insertmacro _GetDLLVersionNumbers`


;----------------------------------------
; Overwrite last DetailPrint line
; From http://nsis.sourceforge.net/DetailUpdate
;----------------------------------------
Function DetailUpdate
  Exch $R0
  Push $R1
  Push $R2
  Push $R3
 
  FindWindow $R2 `#32770` `` $HWNDPARENT
  GetDlgItem $R1 $R2 1006
  SendMessage $R1 ${WM_SETTEXT} 0 `STR:$R0`
  GetDlgItem $R1 $R2 1016
 
  System::Call *(&t${NSIS_MAX_STRLEN}R0)i.R2
  System::Call *(i0,i0,i0,i0,i0,iR2,i${NSIS_MAX_STRLEN},i0,i0)i.R3
 
  !define LVM_GETITEMCOUNT 0x1004
  !define LVM_SETITEMTEXTW 0x1074
  SendMessage $R1 ${LVM_GETITEMCOUNT} 0 0 $R0
  IntOp $R0 $R0 - 1
  System::Call user32::SendMessage(iR1,i${LVM_SETITEMTEXTW},iR0,iR3)
 
  System::Free $R3
  System::Free $R2
 
  Pop $R3
  Pop $R2
  Pop $R1
  Pop $R0
FunctionEnd
!macro DetailUpdate Text
  Push `${Text}`
  Call DetailUpdate
!macroend
!define DetailUpdate `!insertmacro DetailUpdate`
