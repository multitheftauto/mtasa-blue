/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsElement.cpp
 *  PURPOSE:     Lua physics element class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

#include "CLuaPhysicsElement.h"

CLuaPhysicsElement::CLuaPhysicsElement(EIdClass::EIdClassType classType)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, classType);
    m_classType = classType;
}

CLuaPhysicsElement::~CLuaPhysicsElement()
{
    RemoveScriptID();
}

void CLuaPhysicsElement::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, m_classType, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}
