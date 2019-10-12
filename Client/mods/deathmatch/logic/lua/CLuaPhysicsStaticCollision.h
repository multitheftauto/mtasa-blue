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
class btHeightfieldTerrainShape;
class heightfieldTerrainShape;

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

    btCollisionObject* InitializeWithHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData);

    void SetPosition(CVector& vecPosition);
    void SetRotation(CVector& vecRotation);

    void SetScale(CVector& vecScale);
    void GetScale(CVector& vecScale);

    void SetDebugColor(SColor color);
    void GetDebugColor(SColor& color);

    void SetFilterMask(short sIndex, bool bEnabled);
    void GetFilterMask(short sIndex, bool& bEnabled);

    void SetFilterGroup(int iGroup);
    void GetFilterGroup(int& iGroup);

    uint               GetScriptID() const { return m_uiScriptID; }
    btCollisionObject* GetCollisionObject() const { return m_btCollisionObject; }

private:
    btDiscreteDynamicsWorld* m_pWorld;
    uint m_uiScriptID;
    btCollisionObject*       m_btCollisionObject;
    heightfieldTerrainShape* m_pHeightfieldTerrain;

};
