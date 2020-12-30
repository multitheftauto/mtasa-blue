/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaThreadManager.cpp
 *  PURPOSE:     Lua thread manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaThreadManager::DoPulse(CLuaMain* pLuaMain)
{
    assert(m_ProcessQueue.empty());
    assert(!m_pPendingDelete);
    assert(!m_pProcessingThread);

    CTickCount llCurrentTime = CTickCount::Now();

    // Delete all the Threads
    CFastList<CLuaThread*>::const_iterator iter = m_ThreadList.begin();
    for (; iter != m_ThreadList.end(); ++iter)
    {
        (*iter)->DoPulse();
    }
}

void CLuaThreadManager::RemoveThread(CLuaThread* pLuaThread)
{
    assert(pLuaThread);

    // Check if already removed
    if (!ListContains(m_ThreadList, pLuaThread))
        return;

    // Remove all references
    ListRemove(m_ThreadList, pLuaThread);
    ListRemove(m_ProcessQueue, pLuaThread);

    if (m_pProcessingThread == pLuaThread)
    {
        assert(!m_pPendingDelete);
        pLuaThread->RemoveScriptID();
        m_pPendingDelete = pLuaThread;
    }
    else
        delete pLuaThread;
}

void CLuaThreadManager::RemoveAllThreads()
{
    // Delete all the Threads
    CFastList<CLuaThread*>::const_iterator iter = m_ThreadList.begin();
    for (; iter != m_ThreadList.end(); ++iter)
    {
        delete *iter;
    }

    // Clear the Thread list
    m_ThreadList.clear();
    m_ProcessQueue.clear();
    m_pPendingDelete = NULL;
    m_pProcessingThread = NULL;
}

CLuaThread* CLuaThreadManager::GetThreadFromScriptID(uint uiScriptID)
{
    CLuaThread* pLuaThread = (CLuaThread*)CIdArray::FindEntry(uiScriptID, EIdClass::THREAD);
    if (!pLuaThread)
        return NULL;

    if (!ListContains(m_ThreadList, pLuaThread))
        return NULL;
    return pLuaThread;
}

CLuaThread* CLuaThreadManager::AddThread(const std::string& code, const CLuaArguments& Arguments)
{
    CLuaThread* pLuaThread = new CLuaThread(code, Arguments);
    m_ThreadList.push_back(pLuaThread);
    return pLuaThread;
}
