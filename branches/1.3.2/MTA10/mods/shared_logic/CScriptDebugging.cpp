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
    m_bTriggeringOnClientDebugMessage = false;
}

CScriptDebugging::~CScriptDebugging ( void )
{
    // Close the previously loaded file
    if ( m_pLogFile )
    {
        fprintf ( m_pLogFile, "INFO: Logging to this file ended\n" );
        fclose ( m_pLogFile );
        m_pLogFile = NULL;
    }
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
    VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
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
    VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Log it
    LogString ( "INFO: ", luaVM, szBuffer, 3 );
}


void CScriptDebugging::LogWarning ( lua_State* luaVM, const char* szFormat, ... )
{
    assert ( szFormat );

    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Log it
    LogString ("WARNING: ", luaVM, szBuffer, 2 );
}


void CScriptDebugging::LogError ( lua_State* luaVM, const char* szFormat, ... )
{
    assert ( szFormat );

    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Log it
    LogString ( "ERROR: ", luaVM, szBuffer, 1 );
}

void CScriptDebugging::LogError ( SString strFile, int iLine, SString strMsg )
{
    SString strText = SString ( "ERROR: %s:%d: %s", strFile.c_str (), iLine, strMsg.c_str () );

    if ( !m_bTriggeringOnClientDebugMessage )
    {
        m_bTriggeringOnClientDebugMessage = true;

        // Prepare onDebugMessage
        CLuaArguments Arguments;
        Arguments.PushString ( strMsg.c_str ( ) );
        Arguments.PushNumber ( 1 );

        // Push the file name (if any)
        if ( strFile.length ( ) > 0 )
            Arguments.PushString ( strFile.c_str ( ) );
        else
            Arguments.PushNil ( );

        // Push the line (if any)
        if ( iLine > -1 )
            Arguments.PushNumber ( iLine );
        else
            Arguments.PushNil ( );
        
        // Call onDebugMessage
        g_pClientGame->GetRootEntity ( )->CallEvent ( "onClientDebugMessage", Arguments, false );

        m_bTriggeringOnClientDebugMessage = false;
    }

    // Log it to the file if enough level
    if ( m_uiLogFileLevel >= 1 )
    {
        PrintLog ( strText );
    }

    // Log to console
    g_pCore->DebugEchoColor ( strText, 255, 0, 0 );
}

void CScriptDebugging::LogBadPointer ( lua_State* luaVM, const char* szArgumentType, unsigned int uiArgument )
{
    assert ( szArgumentType );

    // Populate a message to print/send
    LogWarning ( luaVM, "Bad '%s' pointer @ '%s'(%u)", szArgumentType, lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), uiArgument );
}


void CScriptDebugging::LogBadType ( lua_State* luaVM )
{
    // Populate a message to print/send
    LogWarning ( luaVM, "Bad argument @ '%s'", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
}


void CScriptDebugging::LogCustom ( lua_State* luaVM, const char* szMessage )
{
    assert ( szMessage );

    // Populate a message to print/send
    LogWarning ( luaVM, "%s", szMessage );
}


void CScriptDebugging::LogBadLevel ( lua_State* luaVM, unsigned int uiRequiredLevel )
{
    // Populate a message to print/send
    LogWarning ( luaVM, "Requires level '%d' @ '%s", uiRequiredLevel, lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
}


bool CScriptDebugging::SetLogfile ( const char* szFilename, unsigned int uiLevel )
{
    assert ( szFilename );

    // Close the previously loaded file
    if ( m_pLogFile )
    {
        fprintf ( m_pLogFile, "INFO: Logging to this file ended\n" );
        fclose ( m_pLogFile );
        m_pLogFile = NULL;
    }

    // Apply log size limit
    uint uiMaxSizeKB = 0;
    g_pCore->GetCVars ()->Get ( "max_clientscript_log_kb", uiMaxSizeKB );
    if ( uiMaxSizeKB > 0 )
    {
        uint uiCurrentSizeKB = FileSize ( szFilename ) / 1024;
        if ( uiCurrentSizeKB > uiMaxSizeKB )
        {
            SString strFilenameBackup ( "%s.bak", szFilename );
            FileDelete ( strFilenameBackup );
            FileRename ( szFilename, strFilenameBackup );
            FileDelete ( szFilename );
        }
    }

    // Try to load the new file
    FILE* pFile = fopen ( szFilename, "a+" );
    if ( pFile )
    {
        // Set the new pointer and level and return true
        m_uiLogFileLevel = uiLevel;
        m_pLogFile = pFile;
        return true;
    }

    return false;
}

void CScriptDebugging::LogString ( const char* szPrePend, lua_State* luaVM, const char* szMessage, unsigned int uiMinimumDebugLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    SString strText;
    lua_Debug debugInfo;

    // Initialize values for onClientDebugMessage
    SString strMsg  = szMessage;
    SString strFile = "";
    int     iLine   = -1;

    // Get a VM from somewhere
    if ( !luaVM && !m_LuaMainStack.empty () )
        luaVM = m_LuaMainStack.back ()->GetVM ();

    if ( luaVM && lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );

        // Make sure this function isn't defined in a string (eg: from runcode)
        if ( debugInfo.source[0] == '@' )
        {
            // Get and store the location of the debug message
            strFile = ConformResourcePath ( debugInfo.source );
            iLine   = debugInfo.currentline;

            // Populate a message to print/send (unless "info" type)
            if ( uiMinimumDebugLevel < 3 )
                strText = SString ( "%s%s:%d: %s", szPrePend, strFile.c_str (), debugInfo.currentline, szMessage );
        }
        else
        {
            strFile = debugInfo.short_src;

            if ( uiMinimumDebugLevel < 3 )
                strText = SString ( "%s%s %s", szPrePend, szMessage, strFile.c_str () );
        }
    }
    else
    {
        strText = SString ( "%s%s", szPrePend, szMessage );
    }

    // Create a different message if type is "INFO"
    if ( uiMinimumDebugLevel > 2 )
        strText = SString ( "%s%s", szPrePend, szMessage );

    if ( !m_bTriggeringOnClientDebugMessage )
    {
        m_bTriggeringOnClientDebugMessage = true;

        // Prepare onClientDebugMessage
        CLuaArguments Arguments;
        Arguments.PushString ( strMsg.c_str ( ) );
        Arguments.PushNumber ( uiMinimumDebugLevel );

        // Push the file name (if any)
        if ( strFile.length ( ) > 0 )
            Arguments.PushString ( strFile.c_str ( ) );
        else
            Arguments.PushNil ( );

        // Push the line (if any)
        if ( iLine > -1 )
            Arguments.PushNumber ( iLine );
        else
            Arguments.PushNil ( );
        
        // Call onClientDebugMessage
        g_pClientGame->GetRootEntity ( )->CallEvent ( "onClientDebugMessage", Arguments, false );

        m_bTriggeringOnClientDebugMessage = false;
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


// Keep a stack of called VMs to give global warnings/errors a context
void CScriptDebugging::PushLuaMain ( CLuaMain* pLuaMain )
{
    m_LuaMainStack.push_back ( pLuaMain );
}

void CScriptDebugging::PopLuaMain ( CLuaMain* pLuaMain )
{
    dassert ( !m_LuaMainStack.empty () );
    if ( !m_LuaMainStack.empty () )
    {
        dassert ( m_LuaMainStack.back () == pLuaMain );
        m_LuaMainStack.pop_back ();
    }
}

void CScriptDebugging::OnLuaMainDestroy ( CLuaMain* pLuaMain )
{
    dassert ( !ListContains ( m_LuaMainStack, pLuaMain ) );
    ListRemove ( m_LuaMainStack, pLuaMain );
}
