; This script is derived of a script Written by dirtydingus :
; "Another String Replace (and Slash/BackSlash Converter)" 
;
; for more information please see :
; http://nsis.sourceforge.net/Another_String_Replace_(and_Slash/BackSlash_Converter)
 
Var MODIFIED_STR
 
!macro ReplaceSubStr OLD_STR SUB_STR REPLACEMENT_STR
	
	Push "${OLD_STR}" ;String to do replacement in (haystack)
	Push "${SUB_STR}" ;String to replace (needle)
	Push "${REPLACEMENT_STR}" ; Replacement
	Call StrRep
	Pop $R0 ;result
	StrCpy $MODIFIED_STR $R0
	
!macroend
 
Function StrRep
 
  ;Written by dirtydingus 2003-02-20 04:30:09 
  ; USAGE
  ;Push String to do replacement in (haystack)
  ;Push String to replace (needle)
  ;Push Replacement
  ;Call StrRep
  ;Pop $R0 result	
 
  Exch $R4 ; $R4 = Replacement String
  Exch
  Exch $R3 ; $R3 = String to replace (needle)
  Exch 2
  Exch $R1 ; $R1 = String to do replacement in (haystack)
  Push $R2 ; Replaced haystack
  Push $R5 ; Len (needle)
  Push $R6 ; len (haystack)
  Push $R7 ; Scratch reg
  StrCpy $R2 ""
  StrLen $R5 $R3
  StrLen $R6 $R1
loop:
  StrCpy $R7 $R1 $R5
  StrCmp $R7 $R3 found
  StrCpy $R7 $R1 1 ; - optimization can be removed if U know len needle=1
  StrCpy $R2 "$R2$R7"
  StrCpy $R1 $R1 $R6 1
  StrCmp $R1 "" done loop
found:
  StrCpy $R2 "$R2$R4"
  StrCpy $R1 $R1 $R6 $R5
  StrCmp $R1 "" done loop
done:
  StrCpy $R3 $R2
  Pop $R7
  Pop $R6
  Pop $R5
  Pop $R2
  Pop $R1
  Pop $R4
  Exch $R3
	
FunctionEnd