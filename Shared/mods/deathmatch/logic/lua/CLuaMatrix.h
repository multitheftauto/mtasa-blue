/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaMatrix.h
 *  PURPOSE:     Lua matrix class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CMatrix.h"

class CLuaMatrix : public CMatrix
{
public:
    CLuaMatrix() : CMatrix() {}
    CLuaMatrix(const CMatrix& matrix) : CMatrix(matrix) {}
};
