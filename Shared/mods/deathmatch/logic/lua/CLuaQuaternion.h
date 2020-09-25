/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaQuaternion.h
 *  PURPOSE:     Lua quaternion class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CLuaQuaternion : public CQuaternion
{
public:
    CLuaQuaternion();
    CLuaQuaternion(const CQuaternion& quat);

    ~CLuaQuaternion();

    unsigned int GetScriptID() const { return m_uiScriptID; }

    static CLuaQuaternion* GetFromScriptID(unsigned int uiScriptID);

private:
    unsigned int m_uiScriptID;
};
