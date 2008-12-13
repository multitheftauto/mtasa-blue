!include nsDialogs.nsh
!include LogicLib.nsh
!include Sections.nsh

XPStyle on
RequestExecutionLevel user
SetCompressor /SOLID lzma

!define TEMP1 $R0
!define TEMP2 $R1
!define TEMP3 $R2

Var GTA_DIR
Var Install_Dir

; ###########################################################################################################
;!define FILES_ROOT "C:\Build\output"
;!define SERVER_FILES_ROOT "c:\build\mta10_server\output"
;!define LIGHTBUILD
!ifndef PRODUCT_VERSION
!define PRODUCT_VERSION "v1.0"
!endif
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

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON            "mta.ico"
!define MUI_UNICON          "mta.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE                       "eula.txt"

; Components page
!insertmacro MUI_PAGE_COMPONENTS

; Game directory page
!define MUI_PAGE_CUSTOMFUNCTION_PRE                 SkipDirectoryPage
!define MUI_DIRECTORYPAGE_VARIABLE                  $INSTDIR
!insertmacro MUI_PAGE_DIRECTORY
!define MUI_PAGE_HEADER_TEXT                        "Grand Theft Auto: San Andreas location"
!define MUI_PAGE_HEADER_SUBTEXT                     ""
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION          "Grand Theft Auto: San Andreas folder"
!define MUI_DIRECTORYPAGE_TEXT_TOP                  "Please select your Grand Theft Auto: San Andreas folder.$\n$\nYou MUST have Grand Theft Auto: San Andreas 1.0 installed to use MTA:SA, it does not support any other versions.$\n$\nClick Install to begin installing."
!define MUI_DIRECTORYPAGE_VARIABLE                  $GTA_DIR
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_RUN                          ""
!define MUI_FINISHPAGE_RUN_FUNCTION                 "LaunchLink"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE                           "English"
LangString DESC_Section1 ${LANG_ENGLISH}            "The core components required to run Multi Theft Auto."
LangString DESC_Section2 ${LANG_ENGLISH}            "The MTA:SA DM modification, allowing you to play online."
LangString DESC_Section3 ${LANG_ENGLISH}            "The Multi Theft Auto:Editor for MTA:SA DM, allowing you to create and edit maps."
LangString DESC_SectionGroupMods ${LANG_ENGLISH}    "Modifications for Multi Theft Auto. Without at least one of these, you cannot play Multi Theft Auto."
LangString DESC_SectionGroupServer ${LANG_ENGLISH}  "The Multi Theft Auto Server. This allows you to host games from your computer. This requires a fast internet connection."
LangString DESC_Section4 ${LANG_ENGLISH}            "The Multi Theft Auto server. This is a required component."
LangString DESC_Section5 ${LANG_ENGLISH}            "The MTA:SA DM modification for the server."
LangString DESC_Section6 ${LANG_ENGLISH}            "This is an set of required resources for your server."
LangString DESC_Section7 ${LANG_ENGLISH}            "This is an optional set of gamemodes and maps for your server."
LangString DESC_Section8 ${LANG_ENGLISH}            "This is a preview of the new MTA editor. It is incomplete and partially non-functional."
LangString DESC_Section9 ${LANG_ENGLISH}            "This is the SDK for creating binary modules for the MTA server. Only install if you have a good understanding of C++!"
LangString DESC_SectionGroupDev ${LANG_ENGLISH}     "Development code and toolss that aid in the creation of mods for Multi Theft Auto"
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
	
	${NSD_CreateLabel} 0 0 100% 20u "Multi Theft Auto: Deathmatch has a central login system that provides a unique username for each user."
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
         
         ;
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
        Ver::CS $StoredUsername $StoredSerial
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
         
         Ver::Verify $Username $Password
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

    Ver::CS ${TEMP1} ${TEMP3}
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

    Ver::Verify ${TEMP1} ${TEMP2}
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
  ExecShell "" "$INSTDIR\Multi Theft Auto.exe"
  !else
  SetOutPath "$INSTDIR\Server"
  ExecShell "" "$INSTDIR\Server\MTA Server.exe"
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
    # elevation was aborted, run as normal?
    MessageBox mb_iconstop "This installer requires admin access, aborting!"
    Abort

UAC_Success:
    StrCmp 1 $3 +4 ;Admin?
    StrCmp 3 $1 0 UAC_ElevationAborted ;Try again?
    MessageBox mb_iconstop "This installer requires admin access, try again"
    goto UAC_Elevate


  ReadRegStr $Install_Dir HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "" ; start of fix for #3743
    ${If} $Install_Dir == '' 
      strcpy $INSTDIR "$PROGRAMFILES\MTA San Andreas"
    ${Else} 
      strcpy $INSTDIR $Install_Dir
    ${EndIf} ; end of fix for #3743
  
  !ifdef CLIENT_SETUP
  ReadRegStr $2 HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path"
  StrCmp $2 "" trynext cont
  trynext:
  ReadRegStr $2 HKLM "SOFTWARE\Rockstar Games\GTA San Andreas\Installation" "ExePath"
  cont:
  !insertmacro ReplaceSubStr $2 "gta_sa.exe" ""
  strcpy $3 '"'
  !insertmacro ReplaceSubStr $MODIFIED_STR $3 ""
  strcpy $GTA_DIR $MODIFIED_STR
  !endif
  
  InitPluginsDir
  File /oname=$PLUGINSDIR\serialdialog.ini "serialdialog.ini"
FunctionEnd

Function .onInstSuccess
!ifdef CLIENT_SETUP
WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path" $GTA_DIR
WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "" $INSTDIR

; Add the protocol handler
WriteRegStr HKCR "mtasa" "" "URL:MTA San Andreas Protocol"
WriteRegStr HKCR "mtasa" "URL Protocol" ""
WriteRegStr HKCR "mtasa\DefaultIcon" "" "$INSTDIR\Multi Theft Auto.exe"
WriteRegStr HKCR "mtasa\shell\open\command" "" '"$INSTDIR\Multi Theft Auto.exe"%1'
!endif
CreateDirectory "$SMPROGRAMS\MTA San Andreas"

  !ifdef CLIENT_SETUP
  IfFileExists "$INSTDIR\Multi Theft Auto.exe" 0 skip1
  SetOutPath "$INSTDIR"
  CreateShortCut "$SMPROGRAMS\\MTA San Andreas\Play MTA San Andreas.lnk" "$INSTDIR\Multi Theft Auto.exe" \
  "" "$INSTDIR\Multi Theft Auto.exe" 0 SW_SHOWNORMAL \
  "" "Play Multi Theft Auto: San Andreas"
  !endif
  skip1:
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
  

  UAC::Unload ;Must call unload!
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
InstallDirRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas" ""
ShowInstDetails show
ShowUnInstDetails show

!ifdef CLIENT_SETUP
SectionGroup /e "Game client" SECGCLIENT
Section "Core components" SEC01
  SectionIn 1 RO ; section is required
  
  WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path" $GTA_DIR
  WriteRegStr HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "" $INSTDIR

  SetOutPath "$GTA_DIR\MTA"
  SetOverwrite on

  File "${FILES_ROOT}\GTA San Andreas\mta\cgui.dll"
  File "${FILES_ROOT}\GTA San Andreas\mta\core.dll"
  File "${FILES_ROOT}\GTA San Andreas\mta\xmll.dll"
  File "${FILES_ROOT}\GTA San Andreas\mta\game_sa.dll"
  File "${FILES_ROOT}\GTA San Andreas\mta\multiplayer_sa.dll"
  File "${FILES_ROOT}\GTA San Andreas\mta\net.dll"
  
  !ifndef LIGHTBUILD
    File "${FILES_ROOT}\GTA San Andreas\mta\d3dx9_33.dll"
    File "${FILES_ROOT}\GTA San Andreas\mta\msvcp71.dll"
    File "${FILES_ROOT}\GTA San Andreas\mta\msvcr71.dll"
    File "${FILES_ROOT}\GTA San Andreas\mta\lua5.1.dll"
    File "${FILES_ROOT}\GTA San Andreas\mta\libcurl.dll"
    File "${FILES_ROOT}\GTA San Andreas\mta\vorbis.ax"
    File "${FILES_ROOT}\GTA San Andreas\mta\xv.ax"
    File "${FILES_ROOT}\GTA San Andreas\mta\xvidcore.dll"
    File "${FILES_ROOT}\GTA San Andreas\mta\matroska.ax"

    SetOutPath "$GTA_DIR\MTA\cgui"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\CGUI.is.xml"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\CGUI.lnf.xml"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\CGUI.png"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\CGUI.xml"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\Falagard.xsd"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\Font.xsd"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\GUIScheme.xsd"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\Imageset.xsd"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\sagothic.ttf"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\saheader.ttf"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\sans.dat"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\sans.tga"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\sans.ttf"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\sabankgothic.ttf"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\pricedown.ttf"

    SetOutPath "$GTA_DIR\MTA\cgui\images"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\images\*.png"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\images\*.jpg"

    SetOutPath "$GTA_DIR\MTA\cgui\images\radarset"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\images\radarset\*.png"

    SetOutPath "$GTA_DIR\MTA\cgui\images\transferset"
    File "${FILES_ROOT}\GTA San Andreas\mta\cgui\images\transferset\*.png"

    SetOutPath "$GTA_DIR\MTA\data"
    File "${FILES_ROOT}\GTA San Andreas\mta\data\*.png"
    ;  File "${FILES_ROOT}\GTA San Andreas\mta\data\menu.mkv"
    File "${FILES_ROOT}\GTA San Andreas\mta\data\pixel.psh"
    File "${FILES_ROOT}\GTA San Andreas\mta\data\scene.x"
    File "${FILES_ROOT}\GTA San Andreas\mta\data\vertex.vsh"

    SetOutPath "$GTA_DIR\MTA\data\textures"
    File "${FILES_ROOT}\GTA San Andreas\mta\data\textures\*.png"
  !endif

  SetOutPath "$INSTDIR"
  File "${FILES_ROOT}\MTA San Andreas\*" ; NOT RECURSIVE
SectionEnd

Section "Game module" SEC02
  SectionIn 1 RO
  SetOutPath "$INSTDIR\mods\deathmatch"
  File "${FILES_ROOT}\MTA San Andreas\mods\deathmatch\Client.dll"
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
    SetOutPath "$INSTDIR\server"
    SetOverwrite on
    File "${SERVER_FILES_ROOT}\core.dll"
    File "${SERVER_FILES_ROOT}\xmll.dll"
    File "${SERVER_FILES_ROOT}\MTA Server.exe"
    File "${SERVER_FILES_ROOT}\net.dll"
    File "${SERVER_FILES_ROOT}\msvcp71.dll"
    File "${SERVER_FILES_ROOT}\msvcr71.dll"

!ifndef LIGHTBUILD
    File "${SERVER_FILES_ROOT}\libcurl.dll"
!endif
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
    
    SetOverwrite off
    File "${SERVER_FILES_ROOT}\mods\deathmatch\accounts.xml"
    File "${SERVER_FILES_ROOT}\mods\deathmatch\acl.xml"
    File "${SERVER_FILES_ROOT}\mods\deathmatch\banlist.xml"
    File "${SERVER_FILES_ROOT}\mods\deathmatch\mtaserver.conf"
    File "${SERVER_FILES_ROOT}\mods\deathmatch\vehiclecolors.conf"

    !ifndef LIGHTBUILD
   
    !ifdef CLIENT_SETUP
    File "${SERVER_FILES_ROOT}\mods\deathmatch\local.conf"
    !endif
    
    SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
    SetOutPath "$INSTDIR\server\mods\deathmatch\resourcecache"
    SetOutPath "$INSTDIR\server\mods\deathmatch\logs"
!endif
SectionEnd

!ifndef LIGHTBUILD
Section "Core resources" SEC06
SectionIn 1 2 RO ; section is required
  SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
  SetOverwrite ifnewer
  File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\required\*.zip"
  File /r /x ".svn" "${SERVER_FILES_ROOT}\mods\deathmatch\resources\edf"
  File /r /x ".svn" "${SERVER_FILES_ROOT}\mods\deathmatch\resources\freecam"
  File /r /x ".svn" "${SERVER_FILES_ROOT}\mods\deathmatch\resources\msgbox"
  File /r /x ".svn" "${SERVER_FILES_ROOT}\mods\deathmatch\resources\move_*"
  File /r /x ".svn" "${SERVER_FILES_ROOT}\mods\deathmatch\resources\tooltip"
SectionEnd
!endif

!ifndef LIGHTBUILD
Section "Resource and maps pack" SEC07
SectionIn 1 2
  SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
  SetOverwrite ifnewer
  File /r "${SERVER_FILES_ROOT}\mods\deathmatch\resources\optional\*.zip"
SectionEnd
!endif

!ifdef INCLUDE_EDITOR
Section /o "Editor (Developer version *still unzipped*)" SEC08
  SetOutPath "$INSTDIR\server\mods\deathmatch\resources"
  SetOverwrite ifnewer
  File /r /x ".svn" "${SERVER_FILES_ROOT}\mods\deathmatch\resources\editor*"
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
	File /r /x ".svn" "${FILES_MODULE_SDK}"
SectionEnd
SectionGroupEnd
!endif

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
  UAC::Unload ;Must call unload! ; #3017 fix
FunctionEnd

Function un.OnUnInstFailed
  HideWindow
  MessageBox MB_ICONSTOP|MB_OK "Uninstallation has failed!"
  UAC::Unload ;Must call unload! ; #3017 fix
FunctionEnd

 
Function un.onInit
 
UAC_Elevate: ; start of #3017 fix
    UAC::RunElevated 
    StrCmp 1223 $0 UAC_ElevationAborted ; UAC dialog aborted by user?
    StrCmp 0 $0 0 UAC_Err ; Error?
    StrCmp 1 $1 0 UAC_Success ;Are we the real deal or just the wrapper?
    Quit
 
UAC_Err:
    MessageBox mb_iconstop "Unable to elevate, error $0"
    Abort
 
UAC_ElevationAborted:
    # elevation was aborted, run as normal?
    MessageBox mb_iconstop "This uninstaller requires admin access, aborting!"
    Abort
 
UAC_Success:
    StrCmp 1 $3 +4 ;Admin?
    StrCmp 3 $1 0 UAC_ElevationAborted ;Try again?
    MessageBox mb_iconstop "This uninstaller requires admin access, try again"
    goto UAC_Elevate ; end of #3017 fix

  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  !ifdef CLIENT_SETUP
  ReadRegStr $GTA_DIR HKLM "SOFTWARE\Multi Theft Auto: San Andreas" "GTA:SA Path"
  IfFileExists "$INSTDIR\mods\map_editor\maps\*.*" ask 0 ;no maps folder, so delete everything
  IfFileExists "$INSTDIR\mods\map_editor\screenshots\*.*" ask 0 ;no maps folder, so delete everything
  IfFileExists "$INSTDIR\mods\race\screenshots\*.*" ask deleteall ;no maps folder, so delete everything
  ask:
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Would you like to keep your data files (such as resources and screenshots)? If you click no, any maps or screenshots you have created will be lost." IDYES preservemapsfolder

  deleteall:
  RmDir /r "$INSTDIR\mods"
  RmDir /r "$GTA_DIR\MTA"

  !ifdef INCLUDE_SERVER
  RmDir /r "$INSTDIR\server"
  !endif
 
  !ifdef INCLUDE_DEVELOPMENT ; start of fix for #3889
  RmDir /r "$INSTDIR\development\module sdk\publicsdk"
  RmDir "$INSTDIR\development\module sdk"
  RmDir "$INSTDIR\development"
  !endif ; end of fix for #3889
  
  preservemapsfolder:
  ; serve CORE FILES
!ifdef INCLUDE_SERVER
    Delete "$INSTDIR\server\core.dll"
    Delete "$INSTDIR\server\xmll.dll"
    Delete "$INSTDIR\server\MTA Server.exe"
    Delete "$INSTDIR\server\net.dll"
    Delete "$INSTDIR\server\msvcp71.dll"
    Delete "$INSTDIR\server\msvcr71.dll"
    !ifndef LIGHTBUILD
        Delete "$INSTDIR\server\pcre3.dll"
        Delete "$INSTDIR\server\pthreadVC2.dll"
        Delete "$INSTDIR\server\sqlite3.dll"
        Delete "$INSTDIR\server\libcurl.dll"
    !endif
    
    ; server DM files
    Delete "$INSTDIR\server\mods\deathmatch\deathmatch.dll"
    Delete "$INSTDIR\server\mods\deathmatch\lua5.1.dll"
    Delete "$INSTDIR\server\mods\deathmatch\pcre3.dll"
    Delete "$INSTDIR\server\mods\deathmatch\pthreadVC2.dll"
    Delete "$INSTDIR\server\mods\deathmatch\sqlite3.dll"
!endif

  Delete "$INSTDIR\Multi Theft Auto.exe"
  Delete "$INSTDIR\Uninstall.exe"
  
  Delete "$INSTDIR\mods\deathmatch\Client.dll"
  
  RmDir /r "$GTA_DIR\MTA\cgui"
  RmDir /r "$GTA_DIR\MTA\data"
  Delete "$GTA_DIR\MTA\data\*.dll"
  Delete "$GTA_DIR\MTA\data\*.ax"
  Delete "$GTA_DIR\MTA\data\*.log"
  Delete "$GTA_DIR\MTA\data\*.txt"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  DeleteRegKey HKLM "SOFTWARE\Multi Theft Auto: San Andreas"
  !else
  RmDir /r "$INSTDIR\server" ; for server only install
  !endif
  RmDir "$INSTDIR" ; fix for #3898
  SetAutoClose true
SectionEnd

# Function that skips the game directory page if client is not selected
Function SkipDirectoryPage
  SectionGetFlags ${SEC01} $R0
  IntOp $R0 $R0 & ${SF_SELECTED}
  IntCmp $R0 ${SF_SELECTED} 0 +2
  Abort
FunctionEnd
