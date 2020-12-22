/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/physics/CPhysicsProxyElement.h
 *  PURPOSE:     Manages enabling and disabling elements
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPhysicsProxyElement;

#pragma once

class CPhysicsProxyElement
{
public:
    CPhysicsProxyElement(){};


    ~CPhysicsProxyElement()
    {
    }

    CClientPhysics* GetPhysics() const { return m_pPhysics; }

    virtual void SetEnabled(bool bEnabled) = 0;
    bool         IsEnabled() const { return m_bEnabled; }

protected:
    CClientPhysics* m_pPhysics;
    bool            m_bEnabled = false;
};
