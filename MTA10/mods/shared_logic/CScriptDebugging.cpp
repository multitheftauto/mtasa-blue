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

#define MAX_STRING_LENGTH 2048
CScriptDebugging::CScriptDebugging ( CLuaManager* pLuaManager )
{
    m_pLuaManager = pLuaManager;
    m_uiLogFileLevel = 0;
    m_pLogFile = NULL;
}

#if 0   // Currently unused
void CScriptDebugging::OutputDebugInfo ( lua_State* luaVM, int iLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        
        // first version includes script path - makes much longer though
        //  strDebugDump = SString::Printf ( "Line: %d (%s + %d) %s", debugInfo.currentline, debugInfo.name, debugInfo.currentline - debugInfo.linedefined, debugInfo.short_src );
        SString strDebugDump ( "Line: %d (%s + %d)", debugInfo.currentline, debugInfo.name, debugInfo.currentline - debugInfo.linedefined );

        LogString ( strDebugDump, iLevel, ucRed, ucGreen, ucBlue );
    }
}
#endif

void CScriptDebugging::LogCustom ( lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ... )
{
    assert ( szFormat );

    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    LogString ( "", luaVM, szBuffer, 0, ucRed, ucGreen, ucBlue );
}

void CScriptDebugging::LogInformation ( lua_State* luaVM, const char* szFormat, ... )
{
    assert ( szFormat );

    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Log it
    LogString ( "INFO: ", NULL, szBuffer, 3 );
}


void CScriptDebugging::LogWarning ( lua_State* luaVM, const char* szFormat, ... )
{
    assert ( szFormat );

    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Log it
    LogString ("WARNING: ",  luaVM, szBuffer, 2 );
}


void CScriptDebugging::LogError ( lua_State* luaVM, const char* szFormat, ... )
{
    assert ( szFormat );

    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Log it
    LogString ( "ERROR: ", luaVM, szBuffer, 1 );
}

void CScriptDebugging::LogBadPointer ( lua_State* luaVM, const char* szFunction, const char* szArgumentType, unsigned int uiArgument )
{
    assert ( szFunction );
    assert ( szArgumentType );

    // Populate a message to print/send
    LogWarning ( luaVM, "Bad '%s' pointer @ '%s'(%u)", szArgumentType, szFunction, uiArgument );
}


void CScriptDebugging::LogBadType ( lua_State* luaVM, const char* szFunction )
{
    assert ( szFunction );

    // Populate a message to print/send
    LogWarning ( luaVM, "Bad argument @ '%s'", szFunction );
}


void CScriptDebugging::LogCustom ( lua_State* luaVM, const char* szMessage )
{
    assert ( szMessage );

    // Populate a message to print/send
    LogWarning ( luaVM, "%s", szMessage );
}


void CScriptDebugging::LogBadLevel ( lua_State* luaVM, const char* szFunction, unsigned int uiRequiredLevel )
{
    assert ( szFunction );

    // Populate a message to print/send
    LogWarning ( luaVM, "Requires level '%d' @ '%s", uiRequiredLevel, szFunction );
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

void CScriptDebugging::LogString ( const char* szPrePend, lua_State* luaVM, const char* szMessage, unsigned int uiMinimumDebugLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    // Add file/line number if required
    SString strText;
    lua_Debug debugInfo;
    if ( luaVM && lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );

        // Make sure this function isn't defined in a string (eg: from runcode)
        if ( debugInfo.linedefined != 0 )
        {
            std::string strFilename = ConformResourcePath ( debugInfo.source );

            // Populate a message to print/send
            strText = SString ( "%s%s:%d: %s", szPrePend, strFilename.c_str (), debugInfo.currentline, szMessage );
        }
        else
        {
            strText = SString ( "%s%s (string-defined function)", szPrePend, szMessage );
       }
    }
    else
    {
        strText = SString ( "%s%s", szPrePend, szMessage );
    }


    // Log it to the file if enough level
    if ( m_uiLogFileLevel >= uiMinimumDebugLevel )
    {
        PrintLog ( strText );
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
    g_pCore->DebugEchoColor ( strText, ucRed, ucGreen, ucBlue );
}


void CScriptDebugging::PrintLog ( const char* szText )
{
    // Got a logfile?
    if ( m_pLogFile )
    {
        // Log it, timestamped
        char szBuffer [64];
        time_t timeNow;
        time ( &timeNow );
        strftime ( szBuffer, 32, "[%Y-%m-%d %H:%M:%S]", localtime ( &timeNow ) );

	    fprintf ( m_pLogFile, "%s %s\n", szBuffer, szText );
        fflush ( m_pLogFile );
    }
}
