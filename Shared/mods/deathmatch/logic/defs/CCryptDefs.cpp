/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/defs/CCryptDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include <SharedUtil.Crypto.h>
#include <lua/CLuaFunctionParser.h>

#ifndef MTA_CLIENT
    #include <core/CServerInterface.h>
    extern CServerInterface* g_pServerInterface;
#endif

void CCryptDefs::LoadLuaFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"md5", ArgumentParserWarn<false, Md5>},
        {"sha256", ArgumentParserWarn<false, Sha256>},
        {"hash", ArgumentParserWarn<false, Hash>},
        {"teaEncode", ArgumentParserWarn<false, TeaEncode>},
        {"teaDecode", ArgumentParserWarn<false, TeaDecode>},
        {"base64Encode", ArgumentParserWarn<false, Base64encode>},
        {"base64Decode", ArgumentParserWarn<false, Base64decode>},
        {"passwordHash", PasswordHash},
        {"passwordVerify", PasswordVerify},
        {"encodeString", EncodeString},
        {"decodeString", DecodeString},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CCryptDefs::LoadJsFunctions()
{
#ifndef MTA_CLIENT
    CV8ModuleBase* pCryptModule = g_pServerInterface->GetV8()->CreateModule("crypt");

    constexpr static const std::pair<const char*, void(*)(CV8FunctionCallbackBase*)> functions[]{
        {"md5", JsArgumentParser<Md5>},
        {"sha256", JsArgumentParser<Sha256>},
        {"teaEncode", JsArgumentParser<TeaEncode>},
        {"base64encode", JsArgumentParser<Base64encode>},
        {"base64decode", JsArgumentParser<Base64decode>},
        {"sleep", JsArgumentParser<Sleep_>},
    };

    for (const auto& [name, func] : functions)
        pCryptModule->AddFunction(name, func);

    CV8ExportObjectBase* bCrypt = g_pServerInterface->GetV8()->CreateExportObject();
    bCrypt->AddFunction("md5", JsArgumentParser<Md5>);
    pCryptModule->AddObject("bCrypt", bCrypt);
#endif
}

//#ifndef MTA_CLIENT

//
//void CJsCryptDefs::BCrypt(CV8FunctionCallbackBase* callback)
//{
//    std::string password;
//    double      value;
//    if (callback->ReadString(password) && callback->ReadNumber(value))
//    {
//        callback->ReturnPromise(std::make_unique<CV8PasswordHash>(password, (int)value));
//        return;
//    }
//    callback->Return(false);
//}
//
//#endif

std::string CCryptDefs::Md5(std::string strMd5)
{
    MD5        md5bytes;
    char       szResult[33];
    CMD5Hasher hasher;
    hasher.Calculate(strMd5.data(), strMd5.length(), md5bytes);
    hasher.ConvertToHex(md5bytes, szResult);
    return szResult;
}

std::string CCryptDefs::Sha256(std::string strSourceData)
{
    return GenerateSha256HexString(strSourceData);
}

std::string CCryptDefs::Hash(EHashFunctionType hashFunction, std::string strSourceData)
{
    return GenerateHashHexString(hashFunction, strSourceData).ToLower();
}

std::string CCryptDefs::TeaEncode(std::string str, std::string key)
{
    SString result;
    SharedUtil::TeaEncode(str, key, &result);
    return SharedUtil::Base64encode(result);
}

std::string CCryptDefs::TeaDecode(std::string str, std::string key)
{
    SString result = SharedUtil::Base64decode(str);
    SString strOutResult;
    SharedUtil::TeaDecode(result, key, &strOutResult);
    return strOutResult;
}

std::string CCryptDefs::Base64encode(std::string str)
{
    return SharedUtil::Base64encode(str);
}

Promise CCryptDefs::Sleep_(int time)
{
    return [time](CV8AsyncContextBase* asyncContext) {
        Sleep(time);
        asyncContext->Resolve("");
    };
}

std::string CCryptDefs::Base64decode(std::string str)
{
    return SharedUtil::Base64decode(str);
}

int CCryptDefs::PasswordHash(lua_State* luaVM)
{
    //  string password_hash(string password, string algorithm, table options = {} [, function callback])
    SString              password;
    PasswordHashFunction algorithm;
    CStringMap           options;
    CLuaFunctionRef      luaFunctionRef;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(password);
    argStream.ReadEnumString(algorithm);
    argStream.ReadStringMap(options);

    if (argStream.NextIsFunction())
    {
        argStream.ReadFunction(luaFunctionRef);
        argStream.ReadFunctionComplete();
    }

    if (!argStream.HasErrors())
    {
        if (algorithm == PasswordHashFunction::Bcrypt)
        {
            // Set default value to 10
            if (options["cost"].empty())
                options["cost"] = "10";

            std::stringstream ss(options["cost"]);
            std::size_t       cost;
            ss >> cost;

            if (!ss.fail())
            {
                // Using custom salts are deprecated (Luxy.c)
                // See: https://stackoverflow.com/questions/40993645/understanding-bcrypt-salt-as-used-by-php-password-hash
                if (options["salt"].length() > 0)
                {
                    m_pScriptDebugging->LogWarning(luaVM, "Custom generated salts are deprecated and will be removed in the future.");
                }

                // Sync
                if (luaFunctionRef == CLuaFunctionRef{})
                {
                    SString hash = SharedUtil::BcryptHash(password, options["salt"], cost);
                    if (!hash.empty())
                    {
                        lua_pushstring(luaVM, hash);
                        return 1;
                    }
                    else
                        m_pScriptDebugging->LogCustom(luaVM, "Invalid value for field 'salt'");
                }
                else            // Async
                {
                    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
                    if (pLuaMain)
                    {
                        CLuaShared::GetAsyncTaskScheduler()->PushTask<SString>(
                            [password, salt = options["salt"], cost] {
                                // Execute time-consuming task
                                return SharedUtil::BcryptHash(password, salt, cost);
                            },
                            [luaFunctionRef](const SString& hash) {
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

                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                }
            }
            else
                m_pScriptDebugging->LogWarning(luaVM, "Invalid value for field 'cost'");
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}


int CCryptDefs::PasswordVerify(lua_State* luaVM)
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

int CCryptDefs::EncodeString(lua_State* luaVM)
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

int CCryptDefs::DecodeString(lua_State* luaVM)
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
