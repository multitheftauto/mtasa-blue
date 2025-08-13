/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaTimer.cpp
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

#ifndef MTA_CLIENT
    #include "CLuaTimer.h"
    #include "lua/CLuaMain.h"
    #include "CIdArray.h"
#endif

CLuaTimer::CLuaTimer(const CLuaFunctionRef& iLuaFunction, const CLuaArguments& Arguments)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::TIMER);
    m_uiRepeats = 1;
    m_iLuaFunction = iLuaFunction;
    m_Arguments = Arguments;
    m_paused = false;
}

CLuaTimer::~CLuaTimer()
{
    RemoveScriptID();
}

void CLuaTimer::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClass::TIMER, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}

void CLuaTimer::ExecuteTimer(CLuaMain* pLuaMain)
{
    if (VERIFY_FUNCTION(m_iLuaFunction))
    {
        lua_State* pState = pLuaMain->GetVM();

        LUA_CHECKSTACK(pState, 1);            // Ensure some room

        // Store the current values of the globals
        lua_getglobal(pState, "sourceTimer");
        CLuaArgument OldSource(pState, -1);
        lua_pop(pState, 1);

        // Set the "sourceTimer" global
        lua_pushtimer(pState, this);
        lua_setglobal(pState, "sourceTimer");

        m_Arguments.Call(pLuaMain, m_iLuaFunction);

        // Reset the globals on that VM
        OldSource.Push(pState);
        lua_setglobal(pState, "sourceTimer");
    }
}

void CLuaTimer::SetPaused(bool paused)
{
    if (paused == IsPaused())
        return;

    CTickCount llTimeRemaining = GetTimeLeft();
    if (paused)
    {
        m_pausedRemainingTime = llTimeRemaining.ToLongLong() == 0LL ? m_llDelay : llTimeRemaining;
    }
    else
    {
        CTickCount llCurrentTime = CTickCount::Now();
        CTickCount llNewStartTime = llCurrentTime - (m_llDelay - llTimeRemaining);
        SetStartTime(llNewStartTime);
    }
    m_paused = paused;
}

CTickCount CLuaTimer::GetTimeLeft()
{
    if (IsPaused())
        return m_pausedRemainingTime;

    CTickCount llCurrentTime = CTickCount::Now();
    CTickCount llTimeLeft = m_llStartTime + m_llDelay - llCurrentTime;
    return llTimeLeft.ToLongLong() < 0LL ? CTickCount(0LL) : llTimeLeft;
}
