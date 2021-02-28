/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsCylinderShape.cpp
 *  PURPOSE:     Lua physics cylinder shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsCylinderShape::CLuaPhysicsCylinderShape(CBulletPhysics* pPhysics, CVector half)
    : CLuaPhysicsConvexShape(pPhysics, CPhysicsSharedLogic::CreateCylinder(half))
{
}

CLuaPhysicsCylinderShape::~CLuaPhysicsCylinderShape()
{

}

bool CLuaPhysicsCylinderShape::SetRadius(float fRadius)
{
    GetBtShape()->setImplicitShapeDimensions(btVector3(fRadius, GetHeight() / 2, fRadius));
    UpdateRigids();
    return true;
}

float CLuaPhysicsCylinderShape::GetRadius()
{
    return GetBtShape()->getRadius();
}

bool CLuaPhysicsCylinderShape::SetHeight(float fHeight)
{
    float            fRadius = GetRadius();
    fRadius += GetBtShape()->getMargin();
    fHeight -= GetBtShape()->getMargin();
    GetBtShape()->setImplicitShapeDimensions(btVector3(fRadius, fHeight, fRadius));
    return true;
}

float CLuaPhysicsCylinderShape::GetHeight()
{
    float fHeight = GetBtShape()->getImplicitShapeDimensions().getY();
    return fHeight + GetBtShape()->getMargin();
}
