/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaBanDefs.h
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaBanDefs : public CLuaDefs
{
public:
    static void     LoadFunctions ( void );
    static void     AddClass ( lua_State* luaVM );

    LUA_DECLARE ( AddBan );
    LUA_DECLARE ( RemoveBan );

    LUA_DECLARE ( GetBans );
    LUA_DECLARE ( ReloadBanList );

    LUA_DECLARE ( GetBanIP );
    LUA_DECLARE ( GetBanSerial );
    LUA_DECLARE ( GetBanUsername );
    LUA_DECLARE ( GetBanNick );
    LUA_DECLARE ( GetBanTime );
    LUA_DECLARE ( GetUnbanTime );
    LUA_DECLARE ( GetBanReason );
    LUA_DECLARE ( GetBanAdmin );

    LUA_DECLARE ( SetUnbanTime );
    LUA_DECLARE ( SetBanReason );
    LUA_DECLARE ( SetBanAdmin );
    LUA_DECLARE ( SetBanNick );
    LUA_DECLARE ( IsBan );
};