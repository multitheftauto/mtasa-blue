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
    : CLuaPhysicsConcaveShape(pPhysics, std::move(CLuaPhysicsSharedLogic::CreateGimpactMeshShape(vecVertices)))
{
}

CLuaPhysicsGimpactTriangleMeshShape::~CLuaPhysicsGimpactTriangleMeshShape()
{
}
