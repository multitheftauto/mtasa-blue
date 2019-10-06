/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConstraint.cpp
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsSharedLogic.h"

CLuaPhysicsConstraint::CLuaPhysicsConstraint(btDiscreteDynamicsWorld* pWorld, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB)
{
    m_pWorld = pWorld;
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::CONSTRAINT);
    m_pRigidBodyA = pRigidBodyA;
    m_pRigidBodyB = pRigidBodyB;
}

CLuaPhysicsConstraint::~CLuaPhysicsConstraint()
{
    RemoveScriptID();
}

void CLuaPhysicsConstraint::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClass::CONSTRAINT, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}

void CLuaPhysicsConstraint::CreatePointToPointConstraint(CVector& anchorA, CVector& anchorB)
{
    btPoint2PointConstraint* constraint = new btPoint2PointConstraint(*m_pRigidBodyA->GetBtRigidBody(), *m_pRigidBodyB->GetBtRigidBody(),
                                                                      reinterpret_cast<btVector3&>(anchorA), reinterpret_cast<btVector3&>(anchorB));
    m_pWorld->addConstraint(constraint, false);
    m_pRigidBodyA->GetBtRigidBody()->activate(true);
    m_pRigidBodyB->GetBtRigidBody()->activate(true);
}
