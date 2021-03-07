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

class CLuaPhysicsSphereShape : public CLuaPhysicsConvexShape, CShapeHolder<btSphereShape>
{
public:
    CLuaPhysicsSphereShape(CBulletPhysics* pPhysics, float radius);
    ~CLuaPhysicsSphereShape();

    void  SetRadius(float fRadius);
    float GetRadius();
    void  Update() {}
    SBoundingSphere GetBoundingSphere();

    btSphereShape*              GetBtShape() const { return (btSphereShape*)InternalGetBtShape(); }
    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::SphereShape; }
};
