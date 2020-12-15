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
#include "CLuaPhysicsSharedLogic.h"
#include "CLuaPhysicsElement.h"
#include "CLuaPhysicsShapeManager.h"

CLuaPhysicsElement::CLuaPhysicsElement(CClientPhysics* pPhysics, EIdClass::EIdClassType classType)
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

void CLuaPhysicsElement::Destroy()
{
    m_pPhysics->DestroyElement(this);
}

bool CLuaPhysicsElement::IsSafeToAccess() const
{
    return m_pPhysics->isDuringSimulation;
}

void CLuaPhysicsElement::ApplyChanges()
{
    while (!m_stackChanges.empty())
    {
        m_stackChanges.top()();
        m_stackChanges.pop();
    }
    m_bHasEnqueuedChanges = false;
}

void CLuaPhysicsElement::ApplyOrEnqueueChange(std::function<void()> change)
{
    if (IsSafeToAccess())
        change();
    else
    {
        if (!m_bHasEnqueuedChanges)
        {
            m_bHasEnqueuedChanges = true;
            GetPhysics()->AddToChangesStack(this);
        }
        m_stackChanges.push(change);
    }
}
