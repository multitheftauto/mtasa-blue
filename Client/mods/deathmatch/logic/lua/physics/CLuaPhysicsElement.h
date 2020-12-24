/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsElement.h
 *  PURPOSE:     Base class for physics elements
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsElement;
struct SPhysicsCollisionContact;
struct SPhysicsCollisionReport;

#pragma once

class ILuaPhysicsElement
{
public:
    virtual void Update() = 0;
};

class CLuaPhysicsElement : public ILuaPhysicsElement
{
protected:
    CLuaPhysicsElement(CClientPhysics* pPhysics, EIdClass::EIdClassType classType);
    ~CLuaPhysicsElement();

public:
    CClientPhysics*        GetPhysics() const { return m_pPhysics; }
    uint                   GetScriptID() const { return m_uiScriptID; }
    EIdClass::EIdClassType GetClassType() const { return m_classType; }
    void                   Destroy();
    bool                   IsReady() const { return m_isReady; }
    void                   Ready() { m_isReady = true; }
    bool                   IsSafeToAccess() const;
    void                   ApplyChanges();


    // Run changes on worker thread, let you modify element before get created
    void CommitChange(std::function<void()> change);

    virtual void ClearCollisionReport() {}
    virtual void ReportCollision(std::unique_ptr<SPhysicsCollisionReport> pCollisionReport) {}
    void         NeedsUpdate();

protected:
    std::atomic<bool> m_bNeedsUpdate = false;

private:
    void RemoveScriptID();

    std::atomic<bool>                                  m_isReady;
    std::atomic<bool>                                  m_bHasEnqueuedChanges = false;
    CClientPhysics*                                    m_pPhysics;
    EIdClass::EIdClassType                             m_classType;
    uint                                               m_uiScriptID;
    SharedUtil::ConcurrentStack<std::function<void()>> m_stackChanges;
};
