!include nsDialogs.nsh
!include LogicLib.nsh
!include Sections.nsh

XPStyle on
RequestExecutionLevel user
SetCompressor /SOLID /FINAL lzma

!define TEMP1 $R0
!define TEMP2 $R1
!define TEMP3 $R2

Var Install_Dir


; ###########################################################################################################
; How to update easily:
; 1. Run the previous version of the data files installer .exe and install into sub directory 'MTA San Andreas' of
;    wherever this script is on your drive.
;		i.e. C:\SVN\multitheftauto\Shared\installer\MTA San Andreas\
; 2. Change the files as you want
; 3. Update the script to reflect the file changes
; 4. Update BUILD_NUMBER
; 5. Run the script
; ###########################################################################################################
!define FILES_ROOT "."
!define BUILD_NUMBER "812"
!define PRODUCT_VERSION "Data-r${BUILD_NUMBER}"
!define INSTALL_OUTPUT "multitheftauto_data-r${BUILD_NUMBER}.exe"
; ###########################################################################################################


!define PRODUCT_NAME "MTA:SA"

!define PRODUCT_PUBLISHER "Multi Theft Auto"
!define PRODUCT_WEB_SITE "http://www.multitheftauto.com"

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "ReplaceSubStr.nsh"

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
	UAC::Unload ;Must call unload!
FunctionEnd

Function .onInit

; FOR VISTA

UAC_Elevate:
	UAC::RunElevated
	StrCmp 1223 $0 UAC_ElevationAborted ; UAC dialog aborted by user?
	StrCmp 0 $0 0 UAC_Err ; Error?
	StrCmp 1 $1 0 UAC_Success ;Are we the real deal or just the wrapper?
	Quit

UAC_Err:
	MessageBox mb_iconstop "Unable to elevate, error $0"
	Abort

UAC_ElevationAborted:
	; elevation was aborted, run as normal?
	MessageBox mb_iconstop "This installer requires admin access, aborting!"
	Abort

UAC_Success:
	StrCmp 1 $3 +4 ; Admin?
	StrCmp 3 $1 0 UAC_ElevationAborted ; Try again?
	MessageBox mb_iconstop "This installer requires admin access, try again"
	goto UAC_Elevate


	ReadRegStr $Install_Dir HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Last Install Location" ; start of fix for #3743
	${If} $Install_Dir == '' 
		strcpy $INSTDIR "$PROGRAMFILES\MTA San Andreas"
	${Else} 
		strcpy $INSTDIR $Install_Dir
	${EndIf} ; end of fix for #3743

	
	InitPluginsDir
	File /oname=$PLUGINSDIR\serialdialog.ini "serialdialog.ini"
FunctionEnd

Function .onInstSuccess
	WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Last Install Location" $INSTDIR

	UAC::Unload ;Must call unload!
FunctionEnd



InstType "Data files only"
InstType /NOCUSTOM


Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${INSTALL_OUTPUT}"

InstallDirRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Last Install Location"
ShowInstDetails show

Section "Data files" SEC01
	SectionIn 1 RO ; section is required

	WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Last Install Location" $INSTDIR

	SetOverwrite on

	SetOutPath "$INSTDIR\MTA"
	File "${FILES_ROOT}\MTA San Andreas\mta\d3dx9_41.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\ikpMP3.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\irrKlang.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\libcurl.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\matroska.ax"
	File "${FILES_ROOT}\MTA San Andreas\mta\mkunicode.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\mkzlib.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\msvcp71.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\msvcr71.dll"
	File "${FILES_ROOT}\MTA San Andreas\mta\vorbis.ax"
	File "${FILES_ROOT}\MTA San Andreas\mta\xv.ax"
	File "${FILES_ROOT}\MTA San Andreas\mta\xvidcore.dll"

	SetOutPath "$INSTDIR\MTA\cgui"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\CGUI.is.xml"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\CGUI.lnf.xml"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\CGUI.png"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\CGUI.xml"
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

	SetOutPath "$INSTDIR\MTA\cgui\images"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\*.png"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\*.jpg"

	SetOutPath "$INSTDIR\MTA\cgui\images\radarset"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\radarset\*.png"

	SetOutPath "$INSTDIR\MTA\cgui\images\transferset"
	File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\transferset\*.png"

	SetOutPath "$INSTDIR\MTA\data"
	File "${FILES_ROOT}\MTA San Andreas\mta\data\*.png"
	;File "${FILES_ROOT}\MTA San Andreas\mta\data\menu.mkv"
	File "${FILES_ROOT}\MTA San Andreas\mta\data\pixel.psh"
	File "${FILES_ROOT}\MTA San Andreas\mta\data\scene.x"
	File "${FILES_ROOT}\MTA San Andreas\mta\data\vertex.vsh"

	SetOutPath "$INSTDIR\MTA\data\textures"
	File "${FILES_ROOT}\MTA San Andreas\mta\data\textures\*.png"

	SetOutPath "$INSTDIR\server"
	File "${FILES_ROOT}\MTA San Andreas\server\libcurl.dll"

SectionEnd


!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC01} $(DESC_Section1)
!insertmacro MUI_FUNCTION_DESCRIPTION_END



