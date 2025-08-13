/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CScriptKeyBinds.h
 *  PURPOSE:     Header for script key binds class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "lua/CLuaMain.h"
#include <list>

enum eScriptKeyBindType
{
    SCRIPT_KEY_BIND_FUNCTION = 0,
    SCRIPT_KEY_BIND_CONTROL_FUNCTION,
    SCRIPT_KEY_BIND_UNDEFINED,
};

struct SScriptBindableKey
{
    const char* szKey;
};

struct SScriptBindableGTAControl
{
    const char* szControl;
};

class CScriptKeyBind
{
public:
    CScriptKeyBind() : boundKey(NULL), luaMain(NULL), beingDeleted(false) {}
    virtual ~CScriptKeyBind() {}
    bool                       IsBeingDeleted() { return beingDeleted; }
    const SScriptBindableKey*  boundKey;
    CLuaMain*                  luaMain;
    bool                       beingDeleted;
    virtual eScriptKeyBindType GetType() = 0;
};

class CScriptKeyBindWithState : public CScriptKeyBind
{
public:
    CScriptKeyBindWithState() { bHitState = true; }
    bool bHitState;
};

class CScriptFunctionBind
{
public:
    CLuaFunctionRef m_iLuaFunction;
    CLuaArguments   m_Arguments;
};

class CScriptKeyFunctionBind : public CScriptKeyBindWithState, public CScriptFunctionBind
{
public:
    eScriptKeyBindType GetType() { return SCRIPT_KEY_BIND_FUNCTION; }
};

class CScriptControlFunctionBind : public CScriptKeyBindWithState, public CScriptFunctionBind
{
public:
    eScriptKeyBindType               GetType() { return SCRIPT_KEY_BIND_CONTROL_FUNCTION; }
    const SScriptBindableGTAControl* boundControl;
};

class CScriptKeyBinds
{
public:
    CScriptKeyBinds() : m_bProcessingKey(false) {}
    ~CScriptKeyBinds();

    static const SScriptBindableKey*        GetBindableFromKey(const char* szKey);
    static const SScriptBindableGTAControl* GetBindableFromControl(const char* szControl);

    // Basic funcs
    void Add(CScriptKeyBind* pKeyBind);
    void Clear(eScriptKeyBindType bindType = SCRIPT_KEY_BIND_UNDEFINED);
    void Call(CScriptKeyBind* pKeyBind);
    bool ProcessKey(const char* szKey, bool bHitState, eScriptKeyBindType bindTypeconst);

    std::list<CScriptKeyBind*>::iterator IterBegin() { return m_List.begin(); }
    std::list<CScriptKeyBind*>::iterator IterEnd() { return m_List.end(); }

    // Key-function bind funcs
    bool AddKeyFunction(const char* szKey, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments);
    bool AddKeyFunction(const SScriptBindableKey* pKey, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments);
    bool RemoveKeyFunction(const char* szKey, CLuaMain* pLuaMain, bool bCheckHitState = false, bool bHitState = true,
                           const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    bool RemoveKeyFunction(const SScriptBindableKey* pKey, CLuaMain* pLuaMain, bool bCheckHitState = false, bool bHitState = true,
                           const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    bool KeyFunctionExists(const char* szKey, CLuaMain* pLuaMain = NULL, bool bCheckHitState = false, bool bHitState = true,
                           const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    bool KeyFunctionExists(const SScriptBindableKey* pKey, CLuaMain* pLuaMain = NULL, bool bCheckHitState = false, bool bHitState = true,
                           const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());

    // Control-function bind funcs
    bool AddControlFunction(const char* szControl, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments);
    bool AddControlFunction(const SScriptBindableGTAControl* pControl, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction,
                            CLuaArguments& Arguments);
    bool RemoveControlFunction(const char* szControl, CLuaMain* pLuaMain, bool bCheckHitState = false, bool bHitState = true,
                               const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    bool RemoveControlFunction(const SScriptBindableGTAControl* pControl, CLuaMain* pLuaMain, bool bCheckHitState = false, bool bHitState = true,
                               const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    bool ControlFunctionExists(const char* szControl, CLuaMain* pLuaMain = NULL, bool bCheckHitState = false, bool bHitState = true,
                               const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    bool ControlFunctionExists(const SScriptBindableGTAControl* pControl, CLuaMain* pLuaMain = NULL, bool bCheckHitState = false, bool bHitState = true,
                               const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());

    void RemoveAllKeys(CLuaMain* pLuaMain);

    static bool IsMouse(const SScriptBindableKey* pKey);
    void        RemoveDeletedBinds();

protected:
    std::list<CScriptKeyBind*> m_List;
    bool                       m_bProcessingKey;
};
