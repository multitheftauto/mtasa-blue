/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsElement.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsElement;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsElement
{
public:
    CLuaPhysicsElement(CClientPhysics* pPhysics, EIdClass::EIdClassType classType);
    ~CLuaPhysicsElement();

    CClientPhysics* GetPhysics() const { return m_pPhysics; }
    uint            GetScriptID() const { return m_uiScriptID; }
    EIdClass::EIdClassType GetClassType() const { return m_classType; }
    void                   Destroy();

private:
    void RemoveScriptID();

    CClientPhysics*        m_pPhysics;
    EIdClass::EIdClassType m_classType;
    uint                   m_uiScriptID;
};
