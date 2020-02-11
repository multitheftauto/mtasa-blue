/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
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
    CLuaPhysicsStaticCollision(CClientPhysics* pPhysics);
    ~CLuaPhysicsStaticCollision();

    void RemoveScriptID();

    // use only once

    void               SetCollisionShape(btCollisionShape* pShape);
    void               SetCollisionShape(CLuaPhysicsShape* pShape);
    btCollisionObject* InitializeWithCompound();

    btCollisionObject* InitializeWithBox(CVector& vecHalf);
    btCollisionObject* InitializeWithBoxes(std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList, CVector& position, CVector& rotation);

    btCollisionObject* InitializeWithSphere(float fRadius);

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

    CClientPhysics*    GetPhysics() const { return m_pPhysics; }
    uint               GetScriptID() const { return m_uiScriptID; }
    btCollisionObject* GetCollisionObject() const { return m_btCollisionObject; }
    void               SetCollisionHandler(const CLuaFunctionRef& iLuaFunction) { m_iLuaCollisionHandlerFunction = iLuaFunction; }

private:
    CClientPhysics*          m_pPhysics;
    uint m_uiScriptID;
    btCollisionObject*       m_btCollisionObject;
    CLuaFunctionRef          m_iLuaCollisionHandlerFunction;

};
