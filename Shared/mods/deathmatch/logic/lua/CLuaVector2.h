/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaVector2.h
 *  PURPOSE:     Lua vector2 class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVector2D.h"

class CLuaVector2D : public CVector2D
{
public:
    CLuaVector2D();
    CLuaVector2D(const CVector2D& vector);
    CLuaVector2D(float fX, float fY);

    ~CLuaVector2D();

    unsigned int GetScriptID() const { return m_uiScriptID; }

    static CLuaVector2D* GetFromScriptID(unsigned int uiScriptID);

private:
    unsigned int m_uiScriptID;
};
