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

#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"
#ifndef MTA_CLIENT
    #include "CGame.h"
#endif

CPhysicsStaticCollisionProxy::CPhysicsStaticCollisionProxy(CLuaPhysicsShape* pPhysicsShape)
{
    setCollisionShape(pPhysicsShape->InternalGetBtShape());
}

std::unique_ptr<CPhysicsStaticCollisionProxy> CPhysicsStaticCollisionProxy::New(CLuaPhysicsShape* pPhysicsShape)
{
    std::unique_ptr<CPhysicsStaticCollisionProxy> m_btCollisionObject = std::make_unique<CPhysicsStaticCollisionProxy>(pPhysicsShape);
    return std::move(m_btCollisionObject);
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
