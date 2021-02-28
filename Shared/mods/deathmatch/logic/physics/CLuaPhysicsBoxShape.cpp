/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsBoxShape.cpp
 *  PURPOSE:     Lua physics box shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsBoxShape::CLuaPhysicsBoxShape(CBulletPhysics* pPhysics, CVector half) : CLuaPhysicsConvexShape(pPhysics, CPhysicsSharedLogic::CreateBox(half))
{
}

bool CLuaPhysicsBoxShape::SetSize(CVector vecSize)
{
    vecSize -= GetBtShape()->getMargin();
    GetBtShape()->setImplicitShapeDimensions(vecSize);
    UpdateRigids();
    return true;
}

CVector CLuaPhysicsBoxShape::GetSize()
{
    CVector                vecHalfSize = GetBtShape()->getImplicitShapeDimensions();
    vecHalfSize += GetBtShape()->getMargin();
    return vecHalfSize * 2;
}
