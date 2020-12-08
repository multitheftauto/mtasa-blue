/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConvexHullShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/


#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"
#include "lua/CLuaPhysicsShape.h"

class CLuaPhysicsConvexHullShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsConvexHullShape(CClientPhysics* pPhysics, std::vector<CVector>& vecPoints);
    ~CLuaPhysicsConvexHullShape();
};
