/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaTimer.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaTimer;

#pragma once

// Define includes
#include "lua/LuaCommon.h"
#include "lua/CLuaArguments.h"

#define LUA_TIMER_MIN_INTERVAL      0

class CLuaTimer
{
public:
    CLuaTimer(const CLuaFunctionRef& iLuaFunction, const CLuaArguments& Arguments);
    ~CLuaTimer();

    void RemoveScriptID();

    CTickCount GetStartTime() const { return m_llStartTime; };
    void       SetStartTime(CTickCount llStartTime) { m_llStartTime = llStartTime; };

    CTickCount GetDelay() const { return m_llDelay; };
    void       SetDelay(CTickCount llDelay) { m_llDelay = llDelay; };

    unsigned int GetRepeats() const { return m_uiRepeats; };
    void         SetRepeats(unsigned int uiRepeats) { m_uiRepeats = uiRepeats; }

    void ExecuteTimer(class CLuaMain* pLuaMain);

    CTickCount GetTimeLeft();

    uint                 GetScriptID() const { return m_uiScriptID; }
    const SLuaDebugInfo& GetLuaDebugInfo() { return m_LuaDebugInfo; }
    void                 SetLuaDebugInfo(const SLuaDebugInfo& luaDebugInfo) { m_LuaDebugInfo = luaDebugInfo; }

private:
    CLuaFunctionRef m_iLuaFunction;
    CLuaArguments   m_Arguments;
    CTickCount      m_llStartTime;
    CTickCount      m_llDelay;
    unsigned int    m_uiRepeats;
    uint            m_uiScriptID;
    SLuaDebugInfo   m_LuaDebugInfo;
};
