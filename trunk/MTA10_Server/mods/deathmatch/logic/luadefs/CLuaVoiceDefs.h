/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaVoiceDefs.h
*  PURPOSE:     Lua Voice function definitions class
*  DEVELOPERS:  Talidan <>
*               Philip Farquharson <philip@philipfarquharson.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAVOICEDEFS_H
#define __CLUAVOICEDEFS_H

#include "CLuaDefs.h"

class CLuaVoiceDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );
    static int      IsVoiceEnabled                      ( lua_State* luaVM );
    static int      SetPlayerVoiceBroadcastTo           ( lua_State* luaVM );
    static int      setPlayerVoiceIgnoreFrom            ( lua_State* luaVM );
};

#endif
