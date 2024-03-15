/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaVector4.cpp
 *  PURPOSE:     Lua vector4 class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CLuaVector4D : public CVector4D
{
public:
    CLuaVector4D();
    CLuaVector4D(const CVector4D& vector);
    CLuaVector4D(float fX, float fY, float fZ, float fW);

    ~CLuaVector4D();

    std::uint32_t GetScriptID() const { return m_uiScriptID; }

    static CLuaVector4D* GetFromScriptID(std::uint32_t uiScriptID);

private:
    std::uint32_t m_uiScriptID;
};
