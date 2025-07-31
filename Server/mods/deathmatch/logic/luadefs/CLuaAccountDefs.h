/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaAccountDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaAccountDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Log in/out
    LUA_DECLARE(LogIn);
    LUA_DECLARE(LogOut);

    // Account get funcs
    LUA_DECLARE(GetAccount);
    LUA_DECLARE(GetAccounts);
    LUA_DECLARE(GetAccountName);

    static std::string GetAccountType(CAccount* pAccount);

    LUA_DECLARE(GetAccountPlayer);
    LUA_DECLARE(IsGuestAccount);
    LUA_DECLARE(GetAccountData);
    LUA_DECLARE(GetAllAccountData);
    LUA_DECLARE(GetAccountsByData);
    LUA_DECLARE(GetAccountSerial);
    LUA_DECLARE(GetAccountsBySerial);
    LUA_DECLARE(GetAccountID);
    LUA_DECLARE(GetAccountByID);
    LUA_DECLARE(GetAccountIP);
    LUA_DECLARE(GetAccountsByIP);

    // Account set funcs
    LUA_DECLARE(AddAccount);
    LUA_DECLARE(RemoveAccount);
    LUA_DECLARE(SetAccountName);
    LUA_DECLARE(SetAccountPassword);
    LUA_DECLARE(SetAccountData);
    LUA_DECLARE(CopyAccountData);
    static bool SetAccountSerial(CAccount* account, std::string serial) noexcept;
};
