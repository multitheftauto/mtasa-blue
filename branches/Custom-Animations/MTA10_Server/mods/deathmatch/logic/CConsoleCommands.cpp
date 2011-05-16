/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConsoleCommands.cpp
*  PURPOSE:     Server console command definitions class
*  DEVELOPERS:  Oliver Brown <>
*               Jax <>
*               Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <CMD5Hasher.h>

extern CGame* g_pGame;

// Helper function
static string GetAdminNameForLog ( CClient* pClient )
{
    string strName = pClient->GetNick ();
    string strAccountName = pClient->GetAccount () ? pClient->GetAccount ()->GetName () : "no account";
    if ( strName == strAccountName )
        return strName;
    return SString ( "%s(%s)", strName.c_str (), strAccountName.c_str () );
}

bool CConsoleCommands::Update ( CConsole* pConsole, const char* szarguments, CClient* pClient, CClient* pEchoClient )
{
    char szBuffer[256];
    szBuffer[0] = '\0';
    if ( szarguments )
        strncpy ( szBuffer, szarguments, 256 );
    szBuffer[255] = '\0';

    char* szResourceName = strtok ( szBuffer, " " );
    char* szVersion = strtok ( NULL, "\0" );

    if ( szResourceName == NULL && szVersion == NULL )
    {
        std::list<CUpdateResource*> resourcelist;
        g_pGame->GetResourceDownloader()->GetAvailableResources ( &resourcelist );
        list < CUpdateResource* > ::iterator iter = resourcelist.begin ();
        for ( ; iter != resourcelist.end (); iter++ )
        {
            list<CUpdateResourceVersion*> resourcelist;
            g_pGame->GetResourceDownloader()->FindUpdates ( szResourceName, 0, 0, 0, 0, &resourcelist );
            char szText[512];
            if ( resourcelist.size() == 1 )
                snprintf ( szText, 511, "    %s (1 version)", (*iter)->GetName().c_str () );
            else
                snprintf ( szText, 511, "    %s (%d versions)", (*iter)->GetName().c_str (), resourcelist.size() );
            szText[511] = '\0';
            pEchoClient->SendConsole ( szText );
        }
    }
    else if ( szResourceName != NULL && szVersion == NULL )
    {
        CResource * existingResource = g_pGame->GetResourceManager()->GetResource ( szResourceName );
        unsigned int currentVersionMajor = 0, currentVersionMinor = 0, currentVersionRevision = 0, currentVersionState = 0;
        if ( existingResource )
        {
            currentVersionMajor = existingResource->GetVersionMajor();
            currentVersionMinor = existingResource->GetVersionMinor();
            currentVersionRevision = existingResource->GetVersionRevision();
            currentVersionState = existingResource->GetVersionState();
        }

        list<CUpdateResourceVersion*> resourcelist;
        g_pGame->GetResourceDownloader()->FindUpdates ( szResourceName, 0, 0, 0, 0, &resourcelist );
        if ( resourcelist.size() != 0 )
        {
            list < CUpdateResourceVersion* > ::iterator iter = resourcelist.begin ();
            char szText[512];
            snprintf ( szText, 511, "Updates for %s", szResourceName );
            szText[511] = '\0';
            pEchoClient->SendConsole ( szText );
            for ( ; iter != resourcelist.end (); iter++ )
            {
                char szStateName[10];
                if ( (*iter)->GetState() == 0 )
                    strcpy ( szStateName, "alpha" );
                else if ( (*iter)->GetState() == 1 )
                    strcpy ( szStateName, "beta" );
                else
                    strcpy ( szStateName, "release" );
                std::string strDomain = (*iter)->GetUpdateResource()->GetUpdateSite()->GetDomain ();

                char szVersion[20];
                snprintf ( szVersion, 19, "%d.%d.%d", (*iter)->GetMajor(), (*iter)->GetMinor(), (*iter)->GetRevision() );

                char szIsCurrent[2] = " ";
                if ( existingResource && currentVersionMajor == (*iter)->GetMajor() && currentVersionMinor == (*iter)->GetMinor() &&
                    currentVersionRevision == (*iter)->GetRevision() && currentVersionState == (*iter)->GetState()  )
                {
                    if ( existingResource->GetLastChecksum() == (*iter)->GetChecksum() )
                        szIsCurrent[0] = '>';
                    else
                        szIsCurrent[0] = '?';
                }

                snprintf ( szText, 511, "%s  %-10.10s  %-10.10s %-30.30s", szIsCurrent, szVersion, szStateName, strDomain.c_str () );

                szText[511] = '\0';
                pEchoClient->SendConsole ( szText );
            }
            snprintf ( szText, 511, "To install, run: update %s X.Y.Z [release state]", szResourceName );

            szText[511] = '\0';
            pEchoClient->SendConsole ( szText );
        }
        else
        {
            char szText[512];
            snprintf ( szText, 511, "No updates available for %s", szResourceName );
            szText[511] = '\0';
            pEchoClient->SendConsole ( szText );
        }
    }
    else if ( szResourceName != NULL && szVersion != NULL )
    {
        char * szMajorVersion = strtok ( szVersion, "." );
        char * szMinorVersion = strtok ( NULL, "." );
        char * szRevisionVersion = strtok ( NULL, " " );
        char * szVersionState = strtok ( NULL, "\0" );

        if ( szMajorVersion && szMinorVersion && szRevisionVersion )
        {
//            _asm int 3
            unsigned int uiMajorVersion = atoi(szMajorVersion);
            unsigned int uiMinorVersion = atoi(szMinorVersion);
            unsigned int uiRevisionVersion = atoi(szRevisionVersion);
            unsigned int uiVersionState = 2;
            if ( szVersionState && stricmp ( szVersionState, "alpha" ) == 0 )
                uiVersionState = 0;
            else if ( szVersionState && stricmp ( szVersionState, "beta" ) == 0 )
                uiVersionState = 1;

            CUpdateResourceVersion * update = g_pGame->GetResourceDownloader()->Get ( szResourceName, uiMajorVersion, uiMinorVersion, uiRevisionVersion, uiVersionState );
            if ( update )
            {
                CResource * resource = g_pGame->GetResourceManager()->GetResource ( szResourceName );
                if ( resource )
                    g_pGame->GetResourceManager()->Unload ( resource );

                if ( update->Download ( true ) )
                {
                    char szText[512];
                    snprintf ( szText, 511, "Downloading resource '%s'", szResourceName );
                    szText[511] = '\0';
                    pEchoClient->SendConsole ( szText );
                }
                else
                {
                    char szText[512];
                    snprintf ( szText, 511, "Download of resource '%s' is already in progress", szResourceName );
                    szText[511] = '\0';
                    pEchoClient->SendConsole ( szText );
                }
            }
            else
            {
                char szText[512];
                snprintf ( szText, 511, "Could not find update site for requested version of '%s'", szResourceName );
                szText[511] = '\0';
                pEchoClient->SendConsole ( szText );
            }
        }
        else
        {
            char szText[512];
            snprintf ( szText, 511, "Syntax: update <resource> <major version>.<minor version>.<revision version> [state]" );
            szText[511] = '\0';
            pEchoClient->SendConsole ( szText );
        }
    }
    return true;
}

bool CConsoleCommands::StartResource ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    char szResponse [ 256 ];
    szResponse[0] = '\0';

    if ( szArguments && szArguments[0] )
    {
        CResource * resource = g_pGame->GetResourceManager()->GetResource ( (char *)szArguments );
        if ( resource )
        {
            if ( pClient->GetNick () )
                CLogger::LogPrintf ( "start: Requested by %s\n", GetAdminNameForLog ( pClient ).c_str () );

            if ( resource->IsLoaded() )
            {
                if ( !resource->IsActive() )
                {
                    if ( g_pGame->GetResourceManager()->StartResource ( resource, NULL, true ) )
                    {
                        snprintf ( szResponse, 256, "start: Resource '%s' started", szArguments );
                    }
                    else
                    {
                        snprintf ( szResponse, 256, "start: Resource '%s' start was requested", szArguments );
                    }
                }
                else
                    snprintf ( szResponse, 256, "start: Resource is already running" );
            }
            else
                snprintf ( szResponse, 256, "start: Resource is loaded, but has errors" );
        }
        else
            snprintf ( szResponse, 256, "start: Resource could not be found" );
    }
    else
        snprintf ( szResponse, 256, "* Syntax: start <resource-name>" );

    szResponse[255] = '\0';
    pEchoClient->SendConsole ( szResponse );
    return true;
}

bool CConsoleCommands::RestartResource ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    if ( szArguments && szArguments[0] )
    {
        CResource * resource = g_pGame->GetResourceManager()->GetResource ( (char *)szArguments );
        if ( resource )
        {
            if ( pClient->GetNick () )
                CLogger::LogPrintf ( "restart: Requested by %s\n", GetAdminNameForLog ( pClient ).c_str () );

            if ( resource->IsLoaded() )
            {
                if ( resource->IsActive() )
                {
                    if ( resource->IsProtected() )
                    {
                        if ( !g_pGame->GetACLManager()->CanObjectUseRight ( pClient->GetNick(), CAccessControlListGroupObject::OBJECT_TYPE_USER, "restart.protected", CAccessControlListRight::RIGHT_TYPE_COMMAND, false ) )
                        {
                            pEchoClient->SendConsole ( "restart: Resource could not be restarted as it is protected" );
                            return false;
                        }
                    }

                    g_pGame->GetResourceManager()->QueueResource ( resource, CResourceManager::QUEUE_RESTART, NULL );
                    pEchoClient->SendConsole ( "restart: Resource restarting..." );
                }
                else
                    pEchoClient->SendConsole ( "restart: Resource is not running" );
            }
            else
                pEchoClient->SendConsole ( "restart: Resource is loaded, but has errors" );
        }
        else
            pEchoClient->SendConsole ( "restart: Resource could not be found" );
        return true;
    }
    else
        pEchoClient->SendConsole ( "* Syntax: restart <resource-name>" );
    return false;
}

bool CConsoleCommands::RefreshResources ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    g_pGame->GetResourceManager()->Refresh();
    return true;
}

bool CConsoleCommands::RefreshAllResources ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    g_pGame->GetResourceManager()->Refresh(true);
    return true;
}

bool CConsoleCommands::ListResources ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    g_pGame->GetResourceManager()->ListResourcesLoaded();
    return true;
}

bool CConsoleCommands::ResourceInfo ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    if ( szArguments && szArguments[0] )
    {
        CResource * resource = g_pGame->GetResourceManager()->GetResource ( (char *)szArguments );
        if ( resource )
        {
            resource->DisplayInfo();
        }
        else
            pEchoClient->SendConsole ( "info: Resource was not found" );
        return true;
    }
    return false;
}

bool CConsoleCommands::StopResource ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    if ( szArguments && szArguments[0] )
    {
        CResource * resource = g_pGame->GetResourceManager()->GetResource ( (char *)szArguments );
        if ( resource )
        {
            if ( pClient->GetNick () )
                CLogger::LogPrintf ( "stop: Requested by %s\n", GetAdminNameForLog ( pClient ).c_str () );

            if ( resource->IsLoaded() )
            {
                if ( resource->IsActive() )
                {
                    if ( resource->IsProtected() )
                    {
                        if ( !g_pGame->GetACLManager()->CanObjectUseRight ( pClient->GetNick(), CAccessControlListGroupObject::OBJECT_TYPE_USER, "stop.protected", CAccessControlListRight::RIGHT_TYPE_COMMAND, false ) )
                        {
                            pEchoClient->SendConsole ( "stop: Resource could not be stopped as it is protected" );
                            return false;
                        }
                    }

                    g_pGame->GetResourceManager ()->QueueResource ( resource, CResourceManager::QUEUE_STOP, NULL );
                    pEchoClient->SendConsole ( "stop: Resource stopping" );
                }
                else
                    pEchoClient->SendConsole ( "stop: Resource is not running" );
            }
            else
                pEchoClient->SendConsole( "stop: Resource is loaded, but has errors" );
        }
        else
            pEchoClient->SendConsole ( "stop: Resource could not be found" );
        return true;
    }
    else
        pEchoClient->SendConsole ( "* Syntax: stop <resource-name>" );

    return false;
}


bool CConsoleCommands::StopAllResources ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    if ( !g_pGame->GetACLManager()->CanObjectUseRight ( pClient->GetNick(), CAccessControlListGroupObject::OBJECT_TYPE_USER, "stopall", CAccessControlListRight::RIGHT_TYPE_COMMAND, false ) )
    {
        pEchoClient->SendConsole ( "stopall: You do not have sufficient rights to stop all the resources." );
        return false;
    }

    g_pGame->GetResourceManager()->QueueResource ( NULL, CResourceManager::QUEUE_STOPALL, NULL );
    pEchoClient->SendConsole ( "stopall: Stopping all resources" );
    return true;
}


bool CConsoleCommands::InstallResource ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    char szBuffer[256];
    szBuffer[0] = '\0';

    if ( strlen ( szBuffer ) < 1  )
        return false;

    strncpy ( szBuffer, szArguments, 256 );
    szBuffer[255] = '\0';

    char* szURL = strtok ( szBuffer, " " );
    char* szName = strtok ( NULL, "\0" );
    if ( strlen ( szURL ) < 150 && strlen ( szURL ) > 1 )
    {
        if ( szName && strncmp(szURL, "http://", 7 ) == 0 )
        {
            if ( strlen ( szName ) < 100 && strlen ( szName ) > 1 )
            {
                if ( g_pGame->GetResourceManager()->Install ( szURL, szName ) )
                {
                    char szOutput[512];
                    snprintf ( szOutput, 511, "Resource %s from %s installed succesfully.", szName, szURL );
                    pEchoClient->SendConsole ( szOutput );
                    g_pGame->GetResourceManager()->Refresh();
                }
            }
        }
        else
        {
            char szNewURL[250];
            szNewURL[249] = '\0';
            snprintf ( szNewURL, 249, "http://development.mtasa.com/%s.zip", szURL );
            if ( g_pGame->GetResourceManager()->Install ( szNewURL, szURL ) )
            {
                char szOutput[512];
                snprintf ( szOutput, 511, "Resource %s installed succesfully.", szURL );
                pEchoClient->SendConsole ( szOutput );
                g_pGame->GetResourceManager()->Refresh();
            }
        }
    }
    return true;
}

bool CConsoleCommands::UpgradeResources ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    g_pGame->GetResourceManager()->Upgrade();
    return true;
}


bool CConsoleCommands::Say ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // say <text>

    // If he isn't muted
    if ( pClient->GetClientType () != CClient::CLIENT_PLAYER || !static_cast < CPlayer* > ( pClient )->IsMuted () )
    {
        // Strip any unwanted characters
        stripControlCodes ( const_cast < char* > ( szArguments ) );

        // Got a message?
        if ( szArguments )
        {
            // Long/short enough?
            size_t sizeArguments = MbUTF8ToUTF16(szArguments).size();

            if ( sizeArguments >= MIN_CHAT_LENGTH && sizeArguments <= MAX_CHAT_LENGTH )
            {
                // Grab the nick
                const char* szNick = pClient->GetNick ();

                if ( szNick )
                {
                    SString strEcho;
                    char szEcho [MAX_CHATECHO_LENGTH];
                    szEcho[0] = '\0';

                    switch ( pClient->GetClientType () )
                    {
                        case CClient::CLIENT_PLAYER:
                        {
                            strEcho.Format("%s: #EBDDB2%s", szNick, szArguments);

                            // Send the chat message and player pointer to the script
                            CLuaArguments Arguments;
                            Arguments.PushString ( szArguments );
                            Arguments.PushNumber ( 0 ); // Normal chat
                            bool bContinue = static_cast < CPlayer* > ( pClient )->CallEvent ( "onPlayerChat", Arguments );
                            if ( bContinue )
                            {
                                // Log it in the console
                                CLogger::LogPrintf ( "CHAT: %s: %s\n", szNick, szArguments );

                                unsigned char ucR = 0xFF, ucG = 0xFF, ucB = 0xFF;
                                CPlayer * pPlayer = static_cast < CPlayer* > ( pClient );
                                CTeam * pTeam = pPlayer->GetTeam ();
                                if ( pTeam )
                                {
                                    pTeam->GetColor ( ucR, ucG, ucB );
                                }

                                // Broadcast the message to all clients
                                pConsole->GetPlayerManager ()->BroadcastOnlyJoined ( CChatEchoPacket ( strEcho, ucR, ucG, ucB, true ) );
                            }

                            break;
                        }
                        case CClient::CLIENT_REMOTECLIENT:
                        {
                            // Populate a chat message
                            snprintf ( szEcho, MAX_CHATECHO_LENGTH, "%s: %s", szNick, szArguments );
                            szEcho [MAX_CHATECHO_LENGTH-1] = '\0';

                            // Log it in the console
                            CLogger::LogPrintf ( "ADMINCHAT: %s: %s\n", szNick, szArguments );

                            // Broadcast the message to all clients
                            pConsole->GetPlayerManager ()->BroadcastOnlyJoined ( CChatEchoPacket ( szEcho, CHATCOLOR_ADMINSAY ) );
                            break;
                        }
                        case CClient::CLIENT_CONSOLE:
                        {
                            // Populate a chat message
                            snprintf ( szEcho, MAX_CHATECHO_LENGTH, "%s: %s", szNick, szArguments );
                            szEcho[MAX_CHATECHO_LENGTH-1] = '\0';

                            // Log it in the console
                            CLogger::LogPrintf ( "CONSOLECHAT: %s\n", szArguments );

                            // Broadcast the message to all clients
                            pConsole->GetPlayerManager ()->BroadcastOnlyJoined ( CChatEchoPacket ( szEcho, CHATCOLOR_CONSOLESAY ) );
                            break;
                        }
                        case CClient::CLIENT_SCRIPT:
                        {
                            // Populate a chat message
                            snprintf ( szEcho, MAX_CHATECHO_LENGTH, "%s: %s", szNick, szArguments );
                            szEcho[MAX_CHATECHO_LENGTH-1] = '\0';

                            // Log it in the console
                            CLogger::LogPrintf ( "SCRIPTCHAT: %s\n", szEcho );

                            // Broadcast the message to all clients
                            pConsole->GetPlayerManager ()->BroadcastOnlyJoined ( CChatEchoPacket ( szEcho, CHATCOLOR_SAY ) );
                            break;
                        }
                        default: break;
                    }

                    // Success
                    return true;
                }
            }
            else
            {
                pEchoClient->SendEcho ( "say: Invalid text length" );
            }
        }
        else
        {
            //pEchoClient->SendEcho ( "say: Syntax is 'say <text>'" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "say: You are muted" );
    }

    return false;
}


bool CConsoleCommands::TeamSay ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // teamsay <text>

    // If its a player (only players have teams)
    if ( pClient->GetClientType () == CClient::CLIENT_PLAYER )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pClient );

        // If he isn't muted
        if ( !pPlayer->IsMuted () )
        {
            // If he's on a team
            CTeam* pTeam = pPlayer->GetTeam ();
            if ( pTeam )
            {
                // Strip any unwanted characters
                stripControlCodes ( const_cast < char* > ( szArguments ) );

                // Got a message?
                if ( szArguments )
                {
                    // Long/short enough?
                    size_t sizeArguments = MbUTF8ToUTF16(szArguments).size();

                    if ( sizeArguments >= MIN_CHAT_LENGTH && sizeArguments <= MAX_CHAT_LENGTH )
                    {
                        // Grab the nick
                        const char* szNick = pClient->GetNick ();

                        if ( szNick )
                        {
                            SString strEcho;

                            // Populate a chat message
                            strEcho.Format("(TEAM) %s: #EBDDB2%s", szNick, szArguments);

                            // Send the chat message and player pointer to the script
                            CLuaArguments Arguments;
                            Arguments.PushString ( szArguments );
                            Arguments.PushNumber ( 2 ); // Team chat
                            bool bContinue = static_cast < CPlayer* > ( pClient )->CallEvent ( "onPlayerChat", Arguments );
                            if ( bContinue )
                            {
                                // Log it in the console
                                CLogger::LogPrintf ( "TEAMCHAT: %s: %s\n", szNick, szArguments );

                                unsigned char ucRed = 0xFF, ucGreen = 0xFF, ucBlue = 0xFF;
                                pTeam->GetColor ( ucRed, ucGreen, ucBlue );
                                // Broadcast to all the team members
                                list < CPlayer* > ::const_iterator iter = pTeam->PlayersBegin ();
                                for ( ; iter != pTeam->PlayersEnd (); iter++ )
                                {
                                    (*iter)->Send ( CChatEchoPacket ( strEcho, ucRed, ucGreen, ucBlue, true ) );
                                }
                            }

                            // Success
                            return true;
                        }
                    }
                    else
                    {
                        pEchoClient->SendEcho ( "teamsay: Invalid text length" );
                    }
                }
            }
            else
            {
                //pEchoClient->SendEcho ( "teamsay: You aren't on a team" );
            }
        }
        else
        {
            pEchoClient->SendEcho ( "teamsay: You are muted" );
        }
    }
    else
    {
        //pEchoClient->SendEcho ( "teamsay: Only players have teams" );
    }

    return false;
}


bool CConsoleCommands::ASay ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // asay <text>

    // If he isn't muted
    if ( pClient->GetClientType () != CClient::CLIENT_PLAYER || !static_cast < CPlayer* > ( pClient )->IsMuted () )
    {
        // Strip any unwanted characters
        stripControlCodes ( const_cast < char* > ( szArguments ) );

        // Got a message?
        if ( szArguments )
        {
            // Long/short enough?
            size_t sizeArguments = MbUTF8ToUTF16(szArguments).size();

            if ( sizeArguments >= MIN_CHAT_LENGTH && sizeArguments <= MAX_CHAT_LENGTH )
            {
                // Grab the nick
                const char* szNick = pClient->GetNick ();

                if ( szNick )
                {
                    char szEcho [MAX_CHATECHO_LENGTH];
                    szEcho[0] = '\0';

                    // Why are we using an empty string in all the log functions below?
                    switch ( pClient->GetClientType () )
                    {
                        case CClient::CLIENT_PLAYER:
                        {
                            // Log it in the console
                            CLogger::LogPrintf ( "CHAT_TO_ALL_ADMINS: %s\n", szEcho );

                            // Send a message back to the player
                            pEchoClient->SendEcho ( "asay: Message sent to all Admins" );
                            break;
                        }
                        case CClient::CLIENT_REMOTECLIENT:
                        {
                            // Log it in the console
                            CLogger::LogPrintf ( "ADMINCHAT_TO_ALL_ADMINS: %s: %s\n", szNick, szArguments );
                             break;
                        }
                        case CClient::CLIENT_CONSOLE:
                        {
                            // Log it in the console
                            CLogger::LogPrintf ( "CONSOLECHAT_TO_ALL_ADMINS: %s\n", szArguments );
                            break;
                        }
                        case CClient::CLIENT_SCRIPT:
                        {
                            // Log it in the console
                            CLogger::LogPrintf ( "SCRIPTCHAT_TO_ALL_ADMINS: %s\n", szEcho );
                            break;
                        }
                        default: break;
                    }

                    // Success
                    return true;
                }
            }
            else
            {
                pEchoClient->SendEcho ( "asay: Invalid text length" );
            }
        }
        else
        {
            //pEchoClient->SendEcho ( "asay: Syntax is 'asay <text>'" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "asay: You are muted" );
    }

    return false;
}


bool CConsoleCommands::Msg ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // msg <player> <text>

    // If he isn't muted
    if ( pClient->GetClientType () != CClient::CLIENT_PLAYER || !static_cast < CPlayer* > ( pClient )->IsMuted () )
    {
        // Strip any unwanted characters
        stripControlCodes ( const_cast < char* > ( szArguments ) );

        // Got a message?
        if ( szArguments )
        {
            char szBuffer[256];
            szBuffer[0] = '\0';

            strncpy ( szBuffer, szArguments, 256 );
            szBuffer[255] = '\0';

            char* szPlayer = strtok ( szBuffer, " " );
            char* szMessage = strtok ( NULL, "\0" );

            if ( szPlayer )
            {
                CPlayer* pPlayer = pConsole->GetPlayerManager ()->Get ( szPlayer );

                if ( pPlayer )
                {
                    if ( szMessage )
                    {
                        // Long/short enough?
                        size_t sizeMessage = MbUTF8ToUTF16(szArguments).size();

                        if ( sizeMessage >= MIN_CHAT_LENGTH && sizeMessage <= MAX_CHAT_LENGTH )
                        {
                            // Grab the nick
                            const char* szNick = pClient->GetNick ();

                            if ( szNick )
                            {
                                // Populate the message to the player
                                SString strMessage("* PM from %s: %s", szNick, szMessage);

                                switch ( pClient->GetClientType () )
                                {
                                    case CClient::CLIENT_PLAYER:
                                    {
                                        CPlayer* pSender = static_cast < CPlayer* > ( pClient );

                                        if ( pPlayer == pSender )
                                        {
                                            pEchoClient->SendEcho ( "msg: You cannot message yourself" );
                                            return false;
                                        }

                                        // Log it
                                        CLogger::LogPrintf ( "MSG: %s to %s: %s\n", szNick, pPlayer->GetNick (), szMessage );

                                        // Send the message and player pointer to the script
                                        CLuaArguments Arguments;
                                        Arguments.PushString ( szArguments );
                                        Arguments.PushElement ( pPlayer );
                                        bool bContinue = pSender->CallEvent ( "onPlayerPrivateMessage", Arguments );
                                        if ( bContinue )
                                        {
                                            // Send it to the player
                                            pPlayer->Send ( CChatEchoPacket ( strMessage, CHATCOLOR_INFO ) );

                                            // Send a reponse to the player who sent it
                                            pEchoClient->SendEcho ( SString ( "-> %s: %s", pPlayer->GetNick (), szMessage ) );
                                        }
                                        break;
                                    }
                                    case CClient::CLIENT_CONSOLE:
                                    {
                                        // Log it
                                        CLogger::LogPrintf ( "CONSOLEMSG: %s to %s: %s\n", szNick, pPlayer->GetNick (), szMessage );

                                        // Send it to the player
                                        pPlayer->Send ( CChatEchoPacket ( strMessage, CHATCOLOR_INFO ) );
                                    }
                                    case CClient::CLIENT_SCRIPT:
                                    {
                                        // Log it
                                        CLogger::LogPrintf ( "SCRIPTMSG: %s to %s: %s\n", szNick, pPlayer->GetNick (), szMessage );

                                        // Send it to the player
                                        pPlayer->Send ( CChatEchoPacket ( strMessage, CHATCOLOR_INFO ) );
                                        break;
                                    }
                                    default: break;
                                }

                                // Success
                                return true;
                            }
                        }
                        else
                        {
                            pEchoClient->SendEcho ( "msg: Invalid message length" );
                        }
                    }
                    else
                    {
                        pEchoClient->SendEcho ( "msg: Invalid message" );
                    }
                }
                else
                {
                    pEchoClient->SendEcho ( "msg: Player not found" );
                }
            }
            else
            {
                pEchoClient->SendEcho ( "msg: Player not found" );
            }
        }
        else
        {
            //pEchoClient->SendEcho ( "msg: Syntax is 'msg <nick> <msg>'" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "msg: You are muted" );
    }
    return false;
}


bool CConsoleCommands::AMsg ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // amsg <admin> <text>

    /*
    // If he isn't muted
    if ( pClient->GetClientType () != CClient::CLIENT_PLAYER || !static_cast < CPlayer* > ( pClient )->IsMuted () )
    {
        // Strip any unwanted characters
        stripString ( const_cast < char* > ( szArguments ) );

        // Got a message?
        if ( szArguments )
        {
            char szBuffer[256];
            szBuffer[0] = '\0';

            strncpy ( szBuffer, szArguments, 256 );
            szBuffer[255] = '\0';

            char* szAdmin = strtok ( szBuffer, " " );
            char* szMessage = strtok ( NULL, "\0" );

            if ( szAdmin )
            {
                CAdminManager* pAdminManager = g_pGame->GetAdminManager ();

                if ( pAdminManager )
                {
                    CAdmin* pAdmin = pAdminManager->Get ( szAdmin );

                    if ( pAdmin )
                    {
                        // Send it to all admins of that name
                        if ( pAdminManager )
                        {
                            list < CAdmin* > ::const_iterator iter = pAdminManager->IterBegin ();
                            for ( ; iter != pAdminManager->IterEnd (); iter++ )
                            {
                                if ( strcmp ( szAdmin, (*iter)->GetNick () ) == 0 && *iter != static_cast < CAdmin* > ( pClient ) )
                                {
                                    (*iter)->AnnounceMessage ( pClient, pAdmin, szMessage );
                                }
                            }
                        }

                        // Announce
                        switch ( pClient->GetClientType () )
                        {
                            case CClient::CLIENT_PLAYER:
                            {
                                // Log it
                                CLogger::LogPrintf ( "MSG_TO_ADMIN: %s to %s: %s\n", pClient->GetNick (), szAdmin, szMessage );

                                // Populate the response to the player
                                char szMsg[256];
                                szMsg[0] = '\0';

                                sprintf ( szMsg, "amsg: Message sent to all Admin's named '%s'", szAdmin );
                                szMsg[255] = '\0';

                                // Send response back to the player who sent it
                                pEchoClient->SendEcho ( szMsg );
                                break;
                            }
                            case CClient::CLIENT_REMOTECLIENT:
                            {
                                // Log it
                                CLogger::LogPrintf ( "ADMINMSG_TO_ADMIN: %s to %s: %s\n", pClient->GetNick (), szAdmin, szMessage );

                                static_cast < CAdmin* > ( pClient )->AnnounceMessage ( pClient, pAdmin, szMessage );
                                break;
                            }
                            case CClient::CLIENT_CONSOLE:
                            {
                                // Log it
                                CLogger::LogPrintf ( "CONSOLEMSG_TO_ADMIN: %s to %s: %s\n", pClient->GetNick (), szAdmin, szMessage );
                                break;
                            }
                            case CClient::CLIENT_SCRIPT:
                            {
                                // Log it
                                CLogger::LogPrintf ( "SCRIPTMSG_TO_ADMIN: %s to %s: %s\n", pClient->GetNick (), szAdmin, szMessage );
                                break;
                            }
                            default: break;
                        }
                    }
                    else
                    {
                        pEchoClient->SendEcho ( "amsg: Admin not found" );
                    }
                }
                else
                {
                    pEchoClient->SendEcho ( "amsg: Admin not found" );
                }
            }
            else
            {
                pEchoClient->SendEcho ( "amsg: Admin not found" );
            }
        }
        else
        {
            //pEchoClient->SendEcho ( "amsg: Syntax is 'amsg <admin> <message>'" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "amsg: You are muted" );
    }
    return false;
    */

    return false;
}


bool CConsoleCommands::Me ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // me <text>

    // If he isn't muted
    if ( pClient->GetClientType () != CClient::CLIENT_PLAYER || !static_cast < CPlayer* > ( pClient )->IsMuted () )
    {
        // Got a message?
        if ( szArguments )
        {
            // Long/short enough?
            size_t sizeArguments = MbUTF8ToUTF16(szArguments).size();

            if ( sizeArguments >= MIN_CHAT_LENGTH && sizeArguments <= MAX_CHAT_LENGTH )
            {
                // Grab the nick
                const char* szNick = pClient->GetNick ();

                if ( szNick )
                {
                    // Populate a chat message depending on if it starts on /me or not
                    SString strEcho("* %s %s", szNick, szArguments);

                    // Send the chat message and player pointer to the script IF the client is a player
                    if ( pClient->GetClientType () == CClient::CLIENT_PLAYER )
                    {
                        CLuaArguments Arguments;
                        Arguments.PushString ( szArguments );       // text
                        Arguments.PushNumber ( 1 );                 // Me chat
                        bool bContinue = static_cast < CPlayer* > ( pClient )->CallEvent ( "onPlayerChat", Arguments );
                        if ( bContinue )
                        {
                            // Log it in the console
                            CLogger::LogPrintf ( "CHAT: %s\n", strEcho.c_str () );

                            // Broadcast the message to all clients
                            pConsole->GetPlayerManager ()->BroadcastOnlyJoined ( CChatEchoPacket ( strEcho, CHATCOLOR_ME ) );
                        }
                    }

                    // Success
                    return true;
                }
            }
        }
    }
    else
    {
        pEchoClient->SendEcho ( "me: You are muted" );
    }

    return false;
}


bool CConsoleCommands::Nick ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // nick <name>

    // Only players can change their nicks
    if ( pClient->GetClientType () == CClient::CLIENT_PLAYER )
    {
        // Got a nickname?
        if ( szArguments )
        {
            // Copy the nickname
            char szNewNick [64];
            szNewNick[0] = '\0';

            strncpy ( szNewNick, szArguments, 64 );
            szNewNick[63] = '\0';

            // Check its validity
            if ( IsNickValid ( szNewNick ) )
            {
                if ( CheckNickProvided ( szNewNick ) )
                {
                    // Verify the length
                    size_t sizeNewNick = strlen ( szNewNick );
                    if ( sizeNewNick >= MIN_NICK_LENGTH && sizeNewNick <= MAX_NICK_LENGTH )
                    {
                        // Does the nickname differ from the previous nickname?
                        const char* szNick = pClient->GetNick ();
                        if ( !szNick || strcmp ( szNewNick, szNick ) != 0 )
                        {
                            // Check that it doesn't already exist, or if it matches our current nick case-independantly (means we changed to the same nick but in a different case)
                            if ( (szNick && stricmp ( szNick, szNewNick ) == 0) || !pConsole->GetPlayerManager ()->Get ( szNewNick ) )
                            {
                                CPlayer* pPlayer = static_cast < CPlayer* > ( pClient );

                                // Call the event
                                CLuaArguments Arguments;
                                Arguments.PushString ( pClient->GetNick () );
                                Arguments.PushString ( szNewNick );
                                if ( pPlayer->CallEvent ( "onPlayerChangeNick", Arguments ) )
                                {
                                    // Tell the console
                                    CLogger::LogPrintf ( "NICK: %s is now known as %s\n", szNick, szNewNick );

                                    // Change the nick
                                    pPlayer->SetNick ( szNewNick );

                                    // Tell all ingame players about the nick change
                                    CPlayerChangeNickPacket Packet ( szNewNick );
                                    Packet.SetSourceElement ( pPlayer );
                                    pConsole->GetPlayerManager ()->BroadcastOnlyJoined ( Packet );

                                    return true;
                                }
                                else
                                    return false;
                            }
                            else
                            {
                                pEchoClient->SendEcho ( "nick: Chosen nickname is already in use" );
                            }
                        }
                        else
                        {
                            // Tell the player
                            char szBuffer [128];
                            szBuffer[0] = '\0';

                            snprintf ( szBuffer, 128, "nick: Nickname is already %s", szNick );
                            szBuffer[127] = '\0';

                            pEchoClient->SendEcho ( szBuffer );
                        }
                    }
                    else
                    {
                        // Tell the player
                        char szBuffer [64];
                        szBuffer[0] = '\0';

                        snprintf ( szBuffer, 64, "nick: Nick must be between %u and %u characters", MIN_NICK_LENGTH, MAX_NICK_LENGTH );
                        szBuffer[63] = '\0';

                        pEchoClient->SendEcho ( szBuffer );
                    }
                }
                else
                {
                    pEchoClient->SendEcho ( "nick: Chosen nickname is not allowed" );
                }
            }
            else
            {
                pEchoClient->SendEcho ( "nick: Chosen nickname contains illegal characters" );
            }
        }
        else
        {
            //pEchoClient->SendEcho ( "nick: Syntax is 'nick <name>'" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "nick: Only players can change their nick" );
    }

    return false;
}


// HACKED IN FOR NOW
#include "CGame.h"
extern CGame* g_pGame;

bool CConsoleCommands::LogIn ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // login [<name>] <pass>

    // Grab the sender's nick
    char* szNick = const_cast < char* > ( pClient->GetNick () );
    char* szPassword = const_cast < char* > ( szArguments );

    // Got any arguments?
    if ( szArguments && szArguments [ 0 ] )
    {
        char szTemp [ 256 ];
        szTemp[0] = '\0';
        strncpy ( szTemp, szArguments, 256 );
        szTemp [ 255 ] = 0;

        char* szTempNick = strtok ( szTemp, " " );
        char* szTempPassword = strtok ( NULL, "\0" );
        if ( szTempPassword )
        {
            szNick = szTempNick;
            szPassword = szTempPassword;
        }

        if ( szNick && szPassword )
        {
            return g_pGame->GetAccountManager ()->LogIn ( pClient, pEchoClient, szNick, szPassword );
        }
        else
        {
            if ( pEchoClient )
                pEchoClient->SendEcho ( "login: Syntax is 'login [<nick>] <password>'" );
        }
    }
    else
    {
        if ( pEchoClient )
            pEchoClient->SendEcho ( "login: Syntax is 'login [<nick>] <password>'" );
    }

    return false;
}


bool CConsoleCommands::LogOut ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // logout
    return g_pGame->GetAccountManager ()->LogOut ( pClient, pEchoClient );
}


bool CConsoleCommands::ChgMyPass ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // chgmypass <oldpass> <newpass>
    // User must be logged in with the account password is changed for

    // Got any arguments?
    if ( szArguments )
    {
        // Is the user logged in?
        if ( pClient->IsRegistered () )
        {
            // Copy the command
            char szBuffer [256];
            strncpy ( szBuffer, szArguments, 256 );
            szBuffer [255] = 0;

            // Split it up into nick and password
            char* szOldPassword = strtok ( szBuffer, " " );
            char* szNewPassword = strtok ( NULL, "\0" );
            if ( szOldPassword && szNewPassword && strlen ( szOldPassword ) > 0 && strlen ( szNewPassword ) > 0 )
            {
                // Grab the account with that nick
                CAccount* pAccount = pClient->GetAccount ();
                if ( pAccount )
                {
                    CMD5Hasher Hasher;
                    MD5 oldPasswordHash;
                    char szOldPasswordHash[128];
                    Hasher.Calculate ( szOldPassword, strlen ( szOldPassword ), oldPasswordHash );
                    Hasher.ConvertToHex ( oldPasswordHash, szOldPasswordHash );
                    // Compare the hash of the given oldPassword to the current password hash
                    if ( stricmp ( pAccount->GetPassword ().c_str (), szOldPasswordHash ) == 0 )
                    {
                        // Old and new password are equal?
                        if ( strcmp ( szOldPassword, szNewPassword ) != 0 )
                        {
                            MD5 newPasswordHash;
                            char szNewPasswordHash[128];
                            Hasher.Calculate ( szNewPassword, strlen ( szNewPassword ), newPasswordHash );
                            Hasher.ConvertToHex ( newPasswordHash, szNewPasswordHash );
                            
                            // Set the new password
                            pAccount->SetPassword ( szNewPassword );

                            // Tell the client
                            char szMessage [128];
                            snprintf ( szMessage, 128, "chgmypass: Your password was changed to '%s'", szNewPassword );
                            szMessage[127] = '\0';

                            pEchoClient->SendEcho ( szMessage );
                            CLogger::LogPrintf ( "ACCOUNTS: %s changed their account password", GetAdminNameForLog ( pClient ).c_str () );
                            return true;
                        }
                        else
                        {
                            pEchoClient->SendEcho ( "chgmypass: Your password is already that" );
                        }
                    }
                    else
                    {
                        pEchoClient->SendEcho ( "chgmypass: Bad old password" );
                        CLogger::LogPrintf ( "ACCOUNTS: %s failed to change their account password", GetAdminNameForLog ( pClient ).c_str () );
                   }
                }
                else
                {
                    pEchoClient->SendEcho ( "chgmypass: The account you were logged in as no longer exists" );
                }
            }
            else
            {
                pEchoClient->SendEcho ( "chgmypass: Syntax is 'chgmypass <oldpass> <newpass>'" );
            }
        }
        else
        {
            pEchoClient->SendEcho ( "chgmypass: You must be logged in to use this command" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "chgmypass: Syntax is 'chgmypass <oldpass> <newpass>'" );
    }

    return false;
}


bool CConsoleCommands::AddAccount ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // addaccount <nick> <password>

    // Got any arguments?
    if ( szArguments )
    {
        // Copy the argument buffer
        char szBuffer [256];
        szBuffer[0] = '\0';

        strncpy ( szBuffer, szArguments, 256 );
        szBuffer [255] = 0;

        // Split it into nick and reason
        char* szNick = strtok ( szBuffer, " " );
        char* szPassword = strtok ( NULL, " " );

        // Check that we got everything
        if ( szNick && szPassword )
        {
            // Long enough strings?
            if ( strlen ( szNick ) > 0 && strlen ( szPassword ) > MIN_PASSWORD_LENGTH && strlen ( szPassword ) <= MAX_PASSWORD_LENGTH )
            {
                // Try creating the account
                if ( !g_pGame->GetAccountManager ()->Get ( szNick ) )
                {
                    CAccount* pAccount = new CAccount ( g_pGame->GetAccountManager (), true, szNick );
                    pAccount->SetPassword ( szPassword );
                    g_pGame->GetAccountManager ()->Register( pAccount );

                    // Tell the user
                    char szMessage [128];
                    szMessage[0] = '\0';

                    snprintf ( szMessage, 128, "addaccount: Added account '%s' with password '%s'", szNick, szPassword );
                    szMessage[127] = '\0';

                    pClient->SendEcho ( szMessage );

                    // Tell the console
                    CLogger::LogPrintf ( "ACCOUNTS: %s added account '%s' with password '%s'\n", GetAdminNameForLog ( pClient ).c_str (), szNick, szPassword );
                    return true;
                }
                else
                {
                    pEchoClient->SendEcho ( "addaccount: Already an account with that name" );
                }
            }
            else
            {
                pEchoClient->SendEcho ( "addaccount: Syntax is 'addaccount <nick> <password>'" );
            }
        }
        else
        {
            pEchoClient->SendEcho ( "addaccount: Syntax is 'addaccount <nick> <password>'" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "addaccount: Syntax is 'addaccount <nick> <password>'" );
    }

    return false;
}


bool CConsoleCommands::DelAccount ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // delaccount <nick>

    char* szNick = const_cast < char* > ( szArguments );

    // Got any arguments?
    if ( szArguments )
    {
        // Grab the account with that nick
        CAccount* pAccount = g_pGame->GetAccountManager ()->Get ( szNick );
        if ( pAccount )
        {
            CClient *pAccountClient = pAccount->GetClient ();

            if ( pAccountClient )
            {
                g_pGame->GetAccountManager ()->LogOut ( pAccountClient, NULL );

                char szClientMessage [128];
                szClientMessage[0] = '\0';

                snprintf ( szClientMessage, 128, "logout: You were logged out of account '%s' due to it being deleted", szArguments );
                szClientMessage[127] = '\0';

                pAccountClient->SendEcho ( szClientMessage );
            }

            // Tell the client
            char szMessage [128];
            szMessage[0] = '\0';

            snprintf ( szMessage, 128, "delaccount: Account '%s' deleted", szArguments );
            szMessage[127] = '\0';

            pEchoClient->SendEcho ( szMessage );

            // Tell the console
            CLogger::LogPrintf ( "ACCOUNTS: %s deleted account '%s'\n", GetAdminNameForLog ( pClient ).c_str (), szArguments );

            // Delete it
            g_pGame->GetAccountManager ()->RemoveAccount ( pAccount );
            delete pAccount;
            return true;
        }
        else
        {
            pEchoClient->SendEcho ( "delaccount: No account with that nick" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "delaccount: Syntax is 'delaccount <nick>'" );
    }

    return false;
}


bool CConsoleCommands::ChgPass ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // chgpass <nick> <password>

    // Got any arguments?
    if ( szArguments )
    {
        // Copy the command
        char szBuffer [256];
        szBuffer[0] = '\0';

        strncpy ( szBuffer, szArguments, 256 );
        szBuffer [255] = 0;

        // Split it up into nick and password
        char* szNick = strtok ( szBuffer, " " );
        char* szPassword = strtok ( NULL, "\0" );
        if ( szNick && szPassword && strlen ( szNick ) > 0 && strlen ( szPassword ) > 0 )
        {
            // Grab the account with that nick
            CAccount* pAccount = g_pGame->GetAccountManager ()->Get ( szNick );
            if ( pAccount )
            {
                // Set the new password
                pAccount->SetPassword ( szPassword );

                // Tell the client
                char szMessage [128];
                szMessage[0] = '\0';

                snprintf ( szMessage, 128, "chgpass: %s's password changed to '%s'", szNick, szPassword );
                szMessage[127] = '\0';

                pEchoClient->SendEcho ( szMessage );

                // Tell the console
                CLogger::LogPrintf ( "ACCOUNTS: %s changed %s's password to '%s'\n", GetAdminNameForLog ( pClient ).c_str (), szNick, szPassword );
                return true;
            }
            else
            {
                pEchoClient->SendEcho ( "chgpass: No account with that nick" );
            }
        }
        else
        {
            pEchoClient->SendEcho ( "chgpass: Syntax is 'chgpass <nick> <pass>'" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "chgpass: Syntax is 'chgpass <nick> <pass>'" );
    }

    return false;
}


bool CConsoleCommands::Shutdown ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // shutdown <reason>

    if ( szArguments && strlen ( szArguments ) > 0 )
    {
        // Copy to a buffer and strip it for bad characters
        char szBuffer [256];
        szBuffer[0] = '\0';
        strncpy ( szBuffer, szArguments, 256 );
        szBuffer [255] = 0;

        // Output the action + reason to the console
        CLogger::LogPrintf ( "SHUTDOWN: Got shutdown command from %s (Reason: %s)\n", GetAdminNameForLog ( pClient ).c_str (), szBuffer );
    }
    else
    {
        // Output the action to the console
        CLogger::LogPrintf ( "SHUTDOWN: Got shutdown command from %s (No reason specified)\n", GetAdminNameForLog ( pClient ).c_str () );
    }

    // Shut the server down asap
    g_pGame->SetIsFinished ( true );
    return true;
}


bool CConsoleCommands::AExec ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // aexec <nick> <command>

    // This command can only be executed by the same client that echos it
    if ( pClient == pEchoClient )
    {
        // Got any arguments?
        if ( szArguments && strlen ( szArguments ) > 0 )
        {
            // Copy the argument buffer
            char szBuffer [256];
            szBuffer[0] = '\0';

            strncpy ( szBuffer, szArguments, 256 );
            szBuffer [255] = 0;

            // Split it into nick and reason
            char* szNick = strtok ( szBuffer, " " );
            char* szCommand = strtok ( NULL, "\0" );

            // Got both?
            if ( szNick && szCommand && strlen ( szCommand ) > 0 )
            {
                // Grab the player with that nick
                CPlayer* pPlayer = pConsole->GetPlayerManager ()->Get ( szNick, false );
                if ( pPlayer && pPlayer->IsJoined () )
                {
                    // Tell the console
                    CLogger::LogPrintf ( "%s used aexec to make %s do '%s'\n", GetAdminNameForLog ( pClient ).c_str (), pPlayer->GetNick (), szCommand );

                    // Execute the command under the player's nick
                    return pConsole->HandleInput ( szCommand, pPlayer, pEchoClient );
                }
                else
                {
                    pEchoClient->SendEcho ( "aexec: No such player" );
                }
            }
            else
            {
                pEchoClient->SendEcho ( "aexec: Syntax is 'aexec <nick> <command>'" );
            }
        }
        else
        {
            pEchoClient->SendEcho ( "aexec: Syntax is 'aexec <nick> <command>'" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "aexec: You can only execute this command for yourself" );
    }

    return false;
}


bool CConsoleCommands::WhoIs ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // Got any arguments?
    if ( szArguments && strlen ( szArguments ) > 0 )
    {
        // Any player requested?
        if ( strcmp ( szArguments, "*" ) == 0 )
        {
            // Iterate the players and echo their IPs and ports if anyone was requested
            unsigned int uiCount = 0;
            CPlayerManager* pPlayerManager = pConsole->GetPlayerManager ();
            CPlayer* pPlayer;
            list < CPlayer* > ::const_iterator iter = pPlayerManager->IterBegin ();
            for ( ; iter != pPlayerManager->IterEnd (); iter++ )
            {
                // Is he joined?
                pPlayer = *iter;
                if ( pPlayer->IsJoined () )
                {
                    // Echo him
                    char szIP [32];
                    szIP[0] = '\0';
                    pPlayer->GetSourceIP ( szIP );

                    // Populate a line about him and send it
                    char szBuffer [256];
                    szBuffer[0] = '\0';

                    snprintf ( szBuffer, 256, "%s - %s:%u", pPlayer->GetNick (), szIP, pPlayer->GetSourcePort () );
                    szBuffer[255] = '\0';
                    pClient->SendEcho ( szBuffer );

                    ++uiCount;
                }
            }

            // No players?
            if ( uiCount == 0 )
            {
                pClient->SendEcho ( "whois: No players connected" );
            }
        }
        else
        {
            // Grab the requested nick
            CPlayer* pPlayer = pConsole->GetPlayerManager ()->Get ( szArguments, false );
            if ( pPlayer && pPlayer->IsJoined () )
            {
                // Echo him
                char szIP [32];
                szIP[0] = '\0';
                pPlayer->GetSourceIP ( szIP );

                // Populate a line about him and send it
                char szBuffer [256];
                szBuffer[0] = '\0';
                snprintf ( szBuffer, 256, "%s - %s:%u", pPlayer->GetNick (), szIP, pPlayer->GetSourcePort () );
                szBuffer[255] = '\0';
                pClient->SendEcho ( szBuffer );
            }
            else
            {
                pClient->SendEcho ( "whois: No such player" );
            }
        }
    }
    else
    {
        pClient->SendEcho ( "whois: Syntax is 'whois <nick>'" );
    }

    return false;
}


bool CConsoleCommands::WhoWas ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // Got any arguments?
    if ( szArguments && strlen ( szArguments ) > 0 )
    {
        // Make lower case
        string strArguments ( szArguments );
        std::transform ( strArguments.begin(), strArguments.end(), strArguments.begin(), ::tolower );

        // Is the nick requested anyone?
        bool bAnyone = ( strArguments == "*" );

        // Start iterating the whowas list
        CWhoWas* pWhoWas = pConsole->GetWhoWas ();
        if ( pWhoWas->Count () > 0 )
        {
            // Start iterating the list
            unsigned int uiCount = 0;
            list < SWhoWasEntry > ::const_iterator iter = pWhoWas->IterBegin ();
            for ( ; iter != pWhoWas->IterEnd (); iter++ )
            {
                // Make player name lower case
                string strNick ( iter->szNick );
                std::transform ( strNick.begin(), strNick.end(), strNick.begin(), ::tolower );

                // Matches?
                if ( bAnyone || strNick.find ( strArguments ) != string::npos )
                {
                    // Haven't got too many entries printed?
                    if ( ++uiCount <= 20 )
                    {
                        // Convert the IP to a string
                        char szIP [32];
                        szIP[0] = '\0';
                        LongToDottedIP ( iter->ulIP, szIP );

                        // Populate a line about him
                        pClient->SendEcho ( SString ( "%s  -  IP:%s  serial:%s  version:%s", iter->szNick, szIP, iter->strSerial.c_str (), iter->strPlayerVersion.c_str () ) );
                    }
                    else
                    {
                        pClient->SendEcho ( "..." );
                        return true;
                    }
                }
            }

            // No matching entries?
            if ( uiCount == 0 )
            {
                pClient->SendEcho ( "whowas: No matching nicks" );
                return false;
            }

            return true;
        }
        else
        {
            pClient->SendEcho ( "whowas: The whowas list is empty" );
        }
    }
    else
    {
        pClient->SendEcho ( "whowas: Syntax is 'whowas <nick>'" );
    }

    return false;
}


bool CConsoleCommands::DebugScript ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // Valid parameter?
    if ( szArguments && szArguments [0] != 0 && szArguments [1] == 0 )
    {
        // Player?
        if ( pClient->GetClientType () == CClient::CLIENT_PLAYER )
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pClient );

            // Convert to number
            int iLevel = atoi ( szArguments );
            if ( iLevel != (int)pPlayer->GetScriptDebugLevel () )
            {
                // Between 0 and 3?
                if ( iLevel >= 0 && iLevel <= 3 )
                {
                    // Set the new level
                    pPlayer->SetScriptDebugLevel ( iLevel );

                    // Tell the player and the console
                    pEchoClient->SendEcho ( SString ( "debugscript: Your debug mode was set to %i", iLevel ) );
                    CLogger::LogPrintf ( "SCRIPT: %s set his script debug mode to %i\n", GetAdminNameForLog ( pClient ).c_str (), iLevel );

                    // Enable/Disable their debugger
                    if ( iLevel == 0 )
                        CStaticFunctionDefinitions::SetPlayerDebuggerVisible ( pPlayer, false );
                    else
                        CStaticFunctionDefinitions::SetPlayerDebuggerVisible ( pPlayer, true );
                }
                else
                {
                    pEchoClient->SendEcho ( "debugscript: Modes available are 0 (None), 1 (Errors), 2 (Errors + Warnings), 3 (All)" );
                }
            }
            else
            {
                pEchoClient->SendEcho ( "debugscript: Your debug mode is already that" );
            }
        }
        else
        {
            pEchoClient->SendConsole ( "debugscript: Incorrect client type for this command" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "debugscript: Syntax is 'debugscript <mode>'" );
    }

    return false;
}


bool CConsoleCommands::Sudo ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // sudo <command>

    // Grab the nick
    char* szNick = const_cast < char* > ( pClient->GetNick () );

    // Got any arguments?
    if ( szArguments && szArguments [ 0 ] )
    {
        char szTemp [ 256 ];
        szTemp[0] = '\0';

        strncpy ( szTemp, szArguments, 256 );
        szTemp [ 255 ] = 0;

        char* szFunction = strtok ( szTemp, " " );

        if ( szFunction && szNick )
        {
            // do something
        }
        else
        {
            pEchoClient->SendEcho ( "sudo: Syntax is 'sudo <command>'" );
        }
    }
    else
    {
        pEchoClient->SendEcho ( "sudo: Syntax is 'sudo <command>'" );
    }

    return false;
}
/* idiot.
#include <iomanip>
#include <sstream>
#include <iostream>
*/

bool CConsoleCommands::Help ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    // Help string
    std::string strHelpText = "Available commands:\n\n";

    // Loop through all added commands
    int iCount = 0;
    list < CConsoleCommand* > ::const_iterator iter = pConsole->CommandsBegin ();
    for ( ; iter != pConsole->CommandsEnd (); iter++ )
    {
        // Add a new line every third command
        if ( iCount == 3 )
        {
            iCount = 0;
            strHelpText.append ( "\n" );
        }

        // Add the commandname and pad it to 20 letters with spaces
        const char* szCommand = (*iter)->GetCommand ();
        strHelpText.append ( szCommand );
        strHelpText.append ( 25 - strlen ( szCommand ), ' ' );

        // Increment count so we can keep track of how many we've put at one line
        ++iCount;
    }

    // Show it
    pEchoClient->SendConsole ( strHelpText.c_str () );
    return true;
}


bool CConsoleCommands::LoadModule ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    if ( szArguments && szArguments[0] )
    {
        if ( pClient->GetNick () )
            CLogger::LogPrintf ( "loadmodule: Requested by %s\n", GetAdminNameForLog ( pClient ).c_str () );

        SString strFilename ( "%s/modules/%s", g_pServerInterface->GetModManager ()->GetModPath (), szArguments );

        // These modules are late loaded
        if ( !g_pGame->GetLuaManager ()->GetLuaModuleManager ()->_LoadModule ( szArguments, strFilename, true ) )
        {
                pEchoClient->SendConsole ( "stop: Resource could not be found" );
                return true;
        }
    }
    else
        pEchoClient->SendConsole ( "* Syntax: loadmodule <module-name-with-extension>" );

    return false;
}


bool CConsoleCommands::UnloadModule ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    if ( szArguments && szArguments[0] )
    {
        if ( pClient->GetNick () )
            CLogger::LogPrintf ( "unloadmodule: Requested by %s\n", GetAdminNameForLog ( pClient ).c_str () );

        SString strFilename ( "%s/modules/%s", g_pServerInterface->GetModManager ()->GetModPath (), szArguments );

        /*if ( !g_pGame->GetLuaManager()->GetLuaModuleManager()->_UnloadModule ( szArguments, strFilename ) )
        {
                pEchoClient->SendConsole ( "stop: Resource could not be found" );
                return true;
        }*/
    }
    else
        pEchoClient->SendConsole ( "* Syntax: loadmodule <module-name-with-extension>" );

    return false;
}


bool CConsoleCommands::ReloadModule ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    if ( szArguments && szArguments[0] )
    {
        if ( pClient->GetNick () )
            CLogger::LogPrintf ( "loadmodule: Requested by %s\n", pClient->GetNick () );

        SString strFilename ( "%s/modules/%s", g_pServerInterface->GetModManager ()->GetModPath (), szArguments );

        /*if ( !g_pGame->GetLuaManager()->GetLuaModuleManager()->_ReloadModule ( szArguments, strFilename ) )
        {
                pEchoClient->SendConsole ( "stop: Resource could not be found" );
                return true;
        }*/
    }
    else
        pEchoClient->SendConsole ( "* Syntax: loadmodule <module-name-with-extension>" );

    return false;
}


bool CConsoleCommands::Ver ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    pEchoClient->SendConsole ( MTA_DM_FULL_STRING " v" MTA_DM_BUILDTAG_LONG );
    return true;
}


bool CConsoleCommands::Ase ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    if ( pClient->GetClientType () == CClient::CLIENT_CONSOLE )
    {
        ASE * ase = ASE::GetInstance();
        if ( ase )
            pEchoClient->SendConsole ( SString ( "Master server list queries: %d", ase->GetMasterServerQueryCount () ) );
        return true;
    }
    return false;
}


bool CConsoleCommands::OpenPortsTest ( CConsole* pConsole, const char* szArguments, CClient* pClient, CClient* pEchoClient )
{
    if ( pClient->GetClientType () == CClient::CLIENT_CONSOLE )
    {
        g_pGame->StartOpenPortsTest ();
        return true;
    }
    return false;
}
