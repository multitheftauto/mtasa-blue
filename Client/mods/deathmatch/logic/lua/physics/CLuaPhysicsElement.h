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

// Define includes
#include "../LuaCommon.h"
#include "../CLuaArguments.h"

struct SPhysicsCollisionContact
{
    CVector vecPositionWorldOn;
    CVector vecLocalPoint;
    CVector vecLateralFrictionDir;
    int     contactTriangle;
    float   appliedImpulse;
    float   appliedImpulseLiteral;
};

struct SPhysicsCollisionReport
{
    std::shared_ptr<CLuaPhysicsElement>                    pElement;
    std::vector<std::shared_ptr<SPhysicsCollisionContact>> m_vecContacts;
};

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

    // Run changes on worker thread, let you modify element before get created
    void                   CommitChange(std::function<void()> change);

    virtual void ClearCollisionReport(){}
    virtual void ReportCollision(std::unique_ptr<SPhysicsCollisionReport> pCollisionReport){}

private:
    void RemoveScriptID();

    std::atomic<bool>      m_isReady;
    std::atomic<bool>      m_bHasEnqueuedChanges;
    CClientPhysics*        m_pPhysics;
    EIdClass::EIdClassType m_classType;
    uint                   m_uiScriptID;
    SharedUtil::ConcurrentStack<std::function<void()>> m_stackChanges;
};
