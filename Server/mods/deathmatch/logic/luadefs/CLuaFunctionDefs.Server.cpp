/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Server.cpp
 *  PURPOSE:     Lua special server function definitions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaFunctionDefs.h"
#include "CScriptArgReader.h"
#include "CSettings.h"
#include "ASE.h"
#include "CStaticFunctionDefinitions.h"
#include "CPerfStatManager.h"
#include "CMapManager.h"

#define MIN_SERVER_REQ_CALLREMOTE_QUEUE_NAME                "1.5.3-9.11270"
#define MIN_SERVER_REQ_CALLREMOTE_CONNECTION_ATTEMPTS       "1.3.0-9.04563"
#define MIN_SERVER_REQ_CALLREMOTE_CONNECT_TIMEOUT           "1.3.5"
#define MIN_SERVER_REQ_CALLREMOTE_OPTIONS_TABLE             "1.5.4-9.11342"
#define MIN_SERVER_REQ_CALLREMOTE_OPTIONS_FORMFIELDS        "1.5.4-9.11413"

int CLuaFunctionDefs::AddCommandHandler(lua_State* luaVM)
{
    //  bool addCommandHandler ( string commandName, function handlerFunction, [bool restricted = false, bool caseSensitive = true] )
    SString         strKey;
    CLuaFunctionRef iLuaFunction;
    bool            bRestricted;
    bool            bCaseSensitive;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);
    argStream.ReadFunction(iLuaFunction);
    argStream.ReadBool(bRestricted, false);
    argStream.ReadBool(bCaseSensitive, true);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Add them to our list over command handlers
            if (m_pRegisteredCommands->AddCommand(pLuaMain, strKey, iLuaFunction, bRestricted, bCaseSensitive))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::RemoveCommandHandler(lua_State* luaVM)
{
    //  bool removeCommandHandler ( string commandName [, function handler] )
    SString         strKey;
    CLuaFunctionRef iLuaFunction;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);
    argStream.ReadFunction(iLuaFunction, LUA_REFNIL);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Remove it from our list
            if (m_pRegisteredCommands->RemoveCommand(pLuaMain, strKey, iLuaFunction))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::ExecuteCommandHandler(lua_State* luaVM)
{
    //  bool executeCommandHandler ( string commandName, player thePlayer, [ string args ] )
    SString   strKey;
    CElement* pElement;
    SString   strArgs;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strArgs, "");

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CClient* pClient = NULL;
            if (pElement->GetType() == CElement::PLAYER)
                pClient = static_cast<CClient*>(static_cast<CPlayer*>(pElement));

            if (pClient)
            {
                // Call it
                if (m_pRegisteredCommands->ProcessCommand(strKey, strArgs, pClient))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetCommandHandlers(lua_State* luaVM)
{
    // table getCommandHandlers ( [ resource sourceResource ] );
    CResource* pResource = nullptr;

    CScriptArgReader argStream(luaVM);

    if (!argStream.NextIsNil() && !argStream.NextIsNone())
        argStream.ReadUserData(pResource);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    if (pResource)
    {
        CLuaMain* pLuaMain = pResource->GetVirtualMachine();

        if (pLuaMain)
            m_pRegisteredCommands->GetCommands(luaVM, pLuaMain);
        else
            lua_newtable(luaVM);
    }
    else
    {
        m_pRegisteredCommands->GetCommands(luaVM);
    }

    return 1;
}

int CLuaFunctionDefs::GetServerPort(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pGame->GetConfig()->GetServerPort());
    return 1;
}

int CLuaFunctionDefs::Set(lua_State* luaVM)
{
    SString       strSetting;
    CLuaArguments Args;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strSetting);
    argStream.ReadLuaArguments(Args);

    if (!argStream.HasErrors())
    {
        CResource* pResource = m_pLuaManager->GetVirtualMachineResource(luaVM);
        if (pResource)
        {
            std::string strResourceName = pResource->GetName();
            std::string strJSON;
            Args.WriteToJSONString(strJSON);

            if (g_pGame->GetSettings()->Set(strResourceName.c_str(), strSetting.c_str(), strJSON.c_str()))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

/* #define PUSH_SETTING(x,buf) \
pAttributes = &(x->GetAttributes ()); \
Args.PushString ( pAttributes->Find ( "name" )->GetValue ().c_str () ); \
buf = const_cast < char* > ( pAttributes->Find ( "value" )->GetValue ().c_str () ); \
if ( !Args.ReadFromJSONString ( buf ) ) { \
Args.PushString ( buf ); \
}
*/

int CLuaFunctionDefs::Get(lua_State* luaVM)
{
    SString       strSetting;
    CLuaArguments Args;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strSetting);

    if (!argStream.HasErrors())
    {
        CResource* pResource = m_pLuaManager->GetVirtualMachineResource(luaVM);
        if (pResource)
        {
            unsigned int uiIndex = 0;
            bool         bDeleteNode;

            // Extract attribute name if setting to be gotten has three parts i.e. resname.settingname.attributename
            SString         strAttribute = "value";
            vector<SString> Result;
            strSetting.Split(".", Result);
            if (Result.size() == 3 && Result[2].length())
            {
                strAttribute = Result[2];
            }

            // Get the setting
            CXMLNode *pSubNode, *pNode = g_pGame->GetSettings()->Get(pResource->GetName().c_str(), strSetting.c_str(), bDeleteNode);

            // Only proceed if we have a valid node
            if (pNode)
            {
                // Argument count
                unsigned int uiArgCount = 1;

                // See if we need to return a table with single or multiple entries
                if (pNode->GetSubNodeCount() == 0)
                {
                    // See if required attribute exists
                    CXMLAttribute* pAttribute = pNode->GetAttributes().Find(strAttribute.c_str());
                    if (!pAttribute)
                    {
                        if (bDeleteNode)
                            delete pNode;
                        lua_pushboolean(luaVM, false);
                        return 1;
                    }
                    // We only have a single entry for a specific setting, so output a string
                    const std::string& strDataValue = pAttribute->GetValue();
                    if (!Args.ReadFromJSONString(strDataValue.c_str()))
                    {
                        // No valid JSON? Parse as plain text
                        Args.PushString(strDataValue);
                    }

                    Args.PushArguments(luaVM);
                    uiArgCount = static_cast<unsigned int>(Args.Count());

                    /* Don't output a table because although it is more consistent with the multiple values output below,
                    ** due to lua's implementation of associative arrays (assuming we use the "setting-name", "value" key-value pairs)
                    ** it would require the scripter to walk through an array that only has a single entry which is a Bad Thing, performance wise.
                    **
                    PUSH_SETTING ( pNode );
                    Args.PushAsTable ( luaVM );
                    **/
                }
                else
                {
                    // We need to return multiply entries, so push all subnodes
                    while ((pSubNode = pNode->FindSubNode("setting", uiIndex++)))
                    {
                        CXMLAttributes& attributes = pSubNode->GetAttributes();
                        Args.PushString(attributes.Find("name")->GetValue());
                        const std::string& strDataValue = attributes.Find("value")->GetValue();
                        if (!Args.ReadFromJSONString(strDataValue.c_str()))
                        {
                            Args.PushString(strDataValue);
                        }
                    }
                    // Push a table and return
                    Args.PushAsTable(luaVM);
                }

                // Check if we have to delete the node
                if (bDeleteNode)
                    delete pNode;

                return uiArgCount;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::SetServerConfigSetting(lua_State* luaVM)
{
    //  bool setServerConfigSetting ( string name, string value [, bool save = false ] )
    SString strName;
    SString strValue;
    bool    bSave;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);
    argStream.ReadString(strValue);
    argStream.ReadBool(bSave, false);

    if (!argStream.HasErrors())
    {
        if (g_pGame->GetConfig()->SetSetting(strName, strValue, bSave))
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

bool CLuaFunctionDefs::Shutdown(lua_State* luaVM, std::optional<std::string_view> maybeReason, std::optional<int> maybeExitCode)
{
    std::string_view reason = maybeReason.value_or("No reason specified");
    CResource&       resource = lua_getownerresource(luaVM);

    if (reason.empty() || reason.find_first_not_of("\x09\x0A\x0B\x0C\x0D\x20") == std::string_view::npos)
        reason = "No reason specified";

    CLogger::LogPrintf("Server shutdown as requested by resource %s (%.*s)\n", resource.GetName().c_str(), reason.size(), reason.data());

    if (maybeExitCode.has_value())
        g_pServerInterface->GetModManager()->SetExitCode(maybeExitCode.value());

    // Call event
    CLuaArguments arguments;
    arguments.PushResource(&resource);
    arguments.PushString(reason.data());
    g_pGame->GetMapManager()->GetRootElement()->CallEvent("onShutdown", arguments);

    g_pGame->SetIsFinished(true);
    return true;
}

int CLuaFunctionDefs::GetMapName(lua_State* luaVM)
{
    // Grab ASE
    ASE* pASE = ASE::GetInstance();
    if (pASE)
    {
        // Grab the mapname string
        const char* szMapName = pASE->GetMapName();
        if (szMapName[0] != 0)
        {
            // Return the gametype string excluding our prefix
            lua_pushstring(luaVM, szMapName);
            return 1;
        }
    }

    // No game type
    lua_pushnil(luaVM);
    return 1;
}

int CLuaFunctionDefs::GetGameType(lua_State* luaVM)
{
    // Grab ASE
    ASE* pASE = ASE::GetInstance();
    if (pASE)
    {
        // Grab the gametype string.
        const char* szGameType = pASE->GetGameType();

        // Return the gametype string if it's not "MTA:SA"
        if (strcmp(szGameType, GAME_TYPE_STRING))
        {
            lua_pushstring(luaVM, szGameType);
            return 1;
        }
    }

    // No game type
    lua_pushnil(luaVM);
    return 1;
}

int CLuaFunctionDefs::SetGameType(lua_State* luaVM)
{
    //  bool setGameType ( string gameType )
    SString strGameType;

    CScriptArgReader argStream(luaVM);
    argStream.ReadIfNextIsString(strGameType, "");            // Default to empty for backward compat with previous implementation

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetGameType(strGameType))
        {
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

int CLuaFunctionDefs::SetMapName(lua_State* luaVM)
{
    //  bool setMapName ( string mapName )
    SString strMapName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadIfNextIsString(strMapName, "");            // Default to empty for backward compat with previous implementation

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetMapName(strMapName))
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

int CLuaFunctionDefs::GetRuleValue(lua_State* luaVM)
{
    //  string getRuleValue ( string key )
    SString strKey;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        const char* szRule = CStaticFunctionDefinitions::GetRuleValue(strKey);
        if (szRule)
        {
            lua_pushstring(luaVM, szRule);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::SetRuleValue(lua_State* luaVM)
{
    //  bool setRuleValue ( string key, string value )
    SString strKey;
    SString strValue;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);
    argStream.ReadString(strValue);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetRuleValue(strKey, strValue))
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

int CLuaFunctionDefs::RemoveRuleValue(lua_State* luaVM)
{
    //  bool removeRuleValue ( string key )
    SString strKey;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::RemoveRuleValue(strKey))
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

int CLuaFunctionDefs::GetNetworkUsageData(lua_State* luaVM)
{
    SPacketStat m_PacketStats[2][256];
    memcpy(m_PacketStats, g_pNetServer->GetPacketStats(), sizeof(m_PacketStats));

    lua_createtable(luaVM, 0, 2);

    lua_pushstring(luaVM, "in");
    lua_createtable(luaVM, 0, 2);
    {
        lua_pushstring(luaVM, "bits");
        lua_createtable(luaVM, 255, 1);
        for (unsigned int i = 0; i < 256; ++i)
        {
            const SPacketStat& statIn = m_PacketStats[CNetServer::STATS_INCOMING_TRAFFIC][i];
            lua_pushnumber(luaVM, statIn.iTotalBytes * 8);
            lua_rawseti(luaVM, -2, i);
        }
        lua_rawset(luaVM, -3);

        lua_pushstring(luaVM, "count");
        lua_createtable(luaVM, 255, 1);
        for (unsigned int i = 0; i < 256; ++i)
        {
            const SPacketStat& statIn = m_PacketStats[CNetServer::STATS_INCOMING_TRAFFIC][i];
            lua_pushnumber(luaVM, statIn.iCount);
            lua_rawseti(luaVM, -2, i);
        }
        lua_rawset(luaVM, -3);
    }
    lua_rawset(luaVM, -3);

    lua_pushstring(luaVM, "out");
    lua_createtable(luaVM, 0, 2);
    {
        lua_pushstring(luaVM, "bits");
        lua_createtable(luaVM, 255, 1);
        for (unsigned int i = 0; i < 256; ++i)
        {
            const SPacketStat& statOut = m_PacketStats[CNetServer::STATS_OUTGOING_TRAFFIC][i];
            lua_pushnumber(luaVM, statOut.iTotalBytes * 8);
            lua_rawseti(luaVM, -2, i);
        }
        lua_rawset(luaVM, -3);

        lua_pushstring(luaVM, "count");
        lua_createtable(luaVM, 255, 1);
        for (unsigned int i = 0; i < 256; ++i)
        {
            const SPacketStat& statOut = m_PacketStats[CNetServer::STATS_OUTGOING_TRAFFIC][i];
            lua_pushnumber(luaVM, statOut.iCount);
            lua_rawseti(luaVM, -2, i);
        }
        lua_rawset(luaVM, -3);
    }
    lua_rawset(luaVM, -3);

    return 1;
}

int CLuaFunctionDefs::GetNetworkStats(lua_State* luaVM)
{
    //  table getNetworkStats ( [element player] )
    CPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer, NULL);

    if (!argStream.HasErrors())
    {
        NetServerPlayerID PlayerID = pPlayer ? pPlayer->GetSocket() : NetServerPlayerID();
        NetStatistics     stats;
        if (g_pNetServer->GetNetworkStatistics(&stats, PlayerID))
        {
            lua_createtable(luaVM, 0, 11);

            lua_pushstring(luaVM, "bytesReceived");
            lua_pushnumber(luaVM, static_cast<double>(stats.bytesReceived));
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "bytesSent");
            lua_pushnumber(luaVM, static_cast<double>(stats.bytesSent));
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "packetsReceived");
            lua_pushnumber(luaVM, stats.packetsReceived);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "packetsSent");
            lua_pushnumber(luaVM, stats.packetsSent);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "packetlossTotal");
            lua_pushnumber(luaVM, stats.packetlossTotal);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "packetlossLastSecond");
            lua_pushnumber(luaVM, stats.packetlossLastSecond);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "messagesInSendBuffer");
            lua_pushnumber(luaVM, stats.messagesInSendBuffer);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "messagesInResendBuffer");
            lua_pushnumber(luaVM, stats.messagesInResendBuffer);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "isLimitedByCongestionControl");
            lua_pushnumber(luaVM, stats.isLimitedByCongestionControl ? 1 : 0);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "isLimitedByOutgoingBandwidthLimit");
            lua_pushnumber(luaVM, stats.isLimitedByOutgoingBandwidthLimit ? 1 : 0);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "encryptionStatus");
            lua_pushnumber(luaVM, 1);
            lua_settable(luaVM, -3);

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetVersion(lua_State* luaVM)
{
    lua_createtable(luaVM, 0, 8);

    lua_pushstring(luaVM, "number");
    lua_pushnumber(luaVM, CStaticFunctionDefinitions::GetVersion());
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "mta");
    lua_pushstring(luaVM, CStaticFunctionDefinitions::GetVersionString());
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "name");
    lua_pushstring(luaVM, CStaticFunctionDefinitions::GetVersionName());
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "netcode");
    lua_pushnumber(luaVM, CStaticFunctionDefinitions::GetNetcodeVersion());
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "os");
    lua_pushstring(luaVM, CStaticFunctionDefinitions::GetOperatingSystemName());
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "type");
    lua_pushstring(luaVM, CStaticFunctionDefinitions::GetVersionBuildType());
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "tag");
    lua_pushstring(luaVM, CStaticFunctionDefinitions::GetVersionBuildTag());
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "sortable");
    lua_pushstring(luaVM, CStaticFunctionDefinitions::GetVersionSortable());
    lua_settable(luaVM, -3);

    return 1;
}

int CLuaFunctionDefs::GetModuleInfo(lua_State* luaVM)
{
    SString strModuleName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strModuleName);

    if (!argStream.HasErrors())
    {
        std::list<CLuaModule*> modules = m_pLuaModuleManager->GetLoadedModules();
        for (const auto mod : modules)
        {
            if (mod->_GetName() == strModuleName)
            {
                lua_newtable(luaVM);

                lua_pushstring(luaVM, "name");
                lua_pushstring(luaVM, mod->_GetFunctions().szModuleName);
                lua_settable(luaVM, -3);

                lua_pushstring(luaVM, "author");
                lua_pushstring(luaVM, mod->_GetFunctions().szAuthor);
                lua_settable(luaVM, -3);

                lua_pushstring(luaVM, "version");
                SString strVersion("%.2f", mod->_GetFunctions().fVersion);
                lua_pushstring(luaVM, strVersion);
                lua_settable(luaVM, -3);

                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetModules(lua_State* luaVM)
{
    lua_newtable(luaVM);
    list<CLuaModule*>           lua_LoadedModules = m_pLuaModuleManager->GetLoadedModules();
    list<CLuaModule*>::iterator iter = lua_LoadedModules.begin();
    unsigned int                uiIndex = 1;
    for (; iter != lua_LoadedModules.end(); ++iter)
    {
        lua_pushnumber(luaVM, uiIndex++);
        lua_pushstring(luaVM, (*iter)->_GetFunctions().szFileName);
        lua_settable(luaVM, -3);
    }
    return 1;
}

int CLuaFunctionDefs::GetPerformanceStats(lua_State* luaVM)
{
    SString strCategory, strOptions, strFilter;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strCategory);
    argStream.ReadString(strOptions, "");
    argStream.ReadString(strFilter, "");

    if (!argStream.HasErrors())
    {
        CPerfStatResult Result;
        CPerfStatManager::GetSingleton()->GetStats(&Result, strCategory, strOptions, strFilter);

        lua_newtable(luaVM);
        for (int c = 0; c < Result.ColumnCount(); c++)
        {
            const SString& name = Result.ColumnName(c);
            lua_pushnumber(luaVM, c + 1);            // row index number (starting at 1, not 0)
            lua_pushlstring(luaVM, name.c_str(), name.length());
            lua_settable(luaVM, -3);
        }

        lua_newtable(luaVM);
        for (int r = 0; r < Result.RowCount(); r++)
        {
            lua_newtable(luaVM);                     // new table
            lua_pushnumber(luaVM, r + 1);            // row index number (starting at 1, not 0)
            lua_pushvalue(luaVM, -2);                // value
            lua_settable(luaVM, -4);                 // refer to the top level table

            for (int c = 0; c < Result.ColumnCount(); c++)
            {
                SString& cell = Result.Data(c, r);
                lua_pushnumber(luaVM, c + 1);
                lua_pushlstring(luaVM, cell.c_str(), cell.length());
                lua_settable(luaVM, -3);
            }
            lua_pop(luaVM, 1);            // pop the inner table
        }
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::SetDevelopmentMode(lua_State* luaVM)
{
    // bool setDevelopmentMode ( bool enable )
    bool enable;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(enable);

    if (!argStream.HasErrors())
    {
        g_pGame->SetDevelopmentMode(enable);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetDevelopmentMode(lua_State* luaVM)
{
    // bool getDevelopmentMode ()
    lua_pushboolean(luaVM, g_pGame->GetDevelopmentMode());
    return 1;
}
