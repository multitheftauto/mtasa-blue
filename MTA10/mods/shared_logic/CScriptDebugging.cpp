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

FILE* CScriptDebugging::m_pLogFile;

CScriptDebugging::CScriptDebugging ( CLuaManager* pLuaManager )
{
    m_pLuaManager = pLuaManager;
    m_uiLogFileLevel = 0;
    m_pLogFile = NULL;
    m_bTriggeringOnClientDebugMessage = false;
    m_flushTimerHandle = NULL;
}

CScriptDebugging::~CScriptDebugging ( void )
{
    // Flush any pending duplicate loggings
    m_DuplicateLineFilter.Flush();
    UpdateLogOutput();

    // Close the previously loaded file
    if ( m_pLogFile )
    {
        fprintf ( m_pLogFile, "INFO: Logging to this file ended\n" );

        // if we have a flush timer
        if ( m_flushTimerHandle != NULL )
        {
            // delete our flush timer
            DeleteTimerQueueTimer ( NULL, m_flushTimerHandle, INVALID_HANDLE_VALUE );   // INVALID_HANDLE_VALUE = wait for running callbacks to finish
        }
        fclose ( m_pLogFile );
        m_pLogFile = NULL;
    }
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


// Handle filename/line number in string
void CScriptDebugging::LogPCallError( lua_State* luaVM, const SString& strRes, bool bInitialCall )
{   
    std::vector < SString > vecSplit;
    strRes.Split( ":", vecSplit );
                
    if ( vecSplit.size() >= 3 )
    {
        // File+line info present
        SString strFile = vecSplit[ 0 ];
        int     iLine   = atoi( vecSplit[ 1 ] );
        
        // Get the message string (we cannot use vecSplit here as the message itself could contain ':')
        auto pos = strRes.find ( ':', vecSplit[0].length () + vecSplit[1].length () ) ;
        SString strMsg = strRes.SubStr ( pos+2 );
        
        if ( iLine == 0 && bInitialCall )
        {
            // Location hint for compiled scripts
            LogError ( SLuaDebugInfo( strFile, iLine ), "(global scope) %s", *strMsg );
        }
        else
            LogError ( SLuaDebugInfo( strFile, iLine ), "%s", *strMsg );
    }
    else
    {
        // File+line info not present
        LogError( luaVM, "%s", strRes.c_str () );
    }
}


void CALLBACK TimerProc( void* lpParametar, BOOLEAN TimerOrWaitFired )
{
    // Got a logfile?
    if ( CScriptDebugging::m_pLogFile != NULL )
    {
        // flush our log file
        fflush ( (FILE*)CScriptDebugging::m_pLogFile );
    }
}


bool CScriptDebugging::SetLogfile ( const char* szFilename, unsigned int uiLevel )
{
    assert ( szFilename );

    // Close the previously loaded file
    if ( m_pLogFile )
    {
        fprintf ( m_pLogFile, "INFO: Logging to this file ended\n" );
        // if we have a flush timer
        if ( m_flushTimerHandle != NULL )
        {
            // delete our flush timer
            DeleteTimerQueueTimer ( NULL, m_flushTimerHandle, INVALID_HANDLE_VALUE );   // INVALID_HANDLE_VALUE = wait for running callbacks to finish
        }
        fclose ( m_pLogFile );
        m_pLogFile = NULL;
    }

    // Apply log size limit
    uint uiMaxSizeKB = 0;
    g_pCore->GetCVars ()->Get ( "max_clientscript_log_kb", uiMaxSizeKB );
    CycleFile( szFilename, uiMaxSizeKB );


    // Try to load the new file
    FILE* pFile = fopen ( szFilename, "a+" );
    if ( pFile )
    {
        // [2014-07-09 14:39:31] WARNING: Bad argument @ 'setElementPosition' [Expected element at argument 1, got nil] [string "return addEventHandler("onClientRender", ro..."]
        // length = 158

        // set our buffer size

        // assumed message length of 158

        // if we pulse at 150FPS (unrealistic but whatever)
        // that's 1 update every 6.66666666ms
        // pulse rate is 50 so 50 / 6.6666666 = 7.5 (close enough)
        // if we are doing 5 error messages a pulse that's 5 * 7.5
        // 5 * 7.5 = 37.5
        // we need room for at least 37.5 messages in this buffer
        // round 37.5 to 38 because we can't have half a message
        // 8 * 256 bytes = 6004B
        // round 6004 up to the nearest divisible by 1024 = 6144
        // we have our buffer size.
        setvbuf ( pFile , NULL , _IOFBF , 6144 );

        // Set the new pointer and level and return true
        m_uiLogFileLevel = uiLevel;
        m_pLogFile = pFile;

        // Create a timer
        ::CreateTimerQueueTimer( &m_flushTimerHandle, NULL, TimerProc, NULL, 50, 50, WT_EXECUTEINTIMERTHREAD );
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

    if ( !m_bTriggeringOnClientDebugMessage )
    {
        m_bTriggeringOnClientDebugMessage = true;

        // Prepare onClientDebugMessage
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
        
        // Call onClientDebugMessage
        g_pClientGame->GetRootEntity ( )->CallEvent ( "onClientDebugMessage", Arguments, false );

        m_bTriggeringOnClientDebugMessage = false;
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

    m_DuplicateLineFilter.AddLine( { strText, uiMinimumDebugLevel, ucRed, ucGreen, ucBlue } );
    if ( g_pCore->GetCVars ()->GetValue < bool > ( "filter_duplicate_log_lines" ) == false )
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
    #ifdef MTA_DEBUG
        if ( !g_pCore->IsDebugVisible () ) return;
    #endif
        g_pCore->DebugEchoColor ( line.strText, line.ucRed, line.ucGreen, line.ucBlue );
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
        SString strInput;

        strftime ( szBuffer, 32, "[%Y-%m-%d %H:%M:%S]", localtime ( &timeNow ) );
        strInput.Format ( "%s %s\n", szBuffer, szText );

        fwrite( strInput.c_str(), strInput.length(), 1, m_pLogFile );
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
