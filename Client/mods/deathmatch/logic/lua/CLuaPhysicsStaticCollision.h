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
    btCollisionObject* InitializeWithCompound();

    btCollisionObject* InitializeWithBox(CVector& vecHalf);
    btCollisionObject* InitializeWithBoxes(std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList, CVector& position, CVector& rotation);

    btCollisionObject* InitializeWithSphere(float fRadius);
    btCollisionObject* InitializeWithTriangleMesh(std::vector<CVector>& vecIndices, CVector position = CVector(0, 0, 0), CVector rotation = CVector(0, 0, 0));

    inline static void SetPosition(btTransform& transform, CVector& vecRotation);
    inline static void SetRotation(btTransform& transform, CVector& vecRotation);

    static void SetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition);
    static void SetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation);

    void SetPosition(CVector& vecPosition);
    void SetRotation(CVector& vecRotation);

    btBoxShape*             CreateBox(CVector& half, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    btSphereShape*          CreateSphere(float fRadius, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    btBvhTriangleMeshShape* CreateTriangleMesh(std::vector<CVector>& vecIndices);

    bool AddBox(CVector& half, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));
    bool AddBoxes(std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList);
    bool AddSphere(float fRadius, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));
    bool AddSpheres(std::vector<std::pair<float, std::pair<CVector, CVector>>>& spheresList);

    uint GetScriptID() const { return m_uiScriptID; }

private:
    btDiscreteDynamicsWorld* m_pWorld;
    uint m_uiScriptID;
    btCollisionObject*       m_btCollisionObject;

};
