/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/physics/CPhysicsStaticCollisionProxy.h
 *  PURPOSE:     Manages creation and destruction of static collision
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPhysicsStaticCollisionProxy;

#include "CPhysicsProxyElement.h"

#pragma once

class CLuaPhysicsShape;

// Create using static member method "New"
class CPhysicsStaticCollisionProxy : public CPhysicsProxyElement, public btCollisionObject
{
public:
    CPhysicsStaticCollisionProxy(CLuaPhysicsShape* pPhysicsShape);

    static std::unique_ptr<CPhysicsStaticCollisionProxy> New(CLuaPhysicsShape* pPhysicsShape);
    void SetEnabled(bool bEnabled);
    bool IsEnabled() const { return m_bEnabled; }
};
