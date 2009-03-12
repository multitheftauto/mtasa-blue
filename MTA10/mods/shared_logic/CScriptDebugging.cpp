/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CScriptDebugging.cpp
*  PURPOSE:     Script debugging class
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Ed Lyons <eai@opencoding.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

CScriptDebugging::CScriptDebugging ( CLuaManager* pLuaManager )
{
    m_pLuaManager = pLuaManager;
    m_uiLogFileLevel = 0;
    m_pLogFile = NULL;
}


void CScriptDebugging::OutputDebugInfo ( lua_State* luaVM, int iLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    char szDebugDump[255];
    
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        
        // first version includes script path - makes much longer though
        //  snprintf ( szDebugDump, 255, "Line: %d (%s + %d) %s", debugInfo.currentline, debugInfo.name, debugInfo.currentline - debugInfo.linedefined, debugInfo.short_src );
        _snprintf ( szDebugDump, 255, "Line: %d (%s + %d)", debugInfo.currentline, debugInfo.name, debugInfo.currentline - debugInfo.linedefined );
        szDebugDump[255-1] = '\0';

        LogString ( szDebugDump, iLevel, ucRed, ucGreen, ucBlue );
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

        // Copy it to a buffer
        va_list ap;
	    va_start ( ap, szFormat );
        _VSNPRINTF ( szBuffer, 256, szFormat, ap );
        va_end ( ap );
        szBuffer[255] = '\0';

        char szOutBuffer [512];
        _snprintf ( szOutBuffer, 512, "%s: %s - Line: %d", szFilename, szBuffer, debugInfo.currentline );
        szOutBuffer[511] = '\0';

        // Log it
        LogString ( szOutBuffer, 0, ucRed, ucGreen, ucBlue );
    }
}

void CScriptDebugging::LogInformation ( lua_State* luaVM, const char* szFormat, ... )
{
    assert ( szFormat );

    // Put the "INFO: " at the start
    char szBuffer [256];
    strcpy ( szBuffer, "INFO: " );

    // Copy it to a buffer
    va_list ap;
	va_start ( ap, szFormat );
    _VSNPRINTF ( szBuffer + 6, 250, szFormat, ap );
    va_end ( ap );
    szBuffer[255] = '\0';

    // Log it
    LogString ( szBuffer, 3 );
}


void CScriptDebugging::LogWarning ( lua_State* luaVM, const char* szFormat, ... )
{
    assert ( szFormat );

    // Put the "WARNING: " at the start
    char szBuffer [256];
    strcpy ( szBuffer, "WARNING: " );

    // Copy it to a buffer
    va_list ap;
	va_start ( ap, szFormat );
    _VSNPRINTF ( &szBuffer [ 9 ], 247, szFormat, ap );
    va_end ( ap );
    szBuffer[255] = '\0';

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
    _VSNPRINTF ( szBuffer + 7, 247, szFormat, ap );
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


void CScriptDebugging::LogCustom ( lua_State* luaVM, const char* szMessage )
{
    assert ( szMessage );

    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        const char* szFilename = GetFilenameFromPath ( debugInfo.source );

        // Populate a message to print/send
        LogWarning ( luaVM, "%s: %s - Line: %d", szFilename, szMessage, debugInfo.currentline );
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
    switch ( uiMinimumDebugLevel )
    {
        case 1:
            ucRed = 255, ucGreen = 0, ucBlue = 0;
            break;
        case 2:
            ucRed = 255, ucGreen = 128, ucBlue = 0;
            break;
        case 3:
            ucRed = 0, ucGreen = 255, ucBlue = 0;
            break;
    }
#ifdef MTA_DEBUG
    if ( !g_pCore->IsDebugVisible () ) return;
#endif
    g_pCore->DebugEchoColor ( szText, ucRed, ucGreen, ucBlue );
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
