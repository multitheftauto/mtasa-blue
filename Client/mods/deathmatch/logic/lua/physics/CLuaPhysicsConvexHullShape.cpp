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
#include "CLuaPhysicsShapeManager.h"
#include "CLuaPhysicsConvexHullShape.h"

CLuaPhysicsConvexHullShape::CLuaPhysicsConvexHullShape(CClientPhysics* pPhysics, std::vector<CVector>& vecPoints) : CLuaPhysicsShape(pPhysics, std::move(CLuaPhysicsSharedLogic::CreateConvexHull(vecPoints)))
{
}

CLuaPhysicsConvexHullShape::~CLuaPhysicsConvexHullShape()
{
}

