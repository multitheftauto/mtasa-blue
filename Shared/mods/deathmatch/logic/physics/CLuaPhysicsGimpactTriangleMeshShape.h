/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsBhvTriangleMeshShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsGimpactTriangleMeshShape;

#pragma once

class CLuaPhysicsGimpactTriangleMeshShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsGimpactTriangleMeshShape(CBulletPhysics* pPhysics, std::vector<CVector>& vecVertices);
    ~CLuaPhysicsGimpactTriangleMeshShape();
    void Update() {}
};
