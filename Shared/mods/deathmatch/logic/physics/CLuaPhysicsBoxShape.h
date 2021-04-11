/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsBoxShape.h
 *  PURPOSE:     Lua physics box shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsBoxShape;

#pragma once

class CLuaPhysicsBoxShape : public CLuaPhysicsConvexShape, CShapeHolder<btBoxShape>
{
    friend class CBulletPhysics;

public:
    CLuaPhysicsBoxShape(CVector half);
    void    Update() {}

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::BoxShape; }
    btBoxShape*                 GetBtShape() const { return (btBoxShape*)InternalGetBtShape(); }
};
