/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaThread.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaThread;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

#define LUA_TIMER_MIN_INTERVAL 0

class CLuaThread
{
public:
    CLuaThread(const std::string& code, const CLuaArguments& Arguments);
    ~CLuaThread();

    void RemoveScriptID();


    uint                 GetScriptID() const { return m_uiScriptID; }

private:
    std::string   m_strCode;
    CLuaArguments m_Arguments;
    uint          m_uiScriptID;
};
