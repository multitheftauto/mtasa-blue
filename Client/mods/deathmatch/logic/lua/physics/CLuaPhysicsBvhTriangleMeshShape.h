/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsBhvTriangleMeshShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsBvhTriangleMeshShape;

#pragma once

// Define includes
#include "../LuaCommon.h"
#include "../CLuaArguments.h"
#include "CLuaPhysicsShape.h"

class CLuaPhysicsBvhTriangleMeshShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsBvhTriangleMeshShape(CClientPhysics* pPhysics, std::vector<CVector>& vecIndices);
    ~CLuaPhysicsBvhTriangleMeshShape();
};
