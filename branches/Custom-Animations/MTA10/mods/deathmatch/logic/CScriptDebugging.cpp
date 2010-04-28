/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptDebugging.cpp
*  PURPOSE:     Script debugging
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*               Ed Lyons <eai@opencoding.net>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "stdinc.h"
#include "CScriptDebugging.h"
#include < assert.h >
#include "Config.h"
//#include "CPlayer.h"
//#include "packets/CDebugEchoPacket.h"

const char* GetFilenameFromPath ( const char* szPath )
{
    if ( szPath && szPath [ 0 ] )
    {
        unsigned int uiLength = (unsigned int)strlen ( szPath );
        for ( unsigned int i = uiLength - 1 ; i >= 0 ; i-- )
        {
            if ( szPath [ i ] == '/' || szPath [ i ] == '\\' )
            {
                return &szPath [ i + 1 ];
            }
        }
    }
    return NULL;
}

CScriptDebugging::CScriptDebugging ( CLuaManager* pLuaManager )
{
    m_pLuaManager = pLuaManager;
    m_uiLogFileLevel = 0;
    m_pLogFile = NULL;
}


/*bool CScriptDebugging::AddPlayer ( CPlayer& Player, unsigned int uiLevel )
{
    // Want a level above 0?
    if ( uiLevel > 0 )
    {
        // He's not already debugging? Add him to our list.
        if ( Player.m_uiScriptDebugLevel == 0 )
        {
            m_Players.push_back ( &Player );
        }
    }
    else
    {
        // He's already script debugging? Remove him from our list. 
        if ( Player.m_uiScriptDebugLevel > 0 )
        {
            m_Players.remove ( &Player );
        }
    }

    // Give him the level
    Player.m_uiScriptDebugLevel = uiLevel;
    return true;
}


bool CScriptDebugging::RemovePlayer ( CPlayer& Player )
{
    // Is he script debugging?
    if ( Player.m_uiScriptDebugLevel > 0 )
    {
        // Mark it as not script debugging and remove it from our list
        Player.m_uiScriptDebugLevel = 0;
        m_Players.remove ( &Player );

        return true;
    }

    return false;
}


void CScriptDebugging::ClearPlayers ( void )
{
    // Unreference us from all players that we are script logging to
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        (*iter)->m_uiScriptDebugLevel = 0;
    }

    // Clear the list
    m_Players.clear ();
}*/

void CScriptDebugging::OutputDebugInfo ( lua_State* luaVM, int iLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    char szDebugDump[255];
    
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        
        // first version includes script path - makes much longer though
        //snprintf ( szDebugDump, 255, "Line: %d (%s + %d) %s", debugInfo.currentline, debugInfo.name, debugInfo.currentline - debugInfo.linedefined, debugInfo.short_src );
        //snprintf ( szDebugDump, 255, "Line: %d (%s + %d)", debugInfo.currentline, debugInfo.name, debugInfo.currentline - debugInfo.linedefined );
        szDebugDump[255-1] = '\0';

        LogString ( szDebugDump, iLevel, ucRed, ucGreen, ucBlue );
    }
}

void CScriptDebugging::LogCustom ( lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ... )
{
    assert ( szFormat );

    char szBuffer [256];
    szBuffer [255] = 0;

    // Copy it to a buffer
    va_list ap;
    va_start ( ap, szFormat );
    //vsnprintf ( szBuffer, 248, szFormat, ap );
    va_end ( ap );

    // Log it
    LogString ( szBuffer, 0, ucRed, ucGreen, ucBlue );
}

void CScriptDebugging::LogInformation ( lua_State* luaVM, const char* szFormat, ... )
{
    assert ( szFormat );

    // Put the "INFO: " at the start
    char szBuffer [256];
    szBuffer [255] = 0;
    strcpy ( szBuffer, "INFO: " );

    // Copy it to a buffer
    va_list ap;
    va_start ( ap, szFormat );
    //vsnprintf ( szBuffer + 6, 248, szFormat, ap );
    va_end ( ap );

    // Log it
    LogString ( szBuffer, 3 );
}


void CScriptDebugging::LogWarning ( lua_State* luaVM, const char* szFormat, ... )
{
    assert ( szFormat );

    // Put the "WARNING: " at the start
    char szBuffer [256];
    szBuffer [255] = 0;
    strcpy ( szBuffer, "WARNING: " );

    // Copy it to a buffer
    va_list ap;
    va_start ( ap, szFormat );
    //vsnprintf ( szBuffer + 9, 245, szFormat, ap );
    va_end ( ap );

    // Log it
    LogString ( szBuffer, 2 );
}


void CScriptDebugging::LogError ( lua_State* luaVM, const char* szFormat, ... )
{
    assert ( szFormat );

    // Put the "ERROR: " at the start
    char szBuffer [256];
    szBuffer [255] = 0;
    strcpy ( szBuffer, "ERROR: " );

    // Copy it to a buffer
    va_list ap;
    va_start ( ap, szFormat );
    //vsnprintf ( szBuffer + 7, 247, szFormat, ap );
    va_end ( ap );

    // Log it
    LogString ( szBuffer, 1 );
}

void CScriptDebugging::LogBadPointer ( lua_State* luaVM, const char* szFunction, const char* szArgumentType, unsigned int uiArgument )
{
    assert ( szFunction );
    assert ( szArgumentType );

    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        const char* szFilename = GetFilenameFromPath ( debugInfo.source );

        // Populate a message to print/send
        LogWarning ( luaVM, "%s: Bad '%s' pointer @ '%s'(%u) - Line: %d", szFilename, szArgumentType, szFunction, uiArgument, debugInfo.currentline );
    }
}


void CScriptDebugging::LogBadType ( lua_State* luaVM, const char* szFunction )
{
    assert ( szFunction );

    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        const char* szFilename = GetFilenameFromPath ( debugInfo.source );

        // Populate a message to print/send
        LogWarning ( luaVM, "%s: Bad argument @ '%s' - Line: %d", szFilename, szFunction, debugInfo.currentline );
    }
}


void CScriptDebugging::LogBadLevel ( lua_State* luaVM, const char* szFunction, unsigned int uiRequiredLevel )
{
    assert ( szFunction );
    
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        const char* szFilename = GetFilenameFromPath ( debugInfo.source );

        // Populate a message to print/send
        LogWarning ( luaVM, "%s: Requires level '%d' @ '%s' - Line: %d", szFilename, uiRequiredLevel, szFunction, debugInfo.currentline );
    }
}


bool CScriptDebugging::SetLogfile ( const char* szFilename, unsigned int uiLevel )
{
    assert ( szFilename );

    // Try to load the new file
    FILE* pFile = fopen ( szFilename, "a+" );
    if ( pFile )
    {
        // Close the previously loaded file
        if ( m_pLogFile )
        {
            fprintf ( m_pLogFile, "INFO: Logging to this file ended\n" );
            fclose ( m_pLogFile );
        }

        // Set the new pointer and level and return true
        m_uiLogFileLevel = uiLevel;
        m_pLogFile = pFile;
        return true;
    }

    return false;
}


void CScriptDebugging::LogString ( const char* szText, unsigned int uiMinimumDebugLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    // Log it to the file if enough level
    if ( m_uiLogFileLevel >= uiMinimumDebugLevel )
    {
        PrintLog ( szText );
    }

    // Tell the players
    //Broadcast ( CDebugEchoPacket ( szText, uiMinimumDebugLevel, ucRed, ucGreen, ucBlue ), uiMinimumDebugLevel );
}


void CScriptDebugging::PrintLog ( const char* szText )
{
    // Got a logfile?
    if ( m_pLogFile )
    {
        // Log it
        fprintf ( m_pLogFile, "%s\n", szText );
        fflush ( m_pLogFile );
    }
}


/*void CScriptDebugging::Broadcast ( const CPacket& Packet, unsigned int uiMinimumDebugLevel )
{
    // Tell everyone we log to about it
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        if ( (*iter)->m_uiScriptDebugLevel >= uiMinimumDebugLevel )
        {
            (*iter)->Send ( Packet );
        }
    }
}*/
