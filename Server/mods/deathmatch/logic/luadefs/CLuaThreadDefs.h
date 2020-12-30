/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaThreadDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaThreadDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static CLuaThread*  CreateThread(lua_State* luaVM, std::string strCode);
    static EThreadState GetThreadState(CLuaThread* pThread);
    static bool ThreadCall(CLuaThread* pThread, std::string strFunction, std::optional<std::vector<CLuaArgument>> optionalArguments);
};
