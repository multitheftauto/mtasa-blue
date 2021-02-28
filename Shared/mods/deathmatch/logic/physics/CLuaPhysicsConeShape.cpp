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
    ((btConeShape*)GetBtShape())->setRadius(fRadius);
    UpdateRigids();
}

float CLuaPhysicsConeShape::GetRadius()
{
    return ((btConeShape*)GetBtShape())->getRadius();
}

bool CLuaPhysicsConeShape::SetHeight(float fHeight)
{
    ((btConeShape*)GetBtShape())->setHeight(fHeight);
    return true;
}

float CLuaPhysicsConeShape::GetHeight()
{
    return ((btConeShape*)GetBtShape())->getHeight();
}
