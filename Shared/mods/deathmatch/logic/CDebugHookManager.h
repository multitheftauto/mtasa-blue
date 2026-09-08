/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CDebugHookManager.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "lua/CLuaFunctionRef.h"
#include "Enums.h"
#include "lua/LuaCommon.h"

#ifdef MTA_CLIENT
    #define CElement CClientEntity
    #define CPlayer  CClientPlayer
#endif

class CLuaMain;
class CLuaArguments;
class CElement;
class CPlayer;
class CMapEvent;

struct SDebugHookCallInfo
{
    CLuaFunctionRef       functionRef;
    CLuaMain*             pLuaMain;
    CFastHashSet<SString> allowedNameMap;
};

enum class EArgType
{
    Password,
    Url,
    MaxArgs,
};

struct SMaskArgument
{
    EArgType argType;
    uint     index;
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
    CDebugHookManager();
    ~CDebugHookManager();
    bool AddDebugHook(EDebugHookType hookType, const CLuaFunctionRef& functionRef, const std::vector<SString>& allowedNameList);
    bool RemoveDebugHook(EDebugHookType hookType, const CLuaFunctionRef& functionRef);
    void OnLuaMainDestroy(CLuaMain* pLuaMain);

    bool OnPreFunction(lua_CFunction f, lua_State* luaVM, bool bAllowed);
    void OnPostFunction(lua_CFunction f, lua_State* luaVM);
    bool OnPreEvent(const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller);
    void OnPostEvent(const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller);

    // Modern decoupled event function hooks accepting direct VM and function reference
    bool OnPreEventFunction(const char* name, const CLuaArguments& args, CElement* source, CPlayer* caller, CLuaMain* functionLuaMain,
                            const CLuaFunctionRef& functionRef);
    void OnPostEventFunction(const char* name, const CLuaArguments& args, CElement* source, CPlayer* caller, CLuaMain* functionLuaMain,
                             const CLuaFunctionRef& functionRef);

    // Legacy overload for Server CMapEvent compatibility
    bool OnPreEventFunction(const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller, CMapEvent* pMapEvent);
    void OnPostEventFunction(const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller, CMapEvent* pMapEvent);

    // Fast inline presence checkers to eliminate function call overhead when no debug hooks are active
    inline bool HasPreEventHooks() const noexcept { return !m_PreEventHookList.empty(); }
    inline bool HasPostEventHooks() const noexcept { return !m_PostEventHookList.empty(); }
    inline bool HasPreFunctionHooks() const noexcept { return !m_PreFunctionHookList.empty(); }
    inline bool HasPostFunctionHooks() const noexcept { return !m_PostFunctionHookList.empty() || m_uiPostFunctionOverride; }
    inline bool HasPreEventFunctionHooks() const noexcept { return !m_PreEventFunctionHookList.empty(); }
    inline bool HasPostEventFunctionHooks() const noexcept { return !m_PostEventFunctionHookList.empty(); }

protected:
    void GetFunctionCallHookArguments(CLuaArguments& NewArguments, const SString& strName, lua_State* luaVM, bool bAllowed);
    void GetEventFunctionCallHookArguments(CLuaArguments& newArgs, const SString& name, const CLuaArguments& args, CElement* source, CPlayer* caller,
                                           CLuaMain* functionLuaMain, const CLuaFunctionRef& functionRef);
    void GetEventFunctionCallHookArguments(CLuaArguments& NewArguments, const SString& strName, const CLuaArguments& Arguments, CElement* pSource,
                                           CPlayer* pCaller, CMapEvent* pMapEvent);
    void GetEventCallHookArguments(CLuaArguments& NewArguments, const SString& strName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller);

    std::vector<SDebugHookCallInfo>& GetHookInfoListForType(EDebugHookType hookType);
    bool                             CallHook(const char* szName, const std::vector<SDebugHookCallInfo>& eventHookList, const CLuaArguments& Arguments,
                                              bool bNameMustBeExplicitlyAllowed = false);
    bool IsNameAllowed(const char* szName, const std::vector<SDebugHookCallInfo>& eventHookList, bool bNameMustBeExplicitlyAllowed = false);
    bool MustNameBeExplicitlyAllowed(const SString& strName);
    void MaybeMaskArgumentValues(const SString& strFunctionName, CLuaArguments& FunctionArguments);

    uint                                          m_uiPostFunctionOverride;
    std::vector<SDebugHookCallInfo>               m_PreEventHookList;
    std::vector<SDebugHookCallInfo>               m_PostEventHookList;
    std::vector<SDebugHookCallInfo>               m_PreFunctionHookList;
    std::vector<SDebugHookCallInfo>               m_PostFunctionHookList;
    std::vector<SDebugHookCallInfo>               m_PreEventFunctionHookList;
    std::vector<SDebugHookCallInfo>               m_PostEventFunctionHookList;
    std::map<SString, std::vector<SMaskArgument>> m_MaskArgumentsMap;
};
