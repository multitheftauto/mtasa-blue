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
        {"createThread", CreateThread},
        {"getThreadState", ArgumentParser<GetThreadState>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaThreadDefs::AddClass(lua_State* luaVM)
{
}

static int writer(lua_State* L, const void* b, size_t size, void* B)
{
    (void)L;
    luaL_addlstring((luaL_Buffer*)B, (const char*)b, size);
    return 0;
}

int CLuaThreadDefs::CreateThread(lua_State* luaVM)
{
    bool            isFunction = false;
    CLuaFunctionRef iLuaFunction;
    CLuaArguments   Arguments;
    SString         strCode;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsFunction())
    {
        isFunction = true;
        argStream.ReadFunction(iLuaFunction);
    }
    else
    {
        argStream.ReadString(strCode);
    }
    argStream.ReadLuaArguments(Arguments);
    argStream.ReadFunctionComplete();

    std::string code;
    if (isFunction)
    {
        LUA_CHECKSTACK(luaVM, 1);
        int luaStackPointer = lua_gettop(luaVM);
        lua_getref(luaVM, iLuaFunction.ToInt());

        luaL_Buffer b;
        luaL_buffinit(luaVM, &b);

        if (lua_dump(luaVM, writer, &b) != 0)
            luaL_error(luaVM, "unable to dump given function");
        size_t l = b.p - b.buffer;

        code = std::string(b.buffer, b.p - b.buffer);
    }
    else
    {
        code = strCode;
    }
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (luaMain)
    {
        CLuaThread* pLuaThread = luaMain->GetThreadManager()->AddThread(code, Arguments);
        if (pLuaThread)
        {
            lua_pushluathread(luaVM, pLuaThread);
            return 1;
        }
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

EThreadState CLuaThreadDefs::GetThreadState(CLuaThread* pThread)
{
    return pThread->GetState();
}
