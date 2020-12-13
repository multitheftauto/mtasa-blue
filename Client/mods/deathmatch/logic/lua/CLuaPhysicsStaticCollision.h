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

#pragma once

#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsStaticCollision : public CLuaPhysicsElement
{
public:
    CLuaPhysicsStaticCollision(CLuaPhysicsShape* pShape);
    ~CLuaPhysicsStaticCollision();

    void    SetPosition(const CVector& vecPosition) const;
    CVector GetPosition() const;
    void    SetRotation(const CVector& vecRotation) const;
    CVector GetRotation() const;
    bool    SetScale(const CVector& vecScale) const;
    CVector GetScale() const;

    void Initialize();

    void    RemoveDebugColor() const;
    void    SetDebugColor(const SColor& color) const;
    SColor& GetDebugColor() const;

    void SetFilterMask(int mask) const;

    void SetFilterGroup(int iGroup) const;
    int  GetFilterGroup() const;

    btCollisionObject* GetCollisionObject() const { return m_btCollisionObject.get(); }

    void Unlink();

private:
    std::unique_ptr<btCollisionObject> m_btCollisionObject;
    CLuaPhysicsShape*                  m_pShape;
};
