/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsTriangleMeshShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsTriangleMeshShape;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"
#include "CLuaPhysicsShape.h"

class CLuaPhysicsTriangleMeshShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsTriangleMeshShape(CClientPhysics* pPhysics, std::vector<CVector>& vecIndices);
    ~CLuaPhysicsTriangleMeshShape();
};
