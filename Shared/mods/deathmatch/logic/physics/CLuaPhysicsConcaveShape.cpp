/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConcaveShape.cpp
 *  PURPOSE:     Lua physics concave shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsConcaveShape::CLuaPhysicsConcaveShape(CBulletPhysics* pPhysics, btCollisionShape* pShape) : CLuaPhysicsShape(pPhysics, pShape)
{
}
CLuaPhysicsConcaveShape::CLuaPhysicsConcaveShape(CBulletPhysics* pPhysics, heightfieldTerrainShape* pHeightfieldTerrainShape)
    : CLuaPhysicsShape(pPhysics, pHeightfieldTerrainShape)
{
}
