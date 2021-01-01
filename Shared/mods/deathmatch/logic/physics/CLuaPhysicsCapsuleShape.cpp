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

CLuaPhysicsCapsuleShape::CLuaPhysicsCapsuleShape(CBulletPhysics* pPhysics, float fRadius, float fHeight) : CLuaPhysicsShape(pPhysics, std::move(CLuaPhysicsSharedLogic::CreateCapsule(fRadius, fHeight)))
{
}

CLuaPhysicsCapsuleShape::~CLuaPhysicsCapsuleShape()
{

}

bool CLuaPhysicsCapsuleShape::SetRadius(float fRadius)
{
    float fHeight;
    GetHeight(fHeight);
    ((btCapsuleShape*)GetBtShape())->setImplicitShapeDimensions(btVector3(fRadius, fHeight / 2, fRadius));
    UpdateRigids();
    return true;
}

bool CLuaPhysicsCapsuleShape::GetRadius(float& fRadius)
{
    btVector3 implicityShapeDimension;
    implicityShapeDimension = ((btSphereShape*)GetBtShape())->getImplicitShapeDimensions();
    fRadius = implicityShapeDimension.getX();
    return true;
}

bool CLuaPhysicsCapsuleShape::SetHeight(float fHeight)
{
    btCapsuleShape* pCapsule = (btCapsuleShape*)GetBtShape();
    float           fRadius;
    GetRadius(fRadius);
    fHeight /= 2;
    pCapsule->setImplicitShapeDimensions(btVector3(fRadius, fHeight, fRadius));
    UpdateRigids();
    return true;
}

bool CLuaPhysicsCapsuleShape::GetHeight(float& fHeight)
{
    btCapsuleShape* pCapsule = (btCapsuleShape*)GetBtShape();
    fHeight = pCapsule->getHalfHeight();
    fHeight *= 2;
    return true;
}
