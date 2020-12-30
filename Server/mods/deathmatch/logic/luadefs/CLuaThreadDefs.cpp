/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaTimerDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"

void CLuaThreadDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createThread", ArgumentParser<CreateThread>},
        {"getThreadState", ArgumentParser<GetThreadState>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaThreadDefs::AddClass(lua_State* luaVM)
{
}

CLuaThread* CLuaThreadDefs::CreateThread(lua_State* luaVM, std::string strCode)
{
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (luaMain)
    {
        CLuaThread* pLuaThread = luaMain->GetThreadManager()->AddThread(strCode);
        if (pLuaThread)
        {
            return pLuaThread;
        }
    }

    throw new std::invalid_argument("Error");
}

EThreadState CLuaThreadDefs::GetThreadState(CLuaThread* pThread)
{
    return pThread->GetState();
}
