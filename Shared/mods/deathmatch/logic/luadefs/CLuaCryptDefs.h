/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaCryptDefs.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "luadefs/CLuaDefs.h"
#include <optional>
#include <variant>

class CLuaCryptDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    static std::string Md5(std::string strMd5);

    static std::variant<std::string, bool> Hash(lua_State* const luaVM, HashFunctionType hashFunction, std::string strSourceData,
                                                std::optional<std::unordered_map<std::string, std::string>> options);

    static std::string                                           TeaEncode(std::string str, std::string key);
    static std::string                                           TeaDecode(std::string str, std::string key);
    static std::string                                           Base64encode(std::string str);
    static std::string                                           Base64decode(std::string str);
    static std::variant<bool, CLuaMultiReturn<SString, SString>> GenerateKeyPair(lua_State* const luaVM, KeyPairAlgorithm algorithm,
                                                                                 std::unordered_map<std::string, std::string> options,
                                                                                 std::optional<CLuaFunctionRef>               callback);
    static std::variant<std::string, bool>                       PasswordHash(lua_State* luaVM, std::string password, PasswordHashFunction algorithm,
                                                                              std::unordered_map<std::string, std::string> options, std::optional<CLuaFunctionRef> callback);
    static std::string                                           Sha256(std::string strSourceData);
    LUA_DECLARE(PasswordVerify);
    LUA_DECLARE(EncodeString);
    LUA_DECLARE(DecodeString);
};
