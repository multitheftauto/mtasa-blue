/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaVoiceDefs.h
 *  PURPOSE:     Lua Voice function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaVoiceDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    LUA_DECLARE(IsVoiceEnabled);
    LUA_DECLARE(SetPlayerVoiceBroadcastTo);
    LUA_DECLARE(setPlayerVoiceIgnoreFrom);
};