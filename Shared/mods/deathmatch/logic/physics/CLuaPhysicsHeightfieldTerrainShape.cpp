/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsHeightfieldTerrainShape.cpp
 *  PURPOSE:     Lua physics heightfield terrain shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsHeightfieldTerrainShape::CLuaPhysicsHeightfieldTerrainShape(CBulletPhysics* pPhysics, int iSizeX, int iSizeY, std::vector<float> vecHeightData)
    : CLuaPhysicsConcaveShape(pPhysics, std::move(CPhysicsSharedLogic::CreateHeightfieldTerrain(iSizeX, iSizeY, vecHeightData)))
{
    m_iSizeX = iSizeX;
    m_iSizeY = iSizeY;
}

CLuaPhysicsHeightfieldTerrainShape::~CLuaPhysicsHeightfieldTerrainShape()
{

}

void CLuaPhysicsHeightfieldTerrainShape::SetHeight(int index, float fHeight)
{
    GetHeightfieldData()[index] = fHeight;
}
