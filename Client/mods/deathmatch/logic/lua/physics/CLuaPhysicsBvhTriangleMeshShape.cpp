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
#include "CLuaPhysicsSharedLogic.h"
#include "CLuaPhysicsBvhTriangleMeshShape.h"
#include "CLuaPhysicsShapeManager.h"

CLuaPhysicsBvhTriangleMeshShape::CLuaPhysicsBvhTriangleMeshShape(CClientPhysics* pPhysics, std::vector<CVector>& vecIndices)
    : CLuaPhysicsShape(pPhysics, std::move(CLuaPhysicsSharedLogic::CreateBvhTriangleMesh(vecIndices)))
{
}

CLuaPhysicsBvhTriangleMeshShape::~CLuaPhysicsBvhTriangleMeshShape()
{
}
