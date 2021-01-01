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

CLuaPhysicsBoxShape::CLuaPhysicsBoxShape(CBulletPhysics* pPhysics, CVector half) : CLuaPhysicsShape(pPhysics, CLuaPhysicsSharedLogic::CreateBox(half))
{
}

bool CLuaPhysicsBoxShape::SetSize(CVector& vecSize)
{
    btConvexInternalShape* pInternalShape = (btConvexInternalShape*)GetBtShape();
    vecSize.fX -= pInternalShape->getMargin();
    vecSize.fY -= pInternalShape->getMargin();
    vecSize.fZ -= pInternalShape->getMargin();
    pInternalShape->setImplicitShapeDimensions(reinterpret_cast<btVector3&>(vecSize));
    UpdateRigids();
    return true;
}

bool CLuaPhysicsBoxShape::GetSize(CVector& vecSize)
{
    btConvexInternalShape* pInternalShape = (btConvexInternalShape*)GetBtShape();
    const btVector3        pSize = pInternalShape->getImplicitShapeDimensions();
    vecSize.fX = pSize.getX();
    vecSize.fY = pSize.getY();
    vecSize.fZ = pSize.getZ();
    return true;
}
