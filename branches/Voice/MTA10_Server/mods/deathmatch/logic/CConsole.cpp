/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConsole.cpp
*  PURPOSE:     Console handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CConsole::CConsole ( CBlipManager* pBlipManager, CMapManager* pMapManager, CPlayerManager* pPlayerManager, CRegisteredCommands* pRegisteredCommands, CVehicleManager* pVehicleManager, CLuaManager* pLuaManager, CWhoWas* pWhoWas, CBanManager* pBanManager, CAccessControlListManager* pACLManager )
{
    // Init
    m_pBlipManager = pBlipManager;
    m_pMapManager = pMapManager;
    m_pPlayerManager = pPlayerManager;
    m_pRegisteredCommands = pRegisteredCommands;
    m_pVehicleManager = pVehicleManager;
    m_pLuaManager = pLuaManager;
    m_pWhoWas = pWhoWas;
    m_pBanManager = pBanManager;
    m_pACLManager = pACLManager;
}


CConsole::~CConsole ( void )
{
    // Delete all our commands
    DeleteAllCommands ();
}


bool CConsole::HandleInput ( const char* szCommand, CClient* pClient, CClient* pEchoClient )
{
    // Copy it
    char szCommandBuffer [256];
    szCommandBuffer [255] = 0;
    strncpy ( szCommandBuffer, szCommand, 255 );
    stripControlCodes ( szCommandBuffer );

    // Split it into two parts: Key and argument
    char* szKey = strtok ( szCommandBuffer, " " );
    char* szArguments = strtok ( NULL, "\0" );

    // Does the key exist?
    if ( szKey && szKey [0] != 0 )
    {
        if ( pClient->GetClientType() == CClient::CLIENT_PLAYER )
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pClient );

            CLuaArguments Arguments;
            Arguments.PushString ( szKey );

            if ( !pPlayer->CallEvent ( "onPlayerCommand", Arguments ) )
                return false;
        }

        CConsoleCommand* pCommand = GetCommand ( szKey );
        if ( pCommand )
        {
            // Can this user use this command?
            if ( m_pACLManager->CanObjectUseRight ( pClient->GetAccount ()->GetName ().c_str (),
                                                    CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                    szKey,
                                                    CAccessControlListRight::RIGHT_TYPE_COMMAND,
                                                    !pCommand->IsRestricted () ) )
            {
                return (*pCommand)( this, szArguments, pClient, pEchoClient );
            }

            // Not enough access, tell the console
            CLogger::LogPrintf ( "DENIED: Denied '%s' access to command '%s'\n", pClient->GetNick (), szKey );

            // Tell the client
            char szBuffer [128];
            _snprintf ( szBuffer, sizeof(szBuffer), "ACL: Access denied for '%s'", szKey );
            szBuffer[sizeof(szBuffer)-1] = '\0';

            pClient->SendEcho ( szBuffer );
            return false;
        }

        // Let the script handle it
        int iClientType = pClient->GetClientType ();

        switch ( iClientType )
        {
            case CClient::CLIENT_PLAYER:
            {
                // See if any registered command can process it
                CPlayer* pPlayer = static_cast < CPlayer* > ( pClient );
                m_pRegisteredCommands->ProcessCommand ( szKey, szArguments, pClient );

                // HACK: if the client gets destroyed before here, dont continue
                if ( m_pPlayerManager->Exists ( pPlayer ) )
                {
                    // Call the console event
                    CLuaArguments Arguments;
                    Arguments.PushString ( szCommand );
                    pPlayer->CallEvent ( "onConsole", Arguments );
                }
                break;
            }
            case CClient::CLIENT_CONSOLE:
            {
                // See if any registered command can process it
                CConsoleClient* pConsole = static_cast < CConsoleClient* > ( pClient );
                m_pRegisteredCommands->ProcessCommand ( szKey, szArguments, pClient );

                // Call the console event
                CLuaArguments Arguments;
                Arguments.PushString ( szCommand );
                pConsole->CallEvent ( "onConsole", Arguments );
                break;
            }
            default: break;
        }
    }

    // Doesn't exist
    return false;
}


void CConsole::AddCommand ( FCommandHandler* pHandler, const char* szCommand, bool bRestricted )
{
    // Make a command class and add it to the list
    CConsoleCommand* pCommand = new CConsoleCommand ( pHandler, szCommand, bRestricted );
    m_Commands.push_back ( pCommand );
}


void CConsole::DeleteCommand ( const char* szCommand )
{
    // Find the command and delete it
    list < CConsoleCommand* > ::const_iterator iter = m_Commands.begin ();
    for ( ; iter != m_Commands.end (); iter++ )
    {
        // Names match?
        if ( strcmp ( szCommand, (*iter)->GetCommand () ) == 0 )
        {
            // Delete it and return (or we might crash)
            delete *iter;
            return;
        }
    }
}


void CConsole::DeleteAllCommands ( void )
{
    // Delete all the command classes
    list < CConsoleCommand* > ::const_iterator iter = m_Commands.begin ();
    for ( ; iter != m_Commands.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the commandlist
    m_Commands.clear ();
}


CConsoleCommand* CConsole::GetCommand ( char* szKey )
{
    // See if we have a command matching the key
    list < CConsoleCommand* > ::const_iterator iter = m_Commands.begin ();
    for ( ; iter != m_Commands.end (); iter++ )
    {
        // Names match?
        if ( strcmp ( szKey, (*iter)->GetCommand () ) == 0 )
        {
            return *iter;
        }
    }

    // Didn't find one
    return NULL;
}
