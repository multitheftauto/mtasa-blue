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

CLuaPhysicsElement::CLuaPhysicsElement(CBulletPhysics* pPhysics, EIdClass::EIdClassType classType)
{
    m_pPhysics = pPhysics;
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

bool CLuaPhysicsElement::Destroy()
{
    m_pPhysics->DestroyElement(this);
    return true;
}

bool CLuaPhysicsElement::IsSafeToAccess() const
{
    return m_pPhysics->isDuringSimulation;
}

void CLuaPhysicsElement::ApplyChanges()
{
    while (!m_listChanges.empty())
    {
        m_listChanges.pop()();
    }
    Update();
    m_bHasEnqueuedChanges = false;
}

void CLuaPhysicsElement::NeedsUpdate()
{
    if (!m_bNeedsUpdate)
    {
        m_bNeedsUpdate = true;
        GetPhysics()->AddToBatchUpdate(this);
    }
}
