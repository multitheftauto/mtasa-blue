RequestExecutionLevel user
SetCompressor /SOLID lzma

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
!include WordFunc.nsh
!include textlog.nsh
!include x64.nsh
!include procfunc.nsh
!include KBInstall.nsh
!include MultiSzMacro.nsh

Var GTA_DIR
Var Install_Dir
Var CreateSMShortcuts
Var CreateDesktopIcon
Var RegisterProtocol
Var AddToGameExplorer
Var DEFAULT_INSTDIR
Var LAST_INSTDIR
Var CUSTOM_INSTDIR
Var WhichRadio
Var ShowLastUsed
Var PermissionsGroup

# Shortcuts names
Var ClientShortcutName
Var ServerShortcutName
Var UninstallShortcutName
# Shostcuts paths
Var StartMenuMTAFolderPath
Var StartMenuClientShortcutPath
Var StartMenuServerShortcutPath
Var StartMenuUninstallShortcutPath
Var DesktopClientShortcutPath
# Exe names
Var ClientExeName
Var ServerExeName
Var UninstallExeName
# Exe paths
Var ClientExePath
Var ServerExePath
Var UninstallExePath

; Games explorer: With each new X.X, update this GUID and the file at MTA10\launch\NEU\Multi Theft Auto.gdf.xml
!define GUID "{119D0ADB-56AF-4C85-9037-26564C0ACD57}"


!ifndef MAJOR_VER
    !define MAJOR_VER "1"
    !define MINOR_VER "6"
    !define MAINT_VER "0"
!endif
!define 0.0 "${MAJOR_VER}.${MINOR_VER}"
!define 0.0.0 "${MAJOR_VER}.${MINOR_VER}.${MAINT_VER}"

; ###########################################################################################################
!ifndef FILES_ROOT
    !define LIGHTBUILD    ; enable LIGHTBUILD for nightly
    !define FILES_ROOT "../../Bin"
    !define SERVER_FILES_ROOT "${FILES_ROOT}/server"
    !define FILES_MODULE_SDK "${FILES_ROOT}/development/publicsdk"
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
!define PRODUCT_WEB_SITE "https://www.multitheftauto.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Multi Theft Auto ${0.0}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "ReplaceSubStr.nsh"
!include "FileIfMD5.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON        "mta.ico"
!define MUI_UNICON      "mta.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "mta_install_header.bmp"
!define MUI_HEADERIMAGE_BITMAP_RTL "mta_install_header_rtl.bmp"

; Welcome page
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_WELCOMEPAGE_TEXT        "$(WELCOME_TEXT)"
!define MUI_PAGE_CUSTOMFUNCTION_PRE "WelcomePreProc"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "WelcomeShowProc"
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "WelcomeLeaveProc"
!insertmacro MUI_PAGE_WELCOME

; License page
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "LicenseShowProc"
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "LicenseLeaveProc"
!insertmacro MUI_PAGE_LICENSE                   "eula.txt"

Page custom CustomNetMessagePage CustomNetMessagePageLeave

; Components page
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "ComponentsShowProc"
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "ComponentsLeaveProc"
!insertmacro MUI_PAGE_COMPONENTS

; Game directory page
#!define MUI_PAGE_CUSTOMFUNCTION_SHOW "DirectoryShowProc"
#!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "DirectoryLeaveProc"
#!define MUI_CUSTOMFUNCTION_ABORT "DirectoryAbort"
#!define MUI_DIRECTORYPAGE_VARIABLE             $INSTDIR
#!insertmacro MUI_PAGE_DIRECTORY
Page custom CustomDirectoryPage CustomDirectoryPageLeave

; Language Tools ----
;Note: Assumes NSIS Unicode edition compiler
!define MUI_LANGDLL_ALLLANGUAGES
!define MUI_LANGDLL_REGISTRY_ROOT "HKLM"
!define MUI_LANGDLL_REGISTRY_KEY "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"
!insertmacro MUI_RESERVEFILE_LANGDLL ;Solid compression optimization for multilang

; INSERT OUR PAGES
!define MUI_PAGE_CUSTOMFUNCTION_PRE             SkipDirectoryPage
!define MUI_PAGE_HEADER_TEXT                    "$(HEADER_Text)"
!define MUI_PAGE_HEADER_SUBTEXT                 ""
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION      "$(DIRECTORY_Text_Dest)"
!define MUI_DIRECTORYPAGE_TEXT_TOP              "$(DIRECTORY_Text_Top)"
!define MUI_DIRECTORYPAGE_VARIABLE              $GTA_DIR
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE           "GTADirectoryLeaveProc"
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_TITLE_3LINES
; Launch from installer with user privileges
!define MUI_FINISHPAGE_RUN                      ""
!define MUI_FINISHPAGE_RUN_FUNCTION             "LaunchLink"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; INSERT OUR LANGUAGE STRINGS -----
!insertmacro MUI_LANGUAGE "English"

; Set file version information
!ifndef VI_PRODUCT_VERSION
    !ifdef REVISION
        !define VI_PRODUCT_VERSION "${0.0.0}.${REVISION}"
    !else
        !define VI_PRODUCT_VERSION "${0.0.0}.0"
    !endif
    !define VI_PRODUCT_NAME "MTA San Andreas"
    !define VI_COMPANY_NAME "Multi Theft Auto"
    !define /date DATE_YEAR "%Y"
    !define VI_LEGAL_COPYRIGHT "(C) 2003 - ${DATE_YEAR} Multi Theft Auto"
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

;@INSERT_TRANSLATIONS@

LangString	GET_XPVISTA_PLEASE	${LANG_ENGLISH} "The version of MTA:SA you've downloaded does not support Windows XP or Vista.  Please download an alternative version from www.mtasa.com."
LangString	GET_WIN81_PLEASE	${LANG_ENGLISH} "The version of MTA:SA you've downloaded does not support Windows 7, 8 or 8.1.  Please download an alternative version from www.mtasa.com."
LangString  GET_MASTER_PLEASE	${LANG_ENGLISH} "The version of MTA:SA you've downloaded is designed for old versions of Windows.  Please download an alternative version from www.mtasa.com."
LangString  WELCOME_TEXT  ${LANG_ENGLISH}   "This wizard will guide you through the installation or update of $(^Name) ${REVISION_TAG}\n\n\
It is recommended that you close all other applications before starting Setup.\n\n\
[Admin access may be requested for Vista and up]\n\n\
Click Next to continue."
LangString  HEADER_Text         ${LANG_ENGLISH} "Grand Theft Auto: San Andreas location"
LangString  DIRECTORY_Text_Dest ${LANG_ENGLISH} "Grand Theft Auto: San Andreas folder"
LangString  DIRECTORY_Text_Top  ${LANG_ENGLISH} "Please select your Grand Theft Auto: San Andreas folder.$\n$\nYou MUST have Grand Theft Auto: San Andreas 1.0 installed to use MTA:SA, it does not support any other versions.$\n$\nClick Install to begin installing."

; Language files
LangString  DESC_Section10          ${LANG_ENGLISH} "Create a Start Menu group for installed applications"
LangString  DESC_Section11          ${LANG_ENGLISH} "Create a Desktop Shortcut for the MTA:SA Client."
LangString  DESC_Section12          ${LANG_ENGLISH} "Register mtasa:// protocol for browser clickable-ness."
LangString  DESC_Section13          ${LANG_ENGLISH} "Add to Windows Games Explorer (if present)."
LangString  DESC_DirectX            ${LANG_ENGLISH} "Install or update DirectX (if required)."
LangString  DESC_Section1           ${LANG_ENGLISH} "The core components required to run Multi Theft Auto."
LangString  DESC_Section2           ${LANG_ENGLISH} "The MTA:SA modification, allowing you to play online."
;LangString DESC_Section3           ${LANG_ENGLISH} "The Multi Theft Auto:Editor for MTA:SA, allowing you to create and edit maps."
;LangString DESC_SectionGroupMods   ${LANG_ENGLISH} "Modifications for Multi Theft Auto. Without at least one of these, you cannot play Multi Theft Auto."
LangString  DESC_SectionGroupServer  ${LANG_ENGLISH}    "The Multi Theft Auto Server. This allows you to host games from your computer. This requires a fast internet connection."
LangString  DESC_Section4           ${LANG_ENGLISH} "The Multi Theft Auto server. This is a required component."
LangString  DESC_Section5           ${LANG_ENGLISH} "The MTA:SA modification for the server."
LangString  DESC_Section6           ${LANG_ENGLISH} "This is a set of required resources for your server."
LangString  DESC_Section7           ${LANG_ENGLISH} "This is an optional set of gamemodes and maps for your server."
LangString  DESC_Section8           ${LANG_ENGLISH} "The MTA:SA 1.0 Map Editor.  This can be used to create your very own maps for use in gamemodes for MTA."
LangString  DESC_Section9           ${LANG_ENGLISH} "This is the SDK for creating binary modules for the MTA server. Only install if you have a good understanding of C++!"
;LangString DESC_Blank          ${LANG_ENGLISH} ""
LangString  DESC_SectionGroupDev        ${LANG_ENGLISH} "Development code and tools that aid in the creation of mods for Multi Theft Auto"
LangString  DESC_SectionGroupClient  ${LANG_ENGLISH}    "The client is the program you run to play on a Multi Theft Auto server"


Function LaunchLink
    SetOutPath "$INSTDIR"
    # Problem: 'non-admin nsis' and 'admin nsis' run at the same time and can have different values for $INSTDIR
    # Fix: Copy to temp variable
    StrCpy $1 "$INSTDIR\Multi Theft Auto.exe"
    !insertmacro UAC_AsUser_ExecShell "" "$1" "" "" ""
FunctionEnd

Function .onInstFailed
    ${LogText} "+Function begin - .onInstFailed"
FunctionEnd

Function .onInit

    ${IfNot} ${UAC_IsInnerInstance}
        !insertmacro MUI_LANGDLL_DISPLAY  # Only display our language selection in the outer (non-admin) instance
    ${Else}
        !insertmacro UAC_AsUser_GetGlobalVar $LANGUAGE # Copy our selected language from the outer to the inner instance
    ${EndIf}

    ${If} ${AtMostWinVista}
        MessageBox MB_OK "$(GET_XPVISTA_PLEASE)"
        ExecShell "open" "https://multitheftauto.com"
        Quit
    ${ElseIf} ${AtMostWin8.1}
        MessageBox MB_OK "$(GET_WIN81_PLEASE)"
        ExecShell "open" "https://multitheftauto.com"
        Quit
    ${EndIf}

    File /oname=$TEMP\image.bmp "connect.bmp"

    SetShellVarContext all

    ${LogSetFileName} "$APPDATA\MTA San Andreas All\Common\Installer" "nsis.log"
    ${LogText} "${PRODUCT_VERSION} ${REVISION_TAG}"
    ${LogText} "+Function begin - .onInit"

    ; Try to find previously saved MTA:SA install path
    ReadRegStr $Install_Dir HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location"
    ${If} $Install_Dir == ""
        ReadRegStr $Install_Dir HKLM "SOFTWARE\Multi Theft Auto: San Andreas ${0.0}" "Last Install Location"
    ${EndIf}
    ${If} $Install_Dir != ""
        Call NoteMTAWasPresent
    ${EndIf}

    ${If} $Install_Dir == ""
        strcpy $Install_Dir "$PROGRAMFILES\MTA San Andreas ${0.0}"
    ${EndIf}
    strcpy $INSTDIR $Install_Dir
    ${LogText} "Using install directory:'$INSTDIR'"

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

    ; Report previous install status
    ${If} $2 != ""
        Call NoteGTAWasPresent
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

    ${LogText} "Default GTA install directory:'$GTA_DIR'"

    InitPluginsDir
    ;File /oname=$PLUGINSDIR\serialdialog.ini "serialdialog.ini"

    # Set Windows SID to use for permissions fixing
    Call SetPermissionsGroup
    ${LogText} "-Function end - .onInit"
FunctionEnd

Function .onInstSuccess
    ${LogText} "+Function begin - .onInstSuccess"
    SetShellVarContext all

    WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\Common" "GTA:SA Path" $GTA_DIR
    WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location" $INSTDIR

    # Add 'MaxLoaderThreads' DWORD value for gta_sa.exe to disable multi-threaded loading of DLLs.
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\gta_sa.exe" "MaxLoaderThreads" 1
	
	# Initilize variables holding paths and names
	Call MTAInitFileNamesAndPaths
    ; Start menu items
    ${If} $CreateSMShortcuts == 1
		${IfNot} ${FileExists} "$StartMenuMTAFolderPath\*.*"
			CreateDirectory $StartMenuMTAFolderPath
		${EndIf}
		# Either update or create Client shortcut
		${If} ${FileExists} $StartMenuClientShortcutPath
			Push $ClientExePath
			Push $StartMenuClientShortcutPath
			Call MTAUpdateShortсutTarget
		${Else}
			Push $ClientExePath
			Push $ClientExePath
			Push $StartMenuClientShortcutPath
			Call MTACreateShortсut
		${EndIf}
		# Either update or create Server shortcut
		${If} ${FileExists} $StartMenuServerShortcutPath
			Push $ServerExePath
			Push $StartMenuServerShortcutPath
			Call MTAUpdateShortсutTarget
		${Else}
			Push $ServerExePath
			Push $ServerExePath
			Push $StartMenuServerShortcutPath
			Call MTACreateShortсut
		${EndIf}
		# Either update or create Uninstall shortcut
		${If} ${FileExists} $StartMenuUninstallShortcutPath
			Push $UninstallExePath
			Push $StartMenuUninstallShortcutPath
			Call MTAUpdateShortсutTarget
		${Else}
			Push $UninstallExePath
			Push $UninstallExePath
			Push $StartMenuUninstallShortcutPath
			Call MTACreateShortсut
		${EndIf}
    ${EndIf}

    ${If} $CreateDesktopIcon == 1
		${If} ${FileExists} $DesktopClientShortcutPath
			Push $ClientExePath
			Push $DesktopClientShortcutPath
			Call MTAUpdateShortсutTarget
		${Else}
			Push $ClientExePath
			Push $ClientExePath
			Push $DesktopClientShortcutPath
			Call MTACreateShortсut
		${EndIf}
			AccessControl::GrantOnFile $DesktopClientShortcutPath "($PermissionsGroup)" "FullAccess"
    ${EndIf}

    ${If} $RegisterProtocol == 1
        ; Add the protocol handler
        WriteRegStr HKCR "mtasa" "" "URL:MTA San Andreas Protocol"
        WriteRegStr HKCR "mtasa" "URL Protocol" ""
        WriteRegStr HKCR "mtasa\DefaultIcon" "" "$INSTDIR\Multi Theft Auto.exe"
        WriteRegStr HKCR "mtasa\shell\open\command" "" '"$INSTDIR\Multi Theft Auto.exe"%1'
    ${EndIf}

    ;UAC::Unload ;Must call unload!
    ${LogText} "-Function end - .onInstSuccess"
FunctionEnd

LangString INST_CLIENTSERVER ${LANG_ENGLISH}    "Client and Server"
LangString INST_SERVER ${LANG_ENGLISH}  "Server only"


InstType "$(INST_CLIENTSERVER)"
InstType "$(INST_SERVER)"

Name "${PRODUCT_NAME_NO_VER} ${PRODUCT_VERSION}"
OutFile "${INSTALL_OUTPUT}"

;InstallDir "$PROGRAMfiles San Andreas"
InstallDirRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}" "Last Install Location"
ShowInstDetails show
ShowUnInstDetails show

LangString INST_STARTMENU_GROUP     ${LANG_ENGLISH} "Start menu group"
LangString INST_DESKTOP_ICON        ${LANG_ENGLISH} "Desktop icon"
LangString INST_PROTOCOL            ${LANG_ENGLISH} "Register mtasa:// protocol"
LangString INST_GAMES_EXPLORER      ${LANG_ENGLISH} "Add to Games Explorer"
LangString INST_DIRECTX             ${LANG_ENGLISH} "Install DirectX"

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

Section "$(INST_DIRECTX)" SEC_DIRECTX
    SectionIn 1 2
    SetOutPath "$TEMP"
    File "${FILES_ROOT}\redist\dxwebsetup.exe"
    DetailPrint "Running DirectX Setup..."
    ExecWait '"$TEMP\dxwebsetup.exe" /Q'
    DetailPrint "Finished DirectX Setup"
    Delete "$TEMP\dxwebsetup.exe"
    SetOutPath "$INSTDIR"
SectionEnd


LangString INST_SEC_CLIENT      ${LANG_ENGLISH} "Game client"
LangString INST_SEC_SERVER      ${LANG_ENGLISH} "Dedicated server"
LangString INST_SEC_CORE            ${LANG_ENGLISH} "Core components"
LangString INST_SEC_GAME            ${LANG_ENGLISH} "Game module"

LangString INFO_INPLACE_UPGRADE ${LANG_ENGLISH} "Performing in-place upgrade..."
LangString INFO_UPDATE_PERMISSIONS ${LANG_ENGLISH}  "Updating permissions. This could take a few minutes..."
LangString MSGBOX_INVALID_GTASA ${LANG_ENGLISH} "A valid Windows version of Grand Theft Auto: San Andreas was not detected.\
$\r$\nHowever installation will continue.\
$\r$\nPlease reinstall if there are problems later."
LangString INST_SEC_CORE_RESOURCES ${LANG_ENGLISH}  "Core Resources"
LangString INST_SEC_OPTIONAL_RESOURCES ${LANG_ENGLISH}  "Optional Resources"
LangString INST_SEC_EDITOR ${LANG_ENGLISH}  "Editor"

SectionGroup /e "$(INST_SEC_CLIENT)" SECGCLIENT
    Section "$(INST_SEC_CORE)" SEC01
        SectionIn 1 RO ; section is required
        ${LogText} "+Section begin - CLIENT CORE"

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

            # Check if install path begins with "..Program Files (x86)\M" or "..Program Files\M"
            StrCpy $3 "0"
            StrCpy $0 "$PROGRAMFILES\M"
            StrLen $2 $0
            StrCpy $1 "$INSTDIR" $2
            ${If} $0 == $1
                StrCpy $3 "1"
            ${EndIf}

            StrCpy $0 "$PROGRAMFILES64\M"
            StrLen $2 $0
            StrCpy $1 "$INSTDIR" $2
            ${If} $0 == $1
                StrCpy $3 "1"
            ${EndIf}

            ${LogText} "FullAccess $INSTDIR"
            ${If} $3 == "1"
                FastPerms::FullAccessPlox "$INSTDIR" "($PermissionsGroup)"
            ${Else}
                # More conservative permissions blat if install directory it too different from default
                CreateDirectory "$INSTDIR\mods"
                CreateDirectory "$INSTDIR\screenshots"
                CreateDirectory "$INSTDIR\server"
                CreateDirectory "$INSTDIR\skins"
                FastPerms::FullAccessPlox "$INSTDIR\mods" "($PermissionsGroup)"
                FastPerms::FullAccessPlox "$INSTDIR\MTA" "($PermissionsGroup)"
                FastPerms::FullAccessPlox "$INSTDIR\screenshots" "($PermissionsGroup)"
                FastPerms::FullAccessPlox "$INSTDIR\server" "($PermissionsGroup)"
                FastPerms::FullAccessPlox "$INSTDIR\skins" "($PermissionsGroup)"
            ${EndIf}
            ${LogText} "FullAccess $APPDATA\MTA San Andreas All"
            FastPerms::FullAccessPlox "$APPDATA\MTA San Andreas All" "($PermissionsGroup)"

            # Remove MTA virtual store
            StrCpy $0 $INSTDIR
            !insertmacro UAC_AsUser_Call Function RemoveVirtualStore ${UAC_SYNCREGISTERS}
            StrCpy $0 $INSTDIR
            Call RemoveVirtualStore

            Push $GTA_DIR
            Call IsGtaDirectory
            Pop $0
            ${If} $0 == "gta"
                # Fix permissions for GTA install directory
                FastPerms::FullAccessPlox "$GTA_DIR" "($PermissionsGroup)"

                # Remove GTA virtual store
                StrCpy $0 $GTA_DIR
                !insertmacro UAC_AsUser_Call Function RemoveVirtualStore ${UAC_SYNCREGISTERS}
                StrCpy $0 $GTA_DIR
                Call RemoveVirtualStore
            ${EndIf}
        ${EndIf}

        #############################################################
        # Issue warning if GTA path looks incorrect
        Push $GTA_DIR
        Call IsGtaDirectory
        Pop $0
        ${If} $0 != "gta"
            MessageBox MB_ICONSTOP "$(MSGBOX_INVALID_GTASA)"
        ${EndIf}
        #############################################################

        #############################################################
        # Install SHA2 support for older Win7 x64
        ${If} ${IsWin7}
            ${If} ${RunningX64}
                ${GetDLLVersionNumbers} "$SYSDIR\crypt32.dll" $0 $1 $2 $3
                ${If} $2 == 7601
                    ${If} $3 < 18741
                        ${InstallKB} "KB3035131" "Windows6.1-KB3035131-x64" "https://download.microsoft.com/download/3/D/F/3DF6B0B1-D849-4272-AA98-3AA8BB456CCC/Windows6.1-KB3035131-x64.msu"
                        ${InstallKB} "KB3033929" "Windows6.1-KB3033929-x64" "https://download.microsoft.com/download/C/8/7/C87AE67E-A228-48FB-8F02-B2A9A1238099/Windows6.1-KB3033929-x64.msu"
                    ${EndIf}
                ${EndIf}
            ${EndIf}
        ${EndIf}
        #############################################################

        #############################################################
        # Delete existing winmm.dll to prevent GTA process start conflicts
        Delete "$INSTDIR\MTA\winmm.dll"
        #############################################################

        SetOutPath "$INSTDIR\MTA"
        SetOverwrite on

        # Make some keys in HKLM read write accessible by all users
        AccessControl::GrantOnRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas All" "($PermissionsGroup)" "FullAccess"

        SetOutPath "$INSTDIR\MTA"
        File "${FILES_ROOT}\mta\cgui.dll"
        File "${FILES_ROOT}\mta\core.dll"
        File "${FILES_ROOT}\mta\xmll.dll"
        File "${FILES_ROOT}\mta\game_sa.dll"
        File "${FILES_ROOT}\mta\multiplayer_sa.dll"
        File "${FILES_ROOT}\mta\netc.dll"
        File "${FILES_ROOT}\mta\loader.dll"
        File "${FILES_ROOT}\mta\mtasa.dll"
        File "${FILES_ROOT}\mta\pthread.dll"
        File "${FILES_ROOT}\mta\cefweb.dll"
        File "${FILES_ROOT}\mta\libwow64.dll"
        File "${FILES_ROOT}\mta\wow64_helper.exe"

        File "${FILES_ROOT}\mta\bass.dll"
        File "${FILES_ROOT}\mta\bass_aac.dll"
        File "${FILES_ROOT}\mta\bass_ac3.dll"
        File "${FILES_ROOT}\mta\bass_fx.dll"
        File "${FILES_ROOT}\mta\bassflac.dll"
        File "${FILES_ROOT}\mta\bassmidi.dll"
        File "${FILES_ROOT}\mta\bassmix.dll"
        File "${FILES_ROOT}\mta\bassopus.dll"
        File "${FILES_ROOT}\mta\basswebm.dll"
        File "${FILES_ROOT}\mta\basswma.dll"
        File "${FILES_ROOT}\mta\tags.dll"

        SetOutPath "$INSTDIR\MTA"
        File "${FILES_ROOT}\mta\chrome_elf.dll"
        File "${FILES_ROOT}\mta\libcef.dll"
        File "${FILES_ROOT}\mta\icudtl.dat"
        File "${FILES_ROOT}\mta\libEGL.dll"
        File "${FILES_ROOT}\mta\libGLESv2.dll"
        File "${FILES_ROOT}\mta\vk_swiftshader.dll"
        File "${FILES_ROOT}\mta\vulkan-1.dll"
        File "${FILES_ROOT}\mta\snapshot_blob.bin"
        File "${FILES_ROOT}\mta\v8_context_snapshot.bin"

        File "${FILES_ROOT}\mta\XInput9_1_0_mta.dll"
        File "${FILES_ROOT}\mta\xinput1_3_mta.dll"
        File "${FILES_ROOT}\mta\d3dcompiler_43.dll"
        File "${FILES_ROOT}\mta\d3dcompiler_47.dll"

        SetOutPath "$INSTDIR\MTA\CEF"
        File "${FILES_ROOT}\mta\CEF\CEFLauncher.exe"
        File "${FILES_ROOT}\mta\CEF\CEFLauncher_DLL.dll"


	# Added as per https://bitbucket.org/chromiumembedded/cef/commits/8424f166ccef
	# Not currently using \mta\cef\ due to https://github.com/chromiumembedded/cef/issues/3749#issuecomment-2278568964 (it's already crashing and likely won't remain supported)
        SetOutPath "$INSTDIR\MTA"

        File "${FILES_ROOT}\mta\chrome_100_percent.pak"
        File "${FILES_ROOT}\mta\chrome_200_percent.pak"
        File "${FILES_ROOT}\mta\resources.pak"

	# Clarification for the below 4 deprecated files: https://bitbucket.org/chromiumembedded/cef/commits/8424f166ccef
        #File "${FILES_ROOT}\mta\CEF\cef.pak"
        #File "${FILES_ROOT}\mta\CEF\cef_100_percent.pak"
        #File "${FILES_ROOT}\mta\CEF\cef_200_percent.pak"
        #File "${FILES_ROOT}\mta\CEF\devtools_resources.pak"
		
	# Below file was included in the deprecation referenced above, but already disabled in MTA beforehand
        #File "${FILES_ROOT}\mta\CEF\cef_extensions.pak"

        SetOutPath "$INSTDIR\MTA\CEF\locales"
        File "${FILES_ROOT}\mta\CEF\locales\en-US.pak"


        ${If} "$(LANGUAGE_CODE)" != ""
            # Write our language to registry
            WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}\Settings\general" "locale" "$(LANGUAGE_CODE)"
        ${EndIf}

        !ifndef LIGHTBUILD

            SetOutPath "$INSTDIR\MTA"
            File "${FILES_ROOT}\mta\d3dx9_42.dll"
            File "${FILES_ROOT}\mta\D3DCompiler_42.dll"
            File "${FILES_ROOT}\mta\sa.dat"
            File "${FILES_ROOT}\mta\XInput9_1_0_mta.dll"
            File "${FILES_ROOT}\mta\xinput1_3_mta.dll"

            File "${FILES_ROOT}\mta\d3dcompiler_43.dll"
            File "${FILES_ROOT}\mta\d3dcompiler_47.dll"

            SetOutPath "$INSTDIR\MTA\data"
            File "${FILES_ROOT}\mta\data\gta_sa_diff.dat"

            SetOutPath "$INSTDIR\MTA\config"
            File "${FILES_ROOT}\mta\config\chatboxpresets.xml"

            SetOutPath "$INSTDIR\skins\Classic"
            File "${FILES_ROOT}\skins\Classic\CGUI.is.xml"
            File "${FILES_ROOT}\skins\Classic\CGUI.lnf.xml"
            File "${FILES_ROOT}\skins\Classic\CGUI.png"
            File "${FILES_ROOT}\skins\Classic\CGUI.xml"

            SetOutPath "$INSTDIR\skins\Default"
            File "${FILES_ROOT}\skins\Default\CGUI.is.xml"
            File "${FILES_ROOT}\skins\Default\CGUI.lnf.xml"
            File "${FILES_ROOT}\skins\Default\CGUI.png"
            File "${FILES_ROOT}\skins\Default\CGUI.xml"

            SetOutPath "$INSTDIR\skins\Lighter black"
            File "${FILES_ROOT}\skins\Lighter black\CGUI.is.xml"
            File "${FILES_ROOT}\skins\Lighter black\CGUI.lnf.xml"
            File "${FILES_ROOT}\skins\Lighter black\CGUI.png"
            File "${FILES_ROOT}\skins\Lighter black\CGUI.xml"

            SetOutPath "$INSTDIR\skins\Default 2023"
            File "${FILES_ROOT}\skins\Default 2023\CGUI.is.xml"
            File "${FILES_ROOT}\skins\Default 2023\CGUI.lnf.xml"
            File "${FILES_ROOT}\skins\Default 2023\CGUI.png"
            File "${FILES_ROOT}\skins\Default 2023\CGUI.xml"

            SetOutPath "$INSTDIR\skins\GWEN Blue"
            File "${FILES_ROOT}\skins\GWEN Blue\CGUI.is.xml"
            File "${FILES_ROOT}\skins\GWEN Blue\CGUI.lnf.xml"
            File "${FILES_ROOT}\skins\GWEN Blue\CGUI.png"
            File "${FILES_ROOT}\skins\GWEN Blue\CGUI.xml"

            SetOutPath "$INSTDIR\skins\GWEN Orange"
            File "${FILES_ROOT}\skins\GWEN Orange\CGUI.is.xml"
            File "${FILES_ROOT}\skins\GWEN Orange\CGUI.lnf.xml"
            File "${FILES_ROOT}\skins\GWEN Orange\CGUI.png"
            File "${FILES_ROOT}\skins\GWEN Orange\CGUI.xml"

            SetOutPath "$INSTDIR\MTA\cgui"
            File "${FILES_ROOT}\mta\cgui\Falagard.xsd"
            File "${FILES_ROOT}\mta\cgui\Font.xsd"
            File "${FILES_ROOT}\mta\cgui\GUIScheme.xsd"
            File "${FILES_ROOT}\mta\cgui\Imageset.xsd"
            File "${FILES_ROOT}\mta\cgui\pricedown.ttf"
            File "${FILES_ROOT}\mta\cgui\sabankgothic.ttf"
            File "${FILES_ROOT}\mta\cgui\sagothic.ttf"
            File "${FILES_ROOT}\mta\cgui\saheader.ttf"
            File "${FILES_ROOT}\mta\cgui\sans.ttf"
            File "${FILES_ROOT}\mta\cgui\unifont.ttf"

            SetOutPath "$INSTDIR\MTA\cgui\images"
            File "${FILES_ROOT}\mta\cgui\images\*.png"
            File "${FILES_ROOT}\mta\cgui\images\*.jpg"

            SetOutPath "$INSTDIR\MTA\cgui\images\radarset"
            File "${FILES_ROOT}\mta\cgui\images\radarset\*.png"

            SetOutPath "$INSTDIR\MTA\cgui\images\transferset"
            File "${FILES_ROOT}\mta\cgui\images\transferset\*.png"

            SetOutPath "$INSTDIR\MTA\cgui\images\serverbrowser"
            File "${FILES_ROOT}\mta\cgui\images\serverbrowser\*.png"

        !endif

        SetOutPath "$INSTDIR\MTA\locale\"
        File /r "${FILES_ROOT}\mta\locale\*.png"
        File /r "${FILES_ROOT}\mta\locale\*.pot"

        SetOutPath "$INSTDIR"
        File "${FILES_ROOT}\Multi Theft Auto.exe"

        # Ensure exe file can be updated without admin
        AccessControl::GrantOnFile "$INSTDIR\Multi Theft Auto.exe" "($PermissionsGroup)" "FullAccess"

        ${If} $AddToGameExplorer == 1
            ${GameExplorer_UpdateGame} ${GUID}
            ${If} ${Errors}
                ${GameExplorer_AddGame} all "$INSTDIR\Multi Theft Auto.exe" "$INSTDIR" "$INSTDIR\Multi Theft Auto.exe" ${GUID}
                CreateDirectory $APPDATA\Microsoft\Windows\GameExplorer\${GUID}\SupportTasks\0
                CreateShortcut "$APPDATA\Microsoft\Windows\GameExplorer\$0\SupportTasks\0\Client Manual.lnk" \ "https://wiki.multitheftauto.com/wiki/Client_Manual"
            ${EndIf}
        ${EndIf}

        Call DoServiceInstall
        ${LogText} "-Section end - CLIENT CORE"
    SectionEnd

    Section "$(INST_SEC_GAME)" SEC02
        ${LogText} "+Section begin - CLIENT GAME"
        SectionIn 1 RO
        SetOutPath "$INSTDIR\mods\deathmatch"
        File "${FILES_ROOT}\mods\deathmatch\client.dll"
        File "${FILES_ROOT}\mods\deathmatch\pcre3.dll"
        SetOutPath "$INSTDIR\mods\deathmatch\resources"
        ${LogText} "-Section end - CLIENT GAME"
    SectionEnd
SectionGroupEnd

SectionGroup /e "$(INST_SEC_SERVER)" SECGSERVER
    Section "$(INST_SEC_CORE)" SEC04
        ${LogText} "+Section begin - SERVER CORE"
        SectionIn 1 2 RO ; section is required

        SetOutPath "$INSTDIR\server"
        SetOverwrite on
        File "${SERVER_FILES_ROOT}\core.dll"
        File "${FILES_ROOT}\mta\xmll.dll"
        File "${SERVER_FILES_ROOT}\MTA Server.exe"
        File "${SERVER_FILES_ROOT}\net.dll"
        File "${FILES_ROOT}\mta\pthread.dll"
        ${LogText} "-Section end - SERVER CORE"
    SectionEnd

    Section "$(INST_SEC_GAME)" SEC05
        ${LogText} "+Section begin - SERVER GAME"
        SectionIn 1 2 RO ; section is required
        SetOutPath "$INSTDIR\server\mods\deathmatch"

        SetOverwrite on
        File "${SERVER_FILES_ROOT}\mods\deathmatch\deathmatch.dll"
        File "${SERVER_FILES_ROOT}\mods\deathmatch\lua5.1.dll"
        File "${FILES_ROOT}\mods\deathmatch\pcre3.dll"
        File "${SERVER_FILES_ROOT}\mods\deathmatch\dbconmy.dll"
        !ifndef LIGHTBUILD
            File "${SERVER_FILES_ROOT}\mods\deathmatch\libmysql.dll"
            File "${SERVER_FILES_ROOT}\mods\deathmatch\libcrypto-3.dll"
            File "${SERVER_FILES_ROOT}\mods\deathmatch\libssl-3.dll"
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
        ${LogText} "-Section end - SERVER GAME"
    SectionEnd

    !ifndef LIGHTBUILD
        Section "$(INST_SEC_CORE_RESOURCES)" SEC06
            ${LogText} "+Section begin - SERVER CORE_RESOURCES"
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
            ${LogText} "-Section end - SERVER CORE_RESOURCES"
        SectionEnd
    !endif

    !ifndef LIGHTBUILD
        SectionGroup "$(INST_SEC_OPTIONAL_RESOURCES)" SEC07
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

LangString INST_SEC_DEVELOPER ${LANG_ENGLISH}   "Development"
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
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_DIRECTX} $(DESC_DirectX)
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
    ${LogText} "+Section begin - -Post"
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    ;WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Multi Theft Auto.exe"

    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Multi Theft Auto.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
    ${LogText} "-Section end - -Post"
SectionEnd

LangString UNINST_SUCCESS ${LANG_ENGLISH}   "$(^Name) was successfully removed from your computer."
Function un.onUninstSuccess
    HideWindow
    MessageBox MB_ICONINFORMATION|MB_OK "$(UNINST_SUCCESS)"
    ;UAC::Unload ;Must call unload! ; #3017 fix
FunctionEnd

LangString UNINST_FAIL ${LANG_ENGLISH}  "Uninstallation has failed!"
Function un.OnUnInstFailed
    HideWindow
    MessageBox MB_ICONSTOP|MB_OK "$(UNINST_FAIL)"
    ;UAC::Unload ;Must call unload! ; #3017 fix
FunctionEnd


LangString UNINST_REQUEST ${LANG_ENGLISH}   "Are you sure you want to completely remove $(^Name) and all of its components?"
LangString UNINST_REQUEST_NOTE ${LANG_ENGLISH}  "Uninstalling before update?\
$\r$\nIt is not necessary to uninstall before installing a new version of MTA:SA\
$\r$\nRun the new installer to upgrade and preserve your settings."

Function un.onInit
    Call un.DoRightsElevation
    SetShellVarContext all
        MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "$(UNINST_REQUEST_NOTE)$\r$\n$\r$\n$\r$\n$(UNINST_REQUEST)" IDYES +2
        Abort

    !insertmacro MUI_UNGETLANGUAGE
FunctionEnd

LangString UNINST_DATA_REQUEST ${LANG_ENGLISH}  "Would you like to keep your data files (such as resources, screenshots and server configuration)? If you click no, any resources, configurations or screenshots you have created will be lost."
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
        RmDir /r "$INSTDIR\skins"
        RmDir /r "$INSTDIR\development"
        RmDir /r "$INSTDIR\screenshots"

    preservemapsfolder:
        Call un.DoServiceUninstall
        ; server CORE FILES
        Delete "$INSTDIR\server\core.dll"
        Delete "$INSTDIR\server\MTA Server.exe"
        Delete "$INSTDIR\server\net.dll"
        Delete "$INSTDIR\server\pthread.dll"
        Delete "$INSTDIR\server\xmll.dll"

        ; server files
        Delete "$INSTDIR\server\mods\deathmatch\dbconmy.dll"
        Delete "$INSTDIR\server\mods\deathmatch\deathmatch.dll"
        Delete "$INSTDIR\server\mods\deathmatch\libmysql.dll"
        Delete "$INSTDIR\server\mods\deathmatch\libcrypto-3.dll"
        Delete "$INSTDIR\server\mods\deathmatch\libssl-3.dll"
        Delete "$INSTDIR\server\mods\deathmatch\lua5.1.dll"
        Delete "$INSTDIR\server\mods\deathmatch\pcre3.dll"

        ; server x64 CORE FILES
        Delete "$INSTDIR\server\MTA Server64.exe"
        Delete "$INSTDIR\server\x64\core.dll"
        Delete "$INSTDIR\server\x64\net.dll"
        Delete "$INSTDIR\server\x64\pthread.dll"
        Delete "$INSTDIR\server\x64\xmll.dll"

        ; server x64 files
        Delete "$INSTDIR\server\x64\dbconmy.dll"
        Delete "$INSTDIR\server\x64\deathmatch.dll"
        Delete "$INSTDIR\server\x64\libmysql.dll"
        Delete "$INSTDIR\server\x64\libcrypto-3-x64.dll"
        Delete "$INSTDIR\server\x64\libssl-3-x64.dll"
        Delete "$INSTDIR\server\x64\lua5.1.dll"
        Delete "$INSTDIR\server\x64\pcre3.dll"
        RmDir "$INSTDIR\server\x64"

        Delete "$INSTDIR\Multi Theft Auto.exe"
        Delete "$INSTDIR\Multi Theft Auto.exe.dat"
        Delete "$INSTDIR\Uninstall.exe"

        Delete "$INSTDIR\mods\deathmatch\client.dll"
        Delete "$INSTDIR\mods\deathmatch\pcre3.dll"

        RmDir /r "$INSTDIR\MTA\cgui"
        RmDir /r "$INSTDIR\MTA\data"
        RmDir /r "$INSTDIR\MTA\CEF"
        RmDir /r "$INSTDIR\MTA\locale"
        Delete "$INSTDIR\MTA\*.dll"
        Delete "$INSTDIR\MTA\*.exe"
        Delete "$INSTDIR\MTA\*.dmp"
        Delete "$INSTDIR\MTA\*.log"
        Delete "$INSTDIR\MTA\*.dat"
        Delete "$INSTDIR\MTA\*.bin"

        RmDir /r "$APPDATA\MTA San Andreas All\${0.0}"
        ; Delete "$APPDATA\MTA San Andreas All" if "Common" is the only directory in it.
        ${RmDirWithSingleChildDir} "$APPDATA\MTA San Andreas All" "Common"

        DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
        DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
        DeleteRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas ${0.0}"
        DeleteRegKey HKCU "SOFTWARE\Multi Theft Auto: San Andreas ${0.0}"
        DeleteRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas All\${0.0}"
        ; Delete "SOFTWARE\Multi Theft Auto: San Andreas All" if "Common" is the only one left.
        ${RemoveRegistryGroupWithSingleKey} HKLM "SOFTWARE\Multi Theft Auto: San Andreas All" "Common"

        ReadRegStr $0 HKLM "Software\Classes\mtasa\DefaultIcon" ""
        ${If} $0 == "$INSTDIR\Multi Theft Auto.exe"
            DeleteRegKey HKCR "mtasa"
        ${EndIf}

        # Remove 'MaxLoaderThreads' DWORD value for gta_sa.exe.
        DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\gta_sa.exe" "MaxLoaderThreads"
        DeleteRegKey /ifempty HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\gta_sa.exe"

        ${GameExplorer_RemoveGame} ${GUID}

        ; Delete client shortcuts
        Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\MTA San Andreas.lnk"
        Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\Uninstall MTA San Andreas.lnk"
        Delete "$DESKTOP\MTA San Andreas ${0.0}.lnk"

        ; Delete server shortcuts
        Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\MTA Server.lnk"
        Delete "$SMPROGRAMS\\MTA San Andreas ${0.0}\Uninstall MTA San Andreas Server.lnk"
        RmDir /r "$SMPROGRAMS\\MTA San Andreas ${0.0}"

        RmDir "$INSTDIR" ; fix for #3898

        DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\Multi Theft Auto.exe.FriendlyAppName"
        DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\Multi Theft Auto.exe.ApplicationCompany"
        DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\MTA\wow64_helper.exe.FriendlyAppName"
        DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\MTA\wow64_helper.exe.ApplicationCompany"
        DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\server\MTA Server.exe.FriendlyAppName"
        DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\server\MTA Server.exe.ApplicationCompany"
        DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\server\MTA Server64.exe.FriendlyAppName"
        DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\server\MTA Server64.exe.ApplicationCompany"
        DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$APPDATA\MTA San Andreas All\${0.0}\GTA San Andreas\gta_sa.exe.FriendlyAppName"
        DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Compatibility Assistant\Store" "$INSTDIR\Multi Theft Auto.exe"
        DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Compatibility Assistant\Store" "$INSTDIR\server\MTA Server.exe"
        DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Compatibility Assistant\Store" "$INSTDIR\server\MTA Server64.exe"
        DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Compatibility Assistant\Store" "$INSTDIR\Uninstall.exe"

        ; Clean up shortcut leftovers from registry (is this even needed or correct?)
        StrCpy $7 0
    loop:
        ClearErrors
        EnumRegValue $8 HKCU Software\Microsoft\Windows\CurrentVersion\UFH\SHC $7
        IfErrors done

        ${ReadRegStrMultiSz} ${HKEY_CURRENT_USER} "Software\Microsoft\Windows\CurrentVersion\UFH\SHC" $8 "$INSTDIR\Multi Theft Auto.exe" $9
        ${If} $9 != ""
            DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\UFH\SHC" $8
            Goto loop
        ${EndIf}

        ${ReadRegStrMultiSz} ${HKEY_CURRENT_USER} "Software\Microsoft\Windows\CurrentVersion\UFH\SHC" $8 "$INSTDIR\server\MTA Server.exe" $9
        ${If} $9 != ""
            DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\UFH\SHC" $8
            Goto loop
        ${EndIf}

        ${ReadRegStrMultiSz} ${HKEY_CURRENT_USER} "Software\Microsoft\Windows\CurrentVersion\UFH\SHC" $8 "$INSTDIR\Uninstall.exe" $9
        ${If} $9 != ""
            DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\UFH\SHC" $8
            Goto loop
        ${EndIf}

        ; Clean up after old bugged path
        ${ReadRegStrMultiSz} ${HKEY_CURRENT_USER} "Software\Microsoft\Windows\CurrentVersion\UFH\SHC" $8 "$INSTDIR\MTA Server.exe" $9
        ${If} $9 != ""
            DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\UFH\SHC" $8
            Goto loop
        ${EndIf}

        ; Clean up after old bugged path
        ${ReadRegStrMultiSz} ${HKEY_CURRENT_USER} "Software\Microsoft\Windows\CurrentVersion\UFH\SHC" $8 "$DESKTOP\Uninstall.exe" $9
        ${If} $9 != ""
            DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\UFH\SHC" $8
            Goto loop
        ${EndIf}

        IntOp $7 $7 + 1
        Goto loop
    done:
        SimpleFC::RemoveApplication "$INSTDIR\Multi Theft Auto.exe"
        SimpleFC::RemoveApplication "$INSTDIR\server\MTA Server.exe"
        SimpleFC::RemoveApplication "$INSTDIR\server\MTA Server64.exe"
        SimpleFC::RemoveApplication "$INSTDIR\Uninstall.exe"
        SimpleFC::RemoveApplication "$APPDATA\MTA San Andreas All\${0.0}\GTA San Andreas\gta_sa.exe"

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
; UAC related functions
;====================================================================================
LangString UAC_RIGHTS1 ${LANG_ENGLISH}  "This installer requires admin access, try again"
LangString UAC_RIGHTS_UN ${LANG_ENGLISH}    "This uninstaller requires admin access, try again"
LangString UAC_RIGHTS3 ${LANG_ENGLISH}  "Logon service not running, aborting!"
LangString UAC_RIGHTS4 ${LANG_ENGLISH}  "Unable to elevate"
!macro RightsElevation AdminError
    uac_tryagain:
    !insertmacro UAC_RunElevated
    #MessageBox mb_TopMost "0=$0 1=$1 2=$2 3=$3"
    ${Switch} $0
        ${Case} 0
            ${IfThen} $1 = 1 ${|} Quit ${|}         ; we are the outer process, the inner process has done its work, we are done
            ${IfThen} $3 <> 0 ${|} ${Break} ${|}    ; we are admin, let the show go on
            ${If} $1 = 3                            ; RunAs completed successfully, but with a non-admin user
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
    ${LogText} "+Function begin - RightsElevation"
    !insertmacro RightsElevation "$(UAC_RIGHTS1)"
    ${LogText} "-Function end - RightsElevation"
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
;   $ITEM_PARENT - Parent window
;   $ITEM_ID     - Dialog ID
;   $X $Y        - Position change
;   $CX $CY      - Size change
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
;   $RESIZE_X $RESIZE_Y     - Resize amount
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
    StrCpy $ITEM_ID 1043    ; Static - "Position your mouse over a component to see its description."
    Call MoveDialogItem

    StrCpy $ITEM_PARENT $HWND_DIALOG
    StrCpy $ITEM_ID 1042    ; Button - Description
    Call MoveDialogItem

    ${If} "$(LANGUAGE_RTL)" == "1"
        StrCpy $X $RESIZE_X
        StrCpy $Y 0
        StrCpy $CX 0
        StrCpy $CY 0

        StrCpy $ITEM_PARENT $HWND_DIALOG
        StrCpy $ITEM_ID 1021    ; Static - "Select the type of install."
        Call MoveDialogItem

        StrCpy $ITEM_PARENT $HWND_DIALOG
        StrCpy $ITEM_ID 1022    ; Static - "Or, select the optional components you wish to install."
        Call MoveDialogItem

        StrCpy $ITEM_PARENT $HWND_DIALOG
        StrCpy $ITEM_ID 1023    ; Static - "Space required: XX MB."
        Call MoveDialogItem
    ${EndIf}

    ;Middle zone bigger
    StrCpy $X 0
    StrCpy $Y 0
    StrCpy $CX $RESIZE_X
    StrCpy $CY $RESIZE_Y
    StrCpy $ITEM_PARENT $HWNDPARENT
    StrCpy $ITEM_ID 0       ; Sub dialog
    Call MoveDialogItem

    ;Make tree view bigger
    StrCpy $X 0
    StrCpy $Y 0
    StrCpy $CX $RESIZE_X
    StrCpy $CY $RESIZE_Y
    StrCpy $ITEM_PARENT $HWND_DIALOG
    StrCpy $ITEM_ID 1032    ; Tree view
    Call MoveDialogItem

    ;Stretch combo box to the right
    StrCpy $X 0
    StrCpy $Y 0
    StrCpy $CX $RESIZE_X
    StrCpy $CY 0

    StrCpy $ITEM_PARENT $HWND_DIALOG
    StrCpy $ITEM_ID 1017    ; Combo box
    Call MoveDialogItem

    ;Move space required text down
    StrCpy $X 0
    StrCpy $Y $RESIZE_Y
    StrCpy $CX 0
    StrCpy $CY 0

    StrCpy $ITEM_PARENT $HWND_DIALOG
    StrCpy $ITEM_ID 1023    ; Static
    Call MoveDialogItem

FunctionEnd


; Input:
;   $RESIZE_X $RESIZE_X     - Resize amount
Function ResizeSharedDialogContents

    ;Move buttons down and right
    StrCpy $X $RESIZE_X
    StrCpy $Y $RESIZE_Y
    StrCpy $CX 0
    StrCpy $CY 0

    StrCpy $ITEM_PARENT $HWNDPARENT
    StrCpy $ITEM_ID 1   ; Button - Next
    Call MoveDialogItem

    StrCpy $ITEM_PARENT $HWNDPARENT
    StrCpy $ITEM_ID 2   ; Button - Cancel
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
    StrCpy $ITEM_ID 1256    ; Static - "Nullsoft Install System..."
    Call MoveDialogItem

    StrCpy $ITEM_PARENT $HWNDPARENT
    StrCpy $ITEM_ID 1028    ; Static - "Nullsoft Install System..."
    Call MoveDialogItem

    ;Move lower horizontal line down and stretch to the right
    StrCpy $X 0
    StrCpy $Y $RESIZE_Y
    StrCpy $CX $RESIZE_X
    StrCpy $CY 0
    StrCpy $ITEM_PARENT $HWNDPARENT
    StrCpy $ITEM_ID 1035    ; Static - Line
    Call MoveDialogItem

    ;Stretch header to the right
    StrCpy $X 0
    StrCpy $Y 0
    StrCpy $CX $RESIZE_X
    StrCpy $CY 0
    StrCpy $ITEM_PARENT $HWNDPARENT
    StrCpy $ITEM_ID 1034    ; Static - White bar
    Call MoveDialogItem

    StrCpy $ITEM_PARENT $HWNDPARENT
    StrCpy $ITEM_ID 1036    ; Static - Line
    Call MoveDialogItem

    ;Move header text to the right
    StrCpy $X $RESIZE_X
    StrCpy $Y 0
    StrCpy $CX 0
    StrCpy $CY 0
    StrCpy $ITEM_PARENT $HWNDPARENT
    StrCpy $ITEM_ID 1037    ; Static - "Choose Components"
    Call MoveDialogItem

    StrCpy $ITEM_PARENT $HWNDPARENT
    StrCpy $ITEM_ID 1038    ; Static - "Choose which features of MTA:SA v1.0 you want to install."
    Call MoveDialogItem

    ${If} "$(LANGUAGE_RTL)" == "1"
        ;Move image to the right most end if RTL
        StrCpy $X $RESIZE_X
        StrCpy $Y 0
        StrCpy $CX 0
        StrCpy $CY 0

        StrCpy $ITEM_PARENT $HWNDPARENT
        StrCpy $ITEM_ID 1046    ; Static - mta_install_header_rtl.bmp
        Call MoveDialogItem
    ${EndIf}

FunctionEnd


!define SWP_NOOWNERZORDER       0x0200

; Input:
;   $RESIZE_X $RESIZE_X     - Resize amount
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
    ${LogText} "+Function begin - WelcomePreProc"
    !insertmacro UAC_IsInnerInstance
    ${If} ${UAC_IsInnerInstance}
        ; If switched to admin, don't show welcome window again
        ${LogText} "-Function end - WelcomePreProc (IsInnerInstance)"
        Abort
    ${EndIf}
    ${LogText} "-Function end - WelcomePreProc"
FunctionEnd

Function "WelcomeShowProc"
    ${LogText} "+Function begin - WelcomeShowProc"
    BringToFront
    ${LogText} "-Function end - WelcomeShowProc"
FunctionEnd

Function "WelcomeLeaveProc"
    ${LogText} "+Function begin - WelcomeLeaveProc"
    HideWindow
    ; Maybe switch to admin after welcome window
    Call DoRightsElevation
    ShowWindow $HWNDPARENT ${SW_SHOW}
    ${LogText} "-Function end - WelcomeLeaveProc"
FunctionEnd


Function "LicenseShowProc"
    ${LogText} "+Function begin - LicenseShowProc"
    Call UnexpandComponentsPage
    Call HideBackButton
    BringToFront
    ${LogText} "-Function end - LicenseShowProc"
FunctionEnd

Function "LicenseLeaveProc"
    ${LogText} "+Function begin - LicenseLeaveProc"
    ${LogText} "-Function end - LicenseLeaveProc"
FunctionEnd


Function "ComponentsShowProc"
    ${LogText} "+Function begin - ComponentsShowProc"
    Call ExpandComponentsPage
    ${LogText} "-Function end - ComponentsShowProc"
FunctionEnd

Function "ComponentsLeaveProc"
    ${LogText} "+Function begin - ComponentsLeaveProc"
    ${LogText} "-Function end - ComponentsLeaveProc"
FunctionEnd


Function "ExpandComponentsPage"
    ${LogText} "+Function begin - ExpandComponentsPage"
    ${If} $COMPONENTS_EXPAND_STATUS != 1
        ${LogText} "Doing expand"
        StrCpy $COMPONENTS_EXPAND_STATUS 1
        IntOp $RESIZE_X 0 + ${EXPAND_DIALOG_X}
        IntOp $RESIZE_Y 0 + ${EXPAND_DIALOG_Y}
        Call ResizeComponentsDialogContents
        Call ResizeSharedDialogContents
        Call ResizeMainWindow
    ${Endif}
    ${LogText} "-Function end - ExpandComponentsPage"
FunctionEnd

Function "UnexpandComponentsPage"
    ${LogText} "+Function begin - UnexpandComponentsPage"
    ${If} $COMPONENTS_EXPAND_STATUS == 1
        ${LogText} "Doing unexpand"
        StrCpy $COMPONENTS_EXPAND_STATUS 0
        IntOp $RESIZE_X 0 - ${EXPAND_DIALOG_X}
        IntOp $RESIZE_Y 0 - ${EXPAND_DIALOG_Y}
        Call ResizeSharedDialogContents
        Call ResizeMainWindow
    ${Endif}
    ${LogText} "-Function end - UnexpandComponentsPage"
FunctionEnd


;****************************************************************
;
; Remove virtual store version of path
;
;****************************************************************
; In $0 = install path
Function RemoveVirtualStore
    StrCpy $2 $0 "" 3     # Remove drive path (first 3 chars)
    StrCpy $3 "$LOCALAPPDATA\VirtualStore\$2"
    StrCpy $4 "$0\FromVirtualStore"
    ${If} ${FileExists} $3
        ${LogText} "Moving VirtualStore files from $3 to $4"
        CopyFiles $3\*.* $4
        RmDir /r "$3"
    ${Else}
        ${LogText} "No VirtualStore detected at $3"
    ${EndIf}

    ; Also remove VirtualStore\ProgramData\MTA San Andreas All
    StrCpy $3 "$LOCALAPPDATA\VirtualStore\ProgramData\MTA San Andreas All"
    ${If} ${FileExists} $3
        ${LogText} "Removing $3"
        RmDir /r "$3"
    ${Else}
        ${LogText} "No VirtualStore detected at $3"
    ${EndIf}
FunctionEnd


;****************************************************************
;
; Returns 1 if file exists and is 32 bit.
;
;****************************************************************
Function IsDll32Bit
    Pop $3
    StrCpy $2 ""
    ClearErrors
    FileOpen $0 $3 r
    IfErrors done
    FileSeek $0 60    ;   IMAGE_DOS_HEADER->e_lfanew
    FileReadWord $0 $1
    FileSeek $0 $1    ;   IMAGE_NT_HEADERS
    FileSeek $0 4 CUR ;   IMAGE_FILE_HEADER->Machine
    FileReadWord $0 $2  ; $2 = Machine
    FileClose $0
done:
    StrCpy $1 "0"
    ${If} $2 == 332     ; 0x014c IMAGE_FILE_MACHINE_I386
        StrCpy $1 "1"
    ${EndIf}
    ${LogText} "IsDll32Bit($3) result:$1"
    Push $1
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
    ${LogText} "+Function begin - GetInstallType"
    Pop $0
    Push $0
    Call GetVersionAtLocation
    StrCpy $1 $0 3  # First 3 chars

    ${If} $1 == "0.0"
        StrCpy $2 "new"
    ${ElseIf} $1 == ${0.0}
        StrCpy $2 "upgrade"
    ${Else}
        StrCpy $2 "overwrite"
    ${EndIf}
    Pop $0
    ${LogText} "GetInstallType($0) result:$1,$2"
    Push $1
    Push $2
    ${LogText} "-Function end - GetInstallType"
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


LangString INST_MTA_CONFLICT ${LANG_ENGLISH}    "A different major version of MTA ($1) already exists at that path.$\n$\n\
            MTA is designed for major versions to be installed in different paths.$\n \
            Are you sure you want to overwrite MTA $1 at \
            $INSTDIR ?"
LangString INST_GTA_CONFLICT ${LANG_ENGLISH}    "MTA cannot be installed into the same directory as GTA:SA.$\n$\n\
            Do you want to use the default install directory$\n\
            $DEFAULT_INSTDIR ?"
LangString INST_GTA_ERROR1 ${LANG_ENGLISH} "The selected directory does not exist.$\n$\n\
            Please select the GTA:SA install directory"
LangString INST_GTA_ERROR2 ${LANG_ENGLISH} "Could not find GTA:SA installed at $GTA_DIR $\n$\n\
            Are you sure you want to continue ?"

Function "CustomDirectoryPageLeave"
    ${LogText} "+Function begin - CustomDirectoryPageLeave"
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
                ${LogText} "-Function end - CustomDirectoryPageLeave (GTA_CONFLICT)"
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
            ${LogText} "-Function end - CustomDirectoryPageLeave (MTA_CONFLICT)"
            Abort
        cont:
    ${Endif}
    ${LogText} "-Function end - CustomDirectoryPageLeave"
FunctionEnd


Function "GTADirectoryLeaveProc"
    ${LogText} "+Function begin - GTADirectoryLeaveProc"

    ; Directory must exist
    IfFileExists "$GTA_DIR\*.*" hasdir
        MessageBox MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND \
            "$(INST_GTA_ERROR1)"
            ${LogText} "-Function end - GTADirectoryLeaveProc (GTA_ERROR1)"
            Abort
    hasdir:

    ; models\gta3.img should exist
    IfFileExists "$GTA_DIR\models\gta3.img" cont
        MessageBox MB_OKCANCEL|MB_ICONQUESTION|MB_TOPMOST|MB_SETFOREGROUND \
            "$(INST_GTA_ERROR2)" \
            IDOK cont1
            ${LogText} "-Function end - GTADirectoryLeaveProc (GTA_ERROR2)"
            Abort
        cont1:
    cont:
    ${LogText} "-Function end - GTADirectoryLeaveProc"
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

    ; models\gta3.img should exist
    ${IfNot} ${FileExists} "$0\models\gta3.img"
        StrCpy $1 ""
    ${EndIf}

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

LangString INST_CHOOSE_LOC_TOP ${LANG_ENGLISH}  "Choose Install Location"
LangString INST_CHOOSE_LOC ${LANG_ENGLISH}  "Choose the folder in which to install ${PRODUCT_NAME_NO_VER} ${PRODUCT_VERSION}"
LangString INST_CHOOSE_LOC2 ${LANG_ENGLISH} "${PRODUCT_NAME_NO_VER} ${PRODUCT_VERSION} will be installed in the following folder.$\n\
To install in a different folder, click Browse and select another folder.$\n$\n Click Next to continue."
LangString INST_CHOOSE_LOC3 ${LANG_ENGLISH} "Destination Folder"
LangString INST_CHOOSE_LOC_BROWSE ${LANG_ENGLISH}   "Browse..."
LangString INST_CHOOSE_LOC_DEFAULT ${LANG_ENGLISH} "Default"
LangString INST_CHOOSE_LOC_LAST_USED ${LANG_ENGLISH} "Last used"
LangString INST_CHOOSE_LOC_CUSTOM ${LANG_ENGLISH} "Custom"
Function CustomDirectoryPage
    ${LogText} "+Function begin - CustomDirectoryPage"

    nsDialogs::Create 1018
    Pop $Dialog
    ${If} $Dialog == error
        ${LogText} "-Function end - CustomDirectoryPage (error)"
        Abort
    ${EndIf}
    ${LogText} "nsDialogs::Create success"

    GetDlgItem $0 $HWNDPARENT 1037
    ${NSD_SetText} $0 "$(INST_CHOOSE_LOC)"
    GetDlgItem $0 $HWNDPARENT 1038
    ${NSD_SetText} $0 "$(INST_CHOOSE_LOC)"

    ${NSD_CreateLabel} 0 0 100% 50u "$(INST_CHOOSE_LOC2)"
    Pop $0
    ${LogText} "Did CreateLabel"

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
    IntOp $PosX $SizeX + 20 # Take into account the x padding of 10, plus spacing of 15
    IntOp $Length ${DIALOG_X} - $PosX # [Total width] - [radio button width]
    IntOp $Length $Length - 10 # [Total width] - [radio button width] - [padding]
    # Add default option
    ${NSD_CreateRadioButton} 10 $PosY $SizeX 12u "$(INST_CHOOSE_LOC_DEFAULT)"
    Pop $RadioDefault
    ${NSD_CreateText} $PosX $PosY $Length 12u $DEFAULT_INSTDIR
    Pop $LabelDefault
    SendMessage $LabelDefault ${EM_SETREADONLY} 1 0
    ${LogText} "Did SendMessage"
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
    ${LogText} "Did Ons"

    # Install type message
    ${NSD_CreateLabel} 0 203 100% 12u ""
    Pop $UpgradeLabel
    Call CustomDirectoryPageSetUpgradeMessage

    Call CustomDirectoryPageShowWhichRadio

    Call UnexpandComponentsPage
    nsDialogs::Show
    ${LogText} "-Function end - CustomDirectoryPage"
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

LangString INST_CHOOSE_LOC4 ${LANG_ENGLISH} "Select the folder to install ${PRODUCT_NAME_NO_VER} ${PRODUCT_VERSION} in:"

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

LangString INST_LOC_OW ${LANG_ENGLISH}  "Warning: A different major version of MTA ($1) already exists at that path."
LangString INST_LOC_UPGRADE ${LANG_ENGLISH} "Installation type:  Upgrade"
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

Function IsWindowsClassicTheme
; Out <stack> = "1" - Is Windows Classic
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


;****************************************************************
;
; CustomNetMessagePage
;
; Show message to get user to unblock installer from firewall or similar
;
;****************************************************************
Var NetDialog
Var NetStatusLabel1
Var NetStatusLabel2
Var NetTryCount
Var NetDone
Var NetImage
Var NetImageHandle
Var NetMsgURL
Var NetPrevInfo
Var NetEnableNext
Var NetOtherSuccessCount
Var NetMirror
!define NEXT_BUTTON_ID 1
LangString NETTEST_TITLE1   ${LANG_ENGLISH} "Online update"
LangString NETTEST_TITLE2   ${LANG_ENGLISH} "Checking for update information"
LangString NETTEST_STATUS1  ${LANG_ENGLISH} "Checking for installer update information..."
LangString NETTEST_STATUS2  ${LANG_ENGLISH} "Please ensure your firewall is not blocking"

Function CustomNetMessagePage
    ${LogText} "+Function begin - CustomNetMessagePage"
    # Initial try with blank page
    Call TryToSendInfo
    ${If} $NetDone == 1
        # If it works, then skip this page
        ${LogText} "-Function end - CustomNetMessagePage (NetDone)"
        Return
    ${EndIf}

    # Setup page
    nsDialogs::Create 1018
    Pop $NetDialog
    ${If} $NetDialog == error
        ${LogText} "-Function end - CustomNetMessagePage (error)"
        Abort
    ${EndIf}

    GetDlgItem $0 $HWNDPARENT 1037
    ${NSD_SetText} $0 "$(NETTEST_TITLE1)"

    GetDlgItem $0 $HWNDPARENT 1038
    ${NSD_SetText} $0 "$(NETTEST_TITLE2)"

    ${NSD_CreateLabel} 0 20 100% 15 "$(NETTEST_STATUS1)"
    Pop $NetStatusLabel1
    ${NSD_AddStyle} $NetStatusLabel1 ${SS_CENTER}

    ${NSD_CreateLabel} 0 155 100% 15 "$(NETTEST_STATUS2)"
    Pop $NetStatusLabel2
    ${NSD_AddStyle} $NetStatusLabel2 ${SS_CENTER}

    ${NSD_CreateBitmap} 155 71 100% 100% ""
    Pop $NetImage
    ${NSD_SetImage} $NetImage $TEMP\image.bmp $NetImageHandle

    # Disable Next button maybe
    ${If} $NetEnableNext != 1
        GetDlgItem $0 $HWNDPARENT ${NEXT_BUTTON_ID}
        EnableWindow $0 0
    ${EndIf}

    Call UnexpandComponentsPage
    ${NSD_CreateTimer} NetFuncTimer 1000
    nsDialogs::Show
    ${NSD_FreeImage} $NetImageHandle
    ${LogText} "-Function end - CustomNetMessagePage"
FunctionEnd

Function CustomNetMessagePageLeave
    ${LogText} "+Function begin - CustomNetMessagePageLeave"
    ${NSD_KillTimer} NetFuncTimer
    Call TryToSendInfo
    Call UnexpandComponentsPage
    ${LogText} "-Function end - CustomNetMessagePageLeave"
FunctionEnd

Function NetFuncTimer
    ${LogText} "+Function begin - NetFuncTimer"
    ${NSD_KillTimer} NetFuncTimer
    IntOp $NetTryCount $NetTryCount + 1
    Call TryToSendInfo

    # Allow Next button after a number of tries
    ${If} $NetTryCount > 3
        StrCpy $NetEnableNext 1
    ${EndIf}

    ${If} $NetEnableNext == 1
        GetDlgItem $0 $HWNDPARENT ${NEXT_BUTTON_ID}
        EnableWindow $0 1
    ${EndIf}

    ${If} $NetDone == 1
        # If it works now, then proceed to the next page
        SendMessage $HWNDPARENT "0x408" "1" ""      # GotoNextPage
    ${Else}
        # Otherwise, try again in a second
        ${NSD_CreateTimer} NetFuncTimer 1000
    ${EndIf}
    ${LogText} "-Function end - NetFuncTimer"
FunctionEnd

;--------------------------
; Out $NetDone = result   (1 = success)
Function TryToSendInfo
    # Check if already done
    ${If} $NetDone == 1
        Return
    ${EndIf}

    ${LogText} "+Function begin - TryToSendInfo"

    # Do attempt
    Call NetComposeURL
    StrCpy $0 $NetMsgURL
    StrCpy $1 3000
    Call DoSendInfo

    # Set result
    ${If} $0 == 1
        StrCpy $NetDone 1
    ${Else}
        # Check if anything else is contactable
        StrCpy $0 "http://www.google.com/"
        StrCpy $1 1000
        Call DoSendInfo
        ${If} $0 == 1
            StrCpy $NetEnableNext 1
            IntOp $NetOtherSuccessCount $NetOtherSuccessCount + 1
            ${If} $NetOtherSuccessCount > 3
                StrCpy $NetDone 1
            ${EndIf}
        ${EndIf}
    ${EndIf}
    ${LogText} "-Function end - TryToSendInfo"
FunctionEnd

;--------------------------
; In $0 = URL
; In $1 = Timeout
; Out $0 = result   (1 = success)
Function DoSendInfo
    ${LogText} "+Function begin - DoSendInfo($0,$1)"
    NSISdl::download_quiet /TIMEOUT=$1 "$0" "$TEMP\prev_install"
    Pop $R0
    ${LogText} "NSISdl::download_quiet result:$R0"

    # Allow for server errors #1
    StrCpy $0 $R0 14
    ${If} $0 == "Server did not"
        StrCpy $R0 "success"
    ${EndIf}

    # Allow for server errors #2
    StrCpy $0 $R0 4
    ${If} $0 == "HTTP"
        StrCpy $R0 "success"
    ${EndIf}

    # Set result
    StrCpy $0 0
    ${If} $R0 == "success"
        StrCpy $0 1
    ${EndIf}
    ${LogText} "-Function end - DoSendInfo result:$0"
FunctionEnd

;--------------------------
; Out $NetMsgURL = URL
Function NetComposeURL
    ${If} $NetMsgURL == ""
        IfFileExists "$APPDATA\MTA San Andreas All" 0 skip
            StrCpy $NetPrevInfo "$NetPrevInfo&pp=1"
        skip:
        StrCpy $NetPrevInfo "$NetPrevInfo&ver=${0.0.0}"
    !ifndef LIGHTBUILD
        StrCpy $NetPrevInfo "$NetPrevInfo&n=1"
    !endif
    !ifdef REVISION
        StrCpy $NetPrevInfo "$NetPrevInfo&rev=${REVISION}"
    !endif
    ${EndIf}

    IntOp $NetMirror $NetMirror + 1
    IntOp $NetMirror $NetMirror % 2
    ${Switch} $NetMirror
        ${Case} 0
            StrCpy $NetMsgURL "https://updatesa.multitheftauto.com/sa/install/1/?x=0"
            ${Break}
        ${Default}
            StrCpy $NetMsgURL "https://updatesa.multitheftauto.com/sa/install/1/?x=0"
            ${Break}
    ${EndSwitch}
    StrCpy $NetMsgURL "$NetMsgURL$NetPrevInfo"
    StrCpy $NetMsgURL "$NetMsgURL&try=$NetTryCount"
    StrCpy $NetMsgURL "$NetMsgURL&other=$NetOtherSuccessCount"
    ${LogText} "NetComposeURL result:$NetMsgURL"
FunctionEnd

Function NoteMTAWasPresent
    StrCpy $NetPrevInfo "$NetPrevInfo&pm=1"
FunctionEnd

Function NoteGTAWasPresent
    StrCpy $NetPrevInfo "$NetPrevInfo&pg=1"
FunctionEnd

# Find valid Windows SID to use for permissions fixing
Function SetPermissionsGroup
    #   BU      = BUILTIN\Users
    #   S-1-2-0 = \LOCAL
    #   S-1-1-0 = \Everyone
    nsArray::SetList array "BU" "S-1-2-0" "S-1-1-0" /end
    ${ForEachIn} array $0 $1
        AccessControl::SidToName $1
        Pop $2  # Domain
        Pop $3  # Name
        StrLen $0 $2
        ${If} $0 < 20   # HACK: Error message is longer than this
            StrCpy $PermissionsGroup "$1"
            ${LogText} "SetPermissionsGroup using '$PermissionsGroup'"
            Return
        ${EndIf}
        ${LogText} "AccessControl::SidToName failed with '$1': '$2' '$3'"
    ${Next}
    ; Default to \LOCAL
    StrCpy $PermissionsGroup "S-1-2-0"
    ${LogText} "SetPermissionsGroup using '$PermissionsGroup'"
FunctionEnd

Function MTACreateShortсut
	# path to a lnk.
	Pop $2
	# path to a target exe.
	Pop $1
	# path to an icon file.
	Pop $0
	# Link
	# Target exe
	# Command line parameters
	# Icon file
	# Icon index number
	# Start options: SW_SHOWNORMAL, SW_SHOWMAXIMIZED, SW_SHOWMINIMIZED
	# Keyboard shortcut
	# Description
	CreateShortCut $2 \
		$1 \
		"" \
		$0 \
		0 \
		SW_SHOWNORMAL \
		"" \
		"Play Multi Theft Auto: San Andreas"
	${If} ${Errors}
		${LogText} "Error creating shortcut for EXE $2"
	${EndIf}
FunctionEnd

Function MTAUpdateShortсutTarget
	# Full path to a shortcut
	Pop $0
	# Full path to a target exe
	Pop $1
	ShellLink::GetShortCutTarget $0
	# Get returned value of given shortcut target
	Pop $2
	${If} $2 == $1
		Abort
	${EndIf}
	${LogText} "Changing shortcut target $0 to Exe $1"
	ShellLink::SetShortCutTarget $0 $1
	Pop $0
	${LogText} "Changing shortcut target error: $0"
FunctionEnd

Function MTAInitFileNamesAndPaths
	# Shortcuts names
	StrCpy $ClientShortcutName "MTA San Andreas"
	StrCpy $ServerShortcutName "MTA Server"
	StrCpy $UninstallShortcutName "Uninstall MTA San Andreas"
	# Shostcuts paths
	StrCpy $StartMenuMTAFolderPath "$SMPROGRAMS\MTA San Andreas ${0.0}"
	StrCpy $StartMenuClientShortcutPath "$StartMenuMTAFolderPath\$ClientShortcutName.lnk"
	StrCpy $StartMenuServerShortcutPath "$StartMenuMTAFolderPath\$ServerShortcutName.lnk"
	StrCpy $StartMenuUninstallShortcutPath "$StartMenuMTAFolderPath\$UninstallShortcutName.lnk"
	# Shortcut names for desktop and start menu are different and can't be safely unified.
	# Obvious fix is to roll 1 update where all shortcuts will be deleted and replaced with a unified names.
	StrCpy $DesktopClientShortcutPath "$DESKTOP\$ClientShortcutName ${0.0}.lnk"
	# Exe names
	StrCpy $ClientExeName "Multi Theft Auto.exe"
	StrCpy $ServerExeName "MTA Server.exe"
	StrCpy $UninstallExeName "Uninstall.exe"
	# Exe paths
	StrCpy $ClientExePath "$INSTDIR\$ClientExeName"
	StrCpy $ServerExePath "$INSTDIR\server\$ServerExeName"
	StrCpy $UninstallExePath "$INSTDIR\$UninstallExeName"
FunctionEnd
