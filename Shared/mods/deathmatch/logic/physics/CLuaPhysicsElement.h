/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsElement.h
 *  PURPOSE:     Base class for physics elements
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsElement;
class CResource;

#pragma once

#include "CIdArray.h"
#include "Enums.h"

class CLuaPhysicsElement
{
protected:
    CLuaPhysicsElement(EIdClass::EIdClassType classType);
    ~CLuaPhysicsElement();

public:
    uint                   GetScriptID() const { return m_uiScriptID; }
    EIdClass::EIdClassType GetClassType() const { return m_classType; }

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::Shape; }

    const char* GetName() { return EnumToString(GetType()); }

    void SetOwnedResource(CResource* pResource) { m_pOwnedResource = pResource; }
    CResource* GetOwnedResource() const { return m_pOwnedResource; }

private:
    void RemoveScriptID();

    EIdClass::EIdClassType m_classType;
    CResource*             m_pOwnedResource = nullptr;
    unsigned int           m_uiScriptID;
};
