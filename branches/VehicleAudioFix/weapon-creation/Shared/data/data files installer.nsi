!addincludedir "NSIS dirs\Include"
!addplugindir "NSIS dirs\Plugins"
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

!ifndef MAJOR_VER
    !define MAJOR_VER "1"
    !define MINOR_VER "4"
    !define MAINT_VER "0"
!endif
!define 0.0 "${MAJOR_VER}.${MINOR_VER}"
!define 0.0.0 "${MAJOR_VER}.${MINOR_VER}.${MAINT_VER}"


; ###########################################################################################################
; Don't forget to set the BUILD_NUMBER if this is built manually
; ###########################################################################################################
!ifndef FILES_ROOT
    !define FILES_ROOT "."
    !define BUILD_NUMBER "0000"
    !define INSTALL_OUTPUT "mtasa-${0.0}-data-r${BUILD_NUMBER}.exe"
!endif
!define PRODUCT_VERSION "Data-r${BUILD_NUMBER}"
; ###########################################################################################################


!define PRODUCT_NAME "MTA:SA ${0.0}"
!define PRODUCT_NAME_NO_VER "MTA:SA"

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

	; Try to find previously saved MTA:SA install path
	ReadRegStr $Install_Dir HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location"
	${If} $Install_Dir == "" 
		ReadRegStr $Install_Dir HKLM "SOFTWARE\Multi Theft Auto: San Andreas ${0.0}" "Last Install Location"
	${EndIf}
	${If} $Install_Dir == "" 
		strcpy $Install_Dir "$PROGRAMFILES\MTA San Andreas ${0.0}"
	${EndIf}
	strcpy $INSTDIR $Install_Dir
	
	InitPluginsDir
	;File /oname=$PLUGINSDIR\serialdialog.ini "serialdialog.ini"
FunctionEnd

Function .onInstSuccess
	WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location" $INSTDIR

	;UAC::Unload ;Must call unload!
FunctionEnd



InstType "Data files only"
InstType /NOCUSTOM


Name "${PRODUCT_NAME_NO_VER} ${PRODUCT_VERSION}"
OutFile "${INSTALL_OUTPUT}"

InstallDirRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location"
ShowInstDetails show

Section "Data files" SEC01
	SectionIn 1 RO ; section is required

	WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location" $INSTDIR

	SetOverwrite on

	SetOutPath "$INSTDIR\MTA"
	File "${FILES_ROOT}\MTA San Andreas\mta\d3dx9_42.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\D3DCompiler_42.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bass.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\basswma.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bassmidi.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bassflac.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bass_aac.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bass_ac3.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bass_fx.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\bassmix.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\tags.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\chatboxpresets.xml"
	File "${FILES_ROOT}\MTA San Andreas\mta\sa.dat"
	File "${FILES_ROOT}\MTA San Andreas\mta\pthreadVC2.dll"

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
                
    SetOutPath "$INSTDIR\skins\Lighter black"
    File "${FILES_ROOT}\MTA San Andreas\skins\Lighter black\CGUI.is.xml"
    File "${FILES_ROOT}\MTA San Andreas\skins\Lighter black\CGUI.lnf.xml"
    File "${FILES_ROOT}\MTA San Andreas\skins\Lighter black\CGUI.png"
    File "${FILES_ROOT}\MTA San Andreas\skins\Lighter black\CGUI.xml"
	
	SetOutPath "$INSTDIR\MTA\cgui"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\Falagard.xsd"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\Font.xsd"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\GUIScheme.xsd"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\Imageset.xsd"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\pricedown.ttf"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sabankgothic.ttf"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sagothic.ttf"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\saheader.ttf"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sans.ttf"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\unifont-5.1.20080907.ttf"

	SetOutPath "$INSTDIR\MTA\cgui\images"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\*.png"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\*.jpg"

	SetOutPath "$INSTDIR\MTA\cgui\images\radarset"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\radarset\*.png"

	SetOutPath "$INSTDIR\MTA\cgui\images\transferset"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\transferset\*.png"
	
	SetOutPath "$INSTDIR\MTA\cgui\images\serverbrowser"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\serverbrowser\*.png"

	SetOutPath "$INSTDIR\server\mods\deathmatch"
	File "${FILES_ROOT}\MTA San Andreas\server\mods\deathmatch\libmysql.dll"
	File "${FILES_ROOT}\MTA San Andreas\server\mods\deathmatch\pthreadVC2.dll"

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
