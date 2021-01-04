/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaGenericDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CLuaDefs.h"

class CLuaGenericDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

private:
    static bool IsTransferBoxVisible();
};
