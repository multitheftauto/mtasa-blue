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

struct SPhysicsCollisionContact
{
    CVector vecPositionWorldOn;
    CVector vecLocalPoint;
    CVector vecLateralFrictionDir;
    int     contactTriangle;
    float   appliedImpulse;
    float   appliedImpulseLiteral;
};

struct SPhysicsCollisionReport
{
    std::shared_ptr<CLuaPhysicsElement>                    pElement;
    std::vector<std::shared_ptr<SPhysicsCollisionContact>> m_vecContacts;
};

class CLuaPhysicsWorldElement : public CLuaPhysicsElement
{
protected:
    CLuaPhysicsWorldElement(CClientPhysics* pPhysics, EIdClass::EIdClassType classType);
    ~CLuaPhysicsWorldElement();

public:

};
