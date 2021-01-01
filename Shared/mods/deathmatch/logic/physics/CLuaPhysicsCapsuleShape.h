/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsCapsuleShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsCapsuleShape;

#pragma once

class CLuaPhysicsCapsuleShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsCapsuleShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    ~CLuaPhysicsCapsuleShape();

    bool SetRadius(float fRadius);
    bool GetRadius(float& fRadius);
    bool SetHeight(float fHeight);
    bool GetHeight(float& fHeight);
    void Update() {}
};
