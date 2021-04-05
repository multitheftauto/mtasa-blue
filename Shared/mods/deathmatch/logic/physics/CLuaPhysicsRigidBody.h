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

#include "physics/CPhysicsRigidBodyProxy.h"
#include "physics/CLuaPhysicsWorldElement.h"

#pragma once

struct SPhysicsCollisionReport;
struct SBoundingBox;
struct SBoundingSphere;

class CLuaPhysicsRigidBody : public CLuaPhysicsWorldElement
{
public:
    CLuaPhysicsRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);
    ~CLuaPhysicsRigidBody();

    bool Destroy();

    void          SetPosition(CVector vecPosition);
    const CVector GetPosition() const;
    void          SetRotation(CVector vecRotation);
    const CVector GetRotation() const;


    CPhysicsRigidBodyProxy* GetBtRigidBody() const { return m_pRigidBodyProxy.get(); }

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::RigidBody; }
    
    CLuaPhysicsShape* GetShape() const { return m_pShape; }
    btCollisionObject* GetBtCollisionObject() const { return m_pRigidBodyProxy.get(); }

    void SetEnabled(bool bEnabled) { m_pRigidBodyProxy->SetEnabled(bEnabled); }
    bool IsEnabled() const { return m_pRigidBodyProxy->IsEnabled(); }

private:
    std::unique_ptr<CPhysicsRigidBodyProxy> m_pRigidBodyProxy = nullptr;
    CLuaPhysicsShape*     m_pShape;
    std::unique_ptr<MotionState>            m_pMotionState;            // Thread safe
};
