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

class CLuaPhysicsElement;
class CLuaPhysicsStaticCollision;
class btHeightfieldTerrainShape;
class heightfieldTerrainShape;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsStaticCollision : public CLuaPhysicsElement
{
public:
    CLuaPhysicsStaticCollision(CClientPhysics* pPhysics);
    ~CLuaPhysicsStaticCollision();

    // use only once

    void               SetCollisionShape(btCollisionShape* pShape);
    void               SetCollisionShape(CLuaPhysicsShape* pShape);
    btCollisionObject* InitializeWithCompound();

    btCollisionObject* InitializeWithBox(CVector& vecHalf);
    btCollisionObject* InitializeWithBoxes(std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList, CVector& position, CVector& rotation);

    btCollisionObject* InitializeWithSphere(float fRadius);

    void SetPosition(CVector vecPosition);
    void GetPosition(CVector& vecPosition);
    void SetRotation(CVector vecRotation);
    void GetRotation(CVector& vecRotation);

    bool SetScale(CVector& vecScale);
    bool GetScale(CVector& vecScale);

    void RemoveDebugColor();
    void SetDebugColor(SColor color);
    void GetDebugColor(SColor& color);

    void SetFilterMask(short sIndex, bool bEnabled);
    void GetFilterMask(short sIndex, bool& bEnabled);

    void SetFilterGroup(int iGroup);
    void GetFilterGroup(int& iGroup);

    btCollisionObject* GetCollisionObject() const { return m_btCollisionObject; }
    void               SetCollisionHandler(const CLuaFunctionRef& iLuaFunction) { m_iLuaCollisionHandlerFunction = iLuaFunction; }

private:
    btCollisionObject*       m_btCollisionObject;
    CLuaFunctionRef          m_iLuaCollisionHandlerFunction;

};
