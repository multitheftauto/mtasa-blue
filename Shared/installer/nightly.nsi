!addincludedir "NSIS dirs\Include"
!addplugindir "NSIS dirs\Plugins"
!include nsDialogs.nsh
!include LogicLib.nsh
!include Sections.nsh
!include UAC.nsh
!include GameExplorer.nsh
!include WinVer.nsh
!include nsArray.nsh
!include Utils.nsh

XPStyle on
RequestExecutionLevel user
SetCompressor /SOLID lzma

Var GTA_DIR
Var Install_Dir
Var CreateSMShortcuts
Var CreateDesktopIcon
Var RegisterProtocol
Var AddToGameExplorer
Var RedistInstalled
Var RedistVC12Installed
Var ExeMD5
Var PatchInstalled
Var DEFAULT_INSTDIR
Var LAST_INSTDIR
Var CUSTOM_INSTDIR
Var WhichRadio
Var ShowLastUsed

; Games explorer: With each new X.X, update this GUID and the file at MTA10\launch\NEU\GDFImp.gdf.xml
!define GUID "{DF780162-2450-4665-9BA2-EAB14ED640A3}"


!ifndef MAJOR_VER
    !define MAJOR_VER "1"
    !define MINOR_VER "4"
    !define MAINT_VER "0"
!endif
!define 0.0 "${MAJOR_VER}.${MINOR_VER}"
!define 0.0.0 "${MAJOR_VER}.${MINOR_VER}.${MAINT_VER}"

; ###########################################################################################################
!ifndef FILES_ROOT
	!define LIGHTBUILD    ; enable LIGHTBUILD for nightly
	!define FILES_ROOT "Install files builder/output"
	!define SERVER_FILES_ROOT "Install files builder/output/server"
	!define FILES_MODULE_SDK "Install files builder/output/development/publicsdk"
	!define INSTALL_OUTPUT "mtasa-${0.0.0}-unstable-00000-0-000-nsis.exe"
	!define PRODUCT_VERSION "v${0.0.0}-unstable-00000-0-000"
	!define REVISION "0"
!endif
!ifndef LIGHTBUILD
	!define INCLUDE_DEVELOPMENT
	!define INCLUDE_EDITOR
!endif
!ifndef PRODUCT_VERSION
	!define PRODUCT_VERSION "v${0.0.0}"
!endif
!define EXPAND_DIALOG_X 134
!define EXPAND_DIALOG_Y 60
!define DIALOG_X 450
; ###########################################################################################################

;ReserveFile "${NSISDIR}\Plugins\InstallOptions.dll"
!ifdef REVISION
	!define REVISION_TAG "(r${REVISION})"
!else
	!define REVISION_TAG ""
!endif

!define PRODUCT_NAME "MTA:SA ${0.0}"
!define PRODUCT_NAME_NO_VER "MTA:SA"

!define PRODUCT_PUBLISHER "Multi Theft Auto"
!define PRODUCT_WEB_SITE "http://www.multitheftauto.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Multi Theft Auto ${0.0}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; Set file version information
!ifndef VI_PRODUCT_VERSION
    !ifdef REVISION
        !define VI_PRODUCT_VERSION "${0.0.0}.${REVISION}"
    !else
        !define VI_PRODUCT_VERSION "${0.0.0}.0"
    !endif
    !define VI_PRODUCT_NAME "MTA San Andreas"
    !define VI_COMPANY_NAME "Multi Theft Auto"
    !define VI_LEGAL_COPYRIGHT "(C) 2003 - 2014 Multi Theft Auto"
    !ifndef LIGHTBUILD
        !define VI_FILE_DESCRIPTION "Multi Theft Auto Full Installer"
    !else
        !define VI_FILE_DESCRIPTION "Multi Theft Auto Nightly Installer"
    !endif
!endif
VIProductVersion "${VI_PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${VI_PRODUCT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${VI_COMPANY_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "${VI_LEGAL_COPYRIGHT}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${VI_FILE_DESCRIPTION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VI_PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VI_PRODUCT_VERSION}"

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "ReplaceSubStr.nsh"
!include "FileIfMD5.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON		"mta.ico"
!define MUI_UNICON		"mta.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "mta_install_header.bmp"
!define MUI_HEADERIMAGE_BITMAP_RTL "mta_install_header_rtl.bmp"

; Welcome page
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_WELCOMEPAGE_TEXT 		"$(WELCOME_TEXT)"
!define MUI_PAGE_CUSTOMFUNCTION_PRE "WelcomePreProc"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "WelcomeShowProc"
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "WelcomeLeaveProc"
!insertmacro MUI_PAGE_WELCOME

; License page
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "LicenseShowProc"
!insertmacro MUI_PAGE_LICENSE					"eula.txt"

; Components page
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "ComponentsShowProc"
!insertmacro MUI_PAGE_COMPONENTS

; Game directory page
#!define MUI_PAGE_CUSTOMFUNCTION_SHOW "DirectoryShowProc"
#!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "DirectoryLeaveProc"
#!define MUI_CUSTOMFUNCTION_ABORT "DirectoryAbort"
#!define MUI_DIRECTORYPAGE_VARIABLE				$INSTDIR
#!insertmacro MUI_PAGE_DIRECTORY
Page custom CustomDirectoryPage DirectoryLeaveProc

; Language Tools ----
;Note: Assumes NSIS Unicode edition compiler
!define MUI_LANGDLL_ALLLANGUAGES
!define MUI_LANGDLL_REGISTRY_ROOT "HKLM" 
!define MUI_LANGDLL_REGISTRY_KEY "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" 
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"
!insertmacro MUI_RESERVEFILE_LANGDLL ;Solid compression optimization for multilang

; INSERT OUR PAGES
!define MUI_PAGE_CUSTOMFUNCTION_PRE				SkipDirectoryPage
!define MUI_PAGE_HEADER_TEXT					"$(HEADER_Text)"
!define MUI_PAGE_HEADER_SUBTEXT					""
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION		"$(DIRECTORY_Text_Dest)"
!define MUI_DIRECTORYPAGE_TEXT_TOP				"$(DIRECTORY_Text_Top)"
!define MUI_DIRECTORYPAGE_VARIABLE				$GTA_DIR
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE           "GTADirectoryLeaveProc"
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_TITLE_3LINES
; Launch from installer with user privileges
!define MUI_FINISHPAGE_RUN						""
!define MUI_FINISHPAGE_RUN_FUNCTION				"LaunchLink"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; INSERT OUR LANGUAGE STRINGS -----
!insertmacro MUI_LANGUAGE "English"
;@INSERT_TRANSLATIONS@

LangString	WELCOME_TEXT  ${LANG_ENGLISH}	"This wizard will guide you through the installation or update of $(^Name) ${REVISION_TAG}\n\n\
It is recommended that you close all other applications before starting Setup.\n\n\
[Admin access may be requested for Vista and up]\n\n\
Click Next to continue."
LangString 	HEADER_Text			${LANG_ENGLISH}	"Grand Theft Auto: San Andreas location"
LangString 	DIRECTORY_Text_Dest	${LANG_ENGLISH}	"Grand Theft Auto: San Andreas folder"
LangString 	DIRECTORY_Text_Top	${LANG_ENGLISH}	"Please select your Grand Theft Auto: San Andreas folder.$\n$\nYou MUST have Grand Theft Auto: San Andreas 1.0 installed to use MTA:SA, it does not support any other versions.$\n$\nClick Install to begin installing."

; Language files
LangString	DESC_Section10			${LANG_ENGLISH}	"Create a Start Menu group for installed applications"
LangString	DESC_Section11			${LANG_ENGLISH}	"Create a Desktop Shortcut for the MTA:SA Client."
LangString	DESC_Section12			${LANG_ENGLISH}	"Register mtasa:// protocol for browser clickable-ness."
LangString	DESC_Section13			${LANG_ENGLISH}	"Add to Windows Games Explorer (if present)."
LangString	DESC_Section1			${LANG_ENGLISH}	"The core components required to run Multi Theft Auto."
LangString	DESC_Section2			${LANG_ENGLISH}	"The MTA:SA modification, allowing you to play online."
;LangString	DESC_Section3			${LANG_ENGLISH}	"The Multi Theft Auto:Editor for MTA:SA, allowing you to create and edit maps."
;LangString	DESC_SectionGroupMods	${LANG_ENGLISH}	"Modifications for Multi Theft Auto. Without at least one of these, you cannot play Multi Theft Auto."
LangString	DESC_SectionGroupServer  ${LANG_ENGLISH}	"The Multi Theft Auto Server. This allows you to host games from your computer. This requires a fast internet connection."
LangString	DESC_Section4			${LANG_ENGLISH}	"The Multi Theft Auto server. This is a required component."
LangString	DESC_Section5			${LANG_ENGLISH}	"The MTA:SA modification for the server."
LangString	DESC_Section6			${LANG_ENGLISH}	"This is a set of required resources for your server."
LangString	DESC_Section7			${LANG_ENGLISH}	"This is an optional set of gamemodes and maps for your server."
LangString	DESC_Section8			${LANG_ENGLISH}	"The MTA:SA 1.0 Map Editor.  This can be used to create your very own maps for use in gamemodes for MTA."
LangString	DESC_Section9			${LANG_ENGLISH}	"This is the SDK for creating binary modules for the MTA server. Only install if you have a good understanding of C++!"
;LangString	DESC_Blank			${LANG_ENGLISH}	""
LangString	DESC_SectionGroupDev		${LANG_ENGLISH}	"Development code and tools that aid in the creation of mods for Multi Theft Auto"
LangString	DESC_SectionGroupClient  ${LANG_ENGLISH}	"The client is the program you run to play on a Multi Theft Auto server"


Function LaunchLink
	SetOutPath "$INSTDIR"
	# Problem: 'non-admin nsis' and 'admin nsis' run at the same time and can have different values for $INSTDIR
	# Fix: Copy to temp variable
	StrCpy $1 "$INSTDIR\Multi Theft Auto.exe"
	!insertmacro UAC_AsUser_ExecShell "" "$1" "" "" ""
FunctionEnd

Function .onInstFailed
FunctionEnd

Function .onInit

	${IfNot} ${UAC_IsInnerInstance}
		!insertmacro MUI_LANGDLL_DISPLAY  # Only display our language selection in the outer (non-admin) instance
	${Else}
		!insertmacro UAC_AsUser_GetGlobalVar $LANGUAGE # Copy our selected language from the outer to the inner instance
	${EndIf}
	
	; #############################################
	; Remove old shortcuts put in rand(user,admin) startmenu by previous installers (shortcuts now go in all users)
	SetShellVarContext current
	; Delete shortcuts
	Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\MTA San Andreas.lnk"
	Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\Uninstall MTA San Andreas.lnk"
	Delete "$DESKTOP\MTA San Andreas ${0.0}.lnk"

	; Delete shortcuts
	Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\MTA Server.lnk"
	Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\Uninstall MTA San Andreas Server.lnk"
	RmDir /r "$SMPROGRAMS\\MTA San Andreas ${0.0}"
	; #############################################

	SetShellVarContext all

	; Check if we must install the Microsoft Visual Studio 2008 SP1 redistributable
	ClearErrors
	ReadRegDWORD $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{9A25302D-30C0-39D9-BD6F-21E6EC160475}" "Version"
	IfErrors 0 DontInstallVC9Redist
	StrCpy $RedistInstalled "0"
	Goto PostVC90Check
DontInstallVC9Redist:
	StrCpy $RedistInstalled "1"
PostVC90Check:

	; Check if we must install the Microsoft Visual Studio 2013 redistributable
	ClearErrors
	ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\DevDiv\vc\Servicing\12.0\RuntimeMinimum" "Install"
	StrCmp "$0" "1" DontInstallVC12Redist
	StrCpy $RedistVC12Installed "0"
	Goto PostVC12Check
DontInstallVC12Redist:
	StrCpy $RedistVC12Installed "1"
PostVC12Check:

	
	; Try to find previously saved MTA:SA install path
	ReadRegStr $Install_Dir HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location"
	${If} $Install_Dir == "" 
		ReadRegStr $Install_Dir HKLM "SOFTWARE\Multi Theft Auto: San Andreas ${0.0}" "Last Install Location"
	${EndIf}
	${If} $Install_Dir == "" 
		strcpy $Install_Dir "$PROGRAMFILES\MTA San Andreas ${0.0}"
	${EndIf}
	strcpy $INSTDIR $Install_Dir

    ; Setup for install dir dialog
	strcpy $DEFAULT_INSTDIR "$PROGRAMFILES\MTA San Andreas ${0.0}"
	strcpy $LAST_INSTDIR $Install_Dir
	strcpy $CUSTOM_INSTDIR $DEFAULT_INSTDIR
	${If} $DEFAULT_INSTDIR == $LAST_INSTDIR 
        StrCpy $WhichRadio "default"
        StrCpy $ShowLastUsed "0"
	${Else}
        Push $LAST_INSTDIR 
        Call GetInstallType
        Pop $0
        Pop $1
        ${If} $0 == "overwrite"
            # Ignore last used if it contains different major MTA version
            StrCpy $WhichRadio "default"
            StrCpy $ShowLastUsed "0"
        ${Else}
            StrCpy $WhichRadio "last"
            StrCpy $ShowLastUsed "1"
        ${EndIf}
	${EndIf}
    
	; Try to find previously saved GTA:SA install path
	ReadRegStr $2 HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\Common" "GTA:SA Path"
	${If} $2 == "" 
		ReadRegStr $2 HKCU "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path"
	${EndIf}
	${If} $2 == "" 
		ReadRegStr $2 HKLM "SOFTWARE\Rockstar Games\GTA San Andreas\Installation" "ExePath"
	${EndIf}
	${If} $2 == "" 
		ReadRegStr $2 HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path"
	${EndIf}
	${If} $2 == "" 
		ReadRegStr $3 HKCU "Software\Valve\Steam\Apps\12120" "Installed"
		StrCpy $3 $3 1
		${If} $3 == "1"
			ReadRegStr $3 HKCU "Software\Valve\Steam" "SteamPath"
			StrCpy $2 "$3\steamapps\common\grand theft auto san andreas"
		${EndIf}
	${EndIf}
	${If} $2 == "" 
		ReadRegStr $2 HKCU "SOFTWARE\Multi Theft Auto: San Andreas ${0.0}" "GTA:SA Path Backup"
	${EndIf}

	; Remove exe name from path
	!insertmacro ReplaceSubStr $2 "gta_sa.exe" ""
	; Conform slash types
	!insertmacro ReplaceSubStr $MODIFIED_STR "/" "\"
	; Remove quotes
	strcpy $3 '"'
	!insertmacro ReplaceSubStr $MODIFIED_STR $3 ""
	; Store result 
	strcpy $GTA_DIR $MODIFIED_STR

    ; Default to standard path if nothing defined
	${If} $GTA_DIR == "" 
		strcpy $GTA_DIR "$PROGRAMFILES\Rockstar Games\GTA San Andreas\"
	${EndIf}

	InitPluginsDir
	;File /oname=$PLUGINSDIR\serialdialog.ini "serialdialog.ini"
FunctionEnd

Function .onInstSuccess
	SetShellVarContext all

	WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\Common" "GTA:SA Path" $GTA_DIR
	WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location" $INSTDIR
	
	; Start menu items
	${If} $CreateSMShortcuts == 1
		CreateDirectory "$SMPROGRAMS\MTA San Andreas ${0.0}"

		IfFileExists "$INSTDIR\Multi Theft Auto.exe" 0 skip1
		SetOutPath "$INSTDIR"
		Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\Play MTA San Andreas.lnk"
		CreateShortCut "$SMPROGRAMS\\MTA San Andreas ${0.0}\MTA San Andreas.lnk" "$INSTDIR\Multi Theft Auto.exe" \
			"" "$INSTDIR\Multi Theft Auto.exe" 0 SW_SHOWNORMAL \
			"" "Play Multi Theft Auto: San Andreas ${0.0}"
		skip1:
		
		IfFileExists "$INSTDIR\Server\MTA Server.exe" 0 skip2
		SetOutPath "$INSTDIR\Server"
		CreateShortCut "$SMPROGRAMS\\MTA San Andreas ${0.0}\MTA Server.lnk" "$INSTDIR\Server\MTA Server.exe" \
			"" "$INSTDIR\Server\MTA Server.exe" 2 SW_SHOWNORMAL \
			"" "Run the Multi Theft Auto: San Andreas ${0.0} Server"
		skip2:
		
		IfFileExists "$INSTDIR\Uninstall.exe" 0 skip3
		SetOutPath "$INSTDIR"
		CreateShortCut "$SMPROGRAMS\\MTA San Andreas ${0.0}\Uninstall MTA San Andreas.lnk" "$INSTDIR\Uninstall.exe" \
			"" "$INSTDIR\Uninstall.exe" 0 SW_SHOWNORMAL \
			"" "Uninstall Multi Theft Auto: San Andreas ${0.0}"
		skip3:
	${EndIf}
    
    ${If} $CreateDesktopIcon == 1
		IfFileExists "$INSTDIR\Multi Theft Auto.exe" 0 skip4
		SetOutPath "$INSTDIR"
		Delete "$DESKTOP\Play MTA San Andreas ${0.0}.lnk"
		CreateShortCut "$DESKTOP\MTA San Andreas ${0.0}.lnk" "$INSTDIR\Multi Theft Auto.exe" \
			"" "$INSTDIR\Multi Theft Auto.exe" 0 SW_SHOWNORMAL \
			"" "Play Multi Theft Auto: San Andreas ${0.0}"
		AccessControl::GrantOnFile "$DESKTOP\MTA San Andreas ${0.0}.lnk" "(BU)" "FullAccess"

		skip4:
	${EndIf}

    ${If} $RegisterProtocol == 1
        ; Add the protocol handler
        WriteRegStr HKCR "mtasa" "" "URL:MTA San Andreas Protocol"
        WriteRegStr HKCR "mtasa" "URL Protocol" ""
        WriteRegStr HKCR "mtasa\DefaultIcon" "" "$INSTDIR\Multi Theft Auto.exe"
        WriteRegStr HKCR "mtasa\shell\open\command" "" '"$INSTDIR\Multi Theft Auto.exe"%1'
	${EndIf}

	;UAC::Unload ;Must call unload!
FunctionEnd

LangString INST_CLIENTSERVER ${LANG_ENGLISH}	"Client and Server"
LangString INST_SERVER ${LANG_ENGLISH}	"Server only"


InstType "$(INST_CLIENTSERVER)"
InstType "$(INST_SERVER)"

Name "${PRODUCT_NAME_NO_VER} ${PRODUCT_VERSION}"
OutFile "${INSTALL_OUTPUT}"

;InstallDir "$PROGRAMfiles San Andreas"
InstallDirRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location"
ShowInstDetails show
ShowUnInstDetails show

LangString INST_STARTMENU_GROUP 	${LANG_ENGLISH}	"Start menu group"
LangString INST_DESKTOP_ICON 		${LANG_ENGLISH}	"Desktop icon"
LangString INST_PROTOCOL 			${LANG_ENGLISH}	"Register mtasa:// protocol"
LangString INST_GAMES_EXPLORER 	${LANG_ENGLISH}	"Add to Games Explorer"

Section "$(INST_STARTMENU_GROUP)" SEC10
	SectionIn 1 2
	StrCpy $CreateSMShortcuts 1
SectionEnd

Section "$(INST_DESKTOP_ICON)" SEC11
	SectionIn 1 2
	StrCpy $CreateDesktopIcon 1
SectionEnd

Section "$(INST_PROTOCOL)" SEC12
	SectionIn 1 2
	StrCpy $RegisterProtocol 1
SectionEnd

Section "$(INST_GAMES_EXPLORER)" SEC13
	SectionIn 1 2
	StrCpy $AddToGameExplorer 1
SectionEnd

LangString INST_SEC_CLIENT		${LANG_ENGLISH}	"Game client"
LangString INST_SEC_SERVER		${LANG_ENGLISH}	"Dedicated server"
LangString INST_SEC_CORE			${LANG_ENGLISH}	"Core components"
LangString INST_SEC_GAME		 	${LANG_ENGLISH}	"Game module"

LangString INFO_INPLACE_UPGRADE ${LANG_ENGLISH}	"Performing in-place upgrade..."
LangString INFO_UPDATE_PERMISSIONS ${LANG_ENGLISH}	"Updating permissions. This could take a few minutes..."
LangString MSGBOX_INVALID_GTASA ${LANG_ENGLISH}	"A valid Windows version of Grand Theft Auto: San Andreas was not detected.\
$\r$\nHowever installation will continue.\
$\r$\nPlease reinstall if there are problems later."
LangString INST_SEC_CORE_RESOURCES ${LANG_ENGLISH}	"Core Resources"
LangString INST_SEC_OPTIONAL_RESOURCES ${LANG_ENGLISH}	"Optional Resources"
LangString INST_SEC_EDITOR ${LANG_ENGLISH}	"Editor"

SectionGroup /e "$(INST_SEC_CLIENT)" SECGCLIENT
	Section "$(INST_SEC_CORE)" SEC01
		SectionIn 1 RO ; section is required
		
		StrCmp "$RedistInstalled" "1" DontInstallRedist
		Call InstallVC90Redistributable
		StrCmp "$RedistInstalled" "1" DontInstallRedist
		Abort
DontInstallRedist:

		StrCmp "$RedistVC12Installed" "1" DontInstallRedistVC12
		Call InstallVC12Redistributable
		StrCmp "$RedistVC12Installed" "1" DontInstallRedistVC12
		Abort
DontInstallRedistVC12:

		SetShellVarContext all

		#############################################################
		# Show that upgrade is catered for
        Push $INSTDIR 
        Call GetInstallType
        Pop $0
        Pop $1

        ${If} $0 == "upgrade"
            DetailPrint "$(INFO_INPLACE_UPGRADE)"
            Sleep 1000
        ${EndIf}
		#############################################################

		WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\Common" "GTA:SA Path" $GTA_DIR
		WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location" $INSTDIR

		# Create fixed path data directories
		CreateDirectory "$APPDATA\MTA San Andreas All\Common"
		CreateDirectory "$APPDATA\MTA San Andreas All\${0.0}"

		# Ensure install dir exists so the permissions can be set
		SetOutPath "$INSTDIR\MTA"
		SetOverwrite on

		#############################################################
		# Make the directory "$INSTDIR" read write accessible by all users
		# Make the directory "$APPDATA\MTA San Andreas All" read write accessible by all users
        # Make the directory "$GTA_DIR" read write accessible by all users

        ${If} ${AtLeastWinVista}
            DetailPrint "$(INFO_UPDATE_PERMISSIONS)"

            # Fix permissions for MTA install directory
            StrCpy $0 "$PROGRAMFILES\M"
            StrLen $2 $0
            StrCpy $1 "$INSTDIR" $2
            ${If} $0 == $1
                FastPerms::FullAccessPlox "$INSTDIR"
            ${Else}
                # More conservative permissions blat if install directory it too different from default
                CreateDirectory "$INSTDIR\mods"
                CreateDirectory "$INSTDIR\screenshots"
                CreateDirectory "$INSTDIR\server"
                CreateDirectory "$INSTDIR\skins"
                FastPerms::FullAccessPlox "$INSTDIR\mods"
                FastPerms::FullAccessPlox "$INSTDIR\MTA"
                FastPerms::FullAccessPlox "$INSTDIR\screenshots"
                FastPerms::FullAccessPlox "$INSTDIR\server"
                FastPerms::FullAccessPlox "$INSTDIR\skins"
            ${EndIf}
            FastPerms::FullAccessPlox "$APPDATA\MTA San Andreas All"

            # Remove MTA virtual store
            StrCpy $0 $INSTDIR
            !insertmacro UAC_AsUser_Call Function RemoveVirtualStore ${UAC_SYNCREGISTERS}
            StrCpy $0 $INSTDIR
            Call RemoveVirtualStore

            IfFileExists $GTA_DIR\gta_sa.exe +1 0
			IfFileExists $GTA_DIR\gta-sa.exe 0 PathBad
                # Fix permissions for GTA install directory
                FastPerms::FullAccessPlox "$GTA_DIR"

                # Remove GTA virtual store
                StrCpy $0 $GTA_DIR
                !insertmacro UAC_AsUser_Call Function RemoveVirtualStore ${UAC_SYNCREGISTERS}
                StrCpy $0 $GTA_DIR
                Call RemoveVirtualStore
            PathBad:
        ${EndIf}
		#############################################################
		
		#############################################################
		# Patch our San Andreas .exe if it is required
			
		IfFileExists $GTA_DIR\gta_sa.exe 0 TrySteamExe
            # Check gta_sa.exe is greater than 1MB (Previous Steam patching may have failed)
            ${GetSize} "$GTA_DIR" "/M=gta_sa.exe /S=0M /G=0" $0 $1 $2
            StrCmp "$0" "0" TrySteamExe
			!insertmacro GetMD5 $GTA_DIR\gta_sa.exe $ExeMD5
			DetailPrint "gta_sa.exe successfully detected ($ExeMD5)"
			${Switch} $ExeMD5
				${Case} "bf25c28e9f6c13bd2d9e28f151899373" #US 2.00
				${Case} "7fd5f9436bd66af716ac584ff32eb483" #US 1.01
				${Case} "d84326ba0e0ace89f87288ffe7504da4" #EU 3.00 Steam Mac
				${Case} "4e99d762f44b1d5e7652dfa7e73d6b6f" #EU 2.00
				${Case} "2ac4b81b3e85c8d0f9846591df9597d3" #EU 1.01
				${Case} "d0ad36071f0e9bead7bddea4fbda583f" #EU 1.01 GamersGate
				${Case} "25405921d1c47747fd01fd0bfe0a05ae" #EU 1.01 DEViANCE
				${Case} "9effcaf66b59b9f8fb8dff920b3f6e63" #DE 2.00
				${Case} "fa490564cd9811978085a7a8f8ed7b2a" #DE 1.01
				${Case} "49dd417760484a18017805df46b308b8" #DE 1.00
				${Case} "185f0970f5913d0912a89789af175ffe" #?? ?.?? 4,496,063 bytes
					#Create a backup of the GTA exe before patching
					CopyFiles "$GTA_DIR\gta_sa.exe" "$GTA_DIR\gta_sa.exe.bak"
					Call InstallPatch
					${If} $PatchInstalled == "1"
						Goto CompletePatchProc
					${EndIf}
					Goto NoExeFound
					${Break}
				${Default}
					Goto CompletePatchProc #This gta_sa.exe doesn't need patching, let's continue
					${Break}
			${EndSwitch}
		TrySteamExe:
            # Try with gta-sa.exe, then testapp.exe
            nsArray::SetList array "gta-sa.exe" "testapp.exe" /end
            ${ForEachIn} array $0 $1
                IfFileExists $GTA_DIR\$1 0 TrySteamNext
                !insertmacro GetMD5 $GTA_DIR\$1 $ExeMD5
                DetailPrint "$1 successfully detected ($ExeMD5)"
                ${Switch} $ExeMD5
                    ${Case} "0fd315d1af41e26e536a78b4d4556488" #EU 3.00 Steam
                    ${Case} "2ed36a3cee7b77da86a343838e3516b6" #EU 3.01 Steam (2014 Nov update)
                        #Copy gta-sa.exe to gta_sa.exe and commence patching process
                        CopyFiles "$GTA_DIR\$1" "$GTA_DIR\gta_sa.exe.bak"
                        Call InstallPatch
                        ${If} $PatchInstalled == "1"
                            Goto CompletePatchProc
                        ${EndIf}
                        Goto TrySteamNext
                        ${Break}
                    ${Default}
                        Goto TrySteamNext
                        ${Break}
                ${EndSwitch}
            TrySteamNext:
            ${Next}

		NoExeFound:
			MessageBox MB_ICONSTOP "$(MSGBOX_INVALID_GTASA)"
		CompletePatchProc:

		SetOutPath "$INSTDIR\MTA"
		SetOverwrite on

		# Make some keys in HKLM read write accessible by all users
		AccessControl::GrantOnRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas All" "(BU)" "FullAccess"

		SetOutPath "$INSTDIR\MTA"
		File "${FILES_ROOT}\MTA San Andreas\mta\cgui.dll"
		File "${FILES_ROOT}\MTA San Andreas\mta\core.dll"
		File "${FILES_ROOT}\MTA San Andreas\mta\xmll.dll"
		File "${FILES_ROOT}\MTA San Andreas\mta\game_sa.dll"
		File "${FILES_ROOT}\MTA San Andreas\mta\multiplayer_sa.dll"
		File "${FILES_ROOT}\MTA San Andreas\mta\netc.dll"
		File "${FILES_ROOT}\MTA San Andreas\mta\libcurl.dll"
		File "${FILES_ROOT}\MTA San Andreas\mta\loader.dll"
        File "${FILES_ROOT}\MTA San Andreas\mta\pthread.dll"
        ; The files below can be moved out of the LIGHTBUILD zone at some point
        File "${FILES_ROOT}\MTA San Andreas\mta\bass_fx.dll"
        File "${FILES_ROOT}\MTA San Andreas\mta\bassopus.dll"
        File "${FILES_ROOT}\MTA San Andreas\mta\tags.dll"
        File "${FILES_ROOT}\MTA San Andreas\mta\XInput9_1_0_mta.dll"
        File "${FILES_ROOT}\MTA San Andreas\mta\vea.dll"
        File "${FILES_ROOT}\MTA San Andreas\mta\vog.dll"
        File "${FILES_ROOT}\MTA San Andreas\mta\vvo.dll"
        File "${FILES_ROOT}\MTA San Andreas\mta\vvof.dll"
        SetOutPath "$INSTDIR\MTA\cgui\images"
        File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\busy_spinner.png"
        File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\rect_edge.png"


		${If} "$(LANGUAGE_CODE)" != ""
			# Write our language to registry
			WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}\Settings\general" "locale" "$(LANGUAGE_CODE)"
		${EndIf}

        !ifndef LIGHTBUILD

		    SetOutPath "$INSTDIR\MTA"
			File "${FILES_ROOT}\MTA San Andreas\mta\d3dx9_42.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\D3DCompiler_42.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\bass.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\basswma.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\bassmidi.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\bassflac.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\bass_aac.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\bass_ac3.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\bassmix.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\chatboxpresets.xml"
			File "${FILES_ROOT}\MTA San Andreas\mta\sa.dat"

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

		!endif
			
        SetOutPath "$INSTDIR\MTA\locale\"
        File /r "${FILES_ROOT}\MTA San Andreas\mta\locale\*.png"
        File /r "${FILES_ROOT}\MTA San Andreas\mta\locale\*.po"

		SetOutPath "$INSTDIR"
        File "${FILES_ROOT}\MTA San Andreas\Multi Theft Auto.exe"
		!ifndef LIGHTBUILD
            File "${FILES_ROOT}\MTA San Andreas\Multi Theft Auto.exe.dat"
		!endif
		
        ${If} $AddToGameExplorer == 1
            ${GameExplorer_UpdateGame} ${GUID}
            ${If} ${Errors}
                ${GameExplorer_AddGame} all "$INSTDIR\Multi Theft Auto.exe" "$INSTDIR" "$INSTDIR\Multi Theft Auto.exe" ${GUID}
                CreateDirectory $APPDATA\Microsoft\Windows\GameExplorer\${GUID}\SupportTasks\0
                CreateShortcut "$APPDATA\Microsoft\Windows\GameExplorer\$0\SupportTasks\0\Client Manual.lnk" \ "http://wiki.multitheftauto.com/wiki/Client_Manual"
            ${EndIf}
        ${EndIf}

        Call DoServiceInstall
	SectionEnd

	Section "$(INST_SEC_GAME)" SEC02
		SectionIn 1 RO
		SetOutPath "$INSTDIR\mods\deathmatch"
		File "${FILES_ROOT}\MTA San Andreas\mods\deathmatch\Client.dll"
		File "${FILES_ROOT}\MTA San Andreas\mods\deathmatch\lua5.1c.dll"
		File "${FILES_ROOT}\MTA San Andreas\mods\deathmatch\pcre3.dll"
		SetOutPath "$INSTDIR\mods\deathmatch\resources"
	SectionEnd
SectionGroupEnd

SectionGroup /e "$(INST_SEC_SERVER)" SECGSERVER
	Section "$(INST_SEC_CORE)" SEC04
		SectionIn 1 2 RO ; section is required
		
		StrCmp "$RedistInstalled" "1" DontInstallRedist
		Call InstallVC90Redistributable
		StrCmp "$RedistInstalled" "1" DontInstallRedist
		Abort
	DontInstallRedist:

		StrCmp "$RedistVC12Installed" "1" DontInstallRedistVC12
		Call InstallVC12Redistributable
		StrCmp "$RedistVC12Installed" "1" DontInstallRedistVC12
		Abort
    DontInstallRedistVC12:

		SetOutPath "$INSTDIR\server"
		SetOverwrite on
		File "${SERVER_FILES_ROOT}\core.dll"
		File "${FILES_ROOT}\MTA San Andreas\mta\xmll.dll"
		File "${SERVER_FILES_ROOT}\MTA Server.exe"
		File "${SERVER_FILES_ROOT}\net.dll"
		File "${FILES_ROOT}\MTA San Andreas\mta\libcurl.dll"
        File "${FILES_ROOT}\MTA San Andreas\mta\pthread.dll"
	SectionEnd

	Section "$(INST_SEC_GAME)" SEC05
		SectionIn 1 2 RO ; section is required
		SetOutPath "$INSTDIR\server\mods\deathmatch"
		
		SetOverwrite on
		File "${SERVER_FILES_ROOT}\mods\deathmatch\deathmatch.dll"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\lua5.1.dll"
		File "${FILES_ROOT}\MTA San Andreas\mods\deathmatch\pcre3.dll"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\dbconmy.dll"
		!ifndef LIGHTBUILD
            File "${SERVER_FILES_ROOT}\mods\deathmatch\libmysql.dll"
		!endif
		
		;Only overwrite the following files if previous versions were bugged and explicitly need replacing
		!insertmacro FileIfMD5 "${SERVER_FILES_ROOT}\mods\deathmatch\editor_acl.xml" "711185d8f4ebb355542053ce408b82b3"
		!insertmacro FileIfMD5 "${SERVER_FILES_ROOT}\mods\deathmatch\editor_acl.xml" "706869E53F508919F987A2F7F2653AD2"

		SetOverwrite off
		File "${SERVER_FILES_ROOT}\mods\deathmatch\acl.xml"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\editor_acl.xml"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\banlist.xml"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\mtaserver.conf"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\local.conf"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\editor.conf"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\vehiclecolors.conf"

		!ifndef LIGHTBUILD
			File "${SERVER_FILES_ROOT}\mods\deathmatch\local.conf"
			
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
			SetOutPath "$INSTDIR\server\mods\deathmatch\resource-cache"
			SetOutPath "$INSTDIR\server\mods\deathmatch\logs"
		!endif
	SectionEnd

	!ifndef LIGHTBUILD
		Section "$(INST_SEC_CORE_RESOURCES)" SEC06
		SectionIn 1 2 ; RO section is now optional
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources\"
            File "${SERVER_FILES_ROOT}\mods\deathmatch\resources\Directory layout readme.txt"

			SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[admin]"
			SetOverwrite ifnewer
			File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[admin]\admin.zip"
			File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[admin]\runcode.zip"
			File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[admin]\acpanel.zip"
			File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[admin]\ipb.zip"
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[play]"
			File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[play]\*.zip"
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gameplay]"
			File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gameplay]\*.zip"
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[managers]"
			File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[managers]\*.zip"
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[web]"
			File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[web]\*.zip"
		SectionEnd
	!endif

	!ifndef LIGHTBUILD
		SectionGroup "$(INST_SEC_OPTIONAL_RESOURCES)" SEC07
			Section "AMX Emulation package"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[amx]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[amx]\amx"
			SectionEnd
			Section "Assault Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[assault]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[assault]\*.zip"
			SectionEnd
			Section "Briefcase Race Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[briefcaserace]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[briefcaserace]\*.zip"
			SectionEnd
			Section "Classic Deathmatch Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[cdm]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[cdm]\*.zip"
			SectionEnd
			Section "Capture the Flag Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[ctf]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[ctf]\*.zip"
			SectionEnd
			Section "Capture the Vehicle Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[ctv]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[ctv]\*.zip"
			SectionEnd
			Section "Deathmatch Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[deathmatch]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[deathmatch]\*.zip"
			SectionEnd
			Section "Fallout Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[fallout]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[fallout]\*.zip"
			SectionEnd
			Section "Hay Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[hay]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[hay]\*.zip"
			SectionEnd
			Section "Race Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[race]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[race]\*.zip"
			SectionEnd
			Section "Stealth Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[stealth]"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[stealth]\*.zip"
			SectionEnd
			Section "Team Deathmatch Arena Gamemode"
		SectionIn 1 2
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[gamemodes]\[tdm]"
			SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[gamemodes]\[tdm]\*.zip"
		SectionEnd
		SectionGroupEnd
	!endif
	
	!ifdef INCLUDE_EDITOR
		Section "$(INST_SEC_EDITOR)" SEC08
			SectionIn 1 2
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources\[editor]"
			SetOverwrite ifnewer
            File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\[editor]\*.zip"
		SectionEnd
	!endif

SectionGroupEnd

LangString INST_SEC_DEVELOPER ${LANG_ENGLISH}	"Development"
!ifdef INCLUDE_DEVELOPMENT
	SectionGroup /e "$(INST_SEC_DEVELOPER)" SECGDEV
		Section /o "Module SDK" SEC09
			SetOutPath "$INSTDIR\development\module SDK"
			SetOverwrite ifnewer
			File /r "${FILES_MODULE_SDK}\"
		SectionEnd
	SectionGroupEnd
!endif

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC10} $(DESC_Section10)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC11} $(DESC_Section11)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC12} $(DESC_Section12)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC13} $(DESC_Section13)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC01} $(DESC_Section1)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC02} $(DESC_Section2)
	;!insertmacro MUI_DESCRIPTION_TEXT ${SEC03} $(DESC_Section3)
	;!insertmacro MUI_DESCRIPTION_TEXT ${SECGMODS} $(DESC_SectionGroupMods)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC04} $(DESC_Section4)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC05} $(DESC_Section5)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC06} $(DESC_Section6)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC07} $(DESC_Section7)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC08} $(DESC_Section8)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC09} $(DESC_Section9)
	;!insertmacro MUI_DESCRIPTION_TEXT ${SECBLANK} $(DESC_Blank)
	!insertmacro MUI_DESCRIPTION_TEXT ${SECGSERVER} $(DESC_SectionGroupServer)
	!insertmacro MUI_DESCRIPTION_TEXT ${SECGDEV} $(DESC_SectionGroupDev)
	!insertmacro MUI_DESCRIPTION_TEXT ${SECGCLIENT} $(DESC_SectionGroupClient)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Section -Post
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	;WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Multi Theft Auto.exe"

	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Multi Theft Auto.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

LangString UNINST_SUCCESS ${LANG_ENGLISH}	"$(^Name) was successfully removed from your computer."
Function un.onUninstSuccess
	HideWindow
	MessageBox MB_ICONINFORMATION|MB_OK "$(UNINST_SUCCESS)"
	;UAC::Unload ;Must call unload! ; #3017 fix
FunctionEnd

LangString UNINST_FAIL ${LANG_ENGLISH}	"Uninstallation has failed!"
Function un.OnUnInstFailed
	HideWindow
	MessageBox MB_ICONSTOP|MB_OK "$(UNINST_FAIL)"
	;UAC::Unload ;Must call unload! ; #3017 fix
FunctionEnd

 
LangString UNINST_REQUEST ${LANG_ENGLISH}	"Are you sure you want to completely remove $(^Name) and all of its components?"
LangString UNINST_REQUEST_NOTE ${LANG_ENGLISH}	"Uninstalling before update?\
$\r$\nIt is not necessary to uninstall before installing a new version of MTA:SA\
$\r$\nRun the new installer to upgrade and preserve your settings."

Function un.onInit
	Call un.DoRightsElevation
	SetShellVarContext all
		MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "$(UNINST_REQUEST_NOTE)$\r$\n$\r$\n$\r$\n$(UNINST_REQUEST)" IDYES +2
		Abort
		
	!insertmacro MUI_UNGETLANGUAGE
FunctionEnd

LangString UNINST_DATA_REQUEST ${LANG_ENGLISH}	"Would you like to keep your data files (such as resources, screenshots and server configuration)? If you click no, any resources, configurations or screenshots you have created will be lost."
Section Uninstall
	IfFileExists "$INSTDIR\server\mods\deathmatch\resources\*.*" ask 0 ;no maps folder, so delete everything
	IfFileExists "$INSTDIR\screenshots\*.*" ask 0 ;no maps folder, so delete everything
	IfFileExists "$INSTDIR\mods\deathmatch\resources\*.*" ask deleteall ;no maps folder, so delete everything
	ask:
	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "$(UNINST_DATA_REQUEST)" IDYES preservemapsfolder

	deleteall:
    Call un.DoServiceUninstall
	RmDir /r "$INSTDIR\mods"
	RmDir /r "$INSTDIR\MTA"

	RmDir /r "$INSTDIR\server"

	!ifdef INCLUDE_DEVELOPMENT ; start of fix for #3889
		RmDir /r "$INSTDIR\development\module sdk\publicsdk"
		RmDir "$INSTDIR\development\module sdk"
		RmDir "$INSTDIR\development"
	!endif ; end of fix for #3889

	preservemapsfolder:
    Call un.DoServiceUninstall
	; server CORE FILES
	Delete "$INSTDIR\server\core.dll"
	Delete "$INSTDIR\server\xmll.dll"
	Delete "$INSTDIR\server\MTA Server.exe"
	Delete "$INSTDIR\server\net.dll"
	Delete "$INSTDIR\server\libcurl.dll"

	; server files
	Delete "$INSTDIR\server\mods\deathmatch\deathmatch.dll"
	Delete "$INSTDIR\server\mods\deathmatch\lua5.1.dll"
	Delete "$INSTDIR\server\mods\deathmatch\pcre3.dll"
	Delete "$INSTDIR\server\mods\deathmatch\pthreadVC2.dll"
	Delete "$INSTDIR\server\mods\deathmatch\pthread.dll"
	Delete "$INSTDIR\server\mods\deathmatch\sqlite3.dll"
	Delete "$INSTDIR\server\mods\deathmatch\dbconmy.dll"
	Delete "$INSTDIR\server\mods\deathmatch\libmysql.dll"

	Delete "$INSTDIR\Multi Theft Auto.exe"
	Delete "$INSTDIR\Multi Theft Auto.exe.dat"
	Delete "$INSTDIR\Uninstall.exe"

	Delete "$INSTDIR\mods\deathmatch\Client.dll"
	Delete "$INSTDIR\mods\deathmatch\lua5.1c.dll"
	Delete "$INSTDIR\mods\deathmatch\pcre3.dll"

	RmDir /r "$INSTDIR\MTA\cgui"
	RmDir /r "$INSTDIR\MTA\data"
	Delete "$INSTDIR\MTA\*.dll"
	Delete "$INSTDIR\MTA\*.ax"
	Delete "$INSTDIR\MTA\*.txt"
	Delete "$INSTDIR\MTA\*.dat"

	RmDir /r "$APPDATA\MTA San Andreas All\${0.0}"
	; TODO if $APPDATA\MTA San Andreas All\Common is the only one left, delete it

	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
	DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
	DeleteRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas ${0.0}"
	DeleteRegKey HKCU "SOFTWARE\Multi Theft Auto: San Andreas ${0.0}"
	DeleteRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}"
	; TODO if HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\Common is the only one left, delete it
	
	${GameExplorer_RemoveGame} ${GUID}
	
	; Delete client shortcuts
	Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\MTA San Andreas.lnk"
	Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\Uninstall MTA San Andreas.lnk"
    Delete "$DESKTOP\MTA San Andreas ${0.0}.lnk"

	RmDir "$INSTDIR" ; fix for #3898

	; Delete server shortcuts
	Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\MTA Server.lnk"
	Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\Uninstall MTA San Andreas Server.lnk"
	RmDir /r "$SMPROGRAMS\\MTA San Andreas ${0.0}"
	
	SetAutoClose true
SectionEnd

; Function that skips the game directory page if client is not selected
Function SkipDirectoryPage
	SectionGetFlags ${SEC01} $R0
	IntOp $R0 $R0 & ${SF_SELECTED}
	IntCmp $R0 ${SF_SELECTED} +2 0
	Abort
FunctionEnd

;====================================================================================
; Download and install Microsoft Visual Studio 2008 SP1 redistributable
;====================================================================================
Var REDIST

LangString MSGBOX_VSRED_ERROR1 ${LANG_ENGLISH}	"Unable to download Microsoft Visual Studio 2008 SP1 redistributable"
LangString MSGBOX_VSRED_ERROR2 ${LANG_ENGLISH}	"Unable to install Microsoft Visual Studio 2008 SP1 redistributable"
LangString MSGBOX_VSRED_ERROR3 ${LANG_ENGLISH}	"Unable to download Microsoft Visual Studio 2008 SP1 redistributable.\
$\r$\nHowever installation will continue.\
$\r$\nPlease reinstall if there are problems later."
Function InstallVC90Redistributable
	DetailPrint "Installing Microsoft Visual Studio 2008 SP1 redistributable ..."
	StrCpy $REDIST "$TEMP\vcredist9_x86.exe"
	NSISdl::download "http://download.microsoft.com/download/d/d/9/dd9a82d0-52ef-40db-8dab-795376989c03/vcredist_x86.exe" $REDIST
	Pop $0
	StrCmp "$0" "success" DownloadSuccessful
	
	DetailPrint "* Download of Microsoft Visual Studio 2008 SP1 redistributable failed:"
	DetailPrint "* $0"
	DetailPrint "* Installation continuing anyway"
	MessageBox MB_ICONSTOP "$(MSGBOX_VSRED_ERROR1)"
	Goto InstallEnd
	
DownloadSuccessful:
    ; /qb! = "Unattended install with no cancel button"
	ExecWait '"$REDIST" /qb!'
	ClearErrors
	ReadRegDWORD $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{9A25302D-30C0-39D9-BD6F-21E6EC160475}" "Version"
	IfErrors VC90RedistInstallFailed
	
	StrCpy $RedistInstalled "1"
	Goto InstallEnd

VC90RedistInstallFailed:
	StrCpy $RedistInstalled "0"
	DetailPrint "* Some error occured installing Microsoft Visual Studio 2008 SP1 redistributable"
	DetailPrint "* It is required in order to run Multi Theft Auto : San Andreas"
	DetailPrint "* Installation continuing anyway"
	MessageBox MB_ICONSTOP "$(MSGBOX_VSRED_ERROR2)"

	
InstallEnd:

	StrCmp "$RedistInstalled" "1" InstallEnd2
	MessageBox MB_ICONSTOP "$(MSGBOX_VSRED_ERROR3)"
	StrCpy $RedistInstalled "1"

InstallEnd2:
FunctionEnd


;====================================================================================
; Download and install Microsoft Visual Studio 2013 redistributable
;====================================================================================
Var REDISTVC12

LangString MSGBOX_VC12RED_ERROR1 ${LANG_ENGLISH}	"Unable to download Microsoft Visual Studio 2013 redistributable"
LangString MSGBOX_VC12RED_ERROR2 ${LANG_ENGLISH}	"Unable to install Microsoft Visual Studio 2013 redistributable"
LangString MSGBOX_VC12RED_ERROR3 ${LANG_ENGLISH}	"Unable to download Microsoft Visual Studio 2013 redistributable.\
$\r$\nHowever installation will continue.\
$\r$\nPlease reinstall if there are problems later."
Function InstallVC12Redistributable
	DetailPrint "Installing Microsoft Visual Studio 2013 redistributable ..."
	StrCpy $REDISTVC12 "$TEMP\vcredist12_x86.exe"
	NSISdl::download "http://download.microsoft.com/download/2/E/6/2E61CFA4-993B-4DD4-91DA-3737CD5CD6E3/vcredist_x86.exe" $REDISTVC12
	Pop $0
	StrCmp "$0" "success" DownloadSuccessful
	
	DetailPrint "* Download of Microsoft Visual Studio 2013 redistributable failed:"
	DetailPrint "* $0"
	DetailPrint "* Installation continuing anyway"
	MessageBox MB_ICONSTOP "$(MSGBOX_VC12RED_ERROR1)"
	Goto InstallEnd
	
DownloadSuccessful:
    ; /passive = 'This option will display a progress dialog (but requires no user interaction) and perform an install.'
    ; /quiet = 'This option will suppress all UI and perform an install.'
	ExecWait '"$REDISTVC12" /quiet'
	ClearErrors
	ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\DevDiv\vc\Servicing\12.0\RuntimeMinimum" "Install"
	StrCmp "$0" "1" 0 VC12RedistInstallFailed
	
	StrCpy $RedistVC12Installed "1"
	Goto InstallEnd

VC12RedistInstallFailed:
	StrCpy $RedistVC12Installed "0"
	DetailPrint "* Some error occured installing Microsoft Visual Studio 2013 redistributable"
	DetailPrint "* It is required in order to run Multi Theft Auto : San Andreas"
	DetailPrint "* Installation continuing anyway"
	MessageBox MB_ICONSTOP "$(MSGBOX_VC12RED_ERROR2)"

	
InstallEnd:

	StrCmp "$RedistVC12Installed" "1" InstallEnd2
	MessageBox MB_ICONSTOP "$(MSGBOX_VC12RED_ERROR3)"
	StrCpy $RedistVC12Installed "1"

InstallEnd2:
FunctionEnd

;====================================================================================
; Patcher related functions
;====================================================================================
Var PATCHFILE

LangString MSGBOX_PATCH_FAIL1 ${LANG_ENGLISH}	"Unable to download the patch file for your version of Grand Theft Auto: San Andreas"
LangString MSGBOX_PATCH_FAIL2 ${LANG_ENGLISH}	"Unable to install the patch file for your version of Grand Theft Auto: San Andreas"
Function InstallPatch
	DetailPrint "Incompatible version of San Andreas detected.  Patching executable..."
	StrCpy $PATCHFILE "$TEMP\$ExeMD5.GTASAPatch"
	NSISdl::download "http://mirror.multitheftauto.com/gdata/$ExeMD5.GTASAPatch" $PATCHFILE
	Pop $0
	StrCmp "$0" "success" PatchDownloadSuccessful
	
	DetailPrint "* Download of patch file failed:"
	DetailPrint "* $0"
	DetailPrint "* Installation continuing anyway"
	MessageBox MB_ICONSTOP "$(MSGBOX_PATCH_FAIL1)"
	StrCpy $PatchInstalled "0"
	Goto FinishPatch
	
PatchDownloadSuccessful:
	DetailPrint "Patch download successful.  Installing patch..."
	vpatch::vpatchfile "$PATCHFILE" "$GTA_DIR\gta_sa.exe.bak" "$GTA_DIR\gta_sa.exe"
	Pop $R0
	
	${If} $R0 == "OK"
		StrCpy $PatchInstalled "1"
		Goto FinishPatch
	${ElseIf} $R0 == "OK, new version already installed"
		StrCpy $PatchInstalled "1"
		Goto FinishPatch
	${EndIf}
	
	DetailPrint "* Some error occured installing the patch for Grand Theft Auto: San Andreas:"
	DetailPrint "* $R0"
	DetailPrint "* It is required in order to run Multi Theft Auto : San Andreas"
	DetailPrint "* Installation continuing anyway"
	MessageBox MB_ICONSTOP MSGBOX_PATCH_FAIL2
	StrCpy $PatchInstalled "0"
	
	FinishPatch:
FunctionEnd

;====================================================================================
; UAC related functions
;====================================================================================
LangString UAC_RIGHTS1 ${LANG_ENGLISH}	"This installer requires admin access, try again"
LangString UAC_RIGHTS_UN ${LANG_ENGLISH}	"This uninstaller requires admin access, try again"
LangString UAC_RIGHTS3 ${LANG_ENGLISH}	"Logon service not running, aborting!"
LangString UAC_RIGHTS4 ${LANG_ENGLISH}	"Unable to elevate"
!macro RightsElevation AdminError
    uac_tryagain:
    !insertmacro UAC_RunElevated
    #MessageBox mb_TopMost "0=$0 1=$1 2=$2 3=$3"
    ${Switch} $0
    ${Case} 0
        ${IfThen} $1 = 1 ${|} Quit ${|} ;we are the outer process, the inner process has done its work, we are done
        ${IfThen} $3 <> 0 ${|} ${Break} ${|} ;we are admin, let the show go on
        ${If} $1 = 3 ;RunAs completed successfully, but with a non-admin user
            MessageBox mb_IconExclamation|mb_TopMost|mb_SetForeground "${AdminError}" /SD IDNO IDOK uac_tryagain IDNO 0
        ${EndIf}
        ;fall-through and die
    ${Case} 1223
        MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "${AdminError}"
        Quit
    ${Case} 1062
        MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "$(UAC_RIGHTS3)"
        Quit
    ${Default}
        MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "$(UAC_RIGHTS4), error $0"
        Quit
    ${EndSwitch}
!macroend

Function DoRightsElevation
    !insertmacro RightsElevation "$(UAC_RIGHTS1)"
FunctionEnd

Function un.DoRightsElevation
    !insertmacro RightsElevation "$(UAC_RIGHTS_UN)"
FunctionEnd


;****************************************************************
;
; Functions relating to the resizing of the 'Components' dialog.
;
;****************************************************************

Var HWND
Var RECT_X
Var RECT_Y
Var RECT_W
Var RECT_H

; In: $HWND $RECT_X $RECT_Y
; Out: $RECT_X $RECT_Y
Function ScreenToClient
	;Save existing register values to the stack
	Push $1
	Push $2
	Push $3

	; Allocate a 2 int struct in $1
	System::Call "*(i $RECT_X, i $RECT_Y) i.r1"
	${If} $1 == 0
		DetailPrint "Memory problem"
	${Else}
		; Call ScreenToClient
		System::Call "User32::ScreenToClient(i, i) i ($HWND, r1r1) .r5"
		System::Call "*$1(i .r2, i .r3)"

		; Set return values
		StrCpy $RECT_X $2
		StrCpy $RECT_Y $3

		; Free 2 int struct
		System::Free $1
	${EndIf}
	
	;Restore register values from the stack
	Pop $3
	Pop $2
	Pop $1
	
FunctionEnd


; In: $HWND
; Out: $RECT_X $RECT_Y $RECT_W $RECT_D
Function GetWindowRect
	;Save existing register values to the stack
	Push $1
	Push $2
	Push $3
	Push $4
	Push $5

	; Allocate a 4 int struct in $1
	System::Call "*(i 0, i 0, i 0, i 0) i.r1"
	${If} $1 == 0
		DetailPrint "Memory problem"
	${Else}
		; Call GetWindowRect
		System::Call "User32::GetWindowRect(i, i) i ($HWND, r1) .r5"
		System::Call "*$1(i .r2, i .r3, i .r4, i .r5)"

		; Set return values
		StrCpy $RECT_X $2
		StrCpy $RECT_Y $3
		IntOp $RECT_W $4 - $2
		IntOp $RECT_H $5 - $3

		; Free 4 int struct
		System::Free $1
	${EndIf}
	
	;Restore register values from the stack
	Pop $5
	Pop $4
	Pop $3
	Pop $2
	Pop $1
	
FunctionEnd


; In: $HWND
; Out: $RECT_X $RECT_Y $RECT_W $RECT_D
Function GetChildRect
	;Save existing register values to the stack
	Push $1

	Call GetWindowRect

	System::Call "User32::GetParent(i) i ($HWND) .r1"
	StrCpy $HWND $1

	Call ScreenToClient

	;Restore register values from the stack
	Pop $1
FunctionEnd


Var ITEM_HWND
Var ITEM_PARENT
Var ITEM_ID
Var X
Var Y
Var CX
Var CY

; Input:
; 	$ITEM_PARENT - Parent window
; 	$ITEM_ID 	 - Dialog ID
; 	$X $Y 		 - Position change
; 	$CX $CY 	 - Size change
Function MoveDialogItem
	;Save existing register values to the stack
	Push $1
	Push $2
	Push $3
	Push $4

	; Get item handle
	GetDlgItem $ITEM_HWND $ITEM_PARENT $ITEM_ID

	StrCpy $HWND $ITEM_HWND
	Call GetChildRect
	
	; Calculate new dims
	IntOp $1 $RECT_X + $X
	IntOp $2 $RECT_Y + $Y
	IntOp $3 $RECT_W + $CX
	IntOp $4 $RECT_H + $CY 
	
	; Set new dims
	System::Call "User32::MoveWindow(i, i, i, i, i, b) b ($ITEM_HWND, $1, $2, $3, $4, true)"

	;Restore register values from the stack
	Pop $4
	Pop $3
	Pop $2
	Pop $1
	
FunctionEnd


Var HWND_DIALOG
Var RESIZE_X
Var RESIZE_Y


Function HideBackButton
	GetDlgItem $ITEM_HWND $HWNDPARENT 3
	ShowWindow $ITEM_HWND ${SW_HIDE}
FunctionEnd


; Input:
; 	$RESIZE_X $RESIZE_Y		- Resize amount
Function ResizeComponentsDialogContents

 	FindWindow $HWND_DIALOG "#32770" "" $HWNDPARENT

	;Move description right and stretch down
	StrCpy $X $RESIZE_X
	StrCpy $Y 0
	StrCpy $CX 0
	StrCpy $CY $RESIZE_Y
	${If} "$(LANGUAGE_RTL)" == "1"
		StrCpy $X 0
	${EndIf}

 	StrCpy $ITEM_PARENT $HWND_DIALOG
 	StrCpy $ITEM_ID 1043	; Static - "Position your mouse over a component to see its description."
	Call MoveDialogItem
    
 	StrCpy $ITEM_PARENT $HWND_DIALOG
	StrCpy $ITEM_ID 1042	; Button - Description
	Call MoveDialogItem
    
	${If} "$(LANGUAGE_RTL)" == "1"
		StrCpy $X $RESIZE_X
		StrCpy $Y 0
		StrCpy $CX 0
		StrCpy $CY 0

		StrCpy $ITEM_PARENT $HWND_DIALOG
		StrCpy $ITEM_ID 1021	; Static - "Select the type of install."
		Call MoveDialogItem

		StrCpy $ITEM_PARENT $HWND_DIALOG
		StrCpy $ITEM_ID 1022	; Static - "Or, select the optional components you wish to install."
		Call MoveDialogItem

		StrCpy $ITEM_PARENT $HWND_DIALOG
		StrCpy $ITEM_ID 1023	; Static - "Space required: XX MB."
		Call MoveDialogItem
	${EndIf}

	;Middle zone bigger
	StrCpy $X 0
	StrCpy $Y 0
	StrCpy $CX $RESIZE_X
	StrCpy $CY $RESIZE_Y
	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 0		; Sub dialog
	Call MoveDialogItem

	;Make tree view bigger
	StrCpy $X 0
	StrCpy $Y 0
	StrCpy $CX $RESIZE_X
	StrCpy $CY $RESIZE_Y
	StrCpy $ITEM_PARENT $HWND_DIALOG
	StrCpy $ITEM_ID 1032	; Tree view
	Call MoveDialogItem
	
	;Stretch combo box to the right
	StrCpy $X 0
	StrCpy $Y 0
	StrCpy $CX $RESIZE_X
	StrCpy $CY 0
	
	StrCpy $ITEM_PARENT $HWND_DIALOG
	StrCpy $ITEM_ID 1017	; Combo box
	Call MoveDialogItem
	
	;Move space required text down
	StrCpy $X 0
	StrCpy $Y $RESIZE_Y
	StrCpy $CX 0
	StrCpy $CY 0
	
	StrCpy $ITEM_PARENT $HWND_DIALOG
	StrCpy $ITEM_ID 1023	; Static
	Call MoveDialogItem

FunctionEnd


; Input:
; 	$RESIZE_X $RESIZE_X		- Resize amount
Function ResizeSharedDialogContents

	;Move buttons down and right
	StrCpy $X $RESIZE_X
	StrCpy $Y $RESIZE_Y
	StrCpy $CX 0
	StrCpy $CY 0

	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 1	; Button - Next
	Call MoveDialogItem

	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 2	; Button - Cancel
	Call MoveDialogItem

	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 3 ; Button - Back
	Call MoveDialogItem
	
	;Move branding text down
	StrCpy $X 0
	StrCpy $Y $RESIZE_Y
	StrCpy $CX 0
	StrCpy $CY 0
	
	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 1256	; Static - "Nullsoft Install System..."
	Call MoveDialogItem

	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 1028	; Static - "Nullsoft Install System..."
	Call MoveDialogItem

	;Move lower horizontal line down and stretch to the right
	StrCpy $X 0
	StrCpy $Y $RESIZE_Y
	StrCpy $CX $RESIZE_X
	StrCpy $CY 0
	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 1035	; Static - Line
	Call MoveDialogItem

	;Stretch header to the right
	StrCpy $X 0
	StrCpy $Y 0
	StrCpy $CX $RESIZE_X
	StrCpy $CY 0
	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 1034	; Static - White bar
	Call MoveDialogItem

	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 1036	; Static - Line
	Call MoveDialogItem
	
	;Move header text to the right
	StrCpy $X $RESIZE_X
	StrCpy $Y 0
	StrCpy $CX 0
	StrCpy $CY 0
	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 1037	; Static - "Choose Components"
	Call MoveDialogItem

	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 1038	; Static - "Choose which features of MTA:SA v1.0 you want to install."
	Call MoveDialogItem
	
	${If} "$(LANGUAGE_RTL)" == "1"
		;Move image to the right most end if RTL
		StrCpy $X $RESIZE_X
		StrCpy $Y 0
		StrCpy $CX 0
		StrCpy $CY 0

		StrCpy $ITEM_PARENT $HWNDPARENT
		StrCpy $ITEM_ID 1046	; Static - mta_install_header_rtl.bmp
		Call MoveDialogItem
	${EndIf}
    
FunctionEnd


!define SWP_NOOWNERZORDER	    0x0200

; Input:
; 	$RESIZE_X $RESIZE_X		- Resize amount
Function ResizeMainWindow
	;Save existing register values to the stack
	Push $0
	Push $1
	Push $2
	Push $3
	Push $4
 
	StrCpy $HWND $HWNDPARENT
	Call GetWindowRect

	IntOp $0 $RESIZE_X / 2
	IntOp $1 $RECT_X - $0
	
	IntOp $0 $RESIZE_Y / 2
	IntOp $2 $RECT_Y - $0
	
	IntOp $3 $RECT_W + $RESIZE_X
	IntOp $4 $RECT_H + $RESIZE_Y

	System::Call "User32::SetWindowPos(i, i, i, i, i, i, i) b ($HWNDPARENT, 0, $1, $2, $3, $4, ${SWP_NOOWNERZORDER})"

	;Restore register values from the stack
	Pop $4
	Pop $3
	Pop $2
	Pop $1
	Pop $0

FunctionEnd


Var COMPONENTS_EXPAND_STATUS

Function "WelcomePreProc"
	!insertmacro UAC_IsInnerInstance
	${If} ${UAC_IsInnerInstance} 
		; If switched to admin, don't show welcome window again
		Abort
	${EndIf}
FunctionEnd

Function "WelcomeShowProc"
    BringToFront
FunctionEnd

Function "WelcomeLeaveProc"
	HideWindow
	; Maybe switch to admin after welcome window
	Call DoRightsElevation
	ShowWindow $HWNDPARENT ${SW_SHOW}
FunctionEnd


Function "LicenseShowProc"
	${If} $COMPONENTS_EXPAND_STATUS == 1
		StrCpy $COMPONENTS_EXPAND_STATUS 0
		IntOp $RESIZE_X 0 - ${EXPAND_DIALOG_X}
		IntOp $RESIZE_Y 0 - ${EXPAND_DIALOG_Y}
		Call ResizeSharedDialogContents
		Call ResizeMainWindow
	${Endif}
	Call HideBackButton
    BringToFront
FunctionEnd


Function "ComponentsShowProc"
	${If} $COMPONENTS_EXPAND_STATUS != 1
		StrCpy $COMPONENTS_EXPAND_STATUS 1
		IntOp $RESIZE_X 0 + ${EXPAND_DIALOG_X}
		IntOp $RESIZE_Y 0 + ${EXPAND_DIALOG_Y}
		Call ResizeComponentsDialogContents
		Call ResizeSharedDialogContents
		Call ResizeMainWindow
	${Endif}
FunctionEnd


Function "DirectoryShowProc"
	${If} $COMPONENTS_EXPAND_STATUS == 1
		StrCpy $COMPONENTS_EXPAND_STATUS 0
		IntOp $RESIZE_X 0 - ${EXPAND_DIALOG_X}
		IntOp $RESIZE_Y 0 - ${EXPAND_DIALOG_Y}
		Call ResizeSharedDialogContents
		Call ResizeMainWindow
	${Endif}
FunctionEnd


;****************************************************************
;
; Remove virtual store version of path
;
;****************************************************************
; In $0 = install path
Function RemoveVirtualStore
    StrCpy $2 $0 "" 3     # Skip first 3 chars
    StrCpy $3 "$LOCALAPPDATA\VirtualStore\$2"
    StrCpy $4 "$0\FromVirtualStore"
    IfFileExists $3 0 NoVirtualStore
        CopyFiles $3\*.* $4
        RmDir /r "$3"
    NoVirtualStore:
FunctionEnd


;****************************************************************
;
; Determine if install/upgrade  this version/previous version
;
;****************************************************************

; In <stack> = install path
; Out <stack> = "new" - New install
;          "upgrade" - In place copy with same Major.Minor
;          "overwrite" - In place copy different Major.Minor
;     <stack> = "Maj.Min"
Function GetInstallType
    Pop $0
    Call GetVersionAtLocation
    StrCpy $1 $0 3  # First 3 chars

    ${If} $1 == "0.0"
        StrCpy $2 "new"
    ${ElseIf} $1 == ${0.0}
        StrCpy $2 "upgrade"
    ${Else}
        StrCpy $2 "overwrite"
    ${EndIf}
    Push $1
    Push $2
FunctionEnd


; In $0 = install path
; Out $0 = "1.1.0.3306"
;          "0.0.0.0" if no file
Function GetVersionAtLocation
	; Check installed version at this location
	StrCpy $5 "$0\MTA\core.dll"

	ClearErrors
	GetDLLVersion $5 $R0 $R1
	IfErrors 0 cont
        IntOp $R0 0 + 0x00000000
        IntOp $R1 0 + 0x00000000
        IfFileExists $5 cont
            IntOp $R0 0 + 0x00000000
            IntOp $R1 0 + 0x00000000
    cont:
	IntOp $R2 $R0 >> 16
	IntOp $R2 $R2 & 0x0000FFFF ; $R2 now contains major version
	IntOp $R3 $R0 & 0x0000FFFF ; $R3 now contains minor version
	IntOp $R4 $R1 >> 16
	IntOp $R4 $R4 & 0x0000FFFF ; $R4 now contains release
	IntOp $R5 $R1 & 0x0000FFFF ; $R5 now contains build
	StrCpy $0 "$R2.$R3.$R4.$R5" ; $0 now contains string like "1.2.0.192"
FunctionEnd


LangString INST_MTA_CONFLICT ${LANG_ENGLISH}	"A different major version of MTA ($1) already exists at that path.$\n$\n\ 
			MTA is designed for major versions to be installed in different paths.$\n \
            Are you sure you want to overwrite MTA $1 at \
            $INSTDIR ?"
LangString INST_GTA_CONFLICT ${LANG_ENGLISH}	"MTA cannot be installed into the same directory as GTA:SA.$\n$\n\ 
			Do you want to use the default install directory$\n\
            $DEFAULT_INSTDIR ?"
LangString INST_GTA_ERROR1 ${LANG_ENGLISH} "The selected directory does not exist.$\n$\n\
            Please select the GTA:SA install directory"
LangString INST_GTA_ERROR2 ${LANG_ENGLISH} "Could not find GTA:SA installed at $GTA_DIR $\n$\n\
            Are you sure you want to continue ?"
			
Function "DirectoryLeaveProc"
    Call CustomDirectoryPageUpdateINSTDIR

    # Check if user is trying to install MTA into GTA directory
	Push $INSTDIR 
	Call IsGtaDirectory
	Pop $0
	${If} $0 == "gta"

        # Don't allow install into GTA directory unless MTA is already there
        Push $INSTDIR 
        Call GetInstallType
        Pop $0
        Pop $1
        ${If} $0 != "upgrade"
            MessageBox MB_OKCANCEL|MB_ICONQUESTION|MB_TOPMOST|MB_SETFOREGROUND \
                "$(INST_GTA_CONFLICT)" \
                IDOK cont2
                Abort
            cont2:
            StrCpy $INSTDIR $DEFAULT_INSTDIR
        ${Endif}
    ${Endif}

    # Check if user is trying to install over a different major version of MTA
	Push $INSTDIR 
	Call GetInstallType
	Pop $0
	Pop $1
	${If} $0 == "overwrite"
        MessageBox MB_OKCANCEL|MB_ICONQUESTION|MB_TOPMOST|MB_SETFOREGROUND \
            "$(INST_MTA_CONFLICT)" \
            IDOK cont
            Abort
        cont:
	${Endif}
FunctionEnd


Function "GTADirectoryLeaveProc"

    ; Directory must exist
    IfFileExists "$GTA_DIR\*.*" hasdir
        MessageBox MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND \
            "$(INST_GTA_ERROR1)"
            Abort
    hasdir:

    ; data subdirectory should exist
    IfFileExists "$GTA_DIR\data\*.*" cont
        MessageBox MB_OKCANCEL|MB_ICONQUESTION|MB_TOPMOST|MB_SETFOREGROUND \
            "$(INST_GTA_ERROR2)" \
            IDOK cont1
            Abort
        cont1:
    cont:

FunctionEnd

;****************************************************************
;
; Determine if gta is installed at supplied directory path
;
;****************************************************************

; In <stack> = directory path
; Out <stack> = "" - gta not detected at path
;               "gta" - gta detected at path
Function IsGtaDirectory
    Pop $0
    StrCpy $1 "gta"

    ; gta_sa.exe or gta-sa.exe should exist
    IfFileExists "$0\gta_sa.exe" cont1
        IfFileExists "$0\gta-sa.exe" cont1
            StrCpy $1 ""
    cont1:

    ; data subdirectory should exist
    IfFileExists "$0\data\*.*" cont2
        StrCpy $1 ""
    cont2:

    Push $1
FunctionEnd

;****************************************************************
;
; Custom MTA directory page
;
; To make sure the directory exists when 'Browse...' is clicked
;
;****************************************************************
Var Dialog
Var UpgradeLabel
Var BrowseButton
Var SetDefaultButton
Var DirRequest
Var RadioDefault
Var LabelDefault
Var RadioLastUsed
Var LabelLastUsed
Var RadioCustom
Var Length
Var SizeX
Var SizeY
Var PosX
Var PosY
!define LT_GREY "0xf0f0f0"
!define MID_GREY "0xb0b0b0"
!define BLACK "0x000000"
!define MID_GREY2K "0x808080"
!define LT_GREY2K "0xD1CEC9"

LangString INST_CHOOSE_LOC_TOP ${LANG_ENGLISH}	"Choose Install Location"
LangString INST_CHOOSE_LOC ${LANG_ENGLISH}	"Choose the folder in which to install ${PRODUCT_NAME_NO_VER} ${PRODUCT_VERSION}"
LangString INST_CHOOSE_LOC2 ${LANG_ENGLISH}	"${PRODUCT_NAME_NO_VER} ${PRODUCT_VERSION} will be installed in the following folder.$\n\
To install in a different folder, click Browse and select another folder.$\n$\n Click Next to continue."
LangString INST_CHOOSE_LOC3 ${LANG_ENGLISH}	"Destination Folder"
LangString INST_CHOOSE_LOC_BROWSE ${LANG_ENGLISH}	"Browse..."
LangString INST_CHOOSE_LOC_DEFAULT ${LANG_ENGLISH} "Default"
LangString INST_CHOOSE_LOC_LAST_USED ${LANG_ENGLISH} "Last used"
LangString INST_CHOOSE_LOC_CUSTOM ${LANG_ENGLISH} "Custom"
Function CustomDirectoryPage

	nsDialogs::Create 1018
	Pop $Dialog
	${If} $Dialog == error
		Abort
	${EndIf}

    GetDlgItem $0 $HWNDPARENT 1037
    ${NSD_SetText} $0 "$(INST_CHOOSE_LOC)"
    GetDlgItem $0 $HWNDPARENT 1038
    ${NSD_SetText} $0 "$(INST_CHOOSE_LOC)"

	${NSD_CreateLabel} 0 0 100% 50u "$(INST_CHOOSE_LOC2)"
	Pop $0

    # Calculate size and position of install dir options
    IntOp $SizeY 27 + 90    # 27 + 30 + 30 + 30
	${If} $ShowLastUsed == "0"
        IntOp $SizeY $SizeY - 30
	${EndIf}
    IntOp $PosY 187 - $SizeY

    # Add group box
	${NSD_CreateGroupBox} 0 $PosY 100% $SizeY "$(INST_CHOOSE_LOC3)"
	Pop $0
    IntOp $PosY $PosY + 24
	
	# Pick the longest string and use that as SizeX
	!insertmacro GetTextExtent "$(INST_CHOOSE_LOC_DEFAULT)" $SizeX
	!insertmacro GetTextExtent "$(INST_CHOOSE_LOC_LAST_USED)" $Length
	!insertmacro Max $SizeX $Length
	!insertmacro GetTextExtent "$(INST_CHOOSE_LOC_CUSTOM)" $Length
	!insertmacro Max $SizeX $Length
	
	IntOp $SizeX $SizeX + 6 # Take into account the radio button itself
    IntOp $PosX $SizeX + 20	# Take into account the x padding of 10, plus spacing of 15
	IntOp $Length ${DIALOG_X} - $PosX # [Total width] - [radio button width]
	IntOp $Length $Length - 10 # [Total width] - [radio button width] - [padding]
    # Add default option
    ${NSD_CreateRadioButton} 10 $PosY $SizeX 12u "$(INST_CHOOSE_LOC_DEFAULT)"
	Pop $RadioDefault
	${NSD_CreateText} $PosX $PosY $Length 12u $DEFAULT_INSTDIR
	Pop $LabelDefault
    SendMessage $LabelDefault ${EM_SETREADONLY} 1 0
    IntOp $PosY $PosY + 30

    # Add last used option
	${If} $ShowLastUsed != "0"
        ${NSD_CreateRadioButton} 10 $PosY $SizeX 12u "$(INST_CHOOSE_LOC_LAST_USED)"
        Pop $RadioLastUsed
        ${NSD_CreateText} $PosX $PosY $Length 12u $LAST_INSTDIR
        Pop $LabelLastUsed
        SendMessage $LabelLastUsed ${EM_SETREADONLY} 1 0
        IntOp $PosY $PosY + 30
	${EndIf}

    # Add custom option
    ${NSD_CreateRadioButton} 10 $PosY $SizeX 12u "$(INST_CHOOSE_LOC_CUSTOM)"
	Pop $RadioCustom
	
	!insertmacro GetTextExtent "$(INST_CHOOSE_LOC_BROWSE)" $R9
	IntOp $R9 $R9 + 5 # Add spacing for the button on top of text
	IntOp $Length $Length - $R9
	${NSD_CreateDirRequest} $PosX $PosY $Length 12u $CUSTOM_INSTDIR
	Pop $DirRequest
    IntOp $PosY $PosY - 1
	IntOp $PosX ${DIALOG_X} - $R9
	IntOp $PosX $PosX - 10
	${NSD_CreateBrowseButton} $PosX $PosY $R9 13u "$(INST_CHOOSE_LOC_BROWSE)"
	Pop $BrowseButton
    IntOp $PosY $PosY + 31

    ${NSD_OnClick} $RadioDefault CustomDirectoryPageRadioClick
    ${NSD_OnClick} $RadioLastUsed CustomDirectoryPageRadioClick
    ${NSD_OnClick} $RadioCustom CustomDirectoryPageRadioClick
    ${NSD_OnClick} $BrowseButton CustomDirectoryPageBrowseButtonClick
    ${NSD_OnClick} $SetDefaultButton CustomDirectoryPageSetDefaultButtonClick
    ${NSD_OnChange} $DirRequest CustomDirectoryPageDirRequestChange

    # Install type message
	${NSD_CreateLabel} 0 203 100% 12u ""
	Pop $UpgradeLabel
    Call CustomDirectoryPageSetUpgradeMessage

    Call CustomDirectoryPageShowWhichRadio

    Call DirectoryShowProc
    nsDialogs::Show
FunctionEnd

# Called when radion button is clicked
Function CustomDirectoryPageRadioClick
    Pop $0
    ${Switch} $0
        ${Case} $RadioDefault
            StrCpy $WhichRadio "default"
            ${Break}
        ${Case} $RadioLastUsed
            StrCpy $WhichRadio "last"
            ${Break}
        ${Case} $RadioCustom
            StrCpy $WhichRadio "custom"
            ${Break}
    ${EndSwitch}
    Call CustomDirectoryPageShowWhichRadio
FunctionEnd

# Ensure GUI reflects $WhichRadio
Function CustomDirectoryPageShowWhichRadio
    # Set all options as not selected
	Call IsWindowsClassicTheme
	Pop $0
	${If} $0 == 1
        SetCtlColors $LabelDefault ${MID_GREY2K} ${LT_GREY2K}
        SetCtlColors $LabelLastUsed ${MID_GREY2K} ${LT_GREY2K}
        SetCtlColors $DirRequest ${MID_GREY2K} ${LT_GREY2K}
    ${Else}
        SetCtlColors $LabelDefault ${MID_GREY} ${LT_GREY}
        SetCtlColors $LabelLastUsed ${MID_GREY} ${LT_GREY}
        SetCtlColors $DirRequest ${MID_GREY} ${LT_GREY}
	${EndIf}

    SendMessage $DirRequest ${EM_SETREADONLY} 1 0
    EnableWindow $BrowseButton 0

    # Highlight selected option
    ${Switch} $WhichRadio
        ${Case} "default"
            StrCpy $INSTDIR $DEFAULT_INSTDIR
            ${NSD_SetState} $RadioDefault ${BST_CHECKED}
            SetCtlColors $LabelDefault ${BLACK}
            ${Break}
        ${Case} "last"
            StrCpy $INSTDIR $LAST_INSTDIR
            ${NSD_SetState} $RadioLastUsed ${BST_CHECKED}
            SetCtlColors $LabelLastUsed ${BLACK}
            ${Break}
        ${Case} "custom"
            StrCpy $INSTDIR $CUSTOM_INSTDIR
            ${NSD_SetState} $RadioCustom ${BST_CHECKED}
            SetCtlColors $DirRequest ${BLACK}
            SendMessage $DirRequest ${EM_SETREADONLY} 0 0
            EnableWindow $BrowseButton 1
            ${Break}
    ${EndSwitch}

    # Redraw controls
    ${NSD_GetText} $LabelDefault $0
    ${NSD_SetText} $LabelDefault $0
    ${NSD_GetText} $LabelLastUsed $0
    ${NSD_SetText} $LabelLastUsed $0
    ${NSD_GetText} $DirRequest $0
    ${NSD_SetText} $DirRequest $0
FunctionEnd

Function CustomDirectoryPageDirRequestChange
    ${NSD_GetText} $DirRequest $0
	${If} $0 != error
		StrCpy $CUSTOM_INSTDIR $0
        Call CustomDirectoryPageSetUpgradeMessage
	${EndIf}
FunctionEnd

Function CustomDirectoryPageSetDefaultButtonClick
    StrCpy $INSTDIR "$PROGRAMFILES\MTA San Andreas ${0.0}"
    ${NSD_SetText} $DirRequest $INSTDIR
    Call CustomDirectoryPageSetUpgradeMessage
FunctionEnd

LangString INST_CHOOSE_LOC4 ${LANG_ENGLISH}	"Select the folder to install ${PRODUCT_NAME_NO_VER} ${PRODUCT_VERSION} in:"

Function CustomDirectoryPageBrowseButtonClick
    ${NSD_GetText} $DirRequest $0

    Call CreateDirectoryAndRememberWhichOnesWeDid
    nsDialogs::SelectFolderDialog "$(INST_CHOOSE_LOC4)" $0
	Pop $0

    Call RemoveDirectoriesWhichWeDid

	${If} $0 != error
		StrCpy $CUSTOM_INSTDIR $0
        ${NSD_SetText} $DirRequest $0
        Call CustomDirectoryPageSetUpgradeMessage
	${EndIf}
FunctionEnd

LangString INST_LOC_OW ${LANG_ENGLISH}	"Warning: A different major version of MTA ($1) already exists at that path."
LangString INST_LOC_UPGRADE ${LANG_ENGLISH}	"Installation type:  Upgrade"
Function CustomDirectoryPageSetUpgradeMessage
    Call CustomDirectoryPageUpdateINSTDIR
	Push $INSTDIR 
	Call GetInstallType
	Pop $0
	Pop $1

    ${NSD_SetText} $UpgradeLabel ""
	${If} $0 == "overwrite"
        ${NSD_SetText} $UpgradeLabel "$(INST_LOC_OW)"
	${Endif}
	${If} $0 == "upgrade"
        ${NSD_SetText} $UpgradeLabel "$(INST_LOC_UPGRADE)"
	${Endif}
FunctionEnd

# Make absolutely sure $INSTDIR is correct
Function CustomDirectoryPageUpdateINSTDIR
    ${Switch} $WhichRadio
        ${Case} "default"
            StrCpy $INSTDIR $DEFAULT_INSTDIR
            ${Break}
        ${Case} "last"
            StrCpy $INSTDIR $LAST_INSTDIR
            ${Break}
        ${Case} "custom"
            StrCpy $INSTDIR $CUSTOM_INSTDIR
            ${Break}
    ${EndSwitch}
FunctionEnd

; Out <stack> = "1" - Is Windows Classic
Function IsWindowsClassicTheme
	System::Call "UxTheme::IsThemeActive() i .r3"
    StrCpy $1 "1"
    ${If} $3 == 1
        StrCpy $1 "0"
    ${EndIf}
    Push $1
FunctionEnd

;****************************************************************
;
; Keep track of temp directories created
;
;****************************************************************
Var SAVED_PATH_TO
Var SAVED_CREATE_DEPTH

; In $0 = path
Function CreateDirectoryAndRememberWhichOnesWeDid
    Push $0
    Push $1
    Push $2
    Push $8
    StrCpy $8 $0

    StrCpy $0 $8
    Call HowManyDepthsNotExist

    StrCpy $SAVED_PATH_TO $8
    StrCpy $SAVED_CREATE_DEPTH $2

    #MessageBox mb_TopMost "CreateDirectoryAndRememberWhichOnesWeDid $\n\
    #        path-to=$SAVED_PATH_TO $\n\
    #        create-depth=$SAVED_CREATE_DEPTH $\n\
    #        "

    CreateDirectory $SAVED_PATH_TO

    Pop $8
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

; In $0 = path
; Out $2 = result
Function HowManyDepthsNotExist
    Push $0
    Push $1
    Push $8
    Push $9
    StrCpy $8 $0
    StrCpy $9 0
    ${Do}
        StrCpy $0 $8
        StrCpy $1 $9
        Call RemoveEndsFromPath

        StrCpy $0 $2
        Call DoesDirExist

        #MessageBox mb_TopMost "HowManyDepthsNotExist $\n\
        #        8-path=$8 $\n\
        #        9-count=$9 $\n\
        #        2-path-shrunk=$2 $\n\
        #        1-dir-exist=$1 $\n\
        #        "

        IntOp $9 $9 + 1
    ${LoopUntil} $1 = 1

    IntOp $2 $9 - 1
    Pop $9
    Pop $8
    Pop $1
    Pop $0
FunctionEnd

Function RemoveDirectoriesWhichWeDid
    Push $0
    Push $1
    Push $2
    Push $3

    ${If} $SAVED_PATH_TO != ""

        #MessageBox mb_TopMost "RemoveDirectoriesWhichWeDid $\n\
        #        path=$SAVED_PATH_TO $\n\
        #        depth=$SAVED_CREATE_DEPTH $\n\
        #        "

        IntOp $3 $SAVED_CREATE_DEPTH - 1
        ${ForEach} $2 0 $3 + 1
            StrCpy $0 $SAVED_PATH_TO
            StrCpy $1 $2
            Call RemoveDirectoryAtNegDepth

        ${Next}

    ${EndIf}

    StrCpy $SAVED_PATH_TO ""
    StrCpy $SAVED_CREATE_DEPTH ""

    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

; In $0 = path
; In $1 = how many end bits to remove
Function RemoveDirectoryAtNegDepth
    Push $2
        Call RemoveEndsFromPath

        #MessageBox mb_TopMost "RemoveDirectoryAtNegDepth $\n\
        #        2-result=$2 $\n\
        #       "

        RmDir $2
    Pop $2
FunctionEnd

; In $0 = path
; In $1 = how many end bits to remove
; Out $2 = result
Function RemoveEndsFromPath
    nsArray::Clear my_array
    nsArray::Split my_array $0 \ /noempty

    ${ForEach} $2 1 $1 + 1
        nsArray::Remove my_array /at=-1
    ${Next}

    nsArray::Join my_array \ /noempty
    Pop $2
FunctionEnd

; In $0 = path
; Out $0 = result
Function ConformDirectoryPath
    nsArray::Clear my_array
    nsArray::Split my_array $0 \ /noempty
    nsArray::Join my_array \ /noempty
    Pop $0
FunctionEnd

; In $0 = path
; Out $1 = result 0/1
Function DoesDirExist
    StrCpy $1 1
    IfFileExists "$0\*.*" alreadyexists
        StrCpy $1 0
    alreadyexists:
FunctionEnd


;****************************************************************
;
; Service maintenance
;
;****************************************************************
Var ServiceModified

Function DoServiceInstall
    ${If} $ServiceModified != 1
        ; Check loader can do command
        GetDLLVersion "$INSTDIR\mta\loader.dll" $R0 $R1
        IntOp $R5 $R1 & 0x0000FFFF ; $R5 now contains build
        ${If} $R5 > 4909
            Exec '"$INSTDIR\Multi Theft Auto.exe" /nolaunch /kdinstall'
            StrCpy $ServiceModified 1
        ${EndIf}
    ${EndIf}
FunctionEnd

Function un.DoServiceUninstall
    ${If} $ServiceModified != 2
        ; Check loader can do command
        GetDLLVersion "$INSTDIR\mta\loader.dll" $R0 $R1
        IntOp $R5 $R1 & 0x0000FFFF ; $R5 now contains build
        ${If} $R5 > 4909
            Exec '"$INSTDIR\Multi Theft Auto.exe" /nolaunch /kduninstall'
            StrCpy $ServiceModified 2
        ${EndIf}
    ${EndIf}
FunctionEnd
