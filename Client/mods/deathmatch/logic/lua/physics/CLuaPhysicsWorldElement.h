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
struct SPhysicsCollisionContact;
struct SPhysicsCollisionReport;

#pragma once

class CLuaPhysicsWorldElement : public CLuaPhysicsElement
{
protected:
    CLuaPhysicsWorldElement(CClientPhysics* pPhysics, EIdClass::EIdClassType classType);
    ~CLuaPhysicsWorldElement();

public:
    virtual void    SetPosition(const CVector& vecPosition) = 0;
    virtual CVector GetPosition() const = 0;
    virtual void    SetRotation(const CVector& vecRotation) = 0;
    virtual CVector GetRotation() const = 0;
    virtual bool    SetScale(const CVector& vecScale) = 0;
    virtual CVector GetScale() const = 0;
    virtual bool    SetMatrix(const CMatrix& matrix) = 0;
    virtual CMatrix GetMatrix() const = 0;

    virtual void          RemoveDebugColor() = 0;
    virtual void          SetDebugColor(const SColor& color) = 0;
    virtual const SColor& GetDebugColor() const = 0;

    virtual int  GetFilterGroup() const = 0;
    virtual void SetFilterGroup(int iGroup) = 0;
    virtual int  GetFilterMask() const = 0;
    virtual void SetFilterMask(int mask) = 0;
};
