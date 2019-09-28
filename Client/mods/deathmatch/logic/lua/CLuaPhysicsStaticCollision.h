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
    void InitializeWithBox(CVector& vecHalf);
    void InitializeWithBoxes(std::vector<std::pair<CVector, CVector>>& halfList, CVector& position, CVector& rotatio);

    void InitializeWithSphere(float fRadius);
    void InitializeWithTriangleMesh(std::vector<CVector>& vecIndices, CVector position = CVector(0, 0, 0), CVector rotation = CVector(0, 0, 0));

    void SetPosition(CVector& vecPosition);
    void SetRotation(CVector& vecRotation);

    uint GetScriptID() const { return m_uiScriptID; }

private:
    btDiscreteDynamicsWorld* m_pWorld;
    uint m_uiScriptID;
    btCollisionObject*       m_btCollisionObject;

};
