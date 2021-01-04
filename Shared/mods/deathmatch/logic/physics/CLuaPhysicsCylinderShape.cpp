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
    : CLuaPhysicsConvexShape(pPhysics, CLuaPhysicsSharedLogic::CreateCylinder(half))
{
}

CLuaPhysicsCylinderShape::~CLuaPhysicsCylinderShape()
{

}

bool CLuaPhysicsCylinderShape::SetRadius(float fRadius)
{
    float fHeight;
    GetHeight(fHeight);
    ((btCylinderShape*)GetBtShape())->setImplicitShapeDimensions(btVector3(fRadius, fHeight / 2, fRadius));
    UpdateRigids();
    return true;
}

bool CLuaPhysicsCylinderShape::GetRadius(float& fRadius)
{
    fRadius = ((btCylinderShape*)GetBtShape())->getRadius();
    return true;
}

bool CLuaPhysicsCylinderShape::SetHeight(float fHeight)
{
    btCylinderShape* pCylinder = (btCylinderShape*)GetBtShape();
    float            fRadius;
    GetRadius(fRadius);
    fRadius += pCylinder->getMargin();
    fHeight -= pCylinder->getMargin();
    pCylinder->setImplicitShapeDimensions(btVector3(fRadius, fHeight, fRadius));
    return true;
}

bool CLuaPhysicsCylinderShape::GetHeight(float& fHeight)
{
    btCylinderShape* pCylinder = (btCylinderShape*)GetBtShape();
    fHeight = pCylinder->getImplicitShapeDimensions().getY();
    fHeight += pCylinder->getMargin();
    return true;
}
