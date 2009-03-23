/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CCommandFuncs.cpp
*  PURPOSE:		Implementation of all built-in commands
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*				Derek Abdine <>
*				Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>

using std::list;

extern CCore* g_pCore;
extern bool g_bBoundsChecker;

#define MTA_HELP_SPACING	32

/* ACHTUNG */
#include "tracking/CTCPManager.h"
#include "tracking/CHTTPClient.h"

void CCommandFuncs::Help ( const char* szParameters )
{
	CConsoleInterface* pConsole = g_pCore->GetConsole ();

    if ( szParameters && atoi(szParameters) == 1 )
        CCore::GetSingleton().GetMultiplayer()->AllowWindowsCursorShowing(true);
    else
        CCore::GetSingleton().GetMultiplayer()->AllowWindowsCursorShowing(false);

    pConsole->Printf ( "***[ COMMAND HELP ]***\n" );

	// Loop through all the available commands
	list < COMMANDENTRY* > ::iterator iter = CCommands::GetSingletonPtr ()->IterBegin ();
	list < COMMANDENTRY* > ::iterator iterEnd = CCommands::GetSingletonPtr ()->IterEnd ();

	char spacer[MTA_HELP_SPACING+1];
	for ( ; iter != iterEnd ; iter++ ) {
		const char *szCommandName = (*iter)->szCommandName;
		const char *szDescription = (*iter)->szDescription;

		// Space out the name and description evenly (tab-like)
		unsigned char spacing = MTA_HELP_SPACING - strlen ( szCommandName );
		if ( spacing <= 0 ) spacing = 1;

		memset(&spacer[0], ' ', MTA_HELP_SPACING);
		spacer[spacing] = NULL;

		pConsole->Printf ( "* %s%s%s\n", (*iter)->szCommandName, &spacer[0], (*iter)->szDescription );
	}

    pConsole->Printf ( "***[--------------]***\n" );
}

void dumpbasj ();;
void CCommandFuncs::Exit ( const char* szParameters )
{
    g_pCore->Quit ();
}

void CCommandFuncs::Ver ( const char* szParameters )
{
    ShowCursor(TRUE);
    HCURSOR hc = LoadCursor ( NULL, IDC_ARROW );
    SetCursor ( hc );  
    CLocalGUI::GetSingleton ( ).EchoConsole ( BLUE_VERSION_STRING );
}

void CCommandFuncs::ScreenShot ( const char* szParameters )
{
	g_pCore->TakeScreenShot ();
}


void CCommandFuncs::Vid ( const char* szParameters )
{
    // ripped from the renderware sdk
    CGameSettings * gameSettings = CCore::GetSingleton ( ).GetGame ( )->GetSettings();

    if ( strlen(szParameters) == 0 )
    {
        VideoMode           vidModemInfo;
        int                 vidMode, numVidModes, currentVidMode;

        numVidModes = gameSettings->GetNumVideoModes();
        currentVidMode = gameSettings->GetCurrentVideoMode();
        // Add the available video modes to the dialog
        for (vidMode = 0; vidMode < numVidModes; vidMode++)
        {
            gameSettings->GetVideoModeInfo(&vidModemInfo, vidMode);

            SString strMode ( "%d: %lu x %lu x %lu %s %s",
                    vidMode, vidModemInfo.width, vidModemInfo.height,
                    vidModemInfo.depth,
                    vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE ?
                    "(Fullscreen)" : "",
                    currentVidMode == vidMode ? "(Current)" : "" );

            CCore::GetSingleton ().GetConsole ()->Printf ( strMode );
        }
        CCore::GetSingleton ().GetConsole ()->Printf( "* Syntax: vid <mode>" );
    }
    else
    {
        // Make sure no mod is loaded
        if ( !CCore::GetSingleton ().GetModManager ()->IsLoaded () )
        {
            // Grab the device window and what mode to switch to
		    HWND hDeviceWindow = CDirect3DData::GetSingleton ().GetDeviceWindow ();
		    int iParameter = atoi ( szParameters );

            // Change the video mode
		    gameSettings->SetCurrentVideoMode ( iParameter );

		    // Apply some extra settings for windowed mode
		    if ( iParameter == 0 )
            {
                // Disable the thick frame so it can't be resized
                LONG lLong = GetWindowLong ( hDeviceWindow, GWL_STYLE ) ^ WS_THICKFRAME;
                lLong ^= WS_MAXIMIZEBOX;
				//lLong ^= WS_MINIMIZEBOX;
				lLong ^= WS_SYSMENU;
			    SetWindowLong ( hDeviceWindow, GWL_STYLE, lLong );

                SetWindowLong ( NULL, GWL_STYLE, WS_BORDER | WS_CAPTION );
			    SetWindowPos ( NULL, HWND_TOP, 0, 0, 0, 0, /* SWP_NOMOVE | SWP_NOSIZE | */ SWP_NOSENDCHANGING  );
		        SetWindowPos( hDeviceWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE );
		    }

            // Grab viewport settings
            int iViewportX = CCore::GetSingleton ().GetGraphics()->GetViewportWidth ();
            int iViewportY = CCore::GetSingleton ().GetGraphics()->GetViewportHeight ();

		    // Re-create all CGUI windows, for correct absolute sizes that depend on the (new) screen resolution
		    CCore::GetSingleton ().GetLocalGUI ()->DestroyWindows ();
		    CCore::GetSingleton ().GetGUI ()->SetResolution ( (float) iViewportX, (float) iViewportY );
		    CCore::GetSingleton ().GetLocalGUI ()->CreateWindows ();

            // Reload console, serverbrowser and chat settings (removed in DestroyWindows)
            g_pCore->ApplyConsoleSettings ();
            g_pCore->ApplyServerBrowserSettings ();
            g_pCore->ApplyMenuSettings ();
        }
        else
        {
            g_pCore->GetConsole ()->Echo ( "vid: Please disconnect before changing video mode" );
        }
    }
}


void CCommandFuncs::Window ( const char* szParameters )
{
    // Make sure no mod is loaded
    if ( !CCore::GetSingleton ().GetModManager ()->IsLoaded () )
    {
        CGameSettings * gameSettings = CCore::GetSingleton ( ).GetGame ( )->GetSettings();
        unsigned int currentMode = gameSettings->GetCurrentVideoMode();

        if ( currentMode == 0 )
        {
            // Maybe not the best way to find out the mode to return to

            VideoMode           vidModeInfo, currentModeInfo;
            int                 vidMode, numVidModes, currentVidMode;

            gameSettings->GetVideoModeInfo(&currentModeInfo, currentMode);

            numVidModes = gameSettings->GetNumVideoModes();
            currentVidMode = gameSettings->GetCurrentVideoMode();

            for (vidMode = 0; vidMode < numVidModes; vidMode++)
            {
                gameSettings->GetVideoModeInfo(&vidModeInfo, vidMode);

                if ( vidModeInfo.width == currentModeInfo.width &&
                     vidModeInfo.height == currentModeInfo.height &&
                     vidModeInfo.depth == currentModeInfo.depth &&
                     ( vidModeInfo.flags & rwVIDEOMODEEXCLUSIVE ) != ( currentModeInfo.flags & rwVIDEOMODEEXCLUSIVE ) )
                        gameSettings->SetCurrentVideoMode ( vidMode );
            }
        }
        else
        {
            // Run "vid 0"
            Vid ( "0" );
        }
    }
    else
    {
        g_pCore->GetConsole ()->Echo ( "window: Please disconnect first" );
    }
}


void CCommandFuncs::Time ( const char* szParameters )
{
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	
	SString strTimeAndDate ( "* The time is %d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );
    CCore::GetSingleton ().ChatEchoColor ( strTimeAndDate, 255, 100, 100 );
}

// this fails randomly, see comments in CConsole
void CCommandFuncs::Clear ( const char* szParameters )
{
    CCore::GetSingleton ().GetConsole ()->Clear ();
}


void CCommandFuncs::Load ( const char* szParameters )
{
    if ( !szParameters )
    {
        CCore::GetSingleton ().GetConsole ()->Printf( "* Syntax: load <mod-name> [<arguments>]" );
        return;
    }

    // Copy the buffer
    char* szTemp = new char [ strlen ( szParameters ) + 1 ];
    strcpy ( szTemp, szParameters );

    // Split it up into mod name and the arguments
    char* szModName = strtok ( szTemp, " " );
    char* szArguments = strtok ( NULL, "\0" );

    if ( szModName )
    {
        // Load the mod with the given arguments
        CModManager::GetSingleton ().RequestLoad ( szModName, szArguments );
    }
    else
        CCore::GetSingleton ().GetConsole ()->Printf( "* Syntax: load <mod-name> [<arguments>]" );

    // Free the temp buffer
    delete [] szTemp;
}


void CCommandFuncs::Unload ( const char* szParameters )
{
    // Any mod loaded?
    if ( CModManager::GetSingleton ().GetCurrentMod () )
    {
        // Unload it
        CModManager::GetSingleton ().RequestUnload ();
    }
    else
    {
        CCore::GetSingleton ().GetConsole ()->Print ( "No mod loaded!" );
    }
}


void CCommandFuncs::ConnectionType ( const char *szParameters )
{
	unsigned short usMTUSize = 0;

	if ( strcmpi ( szParameters, "lan" ) == 0 ) {
		usMTUSize = NET_MTU_LAN;
	} else if ( strcmpi ( szParameters, "dsl" ) == 0 ) {
		usMTUSize = NET_MTU_DSL;
	} else if ( strcmpi ( szParameters, "modem" ) == 0 ) {
		usMTUSize = NET_MTU_MODEM;
	} else {
		CCore::GetSingleton ().GetConsole ()->Print ( "Please specify a correct connection type (lan, dsl or modem)" );
	}

	if ( usMTUSize <= 0 ) 
        return;

    CCore::GetSingleton ().GetConnectManager ()->SetMTUSize ( usMTUSize );
    CVARS_SET ( "mtu_size", usMTUSize );
    CCore::GetSingleton ().GetConsole ()->Printf ( "MTU size was set to %u", usMTUSize );
}

void CCommandFuncs::Connect ( const char* szParameters )
{
    CModManager::GetSingleton ().Unload ();

    // Any mod loaded?
    if ( !CModManager::GetSingleton ().GetCurrentMod () )
    {
        // Parse the arguments (host port nick pass)
        char szBuffer [256];
        strncpy ( szBuffer, szParameters, 256 );
        szBuffer [255] = 0;

        if ( !strncmp( szBuffer, "mtasa://", 8 ) )
        {
            // Using a mtasa:// URI to connect
            SString strArguments = g_pCore->GetConnectCommandFromURI ( szBuffer );

            if ( strArguments.length () > 0 && g_pCore->GetCommands()->Execute ( strArguments ) )
            {
                return;
            }
        }

        char* szHost = strtok ( szBuffer, " " );
        char* szPort = strtok ( NULL, " " );
        char* szNick = strtok ( NULL, " " );
        char* szPass = strtok ( NULL, " " );

        std::string strNick;
        if ( !szNick )  CVARS_GET ( "nick", strNick );
        else            strNick = szNick;
        
        // Got all required arguments?
        if ( !szHost || !szPort || strNick.empty () )
        {
            CCore::GetSingleton ().GetConsole ()->Print ( "connect: Syntax is 'connect <host> <port> [<nick> <pass>]'" );
            return;
        }

        // Verify and convert the port number
        int iPort = atoi ( szPort );
        if ( iPort <= 0 || iPort > 0xFFFF )
        {
            CCore::GetSingleton ().GetConsole ()->Print ( "connect: Bad port number" );
            return;
        }

        unsigned short usPort = static_cast < unsigned short > ( iPort );

        // Got a password?
        char emptyPass = 0;
        if ( !szPass )
        {
            szPass = &emptyPass;
        }

        // Start the connect
        if ( CCore::GetSingleton ().GetConnectManager ()->Connect ( szHost, usPort, strNick.c_str (), szPass ) )
        {
            CCore::GetSingleton ().GetConsole ()->Printf ( "connect: Connecting to %s:%u...", szHost, usPort );
        }
        else
        {
            CCore::GetSingleton ().GetConsole ()->Printf ( "connect: could not connect to %s:%u!", szHost, usPort );
        }
    }
    else
    {
        CCore::GetSingleton ().GetConsole ()->Print ( "connect: Failed to unload current mod" );
    }
}

void CCommandFuncs::Reconnect ( const char* szParameters )
{
    CModManager::GetSingleton ().Unload ();

    std::string strHost, strNick, strPassword;
    unsigned int uiPort;

    CVARS_GET ( "host",         strHost );
    CVARS_GET ( "nick",         strNick );
    CVARS_GET ( "password",     strPassword );
    CVARS_GET ( "port",         uiPort );

    // Restart the connection.
    SString strTemp ( "%s %u %s %s", strHost.c_str (), uiPort, strNick.c_str (), strPassword.c_str () );

    Connect ( strTemp );
}

void CCommandFuncs::Bind ( const char* szParameters )
{
    CCore::GetSingleton ().GetKeyBinds ()->BindCommand ( szParameters );
}

void CCommandFuncs::Unbind ( const char* szParameters )
{
    CCore::GetSingleton ().GetKeyBinds ()->UnbindCommand ( szParameters );
}

void CCommandFuncs::Binds ( const char* szParameters )
{
    CCore::GetSingleton ().GetKeyBinds ()->PrintBindsCommand ( szParameters );
}

void CCommandFuncs::CopyGTAControls ( const char* szParameters )
{
    CKeyBindsInterface* pKeyBinds = CCore::GetSingleton ().GetKeyBinds ();

    if ( pKeyBinds )
    {
        pKeyBinds->RemoveAllGTAControls ();
        pKeyBinds->LoadControlsFromGTA ();
        CCore::GetSingleton ().GetConsole ()->Print ( "Bound all controls from GTA" );
    }
}

void CCommandFuncs::ClearDebug ( const char* szParameters )
{
    CCore::GetSingleton ().GetLocalGUI ()->GetDebugView ()->Clear ();
}

void CCommandFuncs::HUD ( const char* szParameters )
{
    int iCmd = ( szParameters && szParameters [ 0 ] ) ? atoi ( szParameters ) : -1;
	bool bDisabled = ( iCmd == 1 ) ? false : true;
	CCore::GetSingleton ().GetGame ()->GetHud ()->Disable ( bDisabled );
}

void CCommandFuncs::SaveConfig ( const char* szParameters )
{
    CCore::GetSingleton ().SaveConfig ();
    g_pCore->GetConsole ()->Printf ( "Saved configuration file" );
}
