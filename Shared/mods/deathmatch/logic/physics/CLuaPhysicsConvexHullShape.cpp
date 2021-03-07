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

SBoundingBox CLuaPhysicsConvexHullShape::GetBoundingBox(btTransform transform)
{
    btVector3   min, max;
    {
        ElementLock lk(this);
        GetBtShape()->getAabb(transform, min, max);
    }
    SBoundingBox sBox = SBoundingBox();
    sBox.vecMin = min;
    sBox.vecMax = max;
    sBox.vecMin += GetMargin() * 2;
    sBox.vecMax -= GetMargin() * 2;
    return sBox;
}
