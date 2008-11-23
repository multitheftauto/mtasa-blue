/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRegisteredCommands.cpp
*  PURPOSE:     Registered commands manager
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*				Derek Abdine <...>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "stdinc.h"
#include "CRegisteredCommands.h"
//#include "CPlayer.h"
//#include "CAdmin.h"
//#include "CConsoleClient.h"
#include "lua/CLuaMain.h"
#include <assert.h>

bool CRegisteredCommands::AddCommand ( CLuaMain* pLuaMain, const char* szKey, const char* szHandler )
{
    assert ( pLuaMain );
    assert ( szKey );
    assert ( szHandler );

	// Check if we already have this key and handler
	SCommand* pCommand = GetCommand ( szKey, pLuaMain );
	if ( pCommand && !strcmp ( pCommand->szHandler, szHandler )
        return false;

    // Create the entry
    pCommand = new SCommand;
    pCommand->pLuaMain = pLuaMain;
    strncpy ( pCommand->szKey, szKey, MAX_REGISTERED_COMMAND_LENGTH );
    pCommand->szKey [MAX_REGISTERED_COMMAND_LENGTH] = 0;
    strncpy ( pCommand->szHandler, szHandler, MAX_REGISTERED_COMMANDHANDLER_LENGTH );
    pCommand->szHandler [MAX_REGISTERED_COMMANDHANDLER_LENGTH] = 0;

    // Add it to our list
    m_Commands.push_back ( pCommand );
    return true;
}


bool CRegisteredCommands::RemoveCommand ( CLuaMain* pLuaMain, const char* szKey )
{
    assert ( pLuaMain );
    assert ( szKey );

    // Call the handler for every virtual machine that matches the given key
	bool bFound = false;
    list < SCommand* > ::iterator iter = m_Commands.begin ();
    while ( iter != m_Commands.end () )
    {
        // Matching vm's and names?
        if ( (*iter)->pLuaMain == pLuaMain && strcmp ( (*iter)->szKey, szKey ) == 0 )
        {
            // Delete it and remove it from our list
            delete *iter;
            m_Commands.erase ( iter );
			iter = m_Commands.begin ();
			bFound = true;
			continue;
        }
		iter++;
    }

    return bFound;
}


void CRegisteredCommands::ClearCommands ( void )
{
    // Delete all the commands
    list < SCommand* > ::const_iterator iter = m_Commands.begin ();
    for ( ; iter != m_Commands.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_Commands.clear ();
}


void CRegisteredCommands::CleanUpForVM ( CLuaMain* pLuaMain )
{
    assert ( pLuaMain );

    // Delete every command that matches
    list < SCommand* > ::iterator iter = m_Commands.begin ();
    for ( ; iter != m_Commands.end (); )
    {
        // Matching VM's?
        if ( (*iter)->pLuaMain == pLuaMain )
        {
            // Delete the entry and remove it from the list
            delete *iter;
            iter = m_Commands.erase ( iter );
        }
        else
        {
            ++iter;
        }
    }
}


bool CRegisteredCommands::CommandExists ( const char* szKey, CLuaMain* pLuaMain )
{
    assert ( szKey );

    return GetCommand ( szKey, pLuaMain ) != NULL;
}


bool CRegisteredCommands::ProcessCommand ( const char* szKey, const char* szArguments, CClient* pClient )
{
    assert ( szKey );

    // Call the handler for every virtual machine that matches the given key
    list < SCommand* > ::const_iterator iter = m_Commands.begin ();
    for ( ; iter != m_Commands.end (); iter++ )
    {
        // Matching names?
        if ( strcmp ( (*iter)->szKey, szKey ) == 0 )
        {
            // Call it
            CallCommandHandler ( (*iter)->pLuaMain, (*iter)->szHandler, szKey, szArguments, pClient );
            return true;
        }
    }

    // Return whether some handler was called or not
    return false;
}


CRegisteredCommands::SCommand* CRegisteredCommands::GetCommand ( const char* szKey, class CLuaMain* pLuaMain )
{
    assert ( szKey );

    // Try to find an entry with a matching name in our list
    list < SCommand* > ::const_iterator iter = m_Commands.begin ();
    for ( ; iter != m_Commands.end (); iter++ )
    {
        // Matching name and no given VM or matching VM
        if ( strcmp ( (*iter)->szKey, szKey ) == 0 && ( !pLuaMain || pLuaMain == (*iter)->pLuaMain ) )
        {
            return *iter;
        }
    }

    // Doesn't exist
    return NULL;
}


void CRegisteredCommands::CallCommandHandler ( CLuaMain* pLuaMain, const char* szHandler, const char* szKey, const char* szArguments, CClient* pClient )
{
    assert ( pLuaMain );
    assert ( szHandler );
    assert ( szKey );

    CLuaArguments Arguments;
    // First, try to call a handler with the same number of arguments
    if ( pClient )
    {
        /*switch ( pClient->GetClientType () )
        {
            case CClient::CLIENT_PLAYER:
            {
                Arguments.PushUserData ( static_cast < CPlayer* > ( pClient ) );
                break;
            }
            case CClient::CLIENT_REMOTECLIENT:
            {
                Arguments.PushUserData ( static_cast < CAdmin* > ( pClient ) );
                break;
            }
            case CClient::CLIENT_CONSOLE:
            {
                Arguments.PushUserData ( static_cast < CConsoleClient* > ( pClient ) );
                break;
            }
            default:
            {
                Arguments.PushBoolean ( false );
                break;
            }
        }*/
    }
    else
    {
        Arguments.PushBoolean ( false );
    }

    Arguments.PushString ( szKey );

    if ( szArguments )
    {
        // Create a copy and strtok modifies the string
        char * szTempArguments = new char[strlen(szArguments) + 1];
        strcpy ( szTempArguments, szArguments );
        char * arg;
        arg = strtok ( szTempArguments, " " );
        while ( arg )
        {
            Arguments.PushString ( arg );
            arg = strtok ( NULL, " " );
        }
        delete [] szTempArguments;
    }
    else
    {
        Arguments.PushBoolean ( false );
    }

    // Call the handler with the arguments we pushed
    Arguments.Call ( pLuaMain, szHandler );
}
