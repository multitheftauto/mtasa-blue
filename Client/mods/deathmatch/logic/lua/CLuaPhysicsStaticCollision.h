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
    CLuaPhysicsStaticCollision(CLuaPhysicsShape* pShape);
    ~CLuaPhysicsStaticCollision();

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

    btCollisionObject* GetCollisionObject() const { return m_btCollisionObject.get(); }

private:
    std::unique_ptr<btCollisionObject> m_btCollisionObject;
};
