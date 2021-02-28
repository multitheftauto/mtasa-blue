/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsCapsuleShape.cpp
 *  PURPOSE:     Lua physics capsule shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsCapsuleShape::CLuaPhysicsCapsuleShape(CBulletPhysics* pPhysics, float fRadius, float fHeight)
    : CLuaPhysicsConvexShape(pPhysics, std::move(CPhysicsSharedLogic::CreateCapsule(fRadius, fHeight)))
{
}

CLuaPhysicsCapsuleShape::~CLuaPhysicsCapsuleShape()
{
}

bool CLuaPhysicsCapsuleShape::SetRadius(float fRadius)
{
    GetBtShape()->setImplicitShapeDimensions(btVector3(fRadius, GetHeight() / 2, fRadius));
    UpdateRigids();
    return true;
}

float CLuaPhysicsCapsuleShape::GetRadius()
{
    return GetBtShape()->getImplicitShapeDimensions().getX();
}

bool CLuaPhysicsCapsuleShape::SetHeight(float fHeight)
{
    float           fRadius = GetRadius();
    fHeight /= 2;
    GetBtShape()->setImplicitShapeDimensions(btVector3(fRadius, fHeight, fRadius));
    UpdateRigids();
    return true;
}

float CLuaPhysicsCapsuleShape::GetHeight()
{
    return GetBtShape()->getHalfHeight() * 2;
}
