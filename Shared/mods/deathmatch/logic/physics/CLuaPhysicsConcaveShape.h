/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConvexShape.h
 *  PURPOSE:     Lua physics convex shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsConcaveShape;

#pragma once

#include "CLuaPhysicsElement.h"

class CLuaPhysicsConcaveShape : public CLuaPhysicsShape
{
public:
    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::ConcaveShape; }

protected:
    CLuaPhysicsConcaveShape(CBulletPhysics* pPhysics, btCollisionShape* pShape);
    CLuaPhysicsConcaveShape(CBulletPhysics* pPhysics, heightfieldTerrainShape* pHeightfieldTerrainShape);

};
