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
    ((btCapsuleShape*)GetBtShape())->setImplicitShapeDimensions(btVector3(fRadius, GetHeight() / 2, fRadius));
    UpdateRigids();
    return true;
}

float CLuaPhysicsCapsuleShape::GetRadius()
{
    btVector3 implicityShapeDimension;
    implicityShapeDimension = ((btSphereShape*)GetBtShape())->getImplicitShapeDimensions();
    return implicityShapeDimension.getX();
}

bool CLuaPhysicsCapsuleShape::SetHeight(float fHeight)
{
    btCapsuleShape* pCapsule = (btCapsuleShape*)GetBtShape();
    float           fRadius = GetRadius();
    fHeight /= 2;
    pCapsule->setImplicitShapeDimensions(btVector3(fRadius, fHeight, fRadius));
    UpdateRigids();
    return true;
}

float CLuaPhysicsCapsuleShape::GetHeight()
{
    btCapsuleShape* pCapsule = (btCapsuleShape*)GetBtShape();
    return pCapsule->getHalfHeight() * 2;
}
