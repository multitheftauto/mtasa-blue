/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Util.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::GetValidPedModels(lua_State* luaVM)
{
    bool includeCustom;
    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(includeCustom, true);

    auto* modelManager = g_pClientGame->GetManager()->GetModelManager();

    std::size_t index = 0;
    lua_newtable(luaVM);

    // Gather default and possibly custom GTASA ped model IDs
    for (std::size_t i = 0; i <= 312; i++)
    {
        if (CClientPlayerManager::IsValidModel(i))
        {
            // Skip custom skins if not requested
            if (!includeCustom && modelManager->FindModelByID(i))
                continue;

            lua_pushnumber(luaVM, ++index);
            lua_pushnumber(luaVM, i);
            lua_settable(luaVM, -3);
        }
    }

    if (includeCustom)
    {
        // Gather the rest of custom skin model IDs allocated with engineRequestModel
        // (there are usually some < 313 as well, and since we don't want duplicates, we start at 313, others are already included by the loop above)
        for (const auto& model : m_pManager->GetModelManager()->GetModelsByType(eClientModelType::PED, 313))
        {
            lua_pushnumber(luaVM, ++index);
            lua_pushnumber(luaVM, model->GetModelID());
            lua_settable(luaVM, -3);
        }
    }

    return 1;
}

int CLuaFunctionDefs::GetNetworkUsageData(lua_State* luaVM)
{
    SPacketStat m_PacketStats[2][256];
    memcpy(m_PacketStats, g_pNet->GetPacketStats(), sizeof(m_PacketStats));

    lua_createtable(luaVM, 0, 2);

    lua_pushstring(luaVM, "in");
    lua_createtable(luaVM, 0, 2);
    {
        lua_pushstring(luaVM, "bits");
        lua_createtable(luaVM, 255, 1);
        for (unsigned int i = 0; i < 256; ++i)
        {
            const SPacketStat& statIn = m_PacketStats[CNet::STATS_INCOMING_TRAFFIC][i];
            lua_pushnumber(luaVM, statIn.iTotalBytes * 8);
            lua_rawseti(luaVM, -2, i);
        }
        lua_rawset(luaVM, -3);

        lua_pushstring(luaVM, "count");
        lua_createtable(luaVM, 255, 1);
        for (unsigned int i = 0; i < 256; ++i)
        {
            const SPacketStat& statIn = m_PacketStats[CNet::STATS_INCOMING_TRAFFIC][i];
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
            const SPacketStat& statOut = m_PacketStats[CNet::STATS_OUTGOING_TRAFFIC][i];
            lua_pushnumber(luaVM, statOut.iTotalBytes * 8);
            lua_rawseti(luaVM, -2, i);
        }
        lua_rawset(luaVM, -3);

        lua_pushstring(luaVM, "count");
        lua_createtable(luaVM, 255, 1);
        for (unsigned int i = 0; i < 256; ++i)
        {
            const SPacketStat& statOut = m_PacketStats[CNet::STATS_OUTGOING_TRAFFIC][i];
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
    NetStatistics stats;
    if (g_pNet->GetNetworkStatistics(&stats))
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

int CLuaFunctionDefs::GetLocalization(lua_State* luaVM)
{
    lua_createtable(luaVM, 0, 2);

    lua_pushstring(luaVM, "code");
    lua_pushstring(luaVM, g_pCore->GetLocalization()->GetLanguageCode().c_str());
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "name");
    lua_pushstring(luaVM, g_pCore->GetLocalization()->GetLanguageName().c_str());
    lua_settable(luaVM, -3);

    return 1;
}

int CLuaFunctionDefs::GetKeyboardLayout(lua_State* luaVM)
{
    const char* readingLayout = "ltr";

#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
    DWORD readingLayoutValue = 0;

    if (::GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IREADINGLAYOUT | LOCALE_RETURN_NUMBER, reinterpret_cast<LPWSTR>(&readingLayoutValue),
                          sizeof(readingLayoutValue) / sizeof(WCHAR)) != 0)
    {
        switch (readingLayoutValue)
        {
            case 0:            // Left to right (English)
                readingLayout = "ltr";
                break;
            case 1:            // Right to left (Arabic, Hebrew)
                readingLayout = "rtl";
                break;
            case 2:            // Vertical top to bottom with columns to the left and also left to right (Japanese)
                readingLayout = "ttb-rtl-ltr";
                break;
            case 3:            // Vertical top to bottom with columns proceeding to the right (Mongolian)
                readingLayout = "ttb-ltr";
                break;
            default:
                break;
        }
    }

#else
    HKL             keyboardLayout = ::GetKeyboardLayout(0 /* current thread*/);
    LCID            locale = MAKELCID(LOWORD(keyboardLayout), SORT_DEFAULT);
    LOCALESIGNATURE localeSignature = {};

    if (GetLocaleInfoW(locale, LOCALE_FONTSIGNATURE, reinterpret_cast<LPWSTR>(&localeSignature), sizeof(localeSignature) / sizeof(WCHAR)) != 0)
    {
        if ((localeSignature.lsUsb[3] & 0x08000000) != 0)
        {
            readingLayout = "rtl";
        }
    }
#endif

    lua_createtable(luaVM, 0, 1);
    lua_pushstring(luaVM, "readingLayout");
    lua_pushstring(luaVM, readingLayout);
    lua_settable(luaVM, -3);

    return 1;
}

int CLuaFunctionDefs::GetPerformanceStats(lua_State* luaVM)
{
    //  table getPerformanceStats ( string category, string options, string filter )
    SString strCategory, strOptions, strFilter;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strCategory);
    argStream.ReadString(strOptions, "");
    argStream.ReadString(strFilter, "");

    if (!argStream.HasErrors())
    {
        CClientPerfStatResult Result;
        CClientPerfStatManager::GetSingleton()->GetStats(&Result, strCategory, strOptions, strFilter);

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
    //  bool setDevelopmentMode ( bool enable )
    //  bool setDevelopmentMode ( string command )
    bool    bEnable;
    bool    bEnableWeb;
    SString strCommand;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsString())
    {
        argStream.ReadString(strCommand);
        // g_pClientGame->SetDevSetting ( strCommand );
        lua_pushboolean(luaVM, true);
        return 1;
    }
    argStream.ReadBool(bEnable);
    argStream.ReadBool(bEnableWeb, false);

    if (!argStream.HasErrors())
    {
        g_pClientGame->SetDevelopmentMode(bEnable, bEnableWeb);
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
    //  bool getDevelopmentMode ()
    bool bResult = g_pClientGame->GetDevelopmentMode();
    lua_pushboolean(luaVM, bResult);
    return 1;
}

int CLuaFunctionDefs::DownloadFile(lua_State* luaVM)
{
    SString          strFileInput = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strFileInput);

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Grab its resource
            CResource* pThisResource = pLuaMain->GetResource();
            CResource* pOtherResource = pThisResource;

            SString strMetaPath;

            // Resolve other resource from name
            if (CResourceManager::ParseResourcePathInput(strFileInput, pOtherResource, NULL, &strMetaPath))
            {
                std::list<CResourceFile*>::const_iterator iter = pOtherResource->IterBeginResourceFiles();
                for (; iter != pOtherResource->IterEndResourceFiles(); iter++)
                {
                    if (strcmp(strMetaPath, (*iter)->GetShortName()) == 0)
                    {
                        if (CStaticFunctionDefinitions::DownloadFile(pOtherResource, strMetaPath, pThisResource, (*iter)->GetServerChecksum()))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                }
                m_pScriptDebugging->LogCustom(luaVM, SString("%s: File doesn't exist", lua_tostring(luaVM, lua_upvalueindex(1))));
            }
            else
            {
                m_pScriptDebugging->LogCustom(luaVM, 255, 255, 255, "%s: Invalid path", lua_tostring(luaVM, lua_upvalueindex(1)));
            }
        }
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::AddDebugHook(lua_State* luaVM)
{
    //  bool AddDebugHook ( string hookType, function callback[, table allowedNames ] )
    EDebugHookType       hookType;
    CLuaFunctionRef      callBack;
    std::vector<SString> allowedNames;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(hookType);
    argStream.ReadFunction(callBack);
    if (argStream.NextIsTable())
        argStream.ReadStringTable(allowedNames);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        if (g_pClientGame->GetDebugHookManager()->AddDebugHook(hookType, callBack, allowedNames))
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

int CLuaFunctionDefs::RemoveDebugHook(lua_State* luaVM)
{
    //  bool RemoveDebugHook ( string hookType, function callback )
    EDebugHookType  hookType;
    CLuaFunctionRef callBack;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(hookType);
    argStream.ReadFunction(callBack);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        if (g_pClientGame->GetDebugHookManager()->RemoveDebugHook(hookType, callBack))
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
