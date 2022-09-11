/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollision.h
 *  PURPOSE:     Static collision class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "bulletphysics3d/btBulletDynamicsCommon.h"
#include "physics/CPhysicsStaticCollisionProxy.h"
#include "physics/CLuaPhysicsWorldElement.h"

class CLuaPhysicsShape;

class CLuaPhysicsStaticCollision : public CLuaPhysicsWorldElement
{
public:
    CLuaPhysicsStaticCollision(CLuaPhysicsShape* pPhysicsShape);

    void          SetPosition(CVector vecPosition);
    const CVector GetPosition() const;
    void          SetRotation(CVector vecRotation);
    const CVector GetRotation() const;

    void Unlink();

    CPhysicsStaticCollisionProxy* GetCollisionObject() const { return m_btCollisionObject.get(); }
    virtual ePhysicsElementType   GetType() const { return ePhysicsElementType::StaticCollision; }

    // Set's whatever static collision is added to physics world
    void SetEnabled(bool bEnabled) { m_btCollisionObject->SetEnabled(bEnabled); }
    // Checks whatever static collision is in physics world
    bool IsEnabled() const { return m_btCollisionObject->IsEnabled(); }

private:
    std::unique_ptr<CPhysicsStaticCollisionProxy> m_btCollisionObject;
    CLuaPhysicsShape*                             m_pPhysicsShape;
};
