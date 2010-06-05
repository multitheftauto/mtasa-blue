/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMainConfig.cpp
*  PURPOSE:     XML-based main configuration file parser class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Jax <>
*               Oliver Brown <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               Ed Lyons <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame * g_pGame;

using namespace std;

CMainConfig::CMainConfig ( CConsole* pConsole, CLuaManager* pLuaMain ): CXMLConfig ( NULL )
{
    m_pConsole = pConsole;
    m_pLuaManager = pLuaMain;
    m_pRootNode = NULL;
    m_pCommandLineParser = NULL;

    m_usServerPort = 0;
    m_uiMaxPlayers = 0;
	m_bHTTPEnabled = true;
    m_bAseEnabled = false;
	m_usHTTPPort = 0;
    m_ucHTTPDownloadType = HTTP_DOWNLOAD_DISABLED;
    m_bLogFileEnabled = false;
    m_bAutoUpdateAntiCheatEnabled = true;
    m_bJoinFloodProtectionEnabled = false;
    m_bScriptDebugLogEnabled = false;
    m_uiScriptDebugLogLevel = 0;
	m_bAutoUpdateIncludedResources = false;
	m_bDontBroadcastLan = false;
	m_uiMTUSize = MTU_SIZE_DEFAULT;
	m_usFPSLimit = 36;
}


bool CMainConfig::Load ( const char* szFilename )
{
    assert ( szFilename );

    // Eventually destroy the previously loaded xml
    if ( m_pFile )
    {
        delete m_pFile;
        m_pFile = NULL;
    }

    // Load the XML
    m_pFile = g_pServerInterface->GetXML ()->CreateXML ( szFilename );
    if ( !m_pFile )
    {
        CLogger::ErrorPrintf ( "Error loading config file\n" );
		return false;
    }

    // Parse it
    if ( !m_pFile->Parse () )
    {
        CLogger::ErrorPrintf ( "Error parsing config file\n" );
        return false;
    }

    // Grab the XML root node
    m_pRootNode = m_pFile->GetRootNode ();
    if ( !m_pRootNode )
    {
        CLogger::ErrorPrintf ( "Missing root node ('config')\n" );
        return false;
    }

    // Name
    int iResult = GetString ( m_pRootNode, "servername", m_strServerName, 1, 96 );
    if ( iResult == DOESNT_EXIST )
    {
        CLogger::ErrorPrintf ( "Server name not specified in config\n" );
        return false;
    }
    else if ( iResult == INVALID_VALUE )
    {
        CLogger::ErrorPrintf ( "Server name must be between 1 and 96 characters\n" );
        return false;
    }

	// Grab the script debuglog
    GetString ( m_pRootNode, "serverip", m_strServerIP, 1 );

    // Grab the port
    int iTemp;
    iResult = GetInteger ( m_pRootNode, "serverport", iTemp, 1, 65535 );
    if ( iResult == IS_SUCCESS )
    {
        m_usServerPort = static_cast < unsigned short > ( iTemp );
    }
    else
    {
        if ( iResult == DOESNT_EXIST )
            CLogger::ErrorPrintf ( "Server port not specified in config\n" );
        else
            CLogger::ErrorPrintf ( "Server port must be between 1 and 65535\n" );

        return false;
    }

	// Grab the MTU size
    iResult = GetInteger ( m_pRootNode, "mtusize", iTemp, 1, 65535 );
    if ( iResult == IS_SUCCESS )
    {
        m_uiMTUSize = iTemp;
    }
    else
    {
        if ( iResult != DOESNT_EXIST )
            CLogger::ErrorPrintf ( "MTU packet size must be between 1 and 65535, defaulting to %u\n", m_uiMTUSize );
    }

    // Grab the max players
    iResult = GetInteger ( m_pRootNode, "maxplayers", iTemp, 1, MAX_PLAYER_COUNT );
    if ( iResult == IS_SUCCESS )
    {
        m_uiMaxPlayers = iTemp;
    }
    else
    {
        if ( iResult == DOESNT_EXIST )
            CLogger::ErrorPrintf ( "Max players not specified in config\n" );
        else
            CLogger::ErrorPrintf ( "Max players must be between 1 and %u\n", MAX_PLAYER_COUNT );

        return false;
    }

    // httpserver
    iResult = GetBoolean ( m_pRootNode, "httpserver", m_bHTTPEnabled );
    if ( iResult == INVALID_VALUE )
    {
        CLogger::LogPrint ( "WARNING: Invalid value specified in \"httpserver\" tag; defaulting to 1\n" );
        m_bHTTPEnabled = true;
    }
    else if ( iResult == DOESNT_EXIST )
    {
        m_bHTTPEnabled = false;
    }

    // HTTPD port
    iResult = GetInteger ( m_pRootNode, "httpport", iTemp, 1, 65535 );
    if ( iResult == IS_SUCCESS )
    {
        m_usHTTPPort = static_cast < unsigned short > ( iTemp );
    }
    else
    {
        if ( iResult == DOESNT_EXIST )
            CLogger::ErrorPrintf ( "HTTP port is not specified in config\n" );
        else
            CLogger::ErrorPrintf ( "HTTP server port must be between 1 and 65535\n" );

        return false;
    }

    // HTTPD Download URL (if we want to host externally)
    if ( GetString ( m_pRootNode, "httpdownloadurl", m_strHTTPDownloadURL, 5 ) == IS_SUCCESS )
    {
        m_ucHTTPDownloadType = HTTP_DOWNLOAD_ENABLED_URL;
    }
    else
    {
        m_ucHTTPDownloadType = HTTP_DOWNLOAD_ENABLED_PORT;
        m_strHTTPDownloadURL = "";
    }

    // ASE
    iResult = GetBoolean ( m_pRootNode, "ase", m_bAseEnabled );
    if ( iResult == INVALID_VALUE )
    {
        CLogger::LogPrint ( "WARNING: Invalid value specified in \"ase\" tag; defaulting to 0\n" );
        m_bAseEnabled = false;
    }
    else if ( iResult == DOESNT_EXIST )
    {
        m_bAseEnabled = false;
    }

    // Update sites
    int i =0;
    for ( CXMLNode * updateURL = m_pRootNode->FindSubNode("update", i);
        updateURL != NULL; updateURL = m_pRootNode->FindSubNode("update", ++i ) )
    {
        g_pGame->GetResourceDownloader()->AddUpdateSite ( updateURL->GetTagContent ().c_str () );
    }
	
	// Auto update included resources
	iResult = GetBoolean ( m_pRootNode, "autoupdateincludedresources", m_bAutoUpdateIncludedResources );
	if ( iResult == INVALID_VALUE  || iResult == DOESNT_EXIST )
		m_bAutoUpdateIncludedResources = false;

	// Lan server broadcast
	iResult = GetBoolean ( m_pRootNode, "donotbroadcastlan", m_bDontBroadcastLan );
	if ( iResult == INVALID_VALUE  || iResult == DOESNT_EXIST )
		m_bDontBroadcastLan = false;

    // Grab the server password
    iResult = GetString ( m_pRootNode, "password", m_strPassword, 1, 32 );

    // Grab the server fps limit
    int iFPSTemp = 0;
    iResult = GetInteger ( m_pRootNode, "fpslimit", iFPSTemp, 0, 100 );
    if ( iResult == IS_SUCCESS )
    {
        if ( iFPSTemp == 0 || iFPSTemp >= 25 )
        {
             m_usFPSLimit = (unsigned short)iFPSTemp;
             SetInteger ( m_pRootNode, "fpslimit", (int)m_usFPSLimit );
        }
    }

	// Grab the serial verification
	/** ACHTUNG: Unsupported for release 1.0 (#4090)
	iResult = GetBoolean ( m_pRootNode, "verifyserials", m_bVerifySerials );
    if ( iResult == INVALID_VALUE )
    {
        m_bVerifySerials = true;
    }
    else if ( iResult == DOESNT_EXIST )
	*/
    {
        m_bVerifySerials = false;
    }

    // Grab the server logfile
    std::string strBuffer;
    if ( GetString ( m_pRootNode, "logfile", strBuffer, 1 ) == IS_SUCCESS )
    {
        m_strLogFile = g_pServerInterface->GetModManager ()->GetAbsolutePath ( strBuffer.c_str () );
        m_bLogFileEnabled = true;
    }
    else
    {
        m_bLogFileEnabled = false;
    }

    // Grab the server access control list
    if ( GetString ( m_pRootNode, "acl", strBuffer, 255, 1 ) == IS_SUCCESS )
    {
        m_strAccessControlListFile = g_pServerInterface->GetModManager ()->GetAbsolutePath ( strBuffer.c_str () );
    }
    else
    {
        m_strAccessControlListFile = g_pServerInterface->GetModManager ()->GetAbsolutePath ( "acl.xml" );
    }

	return true;
}


bool CMainConfig::LoadExtended ( void )
{
    std::string strBuffer;
    int iTemp = 0, iResult = 0;

	// Grab the script debuglog
    if ( GetString ( m_pRootNode, "scriptdebuglogfile", strBuffer, 255, 1 ) == IS_SUCCESS )
    {
        m_strScriptDebugLogFile = g_pServerInterface->GetModManager ()->GetAbsolutePath ( strBuffer.c_str () );
        m_bScriptDebugLogEnabled = true;
    }
    else
    {
        m_bScriptDebugLogEnabled = false;
    }

    // Grab the script debuglog level
    iResult = GetInteger ( m_pRootNode, "scriptdebugloglevel", iTemp, 0, 3 );
    if ( iResult == IS_SUCCESS )
    {
        m_uiScriptDebugLogLevel = iTemp;
    }
    else
    {
        if ( iResult == INVALID_VALUE )
        {
            CLogger::LogPrint ( "WARNING: Invalid value specified in \"scriptdebugloglevel\" tag; defaulting to 0\n" );
        }

        m_uiScriptDebugLogLevel = 0;
    }

    iResult = GetInteger ( m_pRootNode, "htmldebuglevel", iTemp, 0, 3 );
    if ( iResult == IS_SUCCESS )
    {
        g_pGame->GetScriptDebugging()->SetHTMLLogLevel ( iTemp );
    }
    else
    {
        if ( iResult == INVALID_VALUE )
        {
            CLogger::LogPrint ( "WARNING: Invalid value specified in \"htmldebuglevel\" tag; defaulting to 0\n" );
        }

        g_pGame->GetScriptDebugging()->SetHTMLLogLevel ( 0 );
    }

	// Handle the <module> nodes
    CXMLNode* pNode = NULL;
    unsigned int uiCurrentIndex = 0;
    do
    {
        pNode = m_pRootNode->FindSubNode ( "module", uiCurrentIndex++ );
        if ( pNode )
        {
            CXMLAttribute* pAttribute = pNode->GetAttributes ().Find ( "src" );
            if ( pAttribute )
            {
                std::string strBuffer = pAttribute->GetValue ();
                SString strFilename ( "%s/modules/%s", g_pServerInterface->GetModManager ()->GetModPath (), strBuffer.c_str () );

                if ( IsValidFilePath ( strBuffer.c_str () ) )
                {
                    m_pLuaManager->GetLuaModuleManager ()->_LoadModule ( strBuffer.c_str (), strFilename, false );
                }
            }
        }
    }
	while ( pNode );
    
    // Handle the <resource> nodes
    pNode = NULL;
    uiCurrentIndex = 0;
    bool bFoundDefault = false;
    bool bFirst = true;
    bool bNoProgress = false;

    do
    {
        // Grab the current script node
        pNode = m_pRootNode->FindSubNode ( "resource", uiCurrentIndex++ );
        if ( pNode )
        {
            // Grab its "src" attribute
            CXMLAttribute* pAttribute = pNode->GetAttributes ().Find ( "src" );
            if ( pAttribute )
            {
                // Grab the text in it and convert iwt to a path inside "scripts"
                std::string strBuffer = pAttribute->GetValue ();

                CResource * loadedResource = g_pGame->GetResourceManager ()->GetResource ( strBuffer.c_str () );
                if ( !loadedResource )
                {
                    CLogger::ErrorPrintf ( "Couldn't find resource %s. Check it exists.\n", strBuffer.c_str () );
                    bNoProgress = true;
                }
                else
                {
                    loadedResource->SetPersistent ( true );

                    pAttribute = pNode->GetAttributes ().Find ( "startup" );
                    if ( pAttribute )
                    {
                        std::string strStartup = pAttribute->GetValue ();
                        if ( strStartup.compare ( "true" ) == 0 ||
                             strStartup.compare ( "yes" ) == 0 ||
                             strStartup.compare ( "1" ) == 0 )
                        {
                            CLogger::SetOutputEnabled ( false );
                            if ( loadedResource->Start( NULL, true ) )
                            {
                                CLogger::SetOutputEnabled ( true );
                                if ( !bNoProgress )
                                {
                                    if ( bFirst )
                                        CLogger::LogPrint ( "Starting resources..." );
                                    else
                                        CLogger::LogPrintNoStamp ( "." );

                                    bFirst = false;
                                }
                            }
                            else
                            {
                                CLogger::SetOutputEnabled ( true );
                                CLogger::LogPrintNoStamp ( "\n" );
                                CLogger::ErrorPrintf ( "Unable to start resource %s; %s\n", strBuffer.c_str (), loadedResource->GetFailureReason ().c_str () );
                                bNoProgress = true;
                            }
                        }
                    }

					pAttribute = pNode->GetAttributes ().Find ( "protected" );
                    if ( pAttribute )
                    {
                        std::string strProtected = pAttribute->GetValue ();
                        if ( strProtected.compare ( "true" ) == 0 ||
                             strProtected.compare ( "yes" ) == 0 ||
                             strProtected.compare ( "1" ) == 0 )
                            loadedResource->SetProtected ( true );
                    }

                    // Default resource
                    pAttribute = pNode->GetAttributes ().Find ( "default" );
                    if ( pAttribute )
                    {
                        if ( !bFoundDefault )
                        {
                            std::string strProtected = pAttribute->GetValue ();
                            if ( strProtected.compare ( "true" ) == 0 ||
                                strProtected.compare ( "yes" ) == 0 ||
                                strProtected.compare ( "1" ) == 0 )
                            {
                                std::string strName = loadedResource->GetName ();
                                if ( !strName.empty () )
                                {
                                    g_pGame->GetHTTPD ()->SetDefaultResource ( strName.c_str () );
                                }

                                bFoundDefault = true;
                            }
                        }
                        else
                        {
                            CLogger::LogPrintNoStamp ( "\n" );
                            CLogger::ErrorPrintf ( "More than one default resource specified!\n" );
                            bNoProgress = true;
                        }
                    }
                }
            }
        }
    }
    while ( pNode );

    if ( !bNoProgress ) CLogger::LogPrintNoStamp ( "\n" );

    // Register the commands
    RegisterCommand ( "update", CConsoleCommands::Update, false );
    RegisterCommand ( "start", CConsoleCommands::StartResource, false );
    RegisterCommand ( "stop", CConsoleCommands::StopResource, false );
	RegisterCommand ( "stopall", CConsoleCommands::StopAllResources, false );
    RegisterCommand ( "restart", CConsoleCommands::RestartResource, false );
    RegisterCommand ( "refresh", CConsoleCommands::RefreshResources, false );
    RegisterCommand ( "refreshall", CConsoleCommands::RefreshAllResources, false );
    RegisterCommand ( "list", CConsoleCommands::ListResources, false );
    RegisterCommand ( "info", CConsoleCommands::ResourceInfo, false );
    RegisterCommand ( "install", CConsoleCommands::InstallResource, false );
    RegisterCommand ( "upgrade", CConsoleCommands::UpgradeResources, false );

    RegisterCommand ( "say", CConsoleCommands::Say, false );
    RegisterCommand ( "teamsay", CConsoleCommands::TeamSay, false );
    RegisterCommand ( "asay", CConsoleCommands::ASay, false );
    RegisterCommand ( "msg", CConsoleCommands::Msg, false );
    RegisterCommand ( "amsg", CConsoleCommands::AMsg, false );
    RegisterCommand ( "me", CConsoleCommands::Me, false );
    RegisterCommand ( "nick", CConsoleCommands::Nick, false );

    RegisterCommand ( "login", CConsoleCommands::LogIn, false );
    RegisterCommand ( "logout", CConsoleCommands::LogOut, false );
    RegisterCommand ( "chgmypass", CConsoleCommands::ChgMyPass, false );

    RegisterCommand ( "addaccount", CConsoleCommands::AddAccount, false );
    RegisterCommand ( "delaccount", CConsoleCommands::DelAccount, false );
    RegisterCommand ( "chgpass", CConsoleCommands::ChgPass, false );
    RegisterCommand ( "shutdown", CConsoleCommands::Shutdown, false );

    RegisterCommand ( "aexec", CConsoleCommands::AExec, false );

    RegisterCommand ( "whois", CConsoleCommands::WhoIs, false );
    RegisterCommand ( "whowas", CConsoleCommands::WhoWas, false );

    RegisterCommand ( "debugscript", CConsoleCommands::DebugScript, false );

	RegisterCommand ( "sudo", CConsoleCommands::Sudo, false );

    RegisterCommand ( "help", CConsoleCommands::Help, false );

	RegisterCommand ( "loadmodule", CConsoleCommands::LoadModule, false );
	//RegisterCommand ( "unloadmodule", CConsoleCommands::UnloadModule, false );
	//RegisterCommand ( "reloadmodule", CConsoleCommands::ReloadModule, false );

	return true;
}


bool CMainConfig::Save ( const char* szFilename )
{
    // If we have a file
    if ( m_pFile && m_pRootNode )
    {
        // Save it
        return m_pFile->Write ();
    }

    // No file
    return false;
}


bool CMainConfig::IsValidPassword ( const char* szPassword, unsigned int& uiUnsupportedIndex )
{
    // Test all characters for visibilty
    uiUnsupportedIndex = 0;
    const char* szPtr = szPassword;
    while ( *szPtr != 0 )
    {
        if ( *szPtr == ' ' )
        {
            return false;
        }

        ++szPtr;
        ++uiUnsupportedIndex;
    }
    
    // Bigger than 32 chars?
    return ( szPtr - szPassword <= 32 );
}


void CMainConfig::SetPassword ( const char* szPassword ) 
{
    m_strPassword = szPassword ? szPassword : "";
    SetString ( m_pRootNode, "password", szPassword );
}


void CMainConfig::SetFPSLimit ( unsigned short usFPS )
{
    if ( usFPS == 0 || ( usFPS >= 25 && usFPS <= 100 ) )
    {
        m_usFPSLimit = usFPS;
        SetInteger ( m_pRootNode, "fpslimit", usFPS );
    }
}


void CMainConfig::RegisterCommand ( const char* szName, FCommandHandler* pFunction, bool bRestricted )
{
    // Register the function with the given name and function pointer
    m_pConsole->AddCommand ( pFunction, szName, bRestricted );
}


void CMainConfig::SetCommandLineParser ( CCommandLineParser* pCommandLineParser )
{
    m_pCommandLineParser = pCommandLineParser;
}

std::string CMainConfig::GetServerIP ( void )
{
    std::string strServerIP;
    if ( m_pCommandLineParser && m_pCommandLineParser->GetIP ( strServerIP ) )
        return strServerIP;
    return m_strServerIP;
}

unsigned short CMainConfig::GetServerPort ( void )
{
    unsigned short usPort;
    if ( m_pCommandLineParser && m_pCommandLineParser->GetPort ( usPort ) )
        return usPort;
    return m_usServerPort;
}

unsigned int CMainConfig::GetMaxPlayers ( void )
{
    unsigned int uiMaxPlayers;
    if ( m_pCommandLineParser && m_pCommandLineParser->GetMaxPlayers ( uiMaxPlayers ) )
        return uiMaxPlayers;
    return m_uiMaxPlayers;
}

unsigned short CMainConfig::GetHTTPPort ( void )
{
    unsigned short usHTTPPort;
    if ( m_pCommandLineParser && m_pCommandLineParser->GetHTTPPort ( usHTTPPort ) )
        return usHTTPPort;
    return m_usHTTPPort;
}
