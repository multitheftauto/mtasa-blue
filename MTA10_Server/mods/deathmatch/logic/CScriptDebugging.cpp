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

#define MAX_STRING_LENGTH 2048
CScriptDebugging::CScriptDebugging ( CLuaManager* pLuaManager )
{
    m_pLuaManager = pLuaManager;
    m_uiLogFileLevel = 0;
    m_uiHtmlLogLevel = 0;
    m_pLogFile = NULL;
    m_bTriggeringOnDebugMessage = false;
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

#if 0   // Currently unused
void CScriptDebugging::OutputDebugInfo ( lua_State* luaVM, int iLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    char szDebugDump[255];
    
    lua_Debug debugInfo;
    if ( lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo ( luaVM, "nlS", &debugInfo );
        
        // first version includes script path - makes much longer though
    //  snprintf ( szDebugDump, 255, "Line: %d (%s + %d) %s", debugInfo.currentline, debugInfo.name, debugInfo.currentline - debugInfo.linedefined, debugInfo.short_src );
        snprintf ( szDebugDump, 255, "Line: %d (%s + %d)", debugInfo.currentline, debugInfo.name, debugInfo.currentline - debugInfo.linedefined );
        szDebugDump[255-1] = '\0';

        LogString ( luaVM, szDebugDump, iLevel, ucRed, ucGreen, ucBlue );
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
    LogString ( "WARNING: ", luaVM, szBuffer, 2 );
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

    if ( !m_bTriggeringOnDebugMessage )
    {
        m_bTriggeringOnDebugMessage = true;

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
        g_pGame->GetMapManager ( )->GetRootElement ( )->CallEvent ( "onDebugMessage", Arguments );

        m_bTriggeringOnDebugMessage = false;
    }

    // Log it to the file if enough level
    if ( m_uiLogFileLevel >= 1 )
    {
        PrintLog ( strText );
    }

    // Log to console
    CLogger::LogPrintf( "%s\n", strText.c_str () );

    // Tell the players
    Broadcast ( CDebugEchoPacket ( strText, 1, 255, 255, 255 ), 1 );
}

void CScriptDebugging::LogBadPointer ( lua_State* luaVM, const char* szFunction, const char* szArgumentType, unsigned int uiArgument )
{
    assert ( szFunction );
    assert ( szArgumentType );
    LogWarning ( luaVM, "Bad '%s' pointer @ '%s'(%u)", szArgumentType, szFunction, uiArgument );
}


void CScriptDebugging::LogBadType ( lua_State* luaVM, const char* szFunction )
{
    assert ( szFunction );
    LogWarning ( luaVM, "Bad argument @ '%s'", szFunction );
}


void CScriptDebugging::LogBadAccess ( lua_State* luaVM, const char* szFunction )
{
    assert ( szFunction );
    LogWarning ( luaVM, "Access denied @ '%s'", szFunction );
}


void CScriptDebugging::LogCustom ( lua_State* luaVM, const char* szMessage )
{
    assert ( szMessage );
    LogWarning ( luaVM, "%s", szMessage );
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


void CScriptDebugging::LogString ( const char* szPrePend, lua_State * luaVM, const char* szMessage, unsigned int uiMinimumDebugLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    SString strText;
    lua_Debug debugInfo;

    // Initialize values for onDebugMessage
    SString strMsg  = szMessage;
    SString strFile = "";
    int     iLine   = -1;

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
        strText = SString ( "%s%s%s", szPrePend, m_strLineAndFile.c_str(), szMessage );
    }

    // Create a different message if type is "INFO"
    if ( uiMinimumDebugLevel > 2 )
        strText = SString ( "%s%s", szPrePend, szMessage );

    // Check whether onDebugMessage is currently being triggered
    if ( !m_bTriggeringOnDebugMessage )
    {
        // Make sure the state of onDebugMessage being triggered can be retrieved later
        m_bTriggeringOnDebugMessage = true;

        // Prepare onDebugMessage
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
        
        // Call onDebugMessage
        g_pGame->GetMapManager ( )->GetRootElement ( )->CallEvent ( "onDebugMessage", Arguments );

        // Reset trigger state, so onDebugMessage can be called again at a later moment
        m_bTriggeringOnDebugMessage = false;
    }

    // Log it to the file if enough level
    if ( m_uiLogFileLevel >= uiMinimumDebugLevel )
    {
        PrintLog ( strText );
    }

    // Log to console
    CLogger::LogPrintf( "%s\n", strText.c_str () );

    // Not sure what this is for, seems pretty useless
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
                    html->AppendToPageBuffer ( strText );
                    html->AppendToPageBuffer ( "<br/>" );
                }
            }
        }
    }

    // Tell the players
    Broadcast ( CDebugEchoPacket ( strText, uiMinimumDebugLevel, ucRed, ucGreen, ucBlue ), uiMinimumDebugLevel );
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
