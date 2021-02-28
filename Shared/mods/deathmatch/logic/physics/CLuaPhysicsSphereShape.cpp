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
    ((btSphereShape*)GetBtShape())->setUnscaledRadius(fRadius);
}

float CLuaPhysicsSphereShape::GetRadius()
{
    return ((btSphereShape*)GetBtShape())->getRadius();
}
