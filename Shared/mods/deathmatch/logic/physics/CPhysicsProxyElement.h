/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/physics/CPhysicsProxyElement.h
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
    CPhysicsProxyElement() {}

    ~CPhysicsProxyElement() {}

    virtual void SetEnabled(bool bEnabled) = 0;
    bool         IsEnabled() const { return m_bEnabled; }

protected:
    bool m_bEnabled = false;
};
