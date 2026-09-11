/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaVector4.cpp
 *  PURPOSE:     Lua vector4 class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVector4D.h"

class CLuaVector4D : public CVector4D
{
public:
    CLuaVector4D() : CVector4D() {}
    CLuaVector4D(const CVector4D& vector) : CVector4D(vector) {}
    CLuaVector4D(float fX, float fY, float fZ, float fW) : CVector4D(fX, fY, fZ, fW) {}
};
