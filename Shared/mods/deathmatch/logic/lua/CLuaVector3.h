/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaVector3.h
 *  PURPOSE:     Lua vector3 class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVector.h"

class CLuaVector3D : public CVector
{
public:
    CLuaVector3D() : CVector() {}
    CLuaVector3D(const CVector& vector) : CVector(vector) {}
    CLuaVector3D(float fX, float fY, float fZ) : CVector(fX, fY, fZ) {}
};
