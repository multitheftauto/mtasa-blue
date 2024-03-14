/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaMatrix.h
 *  PURPOSE:     Lua matrix class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CLuaMatrix : public CMatrix
{
public:
    CLuaMatrix();
    CLuaMatrix(const CMatrix& matrix);

    ~CLuaMatrix();

    std::uint32_t GetScriptID() const { return m_uiScriptID; }

    static CLuaMatrix* GetFromScriptID(std::uint32_t uiScriptID);

private:
    std::uint32_t m_uiScriptID;
};
