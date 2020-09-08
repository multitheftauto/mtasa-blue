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

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsBoxShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsBoxShape(CClientPhysics* pPhysics, CVector half);
    ~CLuaPhysicsBoxShape();

    bool GetSize(CVector& vecSize);
    bool SetSize(CVector& vecSize);

};
