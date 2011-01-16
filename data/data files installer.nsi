!include nsDialogs.nsh
!include LogicLib.nsh
!include Sections.nsh
!include UAC.nsh

XPStyle on
RequestExecutionLevel user
SetCompressor /SOLID /FINAL lzma

!define TEMP1 $R0
!define TEMP2 $R1
!define TEMP3 $R2

Var Install_Dir


; ###########################################################################################################
; Don't forget to update the BUILD_NUMBER
; ###########################################################################################################
!define FILES_ROOT "."
!define BUILD_NUMBER "2331"
!define PRODUCT_VERSION "Data-r${BUILD_NUMBER}"
!define INSTALL_OUTPUT "mtasa-1.1-data-r${BUILD_NUMBER}.exe"
; ###########################################################################################################


!define PRODUCT_NAME "MTA:SA 1.1"

!define PRODUCT_PUBLISHER "Multi Theft Auto"
!define PRODUCT_WEB_SITE "http://www.multitheftauto.com"

; MUI 1.67 compatible ------
!include "MUI.nsh"
;!include "ReplaceSubStr.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON		"mta.ico"
!define MUI_UNICON		"mta.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE					"eula.txt"

; Components page
!insertmacro MUI_PAGE_COMPONENTS

; Game directory page
!define MUI_DIRECTORYPAGE_VARIABLE				$INSTDIR
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!insertmacro MUI_PAGE_FINISH


; Language files
!insertmacro MUI_LANGUAGE							"English"
LangString DESC_Section1 ${LANG_ENGLISH}			"The data files required to run Multi Theft Auto."


Function .OnInstFailed
	;UAC::Unload ;Must call unload!
FunctionEnd

Function .onInit
	Call DoRightsElevation

	ReadRegStr $Install_Dir HKLM "SOFTWARE\Multi Theft Auto: San Andreas 1.1" "Last Install Location" ; start of fix for #3743
	${If} $Install_Dir == '' 
		strcpy $INSTDIR "$PROGRAMFILES\MTA San Andreas 1.1"
	${Else} 
		strcpy $INSTDIR $Install_Dir
	${EndIf} ; end of fix for #3743

	
	InitPluginsDir
	;File /oname=$PLUGINSDIR\serialdialog.ini "serialdialog.ini"
FunctionEnd

Function .onInstSuccess
	WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas 1.1" "Last Install Location" $INSTDIR

	;UAC::Unload ;Must call unload!
FunctionEnd



InstType "Data files only"
InstType /NOCUSTOM


Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${INSTALL_OUTPUT}"

InstallDirRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas 1.1" "Last Install Location"
ShowInstDetails show

Section "Data files" SEC01
	SectionIn 1 RO ; section is required

	WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas 1.1" "Last Install Location" $INSTDIR

	SetOverwrite on

	SetOutPath "$INSTDIR\MTA"
	File "${FILES_ROOT}\MTA San Andreas\mta\d3dx9_42.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bass.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\basswma.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bassmidi.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bassflac.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bass_aac.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bass_ac3.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bassmix.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\tags.dll"
	;File "${FILES_ROOT}\MTA San Andreas\mta\matroska.ax"
	;File "${FILES_ROOT}\MTA San Andreas\mta\mkunicode.dll"
	;File "${FILES_ROOT}\MTA San Andreas\mta\mkzlib.dll"
	;File "${FILES_ROOT}\MTA San Andreas\mta\vorbis.ax"
	;File "${FILES_ROOT}\MTA San Andreas\mta\xv.ax"
	;File "${FILES_ROOT}\MTA San Andreas\mta\xvidcore.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\chatboxpresets.xml"

	SetOutPath "$INSTDIR\skins\Classic"
    File "${FILES_ROOT}\MTA San Andreas\skins\Classic\CGUI.is.xml"
    File "${FILES_ROOT}\MTA San Andreas\skins\Classic\CGUI.lnf.xml"
    File "${FILES_ROOT}\MTA San Andreas\skins\Classic\CGUI.png"
    File "${FILES_ROOT}\MTA San Andreas\skins\Classic\CGUI.xml"
    
    SetOutPath "$INSTDIR\skins\Default"
    File "${FILES_ROOT}\MTA San Andreas\skins\Default\CGUI.is.xml"
    File "${FILES_ROOT}\MTA San Andreas\skins\Default\CGUI.lnf.xml"
    File "${FILES_ROOT}\MTA San Andreas\skins\Default\CGUI.png"
    File "${FILES_ROOT}\MTA San Andreas\skins\Default\CGUI.xml"
	
	SetOutPath "$INSTDIR\MTA\cgui"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\Falagard.xsd"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\Font.xsd"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\GUIScheme.xsd"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\Imageset.xsd"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\pricedown.ttf"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sabankgothic.ttf"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sagothic.ttf"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\saheader.ttf"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sans.dat"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sans.tga"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sans.ttf"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\unifont-5.1.20080907.ttf"

	SetOutPath "$INSTDIR\MTA\cgui\images"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\*.png"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\*.jpg"

	SetOutPath "$INSTDIR\MTA\cgui\images\radarset"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\radarset\*.png"

	SetOutPath "$INSTDIR\MTA\cgui\images\transferset"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\transferset\*.png"

	;SetOutPath "$INSTDIR\MTA\data"
	;File "${FILES_ROOT}\MTA San Andreas\mta\data\*.png"
	;;File "${FILES_ROOT}\MTA San Andreas\mta\data\menu.mkv"
	;File "${FILES_ROOT}\MTA San Andreas\mta\data\pixel.psh"
	;File "${FILES_ROOT}\MTA San Andreas\mta\data\scene.x"
	;File "${FILES_ROOT}\MTA San Andreas\mta\data\vertex.vsh"

	;SetOutPath "$INSTDIR\MTA\data\textures"
	;File "${FILES_ROOT}\MTA San Andreas\mta\data\textures\*.png"

SectionEnd


!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC01} $(DESC_Section1)
!insertmacro MUI_FUNCTION_DESCRIPTION_END



;====================================================================================
; UAC related functions
;====================================================================================

!macro RightsElevation un
    uac_tryagain:
    !insertmacro UAC_RunElevated
    #MessageBox mb_TopMost "0=$0 1=$1 2=$2 3=$3"
    ${Switch} $0
    ${Case} 0
        ${IfThen} $1 = 1 ${|} Quit ${|} ;we are the outer process, the inner process has done its work, we are done
        ${IfThen} $3 <> 0 ${|} ${Break} ${|} ;we are admin, let the show go on
        ${If} $1 = 3 ;RunAs completed successfully, but with a non-admin user
            MessageBox mb_IconExclamation|mb_TopMost|mb_SetForeground "This ${un}installer requires admin access, try again" /SD IDNO IDOK uac_tryagain IDNO 0
        ${EndIf}
        ;fall-through and die
    ${Case} 1223
        MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "This ${un}installer requires admin privileges, aborting!"
        Quit
    ${Case} 1062
        MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "Logon service not running, aborting!"
        Quit
    ${Default}
        MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "Unable to elevate , error $0"
        Quit
    ${EndSwitch}
!macroend

Function DoRightsElevation
    !insertmacro RightsElevation ""
FunctionEnd

Function un.DoRightsElevation
    !insertmacro RightsElevation "un"
FunctionEnd
