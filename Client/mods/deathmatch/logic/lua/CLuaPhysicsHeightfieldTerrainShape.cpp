/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsHeightfieldTerrainShape.cpp
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsSharedLogic.h"
#include "CLuaPhysicsShape.h"
#include "CLuaPhysicsHeightfieldTerrainShape.h"

CLuaPhysicsHeightfieldTerrainShape::CLuaPhysicsHeightfieldTerrainShape(CClientPhysics* pPhysics, int iSizeX, int iSizeY, std::vector<float>& vecHeightData)
    : CLuaPhysicsShape(pPhysics)
{
    heightfieldTerrainShape* pHeightfieldTerrain = CLuaPhysicsSharedLogic::CreateHeightfieldTerrain(iSizeX, iSizeY, vecHeightData);
    FinalizeInitialization(pHeightfieldTerrain->pHeightfieldTerrainShape);
    m_heightfieldTerrainData = pHeightfieldTerrain;
}

CLuaPhysicsHeightfieldTerrainShape::~CLuaPhysicsHeightfieldTerrainShape()
{
    if (m_heightfieldTerrainData != nullptr)
        delete m_heightfieldTerrainData;
}
