/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsCylinderShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsCylinderShape;

#pragma once

class CLuaPhysicsCylinderShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsCylinderShape(CBulletPhysics* pPhysics, CVector half);
    ~CLuaPhysicsCylinderShape();

    bool SetRadius(float fRadius);
    bool GetRadius(float& fRadius);
    bool SetHeight(float fHeight);
    bool GetHeight(float& fHeight);
    void Update() {}
};
