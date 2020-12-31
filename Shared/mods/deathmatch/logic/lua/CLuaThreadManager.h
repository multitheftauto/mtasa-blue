/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaThreadManager.h
 *  PURPOSE:     Lua thread manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaThreadManager;

#pragma once

#include "CLuaThread.h"
#include <list>

class CLuaThreadManager
{
public:
    CLuaThreadManager()
    {
        m_pPendingDelete = NULL;
        m_pProcessingThread = NULL;
    }
    ~CLuaThreadManager() { RemoveAllThreads(); };

    void DoPulse(CLuaMain* pLuaMain);

    CLuaThread* GetThreadFromScriptID(unsigned int uiScriptID);

    CLuaThread*   AddThread(const std::string&);
    void          RemoveThread(CLuaThread* pLuaTimer);
    void          RemoveAllThreads();
    unsigned long GetThreadCount() const { return m_ThreadList.size(); }

    CFastList<CLuaThread*>::const_iterator IterBegin() { return m_ThreadList.begin(); }
    CFastList<CLuaThread*>::const_iterator IterEnd() { return m_ThreadList.end(); }

private:
    CFastList<CLuaThread*>  m_ThreadList;
    std::deque<CLuaThread*> m_ProcessQueue;
    CLuaThread*             m_pPendingDelete;
    CLuaThread*             m_pProcessingThread;
};
