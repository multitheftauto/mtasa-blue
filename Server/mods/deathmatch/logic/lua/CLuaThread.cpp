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

CLuaThread::CLuaThread(const std::string& code, const CLuaArguments& Arguments)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::TIMER);
    m_strCode = code;
    m_Arguments = Arguments;
}

CLuaThread::~CLuaThread()
{
    RemoveScriptID();
}

void CLuaThread::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClass::THREAD, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}
