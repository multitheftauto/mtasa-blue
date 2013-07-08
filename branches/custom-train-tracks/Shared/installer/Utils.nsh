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