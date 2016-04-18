/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CDebugHookManager.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifdef MTA_CLIENT
    #define CElement CClientEntity
    #define CPlayer CClientPlayer
#endif

struct SDebugHookCallInfo
{
    CLuaFunctionRef functionRef;
    CLuaMain* pLuaMain;
    CFastHashSet < SString > allowedNameMap;
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
    bool                AddDebugHook                ( EDebugHookType hookType, const CLuaFunctionRef& functionRef, const std::vector < SString >& allowedNameList );
    bool                RemoveDebugHook             ( EDebugHookType hookType, const CLuaFunctionRef& functionRef );
    void                OnLuaMainDestroy            ( CLuaMain* pLuaMain );

    bool                OnPreFunction               ( lua_CFunction f, lua_State* luaVM, bool bAllowed );
    void                OnPostFunction              ( lua_CFunction f, lua_State* luaVM );
    bool                OnPreEvent                  ( const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller );
    void                OnPostEvent                 ( const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller );

    bool                HasPostFunctionHooks        ( void ) const      { return !m_PostFunctionHookList.empty() || m_uiPostFunctionOverride; }

protected:
    std::vector < SDebugHookCallInfo >& GetHookInfoListForType ( EDebugHookType hookType );
    bool                CallHook                    ( const char* szName, const std::vector < SDebugHookCallInfo >& eventHookList, const CLuaArguments& Arguments, bool bNameMustBeExplicitlyAllowed = false );
    bool                IsNameAllowed               ( const char* szName, const std::vector < SDebugHookCallInfo >& eventHookList, bool bNameMustBeExplicitlyAllowed = false );
    bool                MustNameBeExplicitlyAllowed ( const SString& strName );

    uint                                m_uiPostFunctionOverride;
    std::vector < SDebugHookCallInfo >  m_PreEventHookList;
    std::vector < SDebugHookCallInfo >  m_PostEventHookList;
    std::vector < SDebugHookCallInfo >  m_PreFunctionHookList;
    std::vector < SDebugHookCallInfo >  m_PostFunctionHookList;
};
