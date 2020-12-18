/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsPointToPointConstraint.h
 *  PURPOSE:     Physics point to point constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsPointToPointConstraint;

#pragma once

// Define includes
#include "../LuaCommon.h"
#include "../CLuaArguments.h"
#include "lua/physics/CLuaPhysicsConstraint.h"

enum ePhysicsConstraint;

class CLuaPhysicsPointToPointConstraint : public CLuaPhysicsConstraint
{
public:
    CLuaPhysicsPointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, CVector& anchorA, CVector& anchorB,
                                      bool bDisableCollisionsBetweenLinkedBodies = false);
    CLuaPhysicsPointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CVector& position, CVector& anchor, bool bDisableCollisionsBetweenLinkedBodies = false);
    ~CLuaPhysicsPointToPointConstraint();
    void Initialize();

    void SetPivotA(CVector& vecPivotA);
    void SetPivotB(CVector& vecPivotB);

    void Unlink();

private:
    CVector m_vecAnchorA;
    CVector m_vecAnchorB;
    std::unique_ptr<btPoint2PointConstraint> m_pConstraint;
};
