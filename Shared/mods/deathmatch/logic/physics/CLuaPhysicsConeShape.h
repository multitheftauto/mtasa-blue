/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConeShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsConeShape;

#pragma once

class CLuaPhysicsConeShape : public CLuaPhysicsConvexShape, CShapeHolder<btConeShape>
{
public:
    CLuaPhysicsConeShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    ~CLuaPhysicsConeShape();

    void         SetRadius(float fRadius);
    float        GetRadius();
    bool         SetHeight(float fHeight);
    float        GetHeight();
    void         Update() {}
    SBoundingBox GetBoundingBox();


    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::ConeShape; }
    btConeShape*                GetBtShape() const { return (btConeShape*)InternalGetBtShape(); }
};
