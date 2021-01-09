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

class CLuaPhysicsBoxShape : public CLuaPhysicsConvexShape
{
    friend class CBulletPhysics;

public:
    CLuaPhysicsBoxShape(CBulletPhysics* pPhysics, CVector half);
    CVector GetSize();
    bool    SetSize(CVector vecSize);
    void    Update() {}

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::BoxShape; }
};
