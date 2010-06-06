!include nsDialogs.nsh
!include LogicLib.nsh
!include Sections.nsh
!include UAC.nsh
!include GameExplorer.nsh

XPStyle on
RequestExecutionLevel user
SetCompressor /SOLID lzma

!define TEMP1 $R0
!define TEMP2 $R1
!define TEMP3 $R2

Var GTA_DIR
Var Install_Dir
Var CreateSMShortcuts
Var CreateDesktopIcon
Var RedistInstalled

!define GUID "{DC86048C-6401-4356-8533-06EC8CC02AF3}"

; ###########################################################################################################
!ifndef FILES_ROOT
	!define LIGHTBUILD    ; enable LIGHTBUILD for nightly
	!define FILES_ROOT "C:\Build\output"
	!define SERVER_FILES_ROOT "C:\Build\mta10_server\output"
	!define FILES_MODULE_SDK "C:\Build\Shared\publicsdk"
	!define CLIENT_SETUP
	!define INCLUDE_SERVER
	!define INSTALL_OUTPUT "mtasa-1.0.exe"
!endif
!ifndef LIGHTBUILD
	!define INCLUDE_DEVELOPMENT
	!define INCLUDE_EDITOR
!endif
!ifndef PRODUCT_VERSION
	!define PRODUCT_VERSION "v1.0"
!endif
!define EXPAND_DIALOG_X 134
!define EXPAND_DIALOG_Y 60
; ###########################################################################################################

;ReserveFile "${NSISDIR}\Plugins\InstallOptions.dll"


!ifdef CLIENT_SETUP
	!define PRODUCT_NAME "MTA:SA"
!else
	!define PRODUCT_NAME "MTA:SA Server"
!endif

!define PRODUCT_PUBLISHER "Multi Theft Auto"
!define PRODUCT_WEB_SITE "http://www.multitheftauto.com"
!ifdef CLIENT_SETUP
	!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Multi Theft Auto.exe"
	!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!else
	!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\MTA Server.exe"
	!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME} Server"
!endif
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

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

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "LicenseShowProc"
!insertmacro MUI_PAGE_LICENSE					"eula.txt"

; Components page
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "ComponetsShowProc"
!insertmacro MUI_PAGE_COMPONENTS

; Game directory page
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "DirectoryShowProc"
!define MUI_DIRECTORYPAGE_VARIABLE				$INSTDIR
!insertmacro MUI_PAGE_DIRECTORY

!ifdef CLIENT_SETUP
	!define MUI_PAGE_CUSTOMFUNCTION_PRE				SkipDirectoryPage
	!define MUI_PAGE_HEADER_TEXT					"Grand Theft Auto: San Andreas location"
	!define MUI_PAGE_HEADER_SUBTEXT					""
	!define MUI_DIRECTORYPAGE_TEXT_DESTINATION		"Grand Theft Auto: San Andreas folder"
	!define MUI_DIRECTORYPAGE_TEXT_TOP				"Please select your Grand Theft Auto: San Andreas folder.$\n$\nYou MUST have Grand Theft Auto: San Andreas 1.0 installed to use MTA:SA, it does not support any other versions.$\n$\nClick Install to begin installing."
	!define MUI_DIRECTORYPAGE_VARIABLE				$GTA_DIR
	!insertmacro MUI_PAGE_DIRECTORY
!endif

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
; Launch from installer with user privileges
!define MUI_FINISHPAGE_RUN						""
!define MUI_FINISHPAGE_RUN_FUNCTION				"LaunchLink"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE							"English"
LangString DESC_Section1 ${LANG_ENGLISH}			"The core components required to run Multi Theft Auto."
LangString DESC_Section2 ${LANG_ENGLISH}			"The MTA:SA modification, allowing you to play online."
LangString DESC_Section3 ${LANG_ENGLISH}			"The Multi Theft Auto:Editor for MTA:SA, allowing you to create and edit maps."
LangString DESC_SectionGroupMods ${LANG_ENGLISH}	"Modifications for Multi Theft Auto. Without at least one of these, you cannot play Multi Theft Auto."
LangString DESC_SectionGroupServer ${LANG_ENGLISH}  "The Multi Theft Auto Server. This allows you to host games from your computer. This requires a fast internet connection."
LangString DESC_Section4 ${LANG_ENGLISH}			"The Multi Theft Auto server. This is a required component."
LangString DESC_Section5 ${LANG_ENGLISH}			"The MTA:SA modification for the server."
LangString DESC_Section6 ${LANG_ENGLISH}			"This is a set of required resources for your server."
LangString DESC_Section7 ${LANG_ENGLISH}			"This is an optional set of gamemodes and maps for your server."
LangString DESC_Section8 ${LANG_ENGLISH}			"The MTA:SA 1.0 Map Editor.  This can be used to create your very own maps for use in gamemodes for MTA."
LangString DESC_Section9 ${LANG_ENGLISH}			"This is the SDK for creating binary modules for the MTA server. Only install if you have a good understanding of C++!"
LangString DESC_Section10 ${LANG_ENGLISH}			"Create a Start Menu group for installed applications"
LangString DESC_Section11 ${LANG_ENGLISH}			"Create a Desktop Shortcut for the MTA:SA Client."
LangString DESC_Blank ${LANG_ENGLISH}			""
LangString DESC_SectionGroupDev ${LANG_ENGLISH}		"Development code and tools that aid in the creation of mods for Multi Theft Auto"
LangString DESC_SectionGroupClient ${LANG_ENGLISH}  "The client is the program you run to play on a Multi Theft Auto server"

Var Dialog
Var Label
Var CreateAccountRadio
Var LoginAccountRadio
Var NoAccountRadio
Var StoredUsername

Function nsDialogsMtaBetaWelcomePage
	!insertmacro MUI_HEADER_TEXT "MTA account" "About Multi Theft Auto accounts"
	nsDialogs::Create /NOUNLOAD 1018
	Pop $Dialog

	${If} $Dialog == error
		Abort
	${EndIf}
	
	${NSD_CreateLabel} 0 0 100% 20u "Multi Theft Auto: San Andreas has a central login system that provides a unique username for each user."
	Pop $Label

	${NSD_CreateLabel} 0 20u 100% 20u "Although the system is entirely optional, some servers may prevent you joining without an account and some features may be unavailable to you."
	Pop $Label
	
	${NSD_CreateLabel} 0 40u 100% 20u "Creating an account takes a few seconds."
	Pop $Label

	${NSD_CreateRadioButton} 0 60u 100% 13u "I want to create a new account"
	Pop $CreateAccountRadio
	SendMessage $CreateAccountRadio ${BM_SETCHECK} 1 0 ; check the first option
	
	ReadRegStr $StoredUsername HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Username"
	StrLen $0 $StoredUsername
	IntCmp $0 0 noAccountDataStored noAccountDataStored existingAccountStored
	noAccountDataStored:
	${NSD_CreateRadioButton} 0 73u 100% 13u "I've already got an account"
	Pop $LoginAccountRadio
	goto next
	existingAccountStored:
	${NSD_CreateRadioButton} 0 73u 100% 13u "I want to use my current account '$StoredUsername'"
	Pop $LoginAccountRadio
	SendMessage $CreateAccountRadio ${BM_SETCHECK} 0 0 ; check the first option
	SendMessage $LoginAccountRadio ${BM_SETCHECK} 1 0 ; check the first option
	next:
	${NSD_CreateRadioButton} 0 86u 100% 13u "I don't want to create an account"
	Pop $NoAccountRadio
	
	;EnableWindow $NoAccountRadio 0


	nsDialogs::Show
FunctionEnd

Var ShouldCreateAccount
Var ShouldLogin
Var SkipLoginPage

Function nsDialogsMtaBetaWelcomePageLeave
	${NSD_GetState} $CreateAccountRadio $ShouldCreateAccount
	${NSD_GetState} $LoginAccountRadio $ShouldLogin
	IntCmp $ShouldCreateAccount 1 createAccount skip1 skip1
	createAccount:
	  
	MessageBox MB_ICONINFORMATION|MB_OK "A web page is being opened in your web browser. Please use it to register for an account then return to the installer to enter your login details."
	ExecShell "open" "http://community.mtasa.com/test/index.php?p=register"
	
	skip1:
	${NSD_GetState} $NoAccountRadio $SkipLoginPage
	IntCmp $SkipLoginPage 1 noAccount skip skip
	noAccount:
	MessageBox MB_ICONINFORMATION|MB_OK "If you want to create an account in the future, just reinstall MTA."
	skip:
FunctionEnd

Var UsernameTxt
Var Password1Txt
Var Password2Txt
Var EmailTxt

Function nsDialogsCreateAccountPage
	IntCmp $ShouldCreateAccount 1 createAccount
	Abort ; skip this page if create account wasn't chosen
	createAccount:
	
	!insertmacro MUI_HEADER_TEXT "MTA account" "Please fill in all the following fields to create a new account"
	
	nsDialogs::Create /NOUNLOAD 1018
	Pop $Dialog

	${If} $Dialog == error
		Abort
	${EndIf}

	${NSD_CreateLabel} 0 2u 70u 13u "Username:"
	Pop $Label
	
	${NSD_CreateText} 70u 0 60% 13u ""
	Pop $UsernameTxt
	
	${NSD_CreateLabel} 0 20u 70u 13u "Password:"
	Pop $Label

	${NSD_CreatePassword} 70u 18u 60% 13u ""
	Pop $Password1Txt

	${NSD_CreateLabel} 0 38u 70u 13u "Confirm Password:"
	Pop $Label

	${NSD_CreatePassword} 70u 36u 60% 13u ""
	Pop $Password2Txt
	
	${NSD_CreateLabel} 0 56u 70u 13u "Email:"
	Pop $Label

	${NSD_CreateText} 70u 54u 60% 13u ""
	Pop $EmailTxt
	
	nsDialogs::Show
	
FunctionEnd

Var Username
Var Password1
Var Password2
Var Email

; called when the create account next button is clicked
Function nsDialogsCreateAccountPageLeave
	 ${NSD_GetText} $UsernameTxt $Username
	 ${NSD_GetText} $Password1Txt $Password1
	 ${NSD_GetText} $Password2Txt $Password2
	 ${NSD_GetText} $EmailTxt $Email

	 StrCmpS $Password1 $Password2 cont badPassword
	 badPassword:
	 MessageBox MB_ICONINFORMATION|MB_OK "The passwords entered do not match. Please try again."
	 Abort
	 cont:
FunctionEnd

Var PasswordTxt

Var StoredSerial

Function nsDialogsLoginAccountPage

	IntCmp $SkipLoginPage 1 skipLogin
	
	IntCmp $ShouldCreateAccount 1 loginAccount
	
	ReadRegStr $StoredUsername HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Username"
	ReadRegStr $StoredSerial HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Serial"

	StrLen $0 $StoredUsername
	IntCmp $0 0 loginAccount
	;Ver::CS $StoredUsername $StoredSerial
	Pop $0
	
	StrCmp $0 "t" skipLogin cont
	cont:
	MessageBox MB_ICONINFORMATION|MB_OK "Your stored account is no longer valid. Please login again."
	goto loginAccount

	skipLogin:
	Abort ; skip this page if create account wasn't chosen
	loginAccount:

	!insertmacro MUI_HEADER_TEXT "MTA account" "Please enter your account details to login"

	nsDialogs::Create /NOUNLOAD 1018
	Pop $Dialog

	${If} $Dialog == error
		Abort
	${EndIf}

	${NSD_CreateLabel} 0 2u 70u 13u "Username:"
	Pop $Label

	${NSD_CreateText} 70u 0 60% 13u $StoredUsername
	Pop $UsernameTxt

	${NSD_CreateLabel} 0 20u 70u 13u "Password:"
	Pop $Label

	${NSD_CreatePassword} 70u 18u 60% 13u ""
	Pop $PasswordTxt
	
	${NSD_SetFocus} $UsernameTxt
	
	nsDialogs::Show
FunctionEnd

Var Password
Var Serial
Var SerialLength
Function nsDialogsLoginAccountPageLeave
	 ${NSD_GetText} $UsernameTxt $Username
	 ${NSD_GetText} $PasswordTxt $Password
	 
	 ;Ver::Verify $Username $Password
	 Pop $Serial
	 StrLen $SerialLength $Serial
	 IntCmp $SerialLength 19 is19 not19 not19
	 not19:
		   IntCmp $SerialLength 0 endP2
		   MessageBox MB_ICONINFORMATION|MB_OK $Serial
		   endP2:
				 Abort
	 is19:
	 WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Username" $Username
	 WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Serial" $Serial
FunctionEnd


Function SerialPage
	;Display the InstallOptions dialog

	Push ${TEMP1}
	Push ${TEMP2}
	Push ${TEMP3}
	ReadRegStr ${TEMP1} HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Username"
	WriteINIStr  "$PLUGINSDIR\serialdialog.ini" "Field 1" "State" ${TEMP1}

	ReadRegStr ${TEMP3} HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Serial"

	;Ver::CS ${TEMP1} ${TEMP3}
	Pop $0

	StrCmp $0 "t" skipLogin

	!insertmacro MUI_HEADER_TEXT "MTA beta account login" "Please enter your MTA beta account login details"
	!insertmacro MUI_INSTALLOPTIONS_DISPLAY "serialdialog.ini"

skipLogin:
	Pop ${TEMP3}
	Pop ${TEMP2}
	Pop ${TEMP1}
 
FunctionEnd

Function ValidateSerial
	ReadINIStr ${TEMP1} "$PLUGINSDIR\serialdialog.ini" "Field 1" "State"
	ReadINIStr ${TEMP2} "$PLUGINSDIR\serialdialog.ini" "Field 2" "State"
	;ReadINIStr ${TEMP3} "$PLUGINSDIR\serialdialog.ini" "Field 7" "State"

	;Ver::Verify ${TEMP1} ${TEMP2}
	Pop $0
	StrLen $1 $0
	IntCmp $1 19 is19 not19 not19
	not19:
		IntCmp $1 0 endP2
		MessageBox MB_ICONINFORMATION|MB_OK $0
		endP2:
		Abort
	is19:
	WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Username" ${TEMP1}
	WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Serial" $0
FunctionEnd


Function LaunchLink
	!ifdef CLIENT_SETUP
		SetOutPath "$INSTDIR"
		!insertmacro UAC_AsUser_ExecShell "open" "Multi Theft Auto.exe" "" "" ""
	!else
		SetOutPath "$INSTDIR\Server"
		!insertmacro UAC_AsUser_ExecShell "open" "MTA Server.exe" "" "" ""
	!endif
FunctionEnd

Function MTACenterLogin
	;  SetOutPath "$INSTDIR"
	;  ExecWait "$INSTDIR\MTA Center Login.exe"
FunctionEnd

Var ClientSectionSelectedState
/*
Function .onSelChange
	 Push $R0
	 Push $R1
	 SectionGetFlags ${SECGCLIENT} $R0
	 IntOp $R0 $R0 & ${SF_SELECTED}
	 MessageBox MB_ICONINFORMATION|MB_OK $R0
	 MessageBox MB_ICONINFORMATION|MB_OK $ClientSectionSelectedState
	 StrCmp $R0 $ClientSectionSelectedState client_is_selected client_is_not_selected
	 client_is_selected:
						SectionGetFlags ${SEC01} $R0
						IntOp $R0 $R0 | ${SF_SELECTED}
						SectionSetFlags ${SEC01} $R0
						goto endit
	 client_is_not_selected:
						SectionGetFlags ${SEC01} $R0
						IntOp $R0 $R0 ^ ${SF_SELECTED}
						SectionSetFlags ${SEC01} $R0
	endit:
	Pop $R1
	Pop $R0
FunctionEnd
*/

Function .OnInstFailed
	;UAC::Unload ;Must call unload!
FunctionEnd

Function .onInit
	Call DoRightsElevation

	; Check if we must install the Microsoft Visual Studio 2008 SP1 redistributable
	ClearErrors
	ReadRegDWORD $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{9A25302D-30C0-39D9-BD6F-21E6EC160475}" "Version"
	IfErrors 0 DontInstallVC9Redist
	StrCpy $RedistInstalled "0"
	Goto PostVC90Check
DontInstallVC9Redist:
	StrCpy $RedistInstalled "1"
PostVC90Check:
	
	ReadRegStr $Install_Dir HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Last Install Location" ; start of fix for #3743
	${If} $Install_Dir == '' 
		strcpy $INSTDIR "$PROGRAMFILES\MTA San Andreas"
	${Else} 
		strcpy $INSTDIR $Install_Dir
	${EndIf} ; end of fix for #3743

	!ifdef CLIENT_SETUP
		ReadRegStr $2 HKCU "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path"
		StrCmp $2 "" trynext cont
		trynext:
		ReadRegStr $2 HKLM "SOFTWARE\Rockstar Games\GTA San Andreas\Installation" "ExePath"
		cont:
		StrCmp $2 "" trynext2 cont2
		trynext2:
		ReadRegStr $2 HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path"
		cont2:
		StrCmp $2 "" trynext3 cont3
		trynext3:
		ReadRegStr $2 HKCU "Software\Valve\Steam\Apps\12120" "Installed"
		StrCpy $2 $2
		StrCmp $2 "1" usesteam cont3
		usesteam:
		ReadRegStr $2 HKCU "Software\Valve\Steam" "SteamPath"
		StrCpy $2 "$2\steamapps\common\grand theft auto san andreas"
		cont3:
		!insertmacro ReplaceSubStr $2 "gta_sa.exe" ""
		!insertmacro ReplaceSubStr $2 "/" "\"
		strcpy $3 '"'
		!insertmacro ReplaceSubStr $MODIFIED_STR $3 ""
		strcpy $GTA_DIR $MODIFIED_STR
	!endif

	InitPluginsDir
	File /oname=$PLUGINSDIR\serialdialog.ini "serialdialog.ini"
FunctionEnd

Function .onInstSuccess
	!ifdef CLIENT_SETUP
		WriteRegStr HKCU "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path" $GTA_DIR
		WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Last Install Location" $INSTDIR

		; Add the protocol handler
		WriteRegStr HKCR "mtasa" "" "URL:MTA San Andreas Protocol"
		WriteRegStr HKCR "mtasa" "URL Protocol" ""
		WriteRegStr HKCR "mtasa\DefaultIcon" "" "$INSTDIR\Multi Theft Auto.exe"
		WriteRegStr HKCR "mtasa\shell\open\command" "" '"$INSTDIR\Multi Theft Auto.exe"%1'
	!endif
	
	; Start menu items
	${If} $CreateSMShortcuts == 1
		CreateDirectory "$SMPROGRAMS\MTA San Andreas"

		!ifdef CLIENT_SETUP
			IfFileExists "$INSTDIR\Multi Theft Auto.exe" 0 skip1
			SetOutPath "$INSTDIR"
			Delete "$SMPROGRAMS\\MTA San Andreas\Play MTA San Andreas.lnk"
			CreateShortCut "$SMPROGRAMS\\MTA San Andreas\MTA San Andreas.lnk" "$INSTDIR\Multi Theft Auto.exe" \
				"" "$INSTDIR\Multi Theft Auto.exe" 0 SW_SHOWNORMAL \
				"" "Play Multi Theft Auto: San Andreas"
			skip1:
		!endif
		
		IfFileExists "$INSTDIR\Server\MTA Server.exe" 0 skip2
		SetOutPath "$INSTDIR\Server"
		CreateShortCut "$SMPROGRAMS\\MTA San Andreas\MTA Server.lnk" "$INSTDIR\Server\MTA Server.exe" \
			"" "$INSTDIR\Server\MTA Server.exe" 2 SW_SHOWNORMAL \
			"" "Run the Multi Theft Auto: San Andreas Server"
		skip2:
		
		!ifdef CLIENT_SETUP
			IfFileExists "$INSTDIR\Uninstall.exe" 0 skip3
			SetOutPath "$INSTDIR"
			CreateShortCut "$SMPROGRAMS\\MTA San Andreas\Uninstall MTA San Andreas.lnk" "$INSTDIR\Uninstall.exe" \
				"" "$INSTDIR\Uninstall.exe" 0 SW_SHOWNORMAL \
				"" "Uninstall Multi Theft Auto: San Andreas"
		!else
			IfFileExists "$INSTDIR\server\Uninstall.exe" 0 skip3
			SetOutPath "$INSTDIR"
			CreateShortCut "$SMPROGRAMS\\MTA San Andreas\Uninstall MTA San Andreas Server.lnk" "$INSTDIR\server\Uninstall.exe" \
				"" "$INSTDIR\Server\Uninstall.exe" 0 SW_SHOWNORMAL \
				"" "Uninstall Multi Theft Auto: San Andreas"
		!endif
		skip3:
	${EndIf}
    
    ${If} $CreateDesktopIcon == 1
		!ifdef CLIENT_SETUP
			IfFileExists "$INSTDIR\Multi Theft Auto.exe" 0 skip4
			SetOutPath "$INSTDIR"
			Delete "$DESKTOP\Play MTA San Andreas.lnk"
			CreateShortCut "$DESKTOP\MTA San Andreas.lnk" "$INSTDIR\Multi Theft Auto.exe" \
				"" "$INSTDIR\Multi Theft Auto.exe" 0 SW_SHOWNORMAL \
				"" "Play Multi Theft Auto: San Andreas"
			skip4:
		!endif
	${EndIf}

	;UAC::Unload ;Must call unload!
FunctionEnd


!ifdef CLIENT_SETUP
	!ifdef INCLUDE_SERVER
		InstType "Client and Server"
	!else
		InstType "Client only"
	!endif
!endif
!ifdef INCLUDE_SERVER
	InstType "Server only"
!endif

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
!ifdef CLIENT_SETUP
	OutFile "${INSTALL_OUTPUT}"
!else
	OutFile "MTA Server Setup.exe"
!endif

;InstallDir "$PROGRAMfiles San Andreas"
InstallDirRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Last Install Location"
ShowInstDetails show
ShowUnInstDetails show

!ifdef CLIENT_SETUP
	SectionGroup /e "Game client" SECGCLIENT
		Section "Core components" SEC01
			SectionIn 1 RO ; section is required
			
			StrCmp "$RedistInstalled" "1" DontInstallRedist
			Call InstallVC90Redistributable
			StrCmp "$RedistInstalled" "1" DontInstallRedist
			Abort
DontInstallRedist:

			WriteRegStr HKCU "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path" $GTA_DIR
			WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "Last Install Location" $INSTDIR

			SetOutPath "$INSTDIR\MTA"
			SetOverwrite on

			# Make the directory "$INSTDIR" read write accessible by all users
			DetailPrint "Updating permissions. This could take a few minutes..."
			AccessControl::GrantOnFile "$INSTDIR" "(BU)" "FullAccess"

			File "${FILES_ROOT}\MTA San Andreas\mta\cgui.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\core.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\xmll.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\game_sa.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\multiplayer_sa.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\netc.dll"
			File "${FILES_ROOT}\MTA San Andreas\mta\libcurl.dll"

			!ifndef LIGHTBUILD
				File "${FILES_ROOT}\MTA San Andreas\mta\d3dx9_42.dll"
				File "${FILES_ROOT}\MTA San Andreas\mta\ikpMP3.dll"
				File "${FILES_ROOT}\MTA San Andreas\mta\irrKlang.dll"
				File "${FILES_ROOT}\MTA San Andreas\mta\chatboxpresets.xml"

				SetOutPath "$INSTDIR\MTA\cgui"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\CGUI.is.xml"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\CGUI.lnf.xml"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\CGUI.png"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\CGUI.xml"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\Falagard.xsd"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\Font.xsd"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\GUIScheme.xsd"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\Imageset.xsd"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sagothic.ttf"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\saheader.ttf"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sans.dat"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sans.tga"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sans.ttf"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\sabankgothic.ttf"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\pricedown.ttf"

				SetOutPath "$INSTDIR\MTA\cgui\images"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\*.png"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\*.jpg"

				SetOutPath "$INSTDIR\MTA\cgui\images\radarset"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\radarset\*.png"

				SetOutPath "$INSTDIR\MTA\cgui\images\transferset"
				File "${FILES_ROOT}\MTA San Andreas\mta\cgui\images\transferset\*.png"

			!endif

			SetOutPath "$INSTDIR"
			File "${FILES_ROOT}\MTA San Andreas\*" ; NOT RECURSIVE
			
			${GameExplorer_UpdateGame} ${GUID}
			${If} ${Errors}
				${GameExplorer_AddGame} all "$INSTDIR\Multi Theft Auto.exe" "$INSTDIR" "$INSTDIR\Multi Theft Auto.exe" ${GUID}
				CreateDirectory $APPDATA\Microsoft\Windows\GameExplorer\${GUID}\SupportTasks\0
				CreateShortcut "$APPDATA\Microsoft\Windows\GameExplorer\$0\SupportTasks\0\Client Manual.lnk" \ "http://wiki.multitheftauto.com/wiki/Client_Manual"
			${EndIf}
		SectionEnd

		Section "Game module" SEC02
			SectionIn 1 RO
			SetOutPath "$INSTDIR\mods\deathmatch"
			File "${FILES_ROOT}\MTA San Andreas\mods\deathmatch\Client.dll"
			File "${SERVER_FILES_ROOT}\mods\deathmatch\lua5.1.dll"
			SetOutPath "$INSTDIR\mods\deathmatch\resources"
		SectionEnd
	SectionGroupEnd
!endif

!ifdef INCLUDE_SERVER
	!ifdef CLIENT_SETUP
		SectionGroup /e "Dedicated server" SECGSERVER
	!endif
	Section "Core components" SEC04
	SectionIn 1 2 RO ; section is required
	
	StrCmp "$RedistInstalled" "1" DontInstallRedist
	Call InstallVC90Redistributable
	StrCmp "$RedistInstalled" "1" DontInstallRedist
	Abort
DontInstallRedist:

		SetOutPath "$INSTDIR\server"
		SetOverwrite on
		File "${SERVER_FILES_ROOT}\core.dll"
		File "${SERVER_FILES_ROOT}\xmll.dll"
		File "${SERVER_FILES_ROOT}\MTA Server.exe"
		File "${SERVER_FILES_ROOT}\net.dll"
		File "${SERVER_FILES_ROOT}\libcurl.dll"
	SectionEnd

	Section "Game module" SEC05
		SectionIn 1 2 RO ; section is required
		SetOutPath "$INSTDIR\server\mods\deathmatch"
		
		SetOverwrite on
		File "${SERVER_FILES_ROOT}\mods\deathmatch\deathmatch.dll"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\lua5.1.dll"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\pcre3.dll"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\pthreadVC2.dll"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\sqlite3.dll"
		
		;Only overwrite the following files if previous versions were bugged and explicitly need replacing
		!insertmacro FileIfMD5 "${SERVER_FILES_ROOT}\mods\deathmatch\editor_acl.xml" "711185d8f4ebb355542053ce408b82b3"

		SetOverwrite off
		File "${SERVER_FILES_ROOT}\mods\deathmatch\acl.xml"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\editor_acl.xml"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\banlist.xml"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\mtaserver.conf"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\local.conf"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\editor.conf"
		File "${SERVER_FILES_ROOT}\mods\deathmatch\vehiclecolors.conf"

		!ifndef LIGHTBUILD
			!ifdef CLIENT_SETUP
				File "${SERVER_FILES_ROOT}\mods\deathmatch\local.conf"
			!endif
			
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
			SetOutPath "$INSTDIR\server\mods\deathmatch\resource-cache"
			SetOutPath "$INSTDIR\server\mods\deathmatch\logs"
		!endif
	SectionEnd

	!ifndef LIGHTBUILD
		Section "Core resources" SEC06
		SectionIn 1 2 RO ; section is required
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
			SetOverwrite ifnewer
			File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\required\*.zip"
		SectionEnd
	!endif

	!ifndef LIGHTBUILD
		SectionGroup "Optional Resources" SEC07
			Section "AMX Emulation package"
		SectionIn 1 2
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
			SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\amx"
		SectionEnd
			Section "Assault Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\assault.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\as-area51.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\as-cliff.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\as-dam.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\as-docks.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\as-heist.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\as-sharks.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\as-ship.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\as-supermarket.zip"
			SectionEnd
			Section "Briefcase Race Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\briefcaserace.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\briefcase.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\easytext.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\br-sf.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\br-sf-autoteams.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\br-sf-teams.zip"
			SectionEnd
			Section "Classic Deathmatch Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\cdm.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\cdm-ls.zip"
			SectionEnd
			Section "Capture the Flag Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctf.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctf-bombsite.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctf-canals.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctf-csitaly.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctf-goldcove.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctf-hideout_z.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctf-hydrastrike.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctf-sewer.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctf-tbd.zip"
			SectionEnd
			Section "Capture the Vehicle Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctv.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctv-ls.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\ctv-smalltownrumble.zip"
			SectionEnd
			Section "Deathmatch Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\deathmatch.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\tdm.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\dm-canals.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\dm-arena1.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\dm-smallville.zip"
			SectionEnd
			Section "Element browser Utility"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\elementbrowser.zip"
			SectionEnd
			Section "Fallout Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\fallout.zip"
			SectionEnd
			Section "Hay Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\hay.zip"
			SectionEnd
			Section "Race Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-10laphotring.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-3lapdirtring.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-5lap8track.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-airportdogfight.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-airportspeedway.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-apacheassault.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-awalkinthepark.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-badlandsblastaround.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-bandito.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-bayareacircuit.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-bloodring.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-blowthedam.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-boatingblastaround.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-bobcatblastaround.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-break.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-chiliadclimb.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-chrmleasy.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-chrmlhard.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-degenerationofx.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-desertdogfight.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-destructionderby.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-dockbikes.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-docksideblastaround.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-drift.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-driftcity.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-dunebuggy.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-errand.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-farewellmylove.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-farm2city.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-findthecock.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-freeroam.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-highnoon.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-homeinthehills.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-hotroute.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-hydrarace.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-island.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-justanotherbikerace.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-longwayround.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-lsairport.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-lstrenches.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-lvsprint.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-midairmayhem.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-miniputt.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-monsterblastaround.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-mx_sky.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-offroadblastaround.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-predatorzone.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-quarryrun.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-rcairwar.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-rcwarz.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-runway69.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-rustlerrampage.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-sandking.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-santosdrive.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-seadragon.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-seahunter.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-searustler.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-sewers.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-sfbynight.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-smugglersrun.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-sparrowstorm.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-specialdelivery.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-speedforweed.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-squalorace.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-stunt.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-suburbanspeedway.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-superhydrarace.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-technicalitch.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-thepanopticon.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-vinewoodblastaround.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race-wuzimu.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\mapratings.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race_nos.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race_racewar.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race_toptimes.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race_traffic_sensor.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\race_voicepack.zip"
			SectionEnd
			Section "Stealth Gamemode"
			SectionIn 1 2
				SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
				SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\stealth.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-carrier.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-chinatown.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-church.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-coookiepirates.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-dra-park.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-factory.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-junkyard.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-ritzy.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-sewers.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-shopping.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-terminal.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\sth-thebunker.zip"
			SectionEnd
			Section "Team Deathmatch Arena Gamemode"
		SectionIn 1 2
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
			SetOverwrite ifnewer
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\tdma.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\tdma-damwars.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\tdma-gridlock.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\tdma-medieval.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\tdma-nomansland.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\tdma-tbd.zip"
				File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\tdma-vinewood.zip"
		SectionEnd
		SectionGroupEnd
	!endif

	!ifdef INCLUDE_EDITOR
		Section "Editor" SEC08
			SectionIn 1 2
			SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
			SetOverwrite ifnewer
			File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\editor\*.zip"
		SectionEnd
	!endif

	!ifdef CLIENT_SETUP
		SectionGroupEnd
	!endif
!endif

!ifdef INCLUDE_DEVELOPMENT
	SectionGroup /e "Development" SECGDEV
		Section /o "Module SDK" SEC09
			SetOutPath "$INSTDIR\development\module SDK"
			SetOverwrite ifnewer
			File /r "${FILES_MODULE_SDK}\"
		SectionEnd
	SectionGroupEnd
!endif

Section "Start menu group" SEC10
	SectionIn 1 2
	StrCpy $CreateSMShortcuts 1
SectionEnd

Section "Desktop icon" SEC11
	SectionIn 1 2
	StrCpy $CreateDesktopIcon 1
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC01} $(DESC_Section1)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC02} $(DESC_Section2)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC03} $(DESC_Section3)
	!insertmacro MUI_DESCRIPTION_TEXT ${SECGMODS} $(DESC_SectionGroupMods)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC04} $(DESC_Section4)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC05} $(DESC_Section5)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC06} $(DESC_Section6)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC07} $(DESC_Section7)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC08} $(DESC_Section8)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC09} $(DESC_Section9)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC10} $(DESC_Section10)
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC11} $(DESC_Section11)
	!insertmacro MUI_DESCRIPTION_TEXT ${SECBLANK} $(DESC_Blank)
	!insertmacro MUI_DESCRIPTION_TEXT ${SECGSERVER} $(DESC_SectionGroupServer)
	!insertmacro MUI_DESCRIPTION_TEXT ${SECGDEV} $(DESC_SectionGroupDev)
	!insertmacro MUI_DESCRIPTION_TEXT ${SECGCLIENT} $(DESC_SectionGroupClient)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Section -Post
	!ifdef CLIENT_SETUP
		WriteUninstaller "$INSTDIR\Uninstall.exe"
		WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Multi Theft Auto.exe"
	!else
		WriteUninstaller "$INSTDIR\server\Uninstall.exe"
		WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Server\MTA Server.exe"
	!endif

	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
	!ifdef CLIENT_SETUP
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Multi Theft Auto.exe"
	!else
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Server\Uninstall.exe"
	!endif
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onUninstSuccess
	HideWindow
	MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
	;UAC::Unload ;Must call unload! ; #3017 fix
FunctionEnd

Function un.OnUnInstFailed
	HideWindow
	MessageBox MB_ICONSTOP|MB_OK "Uninstallation has failed!"
	;UAC::Unload ;Must call unload! ; #3017 fix
FunctionEnd

 
Function un.onInit
	Call un.DoRightsElevation

		MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
		Abort
FunctionEnd

Section Uninstall
	!ifdef CLIENT_SETUP
		ReadRegStr $GTA_DIR HKCU "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path"
		IfFileExists "$INSTDIR\server\mods\deathmatch\resources\*.*" ask 0 ;no maps folder, so delete everything
		IfFileExists "$INSTDIR\screenshots\*.*" ask 0 ;no maps folder, so delete everything
		IfFileExists "$INSTDIR\mods\deathmatch\resources\*.*" ask deleteall ;no maps folder, so delete everything
		ask:
		MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Would you like to keep your data files (such as resources, screenshots and server configuration)? If you click no, any resources, configurations or screenshots you have created will be lost." IDYES preservemapsfolder

		deleteall:
		RmDir /r "$INSTDIR\mods"
		RmDir /r "$INSTDIR\MTA"

		!ifdef INCLUDE_SERVER
			RmDir /r "$INSTDIR\server"
		!endif

		!ifdef INCLUDE_DEVELOPMENT ; start of fix for #3889
			RmDir /r "$INSTDIR\development\module sdk\publicsdk"
			RmDir "$INSTDIR\development\module sdk"
			RmDir "$INSTDIR\development"
		!endif ; end of fix for #3889

		preservemapsfolder:
		; server CORE FILES
		!ifdef INCLUDE_SERVER
			Delete "$INSTDIR\server\core.dll"
			Delete "$INSTDIR\server\xmll.dll"
			Delete "$INSTDIR\server\MTA Server.exe"
			Delete "$INSTDIR\server\net.dll"
			Delete "$INSTDIR\server\msvcp71.dll"
			Delete "$INSTDIR\server\msvcr71.dll"
			Delete "$INSTDIR\server\libcurl.dll"

			; server files
			Delete "$INSTDIR\server\mods\deathmatch\deathmatch.dll"
			Delete "$INSTDIR\server\mods\deathmatch\lua5.1.dll"
			Delete "$INSTDIR\server\mods\deathmatch\pcre3.dll"
			Delete "$INSTDIR\server\mods\deathmatch\pthreadVC2.dll"
			Delete "$INSTDIR\server\mods\deathmatch\sqlite3.dll"
		!endif

		Delete "$INSTDIR\Multi Theft Auto.exe"
		Delete "$INSTDIR\Multi Theft Auto.exe.dat"
		Delete "$INSTDIR\Uninstall.exe"

		Delete "$INSTDIR\mods\deathmatch\Client.dll"

		RmDir /r "$INSTDIR\MTA\cgui"
		RmDir /r "$INSTDIR\MTA\data"
		Delete "$INSTDIR\MTA\*.dll"
		Delete "$INSTDIR\MTA\*.ax"
		Delete "$INSTDIR\MTA\*.txt"

		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
		DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		DeleteRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas"
		DeleteRegKey HKCU "SOFTWARE\Multi Theft Auto: San Andreas"
		
		${GameExplorer_RemoveGame} ${GUID}
		
		; Delete shortcuts
		Delete "$SMPROGRAMS\\MTA San Andreas\MTA San Andreas.lnk"
		Delete "$SMPROGRAMS\\MTA San Andreas\Uninstall MTA San Andreas.lnk"
	!else
		RmDir /r "$INSTDIR\server" ; for server only install
	!endif

	RmDir "$INSTDIR" ; fix for #3898

	; Delete shortcuts
	Delete "$SMPROGRAMS\\MTA San Andreas\MTA Server.lnk"
	Delete "$SMPROGRAMS\\MTA San Andreas\Uninstall MTA San Andreas Server.lnk"
	RmDir /r "$SMPROGRAMS\\MTA San Andreas"
	
	SetAutoClose true
SectionEnd

; Function that skips the game directory page if client is not selected
Function SkipDirectoryPage
	SectionGetFlags ${SEC01} $R0
	IntOp $R0 $R0 & ${SF_SELECTED}
	IntCmp $R0 ${SF_SELECTED} +2 0
	Abort
FunctionEnd

Var REDIST

Function InstallVC90Redistributable
	DetailPrint "Installing Microsoft Visual Studio 2008 SP1 redistributable ..."
	StrCpy $REDIST "$TEMP\vcredist_x86.exe"
	NSISdl::download "http://download.microsoft.com/download/d/d/9/dd9a82d0-52ef-40db-8dab-795376989c03/vcredist_x86.exe" $REDIST
	Pop $0
	StrCmp "$0" "success" DownloadSuccessful
	
	DetailPrint "* Download of Microsoft Visual Studio 2008 SP1 redistributable failed:"
	DetailPrint "* $0"
	DetailPrint "* Installation continuing anyway"
	MessageBox MB_ICONSTOP "Unable to download Microsoft Visual Studio 2008 SP1 redistributable"
	Goto InstallEnd
	
DownloadSuccessful:
	ExecWait '"$REDIST"'
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
	MessageBox MB_ICONSTOP "Unable to install Microsoft Visual Studio 2008 SP1 redistributable"

InstallEnd:

	StrCmp "$RedistInstalled" "1" InstallEnd2
	MessageBox MB_ICONSTOP "Unable to download Microsoft Visual Studio 2008 SP1 redistributable.\
	$\r$\nHowever installation will continue.\
	$\r$\nPlease reinstall if there are problems later."
	StrCpy $RedistInstalled "1"

InstallEnd2:
FunctionEnd


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

; Input:
; 	$RESIZE_X $RESIZE_X		- Resize amount
Function ResizeComponentsDialogContents

 	FindWindow $HWND_DIALOG "#32770" "" $HWNDPARENT

	;Move description right and stretch down
	StrCpy $X $RESIZE_X
	StrCpy $Y 0
	StrCpy $CX 0
	StrCpy $CY $RESIZE_Y

 	StrCpy $ITEM_PARENT $HWND_DIALOG
 	StrCpy $ITEM_ID 1043	; Static - "Position your mouse over a component to see its description."
	Call MoveDialogItem
	
 	StrCpy $ITEM_PARENT $HWND_DIALOG
	StrCpy $ITEM_ID 1042	; Button - Description
	Call MoveDialogItem

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
	StrCpy $ITEM_ID 1	; Button - Back
	Call MoveDialogItem

	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 2	; Button - Cancel
	Call MoveDialogItem

	StrCpy $ITEM_PARENT $HWNDPARENT
	StrCpy $ITEM_ID 3 ; Button - Next
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
	
FunctionEnd


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

	System::Call "User32::SetWindowPos(i, i, i, i, i, i, i) b ($HWNDPARENT, 0, $1, $2, $3, $4, ${SWP_NOOWNERZORDER}|${SWP_NOSIZE})"

	;Restore register values from the stack
	Pop $4
	Pop $3
	Pop $2
	Pop $1
	Pop $0

FunctionEnd


Var COMPONENTS_EXPAND_STATUS

Function "LicenseShowProc"
	${If} $COMPONENTS_EXPAND_STATUS == 1
		StrCpy $COMPONENTS_EXPAND_STATUS 0
		IntOp $RESIZE_X 0 - ${EXPAND_DIALOG_X}
		IntOp $RESIZE_Y 0 - ${EXPAND_DIALOG_Y}
		Call ResizeSharedDialogContents
		Call ResizeMainWindow
	${Endif}
FunctionEnd


Function "ComponetsShowProc"
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
