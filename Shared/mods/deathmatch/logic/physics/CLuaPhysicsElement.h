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

#pragma once

class CLuaPhysicsElement
{
protected:
    CLuaPhysicsElement(EIdClass::EIdClassType classType);
    ~CLuaPhysicsElement();

public:
    uint                   GetScriptID() const { return m_uiScriptID; }
    EIdClass::EIdClassType GetClassType() const { return m_classType; }
    virtual bool           Destroy() = 0;

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::Shape; }

    const char* GetName() { return EnumToString(GetType()); }

private:
    void RemoveScriptID();

    EIdClass::EIdClassType m_classType;
    unsigned int           m_uiScriptID;
};
