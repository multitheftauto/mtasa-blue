/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaCryptDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include <charconv>
#include <SharedUtil.Crypto.h>
#include <lua/CLuaFunctionParser.h>

void CLuaCryptDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"md5", ArgumentParserWarn<false, Md5>},
        {"sha256", ArgumentParserWarn<false, Sha256>},
        {"hash", ArgumentParserWarn<false, Hash>},
        {"teaEncode", ArgumentParserWarn<false, TeaEncode>},
        {"teaDecode", ArgumentParserWarn<false, TeaDecode>},
        {"base64Encode", ArgumentParserWarn<false, Base64encode>},
        {"base64Decode", ArgumentParserWarn<false, Base64decode>},
        {"passwordHash", ArgumentParserWarn<false, PasswordHash>},
        {"passwordVerify", PasswordVerify},
        {"encodeString", EncodeString},
        {"decodeString", DecodeString},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

std::string CLuaCryptDefs::Md5(std::string strMd5)
{
    MD5        md5bytes;
    char       szResult[33];
    CMD5Hasher hasher;
    hasher.Calculate(strMd5.data(), strMd5.length(), md5bytes);
    hasher.ConvertToHex(md5bytes, szResult);
    return szResult;
}

std::string CLuaCryptDefs::Sha256(std::string strSourceData)
{
    return GenerateSha256HexString(strSourceData);
}

std::string CLuaCryptDefs::Hash(EHashFunctionType hashFunction, std::string strSourceData)
{
    return GenerateHashHexString(hashFunction, strSourceData).ToLower();
}

std::string CLuaCryptDefs::TeaEncode(std::string str, std::string key)
{
    SString result;
    SharedUtil::TeaEncode(str, key, &result);
    return SharedUtil::Base64encode(result);
}

std::string CLuaCryptDefs::TeaDecode(std::string str, std::string key)
{
    SString result = SharedUtil::Base64decode(str);
    SString strOutResult;
    SharedUtil::TeaDecode(result, key, &strOutResult);
    return strOutResult;
}

std::string CLuaCryptDefs::Base64encode(std::string str)
{
    return SharedUtil::Base64encode(str);
}

std::string CLuaCryptDefs::Base64decode(std::string str)
{
    return SharedUtil::Base64decode(str);
}

std::variant<std::string, bool> CLuaCryptDefs::PasswordHash(lua_State* luaVM, std::string password, PasswordHashFunction algorithm,
                                                            std::unordered_map<std::string, std::string> options, std::optional<CLuaFunctionRef> callback)
{
    switch (algorithm)
    {
        case PasswordHashFunction::Bcrypt:
        {
            std::size_t cost = 10;

            if (auto it = options.find("cost"); it != options.end())
            {
                auto [__, err] = std::from_chars(it->second.data(), it->second.data() + it->second.length(), cost);
                if (err != std::errc{})
                    throw std::invalid_argument("Invalid value for field 'cost'");
            }

            if (auto it = options.find("salt"); it != options.end())
            {
                // Using custom salts is deprecated
                // See: https://stackoverflow.com/questions/40993645/understanding-bcrypt-salt-as-used-by-php-password-hash
                m_pScriptDebugging->LogWarning(luaVM, "Custom generated salts are deprecated and will be removed in the future.");
            }

            if (callback.has_value())
            {
                // Async
                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                if (pLuaMain)
                {
                    CLuaShared::GetAsyncTaskScheduler()->PushTask<SString>(
                        [password, salt = options["salt"], cost] {
                            // Execute time-consuming task
                            return SharedUtil::BcryptHash(password, salt, cost);
                        },
                        [luaFunctionRef = callback.value()](const SString& hash) {
                            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                            if (pLuaMain)
                            {
                                CLuaArguments arguments;

                                if (hash.empty())
                                {
                                    m_pScriptDebugging->LogCustom(pLuaMain->GetVM(), "Invalid value for field 'salt'");
                                    arguments.PushBoolean(false);
                                }
                                else
                                    arguments.PushString(hash);

                                arguments.Call(pLuaMain, luaFunctionRef);
                            }
                        });
                }
            }
            else
            {
                // Sync
                SString hash = SharedUtil::BcryptHash(password, options["salt"], cost);
                if (!hash.empty())
                {
                    return hash;
                }
                else
                    throw std::invalid_argument("Invalid value for field 'salt'");
            }
            return true;
        }

        default:
            throw std::invalid_argument("Unknown algorithm");
    }
}

int CLuaCryptDefs::PasswordVerify(lua_State* luaVM)
{
    //  bool passwordVerify(string password, string hash [, table options, function callback])
    SString         password;
    SString         hash;
    CStringMap      options;
    CLuaFunctionRef luaFunctionRef;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(password);
    argStream.ReadString(hash);

    if (argStream.NextIsTable())
    {
        argStream.ReadStringMap(options);
    }

    if (argStream.NextIsFunction())
    {
        argStream.ReadFunction(luaFunctionRef);
        argStream.ReadFunctionComplete();
    }

    if (!argStream.HasErrors())
    {
        // Is it bcrypt?
        if (hash.BeginsWith("$2y$") || hash.BeginsWith("$2a$"))
        {
            bool insecureBcrypt;
            options.ReadBool("insecureBcrypt", insecureBcrypt, false);

            if (hash.BeginsWith("$2a$") && !insecureBcrypt)
            {
                m_pScriptDebugging->LogWarning(luaVM, "Bcrypt 2a prefix is only supported if the 'insecureBcrypt' option is enabled. Consider upgrading to 2y");
                lua_pushnil(luaVM);
                return 1;
            }

            // Sync
            if (luaFunctionRef == CLuaFunctionRef())
            {
                lua_pushboolean(luaVM, SharedUtil::BcryptVerify(password, hash));
            }
            else            // Async
            {
                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                if (pLuaMain)
                {
                    CLuaShared::GetAsyncTaskScheduler()->PushTask<bool>(
                        [password, hash] {
                            // Execute time-consuming task
                            return SharedUtil::BcryptVerify(password, hash);
                        },
                        [luaFunctionRef](const bool& correct) {
                            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                            if (pLuaMain)
                            {
                                CLuaArguments arguments;
                                arguments.PushBoolean(correct);
                                arguments.Call(pLuaMain, luaFunctionRef);
                            }
                        });

                    lua_pushboolean(luaVM, true);
                }
            }
            return 1;
        }
        else
        {
            m_pScriptDebugging->LogWarning(luaVM, "Passed unknown hash");
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaCryptDefs::EncodeString(lua_State* luaVM)
{
    StringEncryptFunction algorithm;
    SString               data;
    CStringMap            options;
    CLuaFunctionRef       luaFunctionRef;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(algorithm);
    argStream.ReadString(data);
    argStream.ReadStringMap(options);

    argStream.ReadFunction(luaFunctionRef, LUA_REFNIL);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        switch (algorithm)
        {
            case StringEncryptFunction::TEA:
            {
                SString& key = options["key"];

                if (key.empty())
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'key'");
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask<SString>(
                            [data, key] {
                                // Execute time-consuming task
                                SString result;
                                SharedUtil::TeaEncode(data, key, &result);
                                return result;
                            },
                            [luaFunctionRef](const SString& result) {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    arguments.PushString(result);
                                    arguments.Call(pLuaMain, luaFunctionRef);
                                }
                            });

                        lua_pushboolean(luaVM, true);
                    }
                }
                else            // Sync
                {
                    SString result;
                    SharedUtil::TeaEncode(data, key, &result);
                    lua_pushlstring(luaVM, result, result.length());
                }
                return 1;
            }
            case StringEncryptFunction::AES128:
            {
                SString& key = options["key"];

                if (key.size() != CryptoPP::AES::DEFAULT_KEYLENGTH)
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid key length (must be 16 characters long)");
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask<std::pair<SString, SString>>(
                            [data, key] {
                                std::pair<SString, SString> result;
                                try
                                {
                                    result = SharedUtil::Aes128encode(data, key);
                                }
                                catch (const CryptoPP::Exception&)
                                {
                                }
                                return result;
                            },
                            [luaFunctionRef](const std::pair<SString, SString> result) {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    if (result.first.empty())
                                    {
                                        arguments.PushBoolean(false);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                    else
                                    {
                                        arguments.PushString(result.first);
                                        arguments.PushString(result.second);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                }
                            });

                        lua_pushboolean(luaVM, true);
                    }
                }
                else            // Sync
                {
                    std::pair<SString, SString> result;
                    try
                    {
                        result = SharedUtil::Aes128encode(data, key);
                    }
                    catch (const CryptoPP::Exception&)
                    {
                        lua_pushboolean(luaVM, false);
                        return 1;
                    }
                    lua_pushlstring(luaVM, result.first, result.first.length());
                    lua_pushlstring(luaVM, result.second, result.second.length());
                }
                return 2;
            }
            default:
            {
                m_pScriptDebugging->LogCustom(luaVM, "Unknown encryption algorithm");
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCryptDefs::DecodeString(lua_State* luaVM)
{
    StringEncryptFunction algorithm;
    SString               data;
    CStringMap            options;
    CLuaFunctionRef       luaFunctionRef;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(algorithm);
    argStream.ReadString(data);
    argStream.ReadStringMap(options);

    argStream.ReadFunction(luaFunctionRef, LUA_REFNIL);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        switch (algorithm)
        {
            case StringEncryptFunction::TEA:
            {
                SString& key = options["key"];

                if (key.empty())
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'key'");
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask<SString>(
                            [data, key] {
                                // Execute time-consuming task
                                SString result;
                                SharedUtil::TeaDecode(data, key, &result);
                                return result;
                            },
                            [luaFunctionRef](const SString& result) {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    arguments.PushString(result);
                                    arguments.Call(pLuaMain, luaFunctionRef);
                                }
                            });

                        lua_pushboolean(luaVM, true);
                    }
                }
                else            // Sync
                {
                    SString result;
                    SharedUtil::TeaDecode(data, key, &result);
                    lua_pushlstring(luaVM, result, result.length());
                }
                return 1;
            }
            case StringEncryptFunction::AES128:
            {
                SString& key = options["key"];
                SString& iv = options["iv"];

                if (key.size() != CryptoPP::AES::DEFAULT_KEYLENGTH)
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid key length (must be 16 characters long)");
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                if (iv.size() != CryptoPP::AES::BLOCKSIZE)
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid iv length (must be 16 characters long)");
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask<SString>(
                            [data, key, iv] {
                                // Execute time-consuming task
                                SString result;
                                try
                                {
                                    result = SharedUtil::Aes128decode(data, key, iv);
                                }
                                catch (const CryptoPP::Exception&)
                                {
                                }
                                return result;
                            },
                            [luaFunctionRef](const SString& result) {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    if (result.empty())
                                    {
                                        arguments.PushBoolean(false);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                    else
                                    {
                                        arguments.PushString(result);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                }
                            });

                        lua_pushboolean(luaVM, true);
                    }
                }
                else            // Sync
                {
                    SString result;
                    try
                    {
                        result = SharedUtil::Aes128decode(data, key, iv);
                    }
                    catch (const CryptoPP::Exception&)
                    {
                        lua_pushboolean(luaVM, false);
                        return 1;
                    }
                    lua_pushlstring(luaVM, result, result.length());
                }
                return 1;
            }
            default:
            {
                m_pScriptDebugging->LogCustom(luaVM, "Unknown encryption algorithm");
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
