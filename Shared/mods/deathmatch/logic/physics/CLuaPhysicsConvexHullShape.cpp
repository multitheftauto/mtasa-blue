/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConvexHullShape.cpp
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsConvexHullShape::CLuaPhysicsConvexHullShape(CBulletPhysics* pPhysics, std::vector<CVector>& vecPoints) : CLuaPhysicsShape(pPhysics, std::move(CPhysicsSharedLogic::CreateConvexHull(vecPoints)))
{
}

CLuaPhysicsConvexHullShape::~CLuaPhysicsConvexHullShape()
{
}

SBoundingBox CLuaPhysicsConvexHullShape::GetBoundingBox()
{
    ElementLock lk(this);
    btTransform transform = btTransform::getIdentity();
    btVector3   min, max;
    GetBtShape()->getAabb(transform, min, max);
    SBoundingBox sBox = SBoundingBox();
    sBox.vecMin = min;
    sBox.vecMax = max;
    sBox.vecMin += GetBtShape()->getMargin() * 2;
    sBox.vecMax -= GetBtShape()->getMargin() * 2;
    return sBox;
}
