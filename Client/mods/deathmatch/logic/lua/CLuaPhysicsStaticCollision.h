/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollision.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsStaticCollision;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsStaticCollision
{
public:
    CLuaPhysicsStaticCollision(btDiscreteDynamicsWorld* pWorld);
    ~CLuaPhysicsStaticCollision();

    void RemoveScriptID();

    // use only once
    btCollisionObject* InitializeWithBox(CVector& vecHalf);
    btCollisionObject* InitializeWithBoxes(std::vector<std::pair<CVector, CVector>>& halfList, CVector& position, CVector& rotatio);

    btCollisionObject* InitializeWithSphere(float fRadius);
    btCollisionObject* InitializeWithTriangleMesh(std::vector<CVector>& vecIndices, CVector position = CVector(0, 0, 0), CVector rotation = CVector(0, 0, 0));

    static void SetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition);
    static void SetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation);

    void SetPosition(CVector& vecPosition);
    void SetRotation(CVector& vecRotation);

    btBoxShape*             CreateBox(CVector half);
    btSphereShape*          CreateSphere(float fRadius);
    btBvhTriangleMeshShape* CreateTriangleMesh(std::vector<CVector>& vecIndices);


    uint GetScriptID() const { return m_uiScriptID; }

private:
    btDiscreteDynamicsWorld* m_pWorld;
    uint m_uiScriptID;
    btCollisionObject*       m_btCollisionObject;

};
