/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConvexHullShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/


#pragma once

class CLuaPhysicsConvexHullShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsConvexHullShape(CBulletPhysics* pPhysics, std::vector<CVector>& vecPoints);
    ~CLuaPhysicsConvexHullShape();
    void Update() {}

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::ConvexHullShape; }
};
