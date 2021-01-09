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

CLuaPhysicsBoxShape::CLuaPhysicsBoxShape(CBulletPhysics* pPhysics, CVector half) : CLuaPhysicsConvexShape(pPhysics, CLuaPhysicsSharedLogic::CreateBox(half))
{
}

bool CLuaPhysicsBoxShape::SetSize(CVector vecSize)
{
    btConvexInternalShape* pInternalShape = (btConvexInternalShape*)GetBtShape();
    vecSize.fX -= pInternalShape->getMargin();
    vecSize.fY -= pInternalShape->getMargin();
    vecSize.fZ -= pInternalShape->getMargin();
    pInternalShape->setImplicitShapeDimensions(vecSize);
    UpdateRigids();
    return true;
}

CVector CLuaPhysicsBoxShape::GetSize()
{
    btConvexInternalShape* pInternalShape = (btConvexInternalShape*)GetBtShape();
    CVector                vecHalfSize = pInternalShape->getImplicitShapeDimensions();
    vecHalfSize += pInternalShape->getMargin();
    return vecHalfSize * 2;
}
