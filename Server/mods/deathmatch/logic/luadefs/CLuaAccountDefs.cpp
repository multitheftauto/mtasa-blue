/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaAccountDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaAccountDefs.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"

void CLuaAccountDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Log in/out funcs
        {"logIn", LogIn},
        {"logOut", LogOut},

        // Account get functions
        {"getAccountName", GetAccountName},
        {"getAccountType", ArgumentParser<GetAccountType>},
        {"getAccountPlayer", GetAccountPlayer},
        {"isGuestAccount", IsGuestAccount},
        {"getAccountData", GetAccountData},
        {"getAllAccountData", GetAllAccountData},
        {"getAccount", GetAccount},
        {"getAccounts", GetAccounts},
        {"getAccountsByData", GetAccountsByData},
        {"getAccountSerial", GetAccountSerial},
        {"getAccountsBySerial", GetAccountsBySerial},
        {"getAccountID", GetAccountID},
        {"getAccountByID", GetAccountByID},
        {"getAccountIP", GetAccountIP},
        {"getAccountsByIP", GetAccountsByIP},

        // Account set functions
        {"addAccount", AddAccount},
        {"removeAccount", RemoveAccount},
        {"setAccountPassword", SetAccountPassword},
        {"setAccountData", SetAccountData},
        {"setAccountName", SetAccountName},
        {"copyAccountData", CopyAccountData},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaAccountDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getAll", "getAccounts");
    lua_classfunction(luaVM, "getAllByData", "getAccountsByData");
    lua_classfunction(luaVM, "getAllBySerial", "getAccountsBySerial");
    lua_classfunction(luaVM, "getAllByIP", "getAccountsByIP");
    lua_classfunction(luaVM, "getFromPlayer", "getPlayerAccount");
    lua_classfunction(luaVM, "logPlayerOut", "logOut");

    lua_classfunction(luaVM, "create", "getAccount");
    lua_classfunction(luaVM, "add", "addAccount");
    lua_classfunction(luaVM, "copyDataFrom", "copyAccountData");
    lua_classfunction(luaVM, "remove", "removeAccount");

    lua_classfunction(luaVM, "setData", "setAccountData");
    lua_classfunction(luaVM, "setPassword", "setAccountPassword");
    lua_classfunction(luaVM, "setName", "setAccountName");

    lua_classfunction(luaVM, "getSerial", "getAccountSerial");
    lua_classfunction(luaVM, "getIP", "getAccountIP");
    lua_classfunction(luaVM, "getID", "getAccountID");
    lua_classfunction(luaVM, "getData", "getAccountData");
    lua_classfunction(luaVM, "getAllData", "getAllAccountData");
    lua_classfunction(luaVM, "getName", "getAccountName");
    lua_classfunction(luaVM, "getType", "getAccountType");
    lua_classfunction(luaVM, "getPlayer", "getAccountPlayer");
    lua_classfunction(luaVM, "isGuest", "isGuestAccount");

    lua_classvariable(luaVM, "serial", NULL, "getAccountSerial");
    lua_classvariable(luaVM, "name", "setAccountName", "getAccountName");
    lua_classvariable(luaVM, "id", NULL, "getAccountID");
    lua_classvariable(luaVM, "ip", NULL, "getAccountIP");
    lua_classvariable(luaVM, "player", NULL, "getAccountPlayer");
    lua_classvariable(luaVM, "guest", NULL, "isGuestAccount");
    lua_classvariable(luaVM, "password", "setAccountPassword", NULL);
    lua_classvariable(luaVM, "data", NULL, "getAllAccountData");

    lua_registerclass(luaVM, "Account");
}

int CLuaAccountDefs::GetAccountName(lua_State* luaVM)
{
    //  string getAccountName ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);

    if (!argStream.HasErrors())
    {
        std::string strName = pAccount->GetName();
        if (!strName.empty())
        {
            lua_pushstring(luaVM, strName.c_str());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

std::optional<std::string> CLuaAccountDefs::GetAccountType(CAccount* pAccount)
{
    switch (pAccount->GetType())
    {
        case EAccountType::Guest:
            return "guest";
        case EAccountType::Console:
            return "console";
        case EAccountType::Player:
            return "player";
    }
    return {};
}

int CLuaAccountDefs::GetAccountPlayer(lua_State* luaVM)
{
    //  player getAccountPlayer ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);

    if (!argStream.HasErrors())
    {
        CClient* pClient = CStaticFunctionDefinitions::GetAccountPlayer(pAccount);
        if (pClient)
        {
            lua_pushelement(luaVM, pClient->GetElement());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::IsGuestAccount(lua_State* luaVM)
{
    //  bool isGuestAccount ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);

    if (!argStream.HasErrors())
    {
        bool bGuest;
        if (CStaticFunctionDefinitions::IsGuestAccount(pAccount, bGuest))
        {
            lua_pushboolean(luaVM, bGuest);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::GetAccountData(lua_State* luaVM)
{
    //  string getAccountData ( account theAccount, string key )
    CAccount* pAccount;
    SString   strKey;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        auto pArgument = CStaticFunctionDefinitions::GetAccountData(pAccount, strKey);
        if (pArgument)
        {
            pArgument->Push(luaVM);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::GetAllAccountData(lua_State* luaVM)
{
    //  string getAllAccountData ( account theAccount )
    CAccount*        pAccount;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);
    if (!argStream.HasErrors())
    {
        lua_newtable(luaVM);
        CStaticFunctionDefinitions::GetAllAccountData(luaVM, pAccount);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::GetAccount(lua_State* luaVM)
{
    //  account getAccount ( string username, [ string password ] )
    SString strName;
    SString strPassword;
    bool    bUsePassword = false;
    bool    bCaseSensitive;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);

    if (!argStream.NextIsNil() && !argStream.NextIsNone())
    {
        argStream.ReadString(strPassword);
        bUsePassword = true;
    }
    else
        argStream.Skip(1);

    argStream.ReadBool(bCaseSensitive, true);

    if (!argStream.HasErrors())
    {
        CAccount* pAccount = CStaticFunctionDefinitions::GetAccount(strName, bUsePassword ? strPassword.c_str() : NULL, bCaseSensitive);
        if (pAccount)
        {
            lua_pushaccount(luaVM, pAccount);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::GetAccounts(lua_State* luaVM)
{
    lua_newtable(luaVM);
    CStaticFunctionDefinitions::GetAccounts(luaVM);
    return 1;
}

int CLuaAccountDefs::GetAccountsByData(lua_State* luaVM)
{
    //  table GetAccountsByData ( string dataName, string value )
    SString dataName;
    SString value;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(dataName);
    argStream.ReadString(value);

    if (!argStream.HasErrors())
    {
        lua_newtable(luaVM);
        std::vector<CAccount*> accounts;

        CStaticFunctionDefinitions::GetAccountsByData(dataName, value, accounts);
        for (unsigned int i = 0; i < accounts.size(); ++i)
        {
            lua_pushnumber(luaVM, i + 1);
            lua_pushaccount(luaVM, accounts[i]);
            lua_settable(luaVM, -3);
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::GetAccountSerial(lua_State* luaVM)
{
    //  string getAccountSerial ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);

    if (!argStream.HasErrors())
    {
        SString strSerial;
        if (CStaticFunctionDefinitions::GetAccountSerial(pAccount, strSerial))
        {
            lua_pushstring(luaVM, strSerial);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::GetAccountsBySerial(lua_State* luaVM)
{
    //  table getAccountsBySerial ( string serial )
    SString strSerial;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strSerial);

    if (!argStream.HasErrors())
    {
        lua_newtable(luaVM);
        std::vector<CAccount*> accounts;

        CStaticFunctionDefinitions::GetAccountsBySerial(strSerial, accounts);
        for (unsigned int i = 0; i < accounts.size(); ++i)
        {
            lua_pushnumber(luaVM, i + 1);
            lua_pushaccount(luaVM, accounts[i]);
            lua_settable(luaVM, -3);
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::GetAccountIP(lua_State* luaVM)
{
    //  string getAccountSerial ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);

    if (!argStream.HasErrors())
    {
        SString strIP;
        if (CStaticFunctionDefinitions::GetAccountIP(pAccount, strIP))
        {
            lua_pushstring(luaVM, strIP);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::GetAccountsByIP(lua_State* luaVM)
{
    // table getAccountsByIP ( string ip )
    SString strIP;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strIP);

    if (!argStream.HasErrors())
    {
        lua_newtable(luaVM);
        std::vector<CAccount*> accounts;

        CStaticFunctionDefinitions::GetAccountsByIP(strIP, accounts);
        for (unsigned int i = 0; i < accounts.size(); ++i)
        {
            lua_pushnumber(luaVM, i + 1);
            lua_pushaccount(luaVM, accounts[i]);
            lua_settable(luaVM, -3);
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::GetAccountID(lua_State* luaVM)
{
    // int getAccountID ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);

    if (!argStream.HasErrors())
    {
        int ID;
        if (CStaticFunctionDefinitions::GetAccountID(pAccount, ID))
        {
            lua_pushnumber(luaVM, ID);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::GetAccountByID(lua_State* luaVM)
{
    // account getAccountByID ( int ID )
    int ID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ID);

    if (!argStream.HasErrors())
    {
        CAccount* account;
        if (CStaticFunctionDefinitions::GetAccountByID(ID, account) && account)
        {
            lua_pushaccount(luaVM, account);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::AddAccount(lua_State* luaVM)
{
    //  account addAccount ( string name, string pass[, bool allowCaseVariations = false ] )
    SString strName;
    SString strPassword;
    bool    bAllowCaseVariations;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);
    argStream.ReadString(strPassword);
    argStream.ReadBool(bAllowCaseVariations, false);

    SString strError;
    if (!argStream.HasErrors())
    {
        CAccount* pAccount;
        if ((pAccount = CStaticFunctionDefinitions::AddAccount(strName, strPassword, bAllowCaseVariations, strError)))
        {
            lua_pushaccount(luaVM, pAccount);
            return 1;
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    lua_pushstring(luaVM, strError);
    return 2;
}

int CLuaAccountDefs::RemoveAccount(lua_State* luaVM)
{
    //  bool removeAccount ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::RemoveAccount(pAccount))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }

        CClient* pClient = pAccount->GetClient();
        if (pClient)
            argStream.SetCustomError("Unable to remove account as unable to log out client. (Maybe onPlayerLogout is cancelled)");
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::SetAccountName(lua_State* luaVM)
{
    // bool setAccountPassword ( account theAccount, string name[, bool allowCaseVariations = false ] )
    CAccount* pAccount;
    SString   strNewName;
    bool      bAllowCaseVariations;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);
    argStream.ReadString(strNewName);
    argStream.ReadBool(bAllowCaseVariations, false);

    SString strError;
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetAccountName(pAccount, strNewName, bAllowCaseVariations, strError))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::SetAccountPassword(lua_State* luaVM)
{
    //  bool setAccountPassword ( account theAccount, string password )
    CAccount*                              pAccount;
    SString                                strPassword;
    CAccountPassword::EAccountPasswordType ePasswordType;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);
    argStream.ReadString(strPassword);
    argStream.ReadEnumString(ePasswordType, CAccountPassword::PLAINTEXT);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetAccountPassword(pAccount, strPassword, ePasswordType))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::SetAccountData(lua_State* luaVM)
{
    //  bool setAccountData ( account theAccount, string key, string value )
    CAccount*    pAccount;
    SString      strKey;
    CLuaArgument Variable;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);
    argStream.ReadString(strKey);
    argStream.ReadLuaArgument(Variable);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetAccountData(pAccount, strKey, &Variable))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::CopyAccountData(lua_State* luaVM)
{
    //  bool copyAccountData ( account theAccount, account fromAccount )
    CAccount* pAccount;
    CAccount* pFromAccount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pAccount);
    argStream.ReadUserData(pFromAccount);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::CopyAccountData(pAccount, pFromAccount))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::LogIn(lua_State* luaVM)
{
    //  bool logIn ( player thePlayer, account theAccount, string thePassword )
    CPlayer*  pPlayer;
    CAccount* pAccount;
    SString   strPassword;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);
    argStream.ReadUserData(pAccount);
    argStream.ReadString(strPassword);

    if (!argStream.HasErrors())
    {
        // Log him in
        if (CStaticFunctionDefinitions::LogIn(pPlayer, pAccount, strPassword))
        {
            // Success
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaAccountDefs::LogOut(lua_State* luaVM)
{
    //  bool logOut ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        // Log out
        if (CStaticFunctionDefinitions::LogOut(pPlayer))
        {
            // Success
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}
