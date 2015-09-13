/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaAccountDefs.h
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaAccountDefs : public CLuaDefs
{
public:
    static void     LoadFunctions ( void );
    static void     AddClass ( lua_State* luaVM );

    // Account get funcs
    LUA_DECLARE ( GetAccount );
    LUA_DECLARE ( GetAccounts );
    LUA_DECLARE ( GetAccountName );
    LUA_DECLARE ( GetAccountPlayer );
    LUA_DECLARE ( IsGuestAccount );
    LUA_DECLARE ( GetAccountData );
    LUA_DECLARE ( GetAllAccountData );
    LUA_DECLARE ( GetAccountSerial );
    LUA_DECLARE ( GetAccountsBySerial );

    // Account set funcs
    LUA_DECLARE ( AddAccount );
    LUA_DECLARE ( RemoveAccount );
    LUA_DECLARE ( SetAccountPassword );
    LUA_DECLARE ( SetAccountData );
    LUA_DECLARE ( CopyAccountData );
};