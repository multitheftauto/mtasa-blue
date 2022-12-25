/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaClientDefs.h
 *  PURPOSE:     Lua client definitions class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CLuaDefs.h"

class CLuaClientDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

private:
    static bool SetTransferBoxVisible(bool visible);
    static bool IsTransferBoxVisible();
    static bool IsTransferBoxAlwaysVisible();
};
