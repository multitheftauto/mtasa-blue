/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsSphereShape.cpp
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsSphereShape::CLuaPhysicsSphereShape(CBulletPhysics* pPhysics, float fRadius)
    : CLuaPhysicsConvexShape(pPhysics, std::move(CPhysicsSharedLogic::CreateSphere(fRadius)))
{
}

CLuaPhysicsSphereShape::~CLuaPhysicsSphereShape()
{

}

void CLuaPhysicsSphereShape::SetRadius(float fRadius)
{
    GetBtShape()->setUnscaledRadius(fRadius);
}

float CLuaPhysicsSphereShape::GetRadius()
{
    return GetBtShape()->getRadius();
}

SBoundingSphere CLuaPhysicsSphereShape::GetBoundingSphere()
{
    ElementLock lk(this);
    btVector3   center;
    btScalar    radius;
    GetBtShape()->getBoundingSphere(center, radius); // ~sqrt(3)/2 for sphere of radius 0.5, invalid
    SBoundingSphere sSphere = SBoundingSphere();
    sSphere.fRadius = GetRadius();
    sSphere.vecCenter = center;
    return sSphere;
}
