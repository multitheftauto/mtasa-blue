/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsTriangleMeshShape.cpp
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsSharedLogic.h"
#include "CLuaPhysicsTriangleMeshShape.h"
#include "CLuaPhysicsShapeManager.h"

CLuaPhysicsTriangleMeshShape::CLuaPhysicsTriangleMeshShape(CClientPhysics* pPhysics, std::vector<CVector>& vecIndices) : CLuaPhysicsShape(pPhysics)
{
    btBvhTriangleMeshShape* pTriangleMeshShape = CLuaPhysicsSharedLogic::CreateTriangleMesh(vecIndices);
    FinalizeInitialization(pTriangleMeshShape);
}

CLuaPhysicsTriangleMeshShape::~CLuaPhysicsTriangleMeshShape()
{
}
