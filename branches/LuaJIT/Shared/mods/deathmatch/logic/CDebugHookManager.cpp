/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CDebugHookManager.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#ifdef MTA_CLIENT
    #define DECLARE_PROFILER_SECTION_CDebugHookManager
    #include "profiler/SharedUtil.Profiler.h"
    #define g_pGame g_pClientGame
#else
    #define DECLARE_PROFILER_SECTION(tag)
#endif

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::CDebugHookManager
//
//
//
///////////////////////////////////////////////////////////////
CDebugHookManager::CDebugHookManager( void )
{
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::~CDebugHookManager
//
//
//
///////////////////////////////////////////////////////////////
CDebugHookManager::~CDebugHookManager( void )
{
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::GetHookInfoListForType
//
//
//
///////////////////////////////////////////////////////////////
std::vector < SDebugHookCallInfo >& CDebugHookManager::GetHookInfoListForType( EDebugHookType hookType )
{
    if ( hookType == EDebugHook::PRE_EVENT )
        return m_PreEventHookList;
    if ( hookType == EDebugHook::POST_EVENT )
        return m_PostEventHookList;
    if ( hookType == EDebugHook::PRE_FUNCTION )
        return m_PreFunctionHookList;
    dassert( hookType == EDebugHook::POST_FUNCTION );
    return m_PostFunctionHookList;
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::AddDebugHook
//
// Returns true if hook was added
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::AddDebugHook( EDebugHookType hookType, const CLuaFunctionRef& functionRef, const std::vector < SString >& allowedNameList )
{
    std::vector < SDebugHookCallInfo >& hookInfoList = GetHookInfoListForType( hookType );
    for( std::vector < SDebugHookCallInfo >::iterator iter = hookInfoList.begin() ; iter != hookInfoList.end() ; ++iter )
    {
        if ( (*iter).functionRef == functionRef )
            return false;
    }

    SDebugHookCallInfo info;
    info.functionRef = functionRef;
    info.pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( functionRef.GetLuaVM() );
    if ( !info.pLuaMain )
        return false;

    for( uint i = 0 ; i < allowedNameList.size() ; i++ )
        MapInsert( info.allowedNameMap, allowedNameList[i] );

    hookInfoList.push_back( info );
    return true;
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::RemoveDebugHook
//
// Returns true if hook was removed
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::RemoveDebugHook( EDebugHookType hookType, const CLuaFunctionRef& functionRef )
{
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( functionRef.GetLuaVM() );

    std::vector < SDebugHookCallInfo >& hookInfoList = GetHookInfoListForType( hookType );
    for( std::vector < SDebugHookCallInfo >::iterator iter = hookInfoList.begin() ; iter != hookInfoList.end() ; ++iter )
    {
        if ( (*iter).pLuaMain == pLuaMain && (*iter).functionRef == functionRef )
        {
            hookInfoList.erase( iter );
            return true;
        }
    }

    return false;
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::OnLuaMainDestroy
//
// When a Lua VM is stopped
//
///////////////////////////////////////////////////////////////
void CDebugHookManager::OnLuaMainDestroy( CLuaMain* pLuaMain )
{
    for( uint hookType = EDebugHook::PRE_EVENT ; hookType <= EDebugHook::POST_FUNCTION ; hookType++ )
    {
        std::vector < SDebugHookCallInfo >& hookInfoList = GetHookInfoListForType( (EDebugHookType)hookType );
        for( uint i = 0 ; i < hookInfoList.size() ; )
        {
            if ( hookInfoList[i].pLuaMain == pLuaMain )
                ListRemoveIndex( hookInfoList, i );
            else
                i++;
        }
    }
}


///////////////////////////////////////////////////////////////
//
// GetDebugInfo
//
// Get current Lua source file and line number
//
///////////////////////////////////////////////////////////////
void GetDebugInfo( lua_State* luaVM, lua_Debug& debugInfo, const char*& szFilename, int& iLineNumber )
{
    if ( luaVM && lua_getstack ( luaVM, 1, &debugInfo ) )
    {
        lua_getinfo( luaVM, "nlS", &debugInfo );

        // Make sure this function isn't defined in a string
        if ( debugInfo.source[0] == '@' )
        {
            szFilename = debugInfo.source;
            iLineNumber = debugInfo.currentline != -1 ? debugInfo.currentline : debugInfo.linedefined;
        }
        else
        {
            szFilename = debugInfo.short_src;
        }

        // Remove path
        if ( const char* szNext = strrchr( szFilename, '\\' ) )
            szFilename = szNext + 1;
        if ( const char* szNext = strrchr( szFilename, '/' ) )
            szFilename = szNext + 1;
    }
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::OnPreFunction
//
// Called before a MTA function is called
//
///////////////////////////////////////////////////////////////
void CDebugHookManager::OnPreFunction( lua_CFunction f, lua_State* luaVM, bool bAllowed )
{
    DECLARE_PROFILER_SECTION( OnPreFunction )

    if ( m_PreFunctionHookList.empty() )
        return;

    CLuaCFunction* pFunction = CLuaCFunctions::GetFunction( f );
    dassert( pFunction );
    if ( !pFunction )
        return;

    const SString& strName = pFunction->GetName();

    // Check if name is not used
    if ( !IsNameAllowed( strName, m_PreFunctionHookList ) )
        return;

    // Don't trace add/removeDebugHook
    if ( strName.EndsWith( "DebugHook" ) )
        return;

    // Get file/line number
    const char* szFilename = "";
    int iLineNumber = 0;
    lua_Debug debugInfo;
    GetDebugInfo( luaVM, debugInfo, szFilename, iLineNumber );

    CLuaMain* pSourceLuaMain = g_pGame->GetScriptDebugging()->GetTopLuaMain();
    CResource* pSourceResource = pSourceLuaMain ? pSourceLuaMain->GetResource() : NULL;

    CLuaArguments NewArguments;
    if ( pSourceResource )
        NewArguments.PushResource( pSourceResource );
    else
        NewArguments.PushNil();
    NewArguments.PushString( strName );
    NewArguments.PushBoolean( bAllowed );
    NewArguments.PushString( szFilename );
    NewArguments.PushNumber( iLineNumber );

    CLuaArguments FunctionArguments;
    FunctionArguments.ReadArguments( luaVM );
    NewArguments.PushArguments( FunctionArguments );

    CallHook( strName, m_PreFunctionHookList, NewArguments );
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::OnPostFunction
//
// Called after a MTA function is called
//
///////////////////////////////////////////////////////////////
void CDebugHookManager::OnPostFunction( lua_CFunction f, lua_State* luaVM )
{
    DECLARE_PROFILER_SECTION( OnPostFunction )

    if ( m_PostFunctionHookList.empty() )
        return;

    CLuaCFunction* pFunction = CLuaCFunctions::GetFunction( f );
    dassert( pFunction );
    if ( !pFunction )
        return;

    const SString& strName = pFunction->GetName();

    // Check if name is not used
    if ( !IsNameAllowed( strName, m_PostFunctionHookList ) )
        return;

    // Don't trace add/removeDebugHook
    if ( strName.EndsWith( "DebugHook" ) )
        return;

    // Get file/line number
    const char* szFilename = "";
    int iLineNumber = 0;
    lua_Debug debugInfo;
    GetDebugInfo( luaVM, debugInfo, szFilename, iLineNumber );

    CLuaMain* pSourceLuaMain = g_pGame->GetScriptDebugging()->GetTopLuaMain();
    CResource* pSourceResource = pSourceLuaMain ? pSourceLuaMain->GetResource() : NULL;

    CLuaArguments NewArguments;
    if ( pSourceResource )
        NewArguments.PushResource( pSourceResource );
    else
        NewArguments.PushNil();
    NewArguments.PushString( strName );
    NewArguments.PushBoolean( true );
    NewArguments.PushString( szFilename );
    NewArguments.PushNumber( iLineNumber );

    CLuaArguments FunctionArguments;
    FunctionArguments.ReadArguments( luaVM );
    NewArguments.PushArguments( FunctionArguments );

    CallHook( strName, m_PostFunctionHookList, NewArguments );
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::OnPreEvent
//
// Called before a MTA event is triggered
//
///////////////////////////////////////////////////////////////
void CDebugHookManager::OnPreEvent( const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller )
{
    if ( m_PreEventHookList.empty() )
        return;

    // Check if name is not used
    if ( !IsNameAllowed( szName, m_PreEventHookList ) )
        return;

    CLuaMain* pSourceLuaMain = g_pGame->GetScriptDebugging()->GetTopLuaMain();
    CResource* pSourceResource = pSourceLuaMain ? pSourceLuaMain->GetResource() : NULL;

    // Get file/line number
    const char* szFilename = "";
    int iLineNumber = 0;
    lua_Debug debugInfo;
    lua_State* luaVM = pSourceLuaMain ? pSourceLuaMain->GetVM() : NULL;
    if ( luaVM )
        GetDebugInfo( luaVM, debugInfo, szFilename, iLineNumber );

    CLuaArguments NewArguments;
    if ( pSourceResource )
        NewArguments.PushResource( pSourceResource );
    else
        NewArguments.PushNil();
    NewArguments.PushString( szName );
    NewArguments.PushElement( pSource );
    NewArguments.PushElement( pCaller );
    NewArguments.PushString( szFilename );
    NewArguments.PushNumber( iLineNumber );
    NewArguments.PushArguments( Arguments );

    CallHook( szName, m_PreEventHookList, NewArguments );
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::OnPostEvent
//
// Called after a MTA event is triggered
//
///////////////////////////////////////////////////////////////
void CDebugHookManager::OnPostEvent( const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller )
{
    if ( m_PostEventHookList.empty() )
        return;

    // Check if name is not used
    if ( !IsNameAllowed( szName, m_PostEventHookList ) )
        return;

    CLuaMain* pSourceLuaMain = g_pGame->GetScriptDebugging()->GetTopLuaMain();
    CResource* pSourceResource = pSourceLuaMain ? pSourceLuaMain->GetResource() : NULL;

    // Get file/line number
    const char* szFilename = "";
    int iLineNumber = 0;
    lua_Debug debugInfo;
    lua_State* luaVM = pSourceLuaMain ? pSourceLuaMain->GetVM() : NULL;
    if ( luaVM )
        GetDebugInfo( luaVM, debugInfo, szFilename, iLineNumber );

    CLuaArguments NewArguments;
    if ( pSourceResource )
        NewArguments.PushResource( pSourceResource );
    else
        NewArguments.PushNil();
    NewArguments.PushString( szName );
    NewArguments.PushElement( pSource );
    NewArguments.PushElement( pCaller );
    NewArguments.PushString( szFilename );
    NewArguments.PushNumber( iLineNumber );
    NewArguments.PushArguments( Arguments );

    CallHook( szName, m_PostEventHookList, NewArguments );
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::IsNameAllowed
//
// Returns true if there is a debughook which handles the name
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::IsNameAllowed( const char* szName, const std::vector < SDebugHookCallInfo >& eventHookList )
{
    for( uint i = 0 ; i < eventHookList.size() ; i++ )
    {
        const SDebugHookCallInfo& info = eventHookList[i];

        if ( info.allowedNameMap.empty() )
            return true;    // All names allowed

        if ( MapContains( info.allowedNameMap, szName ) )
            return true;    // Name allowed
    }
    return false;
}


///////////////////////////////////////////////////////////////
//
// CDebugHookManager::CallHook
//
//
//
///////////////////////////////////////////////////////////////
void CDebugHookManager::CallHook( const char* szName, const std::vector < SDebugHookCallInfo >& eventHookList, const CLuaArguments& Arguments )
{
    static bool bRecurse = false;
    if ( bRecurse )
        return;
    bRecurse = true;

    for( uint i = 0 ; i < eventHookList.size() ; i++ )
    {
        const SDebugHookCallInfo& info = eventHookList[i];

        if ( !info.allowedNameMap.empty() )
        {
            if ( !MapContains( info.allowedNameMap, szName ) )
                continue;
        }

        lua_State* pState = info.pLuaMain->GetVirtualMachine();

        // Save script MTA globals in case hook messes with them
        lua_getglobal ( pState, "source" );
        CLuaArgument OldSource ( pState, -1 );
        lua_pop( pState, 1 );

        lua_getglobal ( pState, "this" );
        CLuaArgument OldThis ( pState, -1 );
        lua_pop( pState, 1 );

        lua_getglobal ( pState, "sourceResource" );
        CLuaArgument OldResource ( pState, -1 );
        lua_pop( pState, 1 );

        lua_getglobal ( pState, "sourceResourceRoot" );
        CLuaArgument OldResourceRoot ( pState, -1 );
        lua_pop( pState, 1 );

        lua_getglobal ( pState, "eventName" );
        CLuaArgument OldEventName ( pState, -1 );
        lua_pop( pState, 1 );

        lua_getglobal ( pState, "client" );
        CLuaArgument OldClient ( pState, -1 );
        lua_pop( pState, 1 );

        Arguments.Call ( info.pLuaMain, info.functionRef );
        // Note: info could be invalid now

        // Reset the globals on that VM
        OldSource.Push ( pState );
        lua_setglobal ( pState, "source" );

        OldThis.Push ( pState );
        lua_setglobal ( pState, "this" );                

        OldResource.Push ( pState );
        lua_setglobal ( pState, "sourceResource" );

        OldResourceRoot.Push ( pState );
        lua_setglobal ( pState, "sourceResourceRoot" );

        OldEventName.Push ( pState );
        lua_setglobal ( pState, "eventName" );

        OldClient.Push ( pState );
        lua_setglobal ( pState, "client" );
    }

    bRecurse = false;
}
