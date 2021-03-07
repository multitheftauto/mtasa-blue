/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConeShape.cpp
 *  PURPOSE:     Lua physics cone shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsConeShape::CLuaPhysicsConeShape(CBulletPhysics* pPhysics, float fRadius, float fHeight)
    : CLuaPhysicsConvexShape(pPhysics, std::move(CPhysicsSharedLogic::CreateCone(fRadius, fHeight)))
{
}

CLuaPhysicsConeShape::~CLuaPhysicsConeShape()
{

}

void CLuaPhysicsConeShape::SetRadius(float fRadius)
{
    GetBtShape()->setRadius(fRadius);
    UpdateRigids();
}

float CLuaPhysicsConeShape::GetRadius()
{
    return GetBtShape()->getRadius();
}

bool CLuaPhysicsConeShape::SetHeight(float fHeight)
{
    GetBtShape()->setHeight(fHeight);
    return true;
}

float CLuaPhysicsConeShape::GetHeight()
{
    return GetBtShape()->getHeight();
}

SBoundingBox CLuaPhysicsConeShape::GetBoundingBox(btTransform transform)
{
    btVector3   min, max;
    {
        ElementLock lk(this);
        GetBtShape()->getAabb(transform, min, max);
    }
    SBoundingBox sBox = SBoundingBox();
    sBox.vecMin = min;
    sBox.vecMax = max;
    sBox.vecMax -= GetMargin() * 2;
    return sBox;
}
