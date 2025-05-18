/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaCryptDefs.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaCryptDefs.h"
#include "CStringMap.h"
#include "CScriptArgReader.h"
#include <charconv>
#include <SharedUtil.Crypto.h>
#include <lua/CLuaFunctionParser.h>
#include <lua/CLuaShared.h>

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
        {"generateKeyPair", ArgumentParser<GenerateKeyPair>},
        {"passwordVerify", PasswordVerify},
        {"encodeString", EncodeString},
        {"decodeString", DecodeString}
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

std::variant<std::string, bool> CLuaCryptDefs::Hash(lua_State* const luaVM, HashFunctionType hashFunction, std::string strSourceData,
                                                    std::optional<std::unordered_map<std::string, std::string>> options)
{
    try
    {
        switch (hashFunction)
        {
            case HashFunctionType::MD5:
                return SharedUtil::Hash<CryptoPP::MD5>(strSourceData).ToLower();

            case HashFunctionType::SHA1:
                return SharedUtil::Hash<CryptoPP::SHA1>(strSourceData).ToLower();

            case HashFunctionType::SHA224:
                return SharedUtil::Hash<CryptoPP::SHA224>(strSourceData).ToLower();

            case HashFunctionType::SHA256:
                return SharedUtil::Hash<CryptoPP::SHA256>(strSourceData).ToLower();

            case HashFunctionType::SHA384:
                return SharedUtil::Hash<CryptoPP::SHA384>(strSourceData).ToLower();

            case HashFunctionType::SHA512:
                return SharedUtil::Hash<CryptoPP::SHA512>(strSourceData).ToLower();

            case HashFunctionType::HMAC:

                // check does option table exists
                if (!options.has_value())
                    throw std::invalid_argument("Invalid value for fields 'key' and 'algorithm'");

                // vars
                std::unordered_map<std::string, std::string>& optionsMap = options.value();
                std::string&                                  key = optionsMap["key"];
                std::string&                                  algorithm = optionsMap["algorithm"];
                HmacAlgorithm                                 hmacAlgorithm;

                // check does key option is empty
                if (key.empty())
                    throw std::invalid_argument("Invalid value for field 'key'");

                // check does algorithm option is empty
                if (algorithm.empty())
                    throw std::invalid_argument("Invalid value for field 'algorithm'");

                // Parse algorithm string to enum
                StringToEnum(algorithm, hmacAlgorithm);

                // process
                switch (hmacAlgorithm)
                {
                    case HmacAlgorithm::MD5:
                        return SharedUtil::Hmac<CryptoPP::MD5>(strSourceData, key).ToLower();

                    case HmacAlgorithm::SHA1:
                        return SharedUtil::Hmac<CryptoPP::SHA1>(strSourceData, key).ToLower();

                    case HmacAlgorithm::SHA224:
                        return SharedUtil::Hmac<CryptoPP::SHA224>(strSourceData, key).ToLower();

                    case HmacAlgorithm::SHA256:
                        return SharedUtil::Hmac<CryptoPP::SHA256>(strSourceData, key).ToLower();

                    case HmacAlgorithm::SHA384:
                        return SharedUtil::Hmac<CryptoPP::SHA384>(strSourceData, key).ToLower();

                    case HmacAlgorithm::SHA512:
                        return SharedUtil::Hmac<CryptoPP::SHA512>(strSourceData, key).ToLower();
                }

                throw std::invalid_argument("Invalid hmac algorithm");
        }

        throw std::invalid_argument("Unknown algorithm");
    }
    catch (std::exception& ex)
    {
        m_pScriptDebugging->LogWarning(luaVM, ex.what());
        return false;
    }
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
                    CLuaShared::GetAsyncTaskScheduler()->PushTask(
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
                    CLuaShared::GetAsyncTaskScheduler()->PushTask(
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

std::variant<bool, CLuaMultiReturn<SString, SString>> CLuaCryptDefs::GenerateKeyPair(lua_State* const luaVM, KeyPairAlgorithm algorithm,
                                                                                     std::unordered_map<std::string, std::string> options,
                                                                                     std::optional<CLuaFunctionRef>               callback)
{
    switch (algorithm)
    {
        case KeyPairAlgorithm::RSA:
        {
            uint32_t size;
            if (auto it = options.find("size"); it != options.end())
            {
                auto [__, err] = std::from_chars(it->second.data(), it->second.data() + it->second.length(), size);
                if (err != std::errc{})
                    throw std::invalid_argument("Invalid value for field 'size'");
            }

            if (callback.has_value())
            {
                // Async
                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                if (pLuaMain)
                {
                    CLuaShared::GetAsyncTaskScheduler()->PushTask(
                        [size]() -> std::variant<KeyPair, SString> {
                            // Execute time-consuming task
                            try
                            {
                                return SharedUtil::GenerateRsaKeyPair(size);
                            }
                            catch (const CryptoPP::Exception& ex)
                            {
                                return {ex.GetWhat()};
                            }
                        },
                        [luaFunctionRef = callback.value()](const auto& result) {
                            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                            if (pLuaMain)
                            {
                                CLuaArguments arguments;
                                if (std::holds_alternative<KeyPair>(result))
                                {
                                    auto keyPair = std::get<0>(result);
                                    arguments.PushString(keyPair.privateKey);
                                    arguments.PushString(keyPair.publicKey);
                                    arguments.Call(pLuaMain, luaFunctionRef);
                                }
                                else if (std::holds_alternative<SString>(result))
                                {
                                    auto exceptionCause = std::get<1>(result);
                                    m_pScriptDebugging->LogWarning(luaFunctionRef.GetLuaVM(), exceptionCause.c_str());
                                    arguments.PushBoolean(false);
                                    arguments.Call(pLuaMain, luaFunctionRef);
                                }
                            }
                        });
                    return true;
                }
                return false;
            }
            else
            {
                // Sync
                try
                {
                    auto rsaKeyPair = SharedUtil::GenerateRsaKeyPair(size);
                    return std::make_pair(rsaKeyPair.privateKey, rsaKeyPair.publicKey);
                }
                catch (const CryptoPP::Exception& ex)
                {
                    m_pScriptDebugging->LogWarning(luaVM, ex.what());
                    return false;
                }
            }
        }
        default:
            throw std::invalid_argument("Unknown algorithm");
    }
}

int CLuaCryptDefs::EncodeString(lua_State* luaVM)
{
    StringEncodeFunction algorithm;
    SString              data;
    CStringMap           options;
    CLuaFunctionRef      luaFunctionRef;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(algorithm);
    argStream.ReadString(data);

    if ((algorithm != StringEncodeFunction::BASE64 && algorithm != StringEncodeFunction::BASE32 && algorithm != StringEncodeFunction::ZLIB) ||
        argStream.NextIsTable())
    {
        argStream.ReadStringMap(options);
    }

    argStream.ReadFunction(luaFunctionRef, LUA_REFNIL);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        switch (algorithm)
        {
            case StringEncodeFunction::TEA:
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
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
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
            case StringEncodeFunction::AES128:
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
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
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
            case StringEncodeFunction::RSA:
            {
                SString& key = options["key"];

                if (key.empty())
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'key'");
                    lua::Push(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
                            [data, key] {
                                try
                                {
                                    return std::make_pair(SharedUtil::RsaEncode(data, key), true);
                                }
                                catch (const CryptoPP::Exception& ex)
                                {
                                    return std::make_pair(SString(ex.GetWhat()), false);
                                }
                            },
                            [luaFunctionRef](const std::pair<SString, bool>& result) {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    if (result.second)
                                    {
                                        arguments.PushString(result.first);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                    else
                                    {
                                        m_pScriptDebugging->LogWarning(luaFunctionRef.GetLuaVM(), result.first.c_str());
                                        arguments.PushBoolean(false);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                }
                            });

                        lua::Push(luaVM, true);
                    }
                }
                else            // Sync
                {
                    try
                    {
                        lua::Push(luaVM, SharedUtil::RsaEncode(data, key));
                    }
                    catch (const CryptoPP::Exception& ex)
                    {
                        m_pScriptDebugging->LogWarning(luaVM, ex.what());
                        lua::Push(luaVM, false);
                    }
                    return 1;
                }
                return 1;
            }
            case StringEncodeFunction::BASE64:
            {
                const SString variant = options["variant"].ToUpper();

                if (!variant.empty() && variant != "URL")
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'variant'");
                    lua::Push(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
                            [data, variant]
                            {
                                try
                                {
                                    return std::make_pair(SharedUtil::Base64encode(data, variant), true);
                                }
                                catch (const CryptoPP::Exception& ex)
                                {
                                    return std::make_pair(SString(ex.GetWhat()), false);
                                }
                            },
                            [luaFunctionRef](const std::pair<SString, bool>& result)
                            {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    if (result.second)
                                    {
                                        arguments.PushString(result.first);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                    else
                                    {
                                        m_pScriptDebugging->LogWarning(luaFunctionRef.GetLuaVM(), result.first.c_str());
                                        arguments.PushBoolean(false);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                }
                            });

                        lua::Push(luaVM, true);
                    }
                }
                else            // Sync
                {
                    try
                    {
                        lua::Push(luaVM, SharedUtil::Base64encode(data, variant));
                    }
                    catch (const CryptoPP::Exception& ex)
                    {
                        m_pScriptDebugging->LogWarning(luaVM, ex.what());
                        lua::Push(luaVM, false);
                    }
                    return 1;
                }
                return 1;
            }
            case StringEncodeFunction::BASE32:
            {
                const SString variant = options["variant"].ToUpper();

                if (!variant.empty() && variant != "HEX")
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'variant'");
                    lua::Push(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
                            [data, variant]
                            {
                                try
                                {
                                    return std::make_pair(SharedUtil::Base32encode(data, variant), true);
                                }
                                catch (const CryptoPP::Exception& ex)
                                {
                                    return std::make_pair(SString(ex.GetWhat()), false);
                                }
                            },
                            [luaFunctionRef](const std::pair<SString, bool>& result)
                            {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    if (result.second)
                                    {
                                        arguments.PushString(result.first);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                    else
                                    {
                                        m_pScriptDebugging->LogWarning(luaFunctionRef.GetLuaVM(), result.first.c_str());
                                        arguments.PushBoolean(false);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                }
                            });

                        lua::Push(luaVM, true);
                    }
                }
                else            // Sync
                {
                    try
                    {
                        lua::Push(luaVM, SharedUtil::Base32encode(data, variant));
                    }
                    catch (const CryptoPP::Exception& ex)
                    {
                        m_pScriptDebugging->LogWarning(luaVM, ex.what());
                        lua::Push(luaVM, false);
                    }
                    return 1;
                }
                return 1;
            }
            case StringEncodeFunction::ZLIB:
            {
                int compression = 9;
                int format = (int)ZLibFormat::GZIP;
                ZLibStrategy strategy = ZLibStrategy::DEFAULT;
                if (!options["format"].empty() && !StringToEnum(options["format"], (ZLibFormat&)format) && !StringToZLibFormat(options["format"], format))
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'format'");
                    lua::Push(luaVM, false);
                    return 1;
                }
                if (!options["strategy"].empty() && !StringToEnum(options["strategy"], strategy))
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'strategy'");
                    lua::Push(luaVM, false);
                    return 1;
                }
                if (!options["compression"].empty())
                {
                    compression = atoi(options["compression"].c_str());
                    if (compression < 0 || compression > 9)
                    {
                        m_pScriptDebugging->LogCustom(luaVM, "Value for field 'compression' is out of range (0-9)");
                        lua::Push(luaVM, false);
                        return 1;
                    }
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
                            [data, format, compression, strategy]
                            {
                                // Execute time-consuming task
                                SString output;
                                int     result = SharedUtil::ZLibCompress(data, output, format, compression, strategy);
                                if (result == Z_STREAM_END)
                                    return std::make_pair(output, true);
                                else
                                    return std::make_pair(SString("zlib error: %i", result), false);
                            },
                            [luaFunctionRef](const std::pair<SString, bool>& result)
                            {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    if (result.second)
                                    {
                                        arguments.PushString(result.first);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                    else
                                    {
                                        m_pScriptDebugging->LogWarning(luaFunctionRef.GetLuaVM(), result.first.c_str());
                                        arguments.PushBoolean(false);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                }
                            });

                        lua_pushboolean(luaVM, true);
                    }
                }
                else            // Sync
                {
                    SString output;
                    int     result = SharedUtil::ZLibCompress(data, output, format, compression, strategy);
                    if (result == Z_STREAM_END)
                        lua::Push(luaVM, output);
                    else
                    {
                        m_pScriptDebugging->LogWarning(luaVM, "zlib error: %i", result);
                        lua::Push(luaVM, false);
                    }
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

int CLuaCryptDefs::DecodeString(lua_State* luaVM)
{
    StringEncodeFunction algorithm;
    SString              data;
    CStringMap           options;
    CLuaFunctionRef      luaFunctionRef;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(algorithm);
    argStream.ReadString(data);

    if ((algorithm != StringEncodeFunction::BASE64 && algorithm != StringEncodeFunction::BASE32 && algorithm != StringEncodeFunction::ZLIB) ||
        argStream.NextIsTable())
    {
        argStream.ReadStringMap(options);
    }

    argStream.ReadFunction(luaFunctionRef, LUA_REFNIL);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        switch (algorithm)
        {
            case StringEncodeFunction::TEA:
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
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
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
            case StringEncodeFunction::AES128:
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
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
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
            case StringEncodeFunction::RSA:
            {
                SString& key = options["key"];

                if (key.empty())
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'key'");
                    lua::Push(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
                            [data, key] {
                                try
                                {
                                    return std::make_pair(SharedUtil::RsaDecode(data, key), true);
                                }
                                catch (const CryptoPP::Exception& ex)
                                {
                                    return std::make_pair(SString(ex.GetWhat()), false);
                                }
                            },
                            [luaFunctionRef](const std::pair<SString, bool>& result) {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    if (result.second)
                                    {
                                        arguments.PushString(result.first);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                    else
                                    {
                                        m_pScriptDebugging->LogWarning(luaFunctionRef.GetLuaVM(), result.first.c_str());
                                        arguments.PushBoolean(false);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                }
                            });

                        lua::Push(luaVM, true);
                    }
                }
                else            // Sync
                {
                    try
                    {
                        lua::Push(luaVM, SharedUtil::RsaDecode(data, key));
                    }
                    catch (const CryptoPP::Exception& ex)
                    {
                        m_pScriptDebugging->LogWarning(luaVM, ex.what());
                        lua::Push(luaVM, false);
                    }
                    return 1;
                }
                return 1;
            }
            case StringEncodeFunction::BASE64:
            {
                const SString variant = options["variant"].ToUpper();

                if (!variant.empty() && variant != "URL")
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'variant'");
                    lua::Push(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
                            [data, variant]
                            {
                                try
                                {
                                    return std::make_pair(SharedUtil::Base64decode(data, variant), true);
                                }
                                catch (const CryptoPP::Exception& ex)
                                {
                                    return std::make_pair(SString(ex.GetWhat()), false);
                                }
                            },
                            [luaFunctionRef](const std::pair<SString, bool>& result)
                            {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    if (result.second)
                                    {
                                        arguments.PushString(result.first);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                    else
                                    {
                                        m_pScriptDebugging->LogWarning(luaFunctionRef.GetLuaVM(), result.first.c_str());
                                        arguments.PushBoolean(false);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                }
                            });

                        lua::Push(luaVM, true);
                    }
                }
                else            // Sync
                {
                    try
                    {
                        lua::Push(luaVM, SharedUtil::Base64decode(data, variant));
                    }
                    catch (const CryptoPP::Exception& ex)
                    {
                        m_pScriptDebugging->LogWarning(luaVM, ex.what());
                        lua::Push(luaVM, false);
                    }
                    return 1;
                }
                return 1;
            }
            case StringEncodeFunction::BASE32:
            {
                const SString variant = options["variant"].ToUpper();

                if (!variant.empty() && variant != "HEX")
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'variant'");
                    lua::Push(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
                            [data, variant]
                            {
                                try
                                {
                                    return std::make_pair(SharedUtil::Base32decode(data, variant), true);
                                }
                                catch (const CryptoPP::Exception& ex)
                                {
                                    return std::make_pair(SString(ex.GetWhat()), false);
                                }
                            },
                            [luaFunctionRef](const std::pair<SString, bool>& result)
                            {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    if (result.second)
                                    {
                                        arguments.PushString(result.first);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                    else
                                    {
                                        m_pScriptDebugging->LogWarning(luaFunctionRef.GetLuaVM(), result.first.c_str());
                                        arguments.PushBoolean(false);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                }
                            });

                        lua::Push(luaVM, true);
                    }
                }
                else            // Sync
                {
                    try
                    {
                        lua::Push(luaVM, SharedUtil::Base32decode(data, variant));
                    }
                    catch (const CryptoPP::Exception& ex)
                    {
                        m_pScriptDebugging->LogWarning(luaVM, ex.what());
                        lua::Push(luaVM, false);
                    }
                    return 1;
                }
                return 1;
            }
            case StringEncodeFunction::ZLIB:
            {
                int format = 0;
                if (!options["format"].empty() && !StringToEnum(options["format"], (ZLibFormat&)format) && !StringToZLibFormat(options["format"], format))
                {
                    m_pScriptDebugging->LogCustom(luaVM, "Not supported value for field 'format'");
                    lua::Push(luaVM, false);
                    return 1;
                }

                // Async
                if (VERIFY_FUNCTION(luaFunctionRef))
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask(
                            [data, format]
                            {
                                // Execute time-consuming task
                                SString output;
                                int     result = SharedUtil::ZLibUncompress(data, output, format);
                                if (result == Z_STREAM_END)
                                    return std::make_pair(output, true);
                                else
                                    return std::make_pair(SString("zlib error: %i", result), false);
                            },
                            [luaFunctionRef](const std::pair<SString, bool>& result)
                            {
                                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaFunctionRef.GetLuaVM());
                                if (pLuaMain)
                                {
                                    CLuaArguments arguments;
                                    if (result.second)
                                    {
                                        arguments.PushString(result.first);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                    else
                                    {
                                        m_pScriptDebugging->LogWarning(luaFunctionRef.GetLuaVM(), result.first.c_str());
                                        arguments.PushBoolean(false);
                                        arguments.Call(pLuaMain, luaFunctionRef);
                                    }
                                }
                            });

                        lua_pushboolean(luaVM, true);
                    }
                }
                else            // Sync
                {
                    SString output;
                    int     result = SharedUtil::ZLibUncompress(data, output, format);
                    if (result == Z_STREAM_END)
                        lua::Push(luaVM, output);
                    else
                    {
                        m_pScriptDebugging->LogWarning(luaVM, "zlib error: %i", result);
                        lua::Push(luaVM, false);
                    }
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
