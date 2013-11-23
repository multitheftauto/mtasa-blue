/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CDebugHookManager.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

struct SDebugHookCallInfo
{
    CLuaFunctionRef functionRef;
    CLuaMain* pLuaMain;
};

///////////////////////////////////////////////////////////////
//
// CDebugHookManager
//
// Manage user added hooks to help debug script happenings
//
///////////////////////////////////////////////////////////////
class CDebugHookManager
{
public:
    ZERO_ON_NEW
                        CDebugHookManager           ( void );
                        ~CDebugHookManager          ( void );
    bool                AddDebugHook                ( EDebugHookType hookType, const CLuaFunctionRef& functionRef );
    bool                RemoveDebugHook             ( EDebugHookType hookType, const CLuaFunctionRef& functionRef );
    void                OnLuaMainDestroy            ( CLuaMain* pLuaMain );

    void                OnPreFunction               ( lua_CFunction f, lua_State* luaVM, bool bAllowed );
    void                OnPostFunction              ( lua_CFunction f, lua_State* luaVM );
    void                OnPreEvent                  ( const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller );
    void                OnPostEvent                 ( const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller );

    bool                HasPostFunctionHooks        ( void ) const      { return !m_PostFunctionHookList.empty(); }

protected:
    std::vector < SDebugHookCallInfo >& GetHookInfoListForType ( EDebugHookType hookType );
    void                CallHook                    ( const std::vector < SDebugHookCallInfo >& eventHookList, const CLuaArguments& Arguments );

    std::vector < SDebugHookCallInfo >  m_PreEventHookList;
    std::vector < SDebugHookCallInfo >  m_PostEventHookList;
    std::vector < SDebugHookCallInfo >  m_PreFunctionHookList;
    std::vector < SDebugHookCallInfo >  m_PostFunctionHookList;
};
