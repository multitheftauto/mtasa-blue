/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaThread.h
 *  PURPOSE:     Lua thread class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaThread;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaThread
{
public:
    CLuaThread(const std::string& code);
    ~CLuaThread();

    void RemoveScriptID();
    void DoPulse();

    uint GetScriptID() const { return m_uiScriptID; }
    EThreadState GetState();
private:
    void SetState(EThreadState state);
    void LoadScript(const char* code);
    void LoadUserProvidedCode();

    std::string                                      m_strCode;
    CLuaArguments                                    m_returnArguments;
    std::unique_ptr<SharedUtil::CAsyncTaskScheduler> m_pAsyncTaskSheduler;
    uint                                             m_uiScriptID;

    EThreadState m_eState;
    std::mutex   m_lock;
    lua_State*  m_luaVM;
    std::string  m_strError;
};
