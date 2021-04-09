/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsWorldElement.h
 *  PURPOSE:     Lua physics common functionality for elements whichin world
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsWorldElement;

#pragma once

class CLuaPhysicsWorldElement : public CLuaPhysicsElement
{
protected:
    CLuaPhysicsWorldElement(EIdClass::EIdClassType classType);
    ~CLuaPhysicsWorldElement();

public:
    virtual void               SetPosition(CVector vecPosition) = 0;
    virtual const CVector      GetPosition() const = 0;
    virtual void               SetRotation(CVector vecRotation) = 0;
    virtual const CVector      GetRotation() const = 0;
    virtual btCollisionObject* GetBtCollisionObject() const = 0;

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::WorldElement; }

    virtual void SetEnabled(bool bEnabled) = 0;
    virtual bool IsEnabled() const = 0;
};
