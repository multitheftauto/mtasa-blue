/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsHeightfieldTerrainShape.h
 *  PURPOSE:     Lua physics heightfield terrain shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsHeightfieldTerrainShape;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsHeightfieldTerrainShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsHeightfieldTerrainShape(CClientPhysics* pPhysics, int iSizeX, int iSizeY, std::vector<float>& vecHeightData);
    ~CLuaPhysicsHeightfieldTerrainShape();


private:
    std::vector<float> m_heightfieldTerrainData;
};
