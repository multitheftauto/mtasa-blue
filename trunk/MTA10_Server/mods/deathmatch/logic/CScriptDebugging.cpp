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


void CScriptDebugging::LogCustom ( lua_State* luaVM, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, const char* szFormat, ... )
{
    assert ( szFormat );

    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    LogString ( "", GetLuaDebugInfo( luaVM ), szBuffer, 0, ucRed, ucGreen, ucBlue );
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
    LogString ( "INFO: ", GetLuaDebugInfo( luaVM ), szBuffer, 3 );
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
    LogString ( "WARNING: ", GetLuaDebugInfo( luaVM ), szBuffer, 2 );
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
    LogString ( "ERROR: ", GetLuaDebugInfo( luaVM ), szBuffer, 1 );
}


void CScriptDebugging::LogWarning ( const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ... )
{
    assert ( szFormat );

    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Log it
    LogString ( "WARNING: ", luaDebugInfo, szBuffer, 2 );
}


void CScriptDebugging::LogError ( const SLuaDebugInfo& luaDebugInfo, const char* szFormat, ... )
{
    assert ( szFormat );

    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Log it
    LogString ( "ERROR: ", luaDebugInfo, szBuffer, 1 );
}


void CScriptDebugging::LogBadPointer ( lua_State* luaVM, const char* szArgumentType, unsigned int uiArgument )
{
    assert ( szArgumentType );
    LogWarning ( luaVM, "Bad '%s' pointer @ '%s'(%u)", szArgumentType, lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), uiArgument );
}


void CScriptDebugging::LogBadType ( lua_State* luaVM )
{
    LogWarning ( luaVM, "Bad argument @ '%s'", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
}


void CScriptDebugging::LogBadAccess ( lua_State* luaVM )
{
    LogWarning ( luaVM, "Access denied @ '%s'", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
}


void CScriptDebugging::LogCustom ( lua_State* luaVM, const char* szMessage )
{
    assert ( szMessage );
    LogWarning ( luaVM, "%s", szMessage );
}


// Handle filename/line number in string
void CScriptDebugging::LogPCallError( lua_State* luaVM, const SString& strRes, const SString& strNiceFilename )
{   
    std::vector < SString > vecSplit;
    strRes.Split( ":", vecSplit );
                
    if ( vecSplit.size() >= 3 )
    {
        // File+line info present
        SString strFile = vecSplit[ 0 ];
        int     iLine   = atoi( vecSplit[ 1 ] );
        SString strMsg  = vecSplit[2].SubStr( 1 );
                    
        LogError ( SLuaDebugInfo( strFile, iLine ), "%s", *strMsg );
    }
    else
    {
        // File+line info not present
        if ( !strNiceFilename.empty() && !strRes.ContainsI( ExtractFilename( strNiceFilename ) ) )
        {
            // Add filename to error message, if not already present
            LogError( luaVM, "%s (global scope) - %s", *strNiceFilename, *strRes );
        }
        else
        {
            LogError( luaVM, "%s", strRes.c_str () );
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


//
// Get best debug info we possibly can from the relevent lua state
//
const SLuaDebugInfo& CScriptDebugging::GetLuaDebugInfo( lua_State * luaVM )
{
    static SLuaDebugInfo scriptDebugInfo;
    scriptDebugInfo = SLuaDebugInfo();

    // Get a VM from somewhere
    if ( !luaVM && !m_LuaMainStack.empty () )
        luaVM = m_LuaMainStack.back ()->GetVM ();

    // Lua oop found at level 4 added one just in case it somehow ends up deeper due to nested calls
    for ( int level = 1; level <= 5; level++ )
    {
        lua_Debug debugInfo;
        if ( luaVM && lua_getstack ( luaVM, level, &debugInfo ) )
        {
            lua_getinfo ( luaVM, "nlS", &debugInfo );
            // Lua oop handlers get marked as "C", ignore these as the information we want is further up the stack (typically level 4+)
            if ( strcmp(debugInfo.source, "=[C]") != 0 )
            {
                 // Make sure this function isn't defined in a string (eg: from runcode)
                if ( debugInfo.source[0] == '@' )
                {
                    // Get and store the location of the debug message
                    scriptDebugInfo.strFile = debugInfo.source + 1;
                    scriptDebugInfo.iLine = debugInfo.currentline;
                    scriptDebugInfo.infoType = DEBUG_INFO_FILE_AND_LINE;
                    // Stop here as we now have the best info
                    break;
                }
                else
                {
                    scriptDebugInfo.strShortSrc = debugInfo.short_src;
                    scriptDebugInfo.infoType = DEBUG_INFO_SHORT_SRC;
                    // Try other levels to see if we can get the calling file
                }
            }
        }
        else
        {
            // Use saved info if set
            if ( m_SavedLuaDebugInfo.infoType != DEBUG_INFO_NONE )
            {
                scriptDebugInfo = m_SavedLuaDebugInfo;
            }
            // No point in trying other levels as lua_getstack will fail
            break;
        }
    }

    return scriptDebugInfo;
}


//
// Make full log message from components
//
SString CScriptDebugging::ComposeErrorMessage( const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage )
{
    SString strText;

    // 1st part - if not empty, it should have a space at the end
    strText = szPrePend;

    // 2nd part if set
    if ( luaDebugInfo.infoType == DEBUG_INFO_FILE_AND_LINE )
    {
        if ( luaDebugInfo.iLine == INVALID_LINE_NUMBER )
            strText += SString( "%s: ", *luaDebugInfo.strFile );
        else
            strText += SString( "%s:%d: ", *luaDebugInfo.strFile, luaDebugInfo.iLine );
    }

    // 3rd part
    strText += szMessage;

    // 5th part - just kidding. This is actually the 4th part!
    if ( !luaDebugInfo.strShortSrc.empty() )
    {
        if ( luaDebugInfo.strShortSrc.BeginsWith( "[" ) )
            strText += SString( " %s", *luaDebugInfo.strShortSrc );
        else
            strText += SString( " [%s]", *luaDebugInfo.strShortSrc );
    }

    return strText;
}


void CScriptDebugging::LogString ( const char* szPrePend, const SLuaDebugInfo& luaDebugInfo, const char* szMessage, unsigned int uiMinimumDebugLevel, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    SString strText = ComposeErrorMessage( szPrePend, luaDebugInfo, szMessage );

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

#if 0
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
#endif

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

CLuaMain* CScriptDebugging::GetTopLuaMain ( void )
{
    if ( !m_LuaMainStack.empty () )
        return m_LuaMainStack.back();
    return NULL;
}
