/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBody.h
 *  PURPOSE:     Lua rigid body class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsRigidBody;

#pragma once

#include "physics/CPhysicsRigidBodyProxy.h"
#include "physics/CLuaPhysicsWorldElement.h"

class CLuaPhysicsRigidBody : public CLuaPhysicsWorldElement
{
public:
    CLuaPhysicsRigidBody(CLuaPhysicsShape* pPhysicsShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);

    void          SetPosition(CVector vecPosition);
    const CVector GetPosition() const;
    void          SetRotation(CVector vecRotation);
    const CVector GetRotation() const;

    void    SetVelocity(CVector vecVelocity);
    CVector GetVelocity();
    void    SetAngularVelocity(CVector vecVelocity);
    CVector GetAngularVelocity();

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::RigidBody; }

    CPhysicsRigidBodyProxy* GetCollisionObject() const { return m_pRigidBodyProxy.get(); }

    // Set's whatever rigid body is added to physics world
    void SetEnabled(bool bEnabled) { m_pRigidBodyProxy->SetEnabled(bEnabled); }
    // Checks whatever rigid body is in physics world
    bool IsEnabled() const { return m_pRigidBodyProxy->IsEnabled(); }

    void Unlink();

private:
    std::unique_ptr<CPhysicsRigidBodyProxy> m_pRigidBodyProxy;
    CLuaPhysicsShape*                       m_pPhysicsShape = nullptr;
    std::unique_ptr<MotionState>            m_pMotionState;            // Thread safe
};
