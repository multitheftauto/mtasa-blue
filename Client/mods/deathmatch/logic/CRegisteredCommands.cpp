/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CRegisteredCommands.cpp
*  PURPOSE:     Registered commands class
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

CRegisteredCommands::CRegisteredCommands ( void )
{
    m_bIteratingList = false;
}


CRegisteredCommands::~CRegisteredCommands ( void )
{
    ClearCommands ();
}


bool CRegisteredCommands::AddCommand ( CLuaMain* pLuaMain, const char* szKey, const CLuaFunctionRef& iLuaFunction, bool bCaseSensitive )
{
    assert ( pLuaMain );
    assert ( szKey );

    // Check if we already have this key and handler
    SCommand* pCommand = GetCommand ( szKey, pLuaMain );
    if ( pCommand )
    {
        if ( pCommand->iLuaFunction == iLuaFunction )
            return false;
    }

    // Create the entry
    pCommand = new SCommand;
    pCommand->pLuaMain = pLuaMain;
    pCommand->strKey.AssignLeft ( szKey, MAX_REGISTERED_COMMAND_LENGTH );
    pCommand->iLuaFunction = iLuaFunction;
    pCommand->bCaseSensitive = bCaseSensitive;

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
    
    int iCompareResult;

    auto iter = m_Commands.begin();
    while ( iter != m_Commands.end () )
    {
        if ( (*iter)->bCaseSensitive )
            iCompareResult = strcmp ( (*iter)->strKey, szKey );
        else
            iCompareResult = stricmp ( (*iter)->strKey, szKey );

        // Matching vm's and names?
        if ( (*iter)->pLuaMain == pLuaMain && iCompareResult == 0 )
        {
            bFound = true;
            // Delete it and remove it from our list
            if ( m_bIteratingList )
            {
                m_TrashCan.push_back ( *iter );
            }
            else
            {
                delete *iter;
                m_Commands.erase ( iter );
                iter = m_Commands.begin ();
                continue;
            }
        }
        iter++;
    }

    return bFound;
}


void CRegisteredCommands::ClearCommands ( void )
{
    // Delete all the commands
    for ( auto& pCommand : m_Commands )
    {
        delete pCommand;
    }

    // Clear the list
    m_Commands.clear ();
}


void CRegisteredCommands::CleanUpForVM ( CLuaMain* pLuaMain )
{
    assert ( pLuaMain );

    // Delete every command that matches
    for ( auto iter = m_Commands.begin() ; iter != m_Commands.end (); )
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


bool CRegisteredCommands::ProcessCommand ( const char* szKey, const char* szArguments )
{
    assert ( szKey );

    // Call the handler for every virtual machine that matches the given key
    int iCompareResult;
    bool bHandled = false;
    m_bIteratingList = true;
    for ( auto& pCommand : m_Commands )
    {
        if ( pCommand->bCaseSensitive )
            iCompareResult = strcmp ( pCommand->strKey, szKey );
        else
            iCompareResult = stricmp ( pCommand->strKey, szKey );

        // Matching names?
        if ( iCompareResult == 0 )
        {
            // Call it
            CallCommandHandler ( pCommand->pLuaMain, pCommand->iLuaFunction, pCommand->strKey, szArguments );
            bHandled = true;
        }
    }
    m_bIteratingList = false;
    TakeOutTheTrash ();

    // Return whether some handler was called or not
    return bHandled;
}


CRegisteredCommands::SCommand* CRegisteredCommands::GetCommand ( const char* szKey, class CLuaMain* pLuaMain )
{
    assert ( szKey );

    // Try to find an entry with a matching name in our list
    int iCompareResult;
    for (auto& pCommand : m_Commands)
    {
        if (pCommand->bCaseSensitive)
            iCompareResult = strcmp(pCommand->strKey, szKey);
        else
            iCompareResult = stricmp(pCommand->strKey, szKey);

        // Matching name and no given VM or matching VM
        if ( iCompareResult == 0 && ( !pLuaMain || pLuaMain == pCommand->pLuaMain ) )
        {
            return pCommand;
        }
    }

    // Doesn't exist
    return NULL;
}


void CRegisteredCommands::CallCommandHandler ( CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, const char* szKey, const char* szArguments )
{
    assert ( pLuaMain );
    assert ( szKey );

    CLuaArguments Arguments;
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

    // Call the handler with the arguments we pushed
    Arguments.Call ( pLuaMain, iLuaFunction );
}


void CRegisteredCommands::TakeOutTheTrash ( void )
{
    for ( auto& pCommand : m_TrashCan )
    {
        if ( !m_Commands.empty() ) 
            m_Commands.remove ( pCommand );
        delete pCommand;
    }
    m_TrashCan.clear ();
}
