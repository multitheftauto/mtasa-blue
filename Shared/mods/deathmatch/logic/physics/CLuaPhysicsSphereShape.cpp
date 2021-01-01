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
    : CLuaPhysicsShape(pPhysics, std::move(CLuaPhysicsSharedLogic::CreateSphere(fRadius)))
{
}

CLuaPhysicsSphereShape::~CLuaPhysicsSphereShape()
{

}

bool CLuaPhysicsSphereShape::SetRadius(float fRadius)
{
    ((btSphereShape*)GetBtShape())->setUnscaledRadius(fRadius);
    return true;
}

bool CLuaPhysicsSphereShape::GetRadius(float& fRadius)
{
    fRadius = ((btSphereShape*)GetBtShape())->getRadius();
    return true;
}
