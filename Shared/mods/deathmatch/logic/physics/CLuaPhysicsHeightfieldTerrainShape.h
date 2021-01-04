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

class CLuaPhysicsHeightfieldTerrainShape : public CLuaPhysicsConcaveShape
{
public:
    CLuaPhysicsHeightfieldTerrainShape(CBulletPhysics* pPhysics, int iSizeX, int iSizeY, std::vector<float> vecHeightData);
    ~CLuaPhysicsHeightfieldTerrainShape();

    void Update() {}

    int  GetVerticesNum() const { return m_iSizeX * m_iSizeY; }
    void SetHeight(int index, float fHeight);

private:
    int                m_iSizeX;
    int                m_iSizeY;
};
