/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsSphereShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsSphereShape;

#pragma once

class CLuaPhysicsSphereShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsSphereShape(CBulletPhysics* pPhysics, float radius);
    ~CLuaPhysicsSphereShape();

    bool SetRadius(float fRadius);
    bool GetRadius(float& fRadius);
    void Update() {}
};
