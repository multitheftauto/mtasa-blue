/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsBvhTriangleMeshShape.cpp
 *  PURPOSE:     Lua physics triangle mesh shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsGimpactTriangleMeshShape::CLuaPhysicsGimpactTriangleMeshShape(CBulletPhysics* pPhysics, std::vector<CVector>& vecVertices)
    : CLuaPhysicsConcaveShape(pPhysics, std::move(CPhysicsSharedLogic::CreateGimpactMeshShape(vecVertices)))
{
}

CLuaPhysicsGimpactTriangleMeshShape::~CLuaPhysicsGimpactTriangleMeshShape()
{
    delete GetBtShape()->getMeshInterface();
}

SBoundingBox CLuaPhysicsGimpactTriangleMeshShape::GetBoundingBox()
{
    ElementLock lk(this);
    btTransform transform = btTransform::getIdentity();
    btVector3   min, max;
    GetBtShape()->getAabb(transform, min, max);
    SBoundingBox sBox = SBoundingBox();
    sBox.vecMin = min;
    sBox.vecMax = max;
    float fMargin = GetBtShape()->getMeshPart(0)->getMargin();
    sBox.vecMin += fMargin;
    sBox.vecMax -= fMargin;
    return sBox;
}
