/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaTimerDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaTimerDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"setTimer", SetTimer}, {"killTimer", KillTimer}, {"resetTimer", ResetTimer},
        {"setTimerPaused", ArgumentParser<SetTimerPaused>},{"isTimerPaused", ArgumentParser<IsTimerPaused>},
        {"getTimers", GetTimers}, {"isTimer", IsTimer},       {"getTimerDetails", GetTimerDetails},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaTimerDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "setTimer");
    lua_classfunction(luaVM, "destroy", "killTimer");
    lua_classfunction(luaVM, "reset", "resetTimer");
    lua_classfunction(luaVM, "isValid", "isTimer");
    lua_classfunction(luaVM, "getDetails", "getTimerDetails");

    lua_classvariable(luaVM, "valid", NULL, "isTimer");
    lua_classvariable(luaVM, "paused", "setTimerPaused", "isTimerPaused");

    lua_registerclass(luaVM, "Timer");
}

int CLuaTimerDefs::SetTimer(lua_State* luaVM)
{
    //  timer setTimer ( function theFunction, int timeInterval, int timesToExecute, [ var arguments... ] )
    CLuaFunctionRef iLuaFunction;
    double          dTimeInterval;
    uint            uiTimesToExecute;
    CLuaArguments   Arguments;

    CScriptArgReader argStream(luaVM);
    argStream.ReadFunction(iLuaFunction);
    argStream.ReadNumber(dTimeInterval);
    argStream.ReadNumber(uiTimesToExecute);
    argStream.ReadLuaArguments(Arguments);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            // Check for the minimum interval
            if (dTimeInterval < LUA_TIMER_MIN_INTERVAL)
            {
                argStream.SetCustomError("Interval is below " MTA_STR(LUA_TIMER_MIN_INTERVAL));
            }
            else
            {
                CLuaTimer* pLuaTimer = luaMain->GetTimerManager()->AddTimer(iLuaFunction, CTickCount(dTimeInterval), uiTimesToExecute, Arguments);
                if (pLuaTimer)
                {
                    // Set our timer debug info (in case we don't have any debug info which is usually when you do setTimer(destroyElement, 50, 1) or such)
                    pLuaTimer->SetLuaDebugInfo(g_pClientGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));

                    lua_pushtimer(luaVM, pLuaTimer);
                    return 1;
                }
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaTimerDefs::KillTimer(lua_State* luaVM)
{
    //  bool killTimer ( timer theTimer )
    CLuaTimer* pLuaTimer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLuaTimer);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            luaMain->GetTimerManager()->RemoveTimer(pLuaTimer);

            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaTimerDefs::IsTimerPaused(CLuaTimer* timer) noexcept
{
    return timer->IsPaused();
}

bool CLuaTimerDefs::SetTimerPaused(lua_State* luaVM, CLuaTimer* timer, bool paused)
{
    //  bool setTimerPaused ( timer theTimer, bool paused )
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    luaMain->GetTimerManager()->SetTimerPaused(timer, paused);
    return true;
}

int CLuaTimerDefs::ResetTimer(lua_State* luaVM)
{
    //  bool resetTimer ( timer theTimer )
    CLuaTimer* pLuaTimer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLuaTimer);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            luaMain->GetTimerManager()->ResetTimer(pLuaTimer);

            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaTimerDefs::GetTimers(lua_State* luaVM)
{
    //  table getTimers ( [ time ] )
    double dTime;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(dTime, 0);

    if (!argStream.HasErrors())
    {
        // Find our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Create a new table
            lua_newtable(luaVM);

            // Add all the timers with less than ulTime left
            CLuaTimerManager*                     pLuaTimerManager = pLuaMain->GetTimerManager();
            CTickCount                            llCurrentTime = CTickCount::Now();
            unsigned int                          uiIndex = 0;
            CFastList<CLuaTimer*>::const_iterator iter = pLuaTimerManager->IterBegin();
            for (; iter != pLuaTimerManager->IterEnd(); iter++)
            {
                CLuaTimer* pLuaTimer = *iter;

                // If the time left is less than the time specified, or the time specifed is 0
                CTickCount llTimeLeft = (pLuaTimer->GetStartTime() + pLuaTimer->GetDelay()) - llCurrentTime;
                if (dTime == 0 || llTimeLeft.ToDouble() <= dTime)
                {
                    // Add it to the table
                    lua_pushnumber(luaVM, ++uiIndex);
                    lua_pushtimer(luaVM, pLuaTimer);
                    lua_settable(luaVM, -3);
                }
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaTimerDefs::IsTimer(lua_State* luaVM)
{
    //  bool isTimer ( timer theTimer )
    CLuaTimer* pLuaTimer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLuaTimer);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaTimerDefs::GetTimerDetails(lua_State* luaVM)
{
    //  int, int, int getTimerDetails ( timer theTimer )
    CLuaTimer* pLuaTimer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLuaTimer);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pLuaTimer->GetTimeLeft().ToDouble());
        lua_pushnumber(luaVM, pLuaTimer->GetRepeats());
        lua_pushnumber(luaVM, pLuaTimer->GetDelay().ToDouble());
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
