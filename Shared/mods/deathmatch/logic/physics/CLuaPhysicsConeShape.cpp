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
    : CLuaPhysicsConvexShape(pPhysics, std::move(CLuaPhysicsSharedLogic::CreateCone(fRadius, fHeight)))
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

bool CLuaPhysicsConeShape::GetRadius(float& fRadius)
{
    fRadius = ((btConeShape*)GetBtShape())->getRadius();
    return true;
}

bool CLuaPhysicsConeShape::SetHeight(float fHeight)
{
    btConeShape* pCone = (btConeShape*)GetBtShape();
    pCone->setHeight(fHeight);
    return true;
}

bool CLuaPhysicsConeShape::GetHeight(float& fHeight)
{
    btConeShape* pCone = (btConeShape*)GetBtShape();
    fHeight = pCone->getHeight();
    return true;
}
