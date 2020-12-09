/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsBoxShape.h
 *  PURPOSE:     Lua physics box shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsBoxShape;
class CLuaPhysicsShape;

#pragma once

#include "LuaCommon.h"
#include "CLuaArguments.h"
#include "lua/CLuaPhysicsShape.h"
#include "lua/CLuaPhysicsBoxShape.h"

class CLuaPhysicsBoxShape : public CLuaPhysicsShape
{
    friend class CClientPhysics;

public:
    CLuaPhysicsBoxShape(CClientPhysics* pPhysics, CVector half);
    bool GetSize(CVector& vecSize);
    bool SetSize(CVector& vecSize);
};
