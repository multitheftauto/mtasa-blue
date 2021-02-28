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

class CLuaPhysicsCapsuleShape : public CLuaPhysicsConvexShape
{
public:
    CLuaPhysicsCapsuleShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    ~CLuaPhysicsCapsuleShape();

    bool SetRadius(float fRadius);
    float GetRadius();
    bool SetHeight(float fHeight);
    float GetHeight();
    void Update() {}

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::BvhTriangleMeshShape; }
};
