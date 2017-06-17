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
    m_bTriggeringOnDebugMessage = false;
}

CScriptDebugging::~CScriptDebugging ( void )
{
    // Flush any pending duplicate loggings
    m_DuplicateLineFilter.Flush();
    UpdateLogOutput();

    ClearPlayers ();
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
}

void CScriptDebugging::LogBadAccess ( lua_State* luaVM )
{
    LogWarning ( luaVM, "Access denied @ '%s'", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
}

bool CScriptDebugging::SetLogfile ( const char* szFilename, unsigned int uiLevel )
{
    assert ( szFilename );

    // Try to load the new file
    FILE* pFile = File::Fopen ( szFilename, "a+" );
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

void CScriptDebugging::LogString ( const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage, unsigned int uiMinimumDebugLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    SString strText = ComposeErrorMessage( szPrePend, luaDebugInfo, szMessage );

    // Create a different message if type is "INFO"
    if ( uiMinimumDebugLevel > 2 )
        strText = SString ( "%s%s", szPrePend, szMessage );

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
        default:
            break;
    }

    // Check whether onDebugMessage is currently being triggered
    if ( !m_bTriggeringOnDebugMessage )
    {
        // Make sure the state of onDebugMessage being triggered can be retrieved later
        m_bTriggeringOnDebugMessage = true;

        // Prepare onDebugMessage
        CLuaArguments Arguments;
        Arguments.PushString ( szMessage );
        Arguments.PushNumber ( uiMinimumDebugLevel );

        // Push the file name (if any)
        if ( !luaDebugInfo.strFile.empty() )
            Arguments.PushString ( luaDebugInfo.strFile );
        else
            Arguments.PushNil ( );

        // Push the line (if any)
        if ( luaDebugInfo.iLine != INVALID_LINE_NUMBER )
            Arguments.PushNumber ( luaDebugInfo.iLine );
        else
            Arguments.PushNil ( );

        // Push the colors
        Arguments.PushNumber ( ucRed );
        Arguments.PushNumber ( ucGreen );
        Arguments.PushNumber ( ucBlue );
        
        // Call onDebugMessage
        g_pGame->GetMapManager ( )->GetRootElement ( )->CallEvent ( "onDebugMessage", Arguments );

        // Reset trigger state, so onDebugMessage can be called again at a later moment
        m_bTriggeringOnDebugMessage = false;
    }

    m_DuplicateLineFilter.AddLine( { strText, uiMinimumDebugLevel, ucRed, ucGreen, ucBlue } );
    if ( g_pGame->GetConfig()->GetFilterDuplicateLogLinesEnabled() == false )
        m_DuplicateLineFilter.Flush();
    UpdateLogOutput();
}

void CScriptDebugging::UpdateLogOutput( void )
{
    SLogLine line;
    while( m_DuplicateLineFilter.PopOutputLine( line ) )
    {
        // Log it to the file if enough level
        if ( m_uiLogFileLevel >= line.uiMinimumDebugLevel )
        {
            PrintLog ( line.strText );
        }
        // Log to console
        CLogger::LogPrintf( "%s\n", line.strText.c_str () );
        // Tell the players
        Broadcast ( CDebugEchoPacket ( line.strText, line.uiMinimumDebugLevel, line.ucRed, line.ucGreen, line.ucBlue ), line.uiMinimumDebugLevel );
    }
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
