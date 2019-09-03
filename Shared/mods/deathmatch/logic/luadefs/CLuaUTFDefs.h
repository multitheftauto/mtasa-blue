/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaUTFDefs.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "luadefs/CLuaDefs.h"

class CLuaUTFDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    LUA_DECLARE(UtfLen);
    LUA_DECLARE(UtfSeek);
    LUA_DECLARE(UtfSub);
    LUA_DECLARE(UtfChar);
    LUA_DECLARE(UtfCode);
};
