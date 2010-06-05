/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptDebugging.cpp
*  PURPOSE:     Script debugging facility class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame * g_pGame;

CScriptDebugging::CScriptDebugging ( CLuaManager* pLuaManager )
{
    m_pLuaManager = pLuaManager;
    m_uiLogFileLevel = 0;
    m_uiHtmlLogLevel = 0;
    m_pLogFile = NULL;
}


bool CScriptDebugging::AddPlayer ( CPlayer& Player, unsigned int uiLevel )
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
            if ( !m_Players.empty() ) m_Players.remove ( &Player );
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
        if ( !m_Players.empty() ) m_Players.remove ( &Player );

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
}

void CScriptDebugging::OutputDebugInfo ( lua_State* luaVM, int iLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    char szDebugDump[255];
    
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        
        // first version includes script path - makes much longer though
    //  _snprintf ( szDebugDump, 255, "Line: %d (%s + %d) %s", debugInfo.currentline, debugInfo.name, debugInfo.currentline - debugInfo.linedefined, debugInfo.short_src );
        _snprintf ( szDebugDump, 255, "Line: %d (%s + %d)", debugInfo.currentline, debugInfo.name, debugInfo.currentline - debugInfo.linedefined );
        szDebugDump[255-1] = '\0';

        LogString ( luaVM, szDebugDump, iLevel, ucRed, ucGreen, ucBlue );
    }
}

void CScriptDebugging::LogCustom ( lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ... )
{
        assert ( szFormat );

    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        const char* szFilename = GetFilenameFromPath ( debugInfo.source );

        char szBuffer [256];
        szBuffer [255] = 0;

        // Copy it to a buffer
        va_list ap;
	    va_start ( ap, szFormat );
        _VSNPRINTF ( szBuffer, 248, szFormat, ap );
        va_end ( ap );

        char szOutbuffer [512];
        _snprintf ( szOutbuffer, 510, "%s: %s - Line: %d", szFilename, szBuffer, debugInfo.currentline );

        // Log it
        LogString ( luaVM, szOutbuffer, 0, ucRed, ucGreen, ucBlue );
    }
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
    _VSNPRINTF ( szBuffer + 6, 248, szFormat, ap );
    va_end ( ap );

    // Log it
    LogString ( luaVM, szBuffer, 3 );
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
    _VSNPRINTF ( szBuffer + 9, 245, szFormat, ap );
    va_end ( ap );

    // Log it
    LogString ( luaVM, szBuffer, 2 );
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
    _VSNPRINTF ( szBuffer + 7, 247, szFormat, ap );
    va_end ( ap );

    // Log it
    LogString ( luaVM, szBuffer, 1 );
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


void CScriptDebugging::LogBadAccess ( lua_State* luaVM, const char* szFunction )
{
    assert ( szFunction );
    
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        // Make sure this function isn't defined in a string (eg: from runcode)
        if ( debugInfo.linedefined != 0 )
        {
            const char* szFilename = GetFilenameFromPath ( debugInfo.source );

            // Populate a message to print/send
            LogWarning ( luaVM, "%s: Access denied @ '%s' - Line: %d", szFilename, szFunction, debugInfo.currentline );
        }
        else
        {
            LogWarning ( luaVM, "Access denied @ '%s' (string-defined function)", szFunction );
        }
    }
}


void CScriptDebugging::LogCustom ( lua_State* luaVM, const char* szMessage )
{
    assert ( szMessage );
    
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );

        // Make sure this function isn't defined in a string (eg: from runcode)
        if ( debugInfo.linedefined != 0 )
        {
            const char* szFilename = GetFilenameFromPath ( debugInfo.source );

            // Populate a message to print/send
            LogWarning ( luaVM, "%s: %s - Line: %d", szFilename, szMessage, debugInfo.currentline );
        }
        else
        {
            LogWarning ( luaVM, "%s (string-defined function)", szMessage );
        }
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


void CScriptDebugging::LogString ( lua_State * luaVM, const char* szText, unsigned int uiMinimumDebugLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    // Log it to the file if enough level
    if ( m_uiLogFileLevel >= uiMinimumDebugLevel )
    {
        PrintLog ( szText );
    }

	// Log to console
	CLogger::LogPrintf( "%s\n", szText );

    if ( m_uiHtmlLogLevel >= uiMinimumDebugLevel )
    {
        if ( luaVM )
        {
            CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CResourceFile * file = pLuaMain->GetResourceFile();
                if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
                {
                    CResourceHTMLItem * html = (CResourceHTMLItem *)file;
                    html->AppendToPageBuffer ( (char *)szText );
                    html->AppendToPageBuffer ( "<br/>" );
                }
            }
        }
    }

    // Tell the players
    Broadcast ( CDebugEchoPacket ( szText, uiMinimumDebugLevel, ucRed, ucGreen, ucBlue ), uiMinimumDebugLevel );
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


void CScriptDebugging::Broadcast ( const CPacket& Packet, unsigned int uiMinimumDebugLevel )
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
}
