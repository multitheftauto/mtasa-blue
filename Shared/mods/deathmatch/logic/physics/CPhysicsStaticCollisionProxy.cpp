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

std::unique_ptr<CPhysicsStaticCollisionProxy> CPhysicsStaticCollisionProxy::New(CLuaPhysicsShape* pShape)
{
    std::unique_ptr<CPhysicsStaticCollisionProxy> m_btCollisionObject = std::make_unique<CPhysicsStaticCollisionProxy>();
    m_btCollisionObject->setCollisionShape(pShape->InternalGetBtShape());
    return std::move(m_btCollisionObject);
}

CPhysicsStaticCollisionProxy::~CPhysicsStaticCollisionProxy()
{
    setCollisionShape(nullptr);            // don't destroy shape, only static collision
    SetEnabled(false);
}

void CPhysicsStaticCollisionProxy::SetEnabled(bool bEnabled)
{
    if (bEnabled != m_bEnabled)
    {
        if (bEnabled)
        {
#ifdef MTA_CLIENT
            g_pClientGame->GetPhysics()->AddStaticCollision(this);
#else
            g_pGame->GetPhysics()->AddStaticCollision(this);
#endif
        }
        else
        {
#ifdef MTA_CLIENT
            g_pClientGame->GetPhysics()->RemoveStaticCollision(this);
#else
            g_pGame->GetPhysics()->RemoveStaticCollision(this);
#endif
        }
        m_bEnabled = bEnabled;
    }
}
