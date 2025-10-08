/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CKeyBinds.h
 *  PURPOSE:     Server keybind manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "lua/LuaCommon.h"
#include "lua/CLuaArguments.h"
#include <list>

enum eKeyBindType
{
    KEY_BIND_FUNCTION = 0,
    KEY_BIND_CONTROL_FUNCTION,
    KEY_BIND_UNDEFINED,
};

struct SBindableKey
{
    const char* szKey;
};

struct SBindableGTAControl
{
    const char* szControl;
};

class CKeyBind
{
public:
    CKeyBind()
    {
        boundKey = NULL;
        luaMain = NULL;
        beingDeleted = false;
    }
    virtual ~CKeyBind() {}
    bool IsBeingDeleted() { return beingDeleted; }

    const SBindableKey*  boundKey;
    CLuaMain*            luaMain;
    bool                 beingDeleted;
    virtual eKeyBindType GetType() = 0;
};

class CKeyBindWithState : public CKeyBind
{
public:
    CKeyBindWithState() { bHitState = true; }
    bool bHitState;
};

class CFunctionBind
{
public:
    CFunctionBind() {}
    ~CFunctionBind() {}
    CLuaFunctionRef m_iLuaFunction;
    CLuaArguments   m_Arguments;
};

class CKeyFunctionBind : public CKeyBindWithState, public CFunctionBind
{
public:
    eKeyBindType GetType() { return KEY_BIND_FUNCTION; }
};

class CControlFunctionBind : public CKeyBindWithState, public CFunctionBind
{
public:
    eKeyBindType               GetType() { return KEY_BIND_CONTROL_FUNCTION; }
    const SBindableGTAControl* boundControl;
};

class CKeyBinds
{
public:
    CKeyBinds(class CPlayer* pPlayer);
    ~CKeyBinds();

    static const SBindableKey*        GetBindableFromKey(const char* szKey);
    static const SBindableGTAControl* GetBindableFromControl(const char* szControl);

    // Basic funcs
    void Add(CKeyBind* pKeyBind);
    void Clear(eKeyBindType bindType = KEY_BIND_UNDEFINED);
    void Call(CKeyBind* pKeyBind);
    bool ProcessKey(const char* szKey, bool bHitState, eKeyBindType bindType);

    std::list<CKeyBind*>::iterator IterBegin() { return m_List.begin(); }
    std::list<CKeyBind*>::iterator IterEnd() { return m_List.end(); }

    // Key-function bind funcs
    bool AddKeyFunction(const char* szKey, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments);
    bool AddKeyFunction(const SBindableKey* pKey, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments);
    bool RemoveKeyFunction(const char* szKey, CLuaMain* pLuaMain, bool bCheckHitState = false, bool bHitState = true,
                           const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    bool KeyFunctionExists(const char* szKey, CLuaMain* pLuaMain = NULL, bool bCheckHitState = false, bool bHitState = true,
                           const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());

    // Control-function bind funcs
    bool AddControlFunction(const char* szControl, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments);
    bool AddControlFunction(const SBindableGTAControl* pControl, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction,
                            CLuaArguments& Arguments);
    bool RemoveControlFunction(const char* szControl, CLuaMain* pLuaMain, bool bCheckHitState = false, bool bHitState = true,
                               const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    bool ControlFunctionExists(const char* szControl, CLuaMain* pLuaMain = NULL, bool bCheckHitState = false, bool bHitState = true,
                               const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());

    void RemoveAllKeys(CLuaMain* pLuaMain);

    static bool IsMouse(const SBindableKey* pKey);
    void        RemoveDeletedBinds();

protected:
    bool Remove(CKeyBind* pKeyBind);

    CPlayer*             m_pPlayer;
    std::list<CKeyBind*> m_List;
    bool                 m_bProcessingKey;
};
