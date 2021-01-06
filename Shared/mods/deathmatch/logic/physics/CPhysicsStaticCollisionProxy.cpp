/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/physics/CPhysicsStaticCollisionProxy.cpp
 *  PURPOSE:     Manages creation and destruction of static collision
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

std::unique_ptr<CPhysicsStaticCollisionProxy> CPhysicsStaticCollisionProxy::Create(CLuaPhysicsShape* pShape)
{
    std::unique_ptr<CPhysicsStaticCollisionProxy> m_btCollisionObject = std::make_unique<CPhysicsStaticCollisionProxy>();
    m_btCollisionObject->m_pPhysics = pShape->GetPhysics();
    m_btCollisionObject->setCollisionShape(pShape->GetBtShape());
    m_btCollisionObject->SetEnabled(true);
    return std::move(m_btCollisionObject);
}

CPhysicsStaticCollisionProxy::~CPhysicsStaticCollisionProxy()
{
    SetEnabled(false);
    setCollisionShape(nullptr);            // don't destroy shape, only static collision
}
void CPhysicsStaticCollisionProxy::SetEnabled(bool bEnabled)
{
    if (bEnabled != m_bEnabled)
    {
        if (bEnabled)
        {
            m_pPhysics->AddStaticCollision(this);
        }
        else
        {
            m_pPhysics->RemoveStaticCollision(this);
        }
        m_bEnabled = bEnabled;
    }
}
