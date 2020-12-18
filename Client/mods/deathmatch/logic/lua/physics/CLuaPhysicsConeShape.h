/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConeShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsConeShape;

#pragma once

// Define includes
#include "../LuaCommon.h"
#include "../CLuaArguments.h"

class CLuaPhysicsConeShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsConeShape(CClientPhysics* pPhysics, float fRadius, float fHeight);
    ~CLuaPhysicsConeShape();

    bool SetRadius(float fRadius);
    bool GetRadius(float& fRadius);
    bool SetHeight(float fHeight);
    bool GetHeight(float& fHeight);
};
