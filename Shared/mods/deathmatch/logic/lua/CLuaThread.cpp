/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaThread.cpp
 *  PURPOSE:     Lua thread class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CLuaThread::CLuaThread(const std::string& code)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::THREAD);
    m_strCode = code;
    m_pAsyncTaskSheduler = std::make_unique<SharedUtil::CAsyncTaskScheduler>(1);
    SetState(EThreadState::INITIALIZING);
    m_pAsyncTaskSheduler->PushTask<bool>(
        [&] {
            m_luaVM = lua_open();

            luaopen_base(m_luaVM);
            luaopen_math(m_luaVM);
            luaopen_string(m_luaVM);
            luaopen_table(m_luaVM);
            luaopen_debug(m_luaVM);
            luaopen_utf8(m_luaVM);
            luaopen_os(m_luaVM);

            CLuaMain::InitSecurity(m_luaVM);

            // Registering C functions
            CLuaCFunctions::RegisterThreadFunctionsWithVM(m_luaVM);

            int stackSize = lua_gettop(m_luaVM);
            lua_pop(m_luaVM, stackSize);

            LoadUserCode();

            return true;
        },
        [&](bool success) {
            if (success)
            {
            }
        });
}

CLuaThread::~CLuaThread()
{
    Close();
    RemoveScriptID();
}

void CLuaThread::SetState(EThreadState state)
{
    std::lock_guard guard(m_lock);
    m_eState = state;
}

void CLuaThread::Close()
{
    if (m_luaVM)
    {
        lua_close(m_luaVM);
        m_luaVM = nullptr;
    }
}
void CLuaThread::LoadUserCode()
{
    int iResult = luaL_loadbuffer(m_luaVM, reinterpret_cast<const char*>(m_strCode.c_str()), m_strCode.length(), "foo");
    if (iResult == LUA_ERRRUN || iResult == LUA_ERRMEM || iResult == LUA_ERRSYNTAX)
    {
        const char* result = lua_tostring(m_luaVM, -1);
        m_strError = std::string(result);
        SetState(EThreadState::FAILURE);
        return;
    }

    SetState(EThreadState::BUSY);

    int iret = lua_pcall(m_luaVM, 0, LUA_MULTRET, 0);

    CScriptArgReader argStream(m_luaVM);
    {
        std::lock_guard  guard(m_lockReturnArguments);
        argStream.ReadLuaArguments(m_returnArguments);
        m_bHasReturnArguments = true;
    }

    Idle();
    return;
}

void CLuaThread::LoadScript(const char* code)
{
    // Run the script
    if (!CLuaMain::LuaLoadBuffer(m_luaVM, code, strlen(code), NULL))
    {
        int luaSavedTop = lua_gettop(m_luaVM);
        int iret = lua_pcall(m_luaVM, 0, LUA_MULTRET, 0);
        if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
        {
            std::string strRes = ConformResourcePath(lua_tostring(m_luaVM, -1));
            //g_pGame->GetScriptDebugging()->LogPCallError(m_luaVM, strRes);
        }
        // Cleanup any return values
        if (lua_gettop(m_luaVM) > luaSavedTop)
            lua_settop(m_luaVM, luaSavedTop);
    }
    else
    {
        std::string strRes = ConformResourcePath(lua_tostring(m_luaVM, -1));
        //g_pGame->GetScriptDebugging()->LogError(m_luaVM, "Loading in-line script failed: %s", strRes.c_str());
    }
}

void CLuaThread::Idle()
{
    SetState(EThreadState::IDLE);
}

void CLuaThread::DoPulse()
{
    m_pAsyncTaskSheduler->CollectResults();
}

void CLuaThread::Call(const std::string& functionName, const CLuaArguments& arguments, CLuaArguments& returns)
{
    SetState(EThreadState::BUSY);
    arguments.CallGlobal(m_luaVM, functionName.c_str(), &returns);
    Idle();
}

void CLuaThread::Call(const std::string& functionName, const CLuaArguments& arguments)
{
    m_pAsyncTaskSheduler->PushTask<bool>(
        [&, arguments] {
            CLuaArguments returns;
            SetState(EThreadState::BUSY);
            arguments.CallGlobal(m_luaVM, functionName.c_str(), &returns);
            return true;
        },
        [&](bool __) { Idle(); });
}

EThreadState CLuaThread::GetState()
{
    std::lock_guard guard(m_lock);
    return m_eState;
}

bool CLuaThread::GetReturnArguments(CLuaArguments& arguments)
{
    std::lock_guard guard(m_lockReturnArguments);
    if (m_bHasReturnArguments)
    {
        arguments = m_returnArguments;
        return true;
    }
    return false;
}

void CLuaThread::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClass::THREAD, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}
