/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsElement.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsElement;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsElement
{

protected:
    CLuaPhysicsElement(CClientPhysics* pPhysics, EIdClass::EIdClassType classType);
    ~CLuaPhysicsElement();

public:

    CClientPhysics* GetPhysics() const { return m_pPhysics; }
    uint            GetScriptID() const { return m_uiScriptID; }
    EIdClass::EIdClassType GetClassType() const { return m_classType; }
    void                   Destroy();
    bool                   IsReady() const { return m_isReady; }
    void                   Ready() { m_isReady = true; }
    bool                   IsSafeToAccess() const;
    void                   ApplyChanges();
    void                   CommitChange(std::function<void()> change);

private:
    void RemoveScriptID();

    std::atomic<bool>      m_isReady;
    std::atomic<bool>      m_bHasEnqueuedChanges;
    CClientPhysics*        m_pPhysics;
    EIdClass::EIdClassType m_classType;
    uint                   m_uiScriptID;
    SharedUtil::ConcurrentStack<std::function<void()>> m_stackChanges;
};
