/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMainConfig.cpp
 *  PURPOSE:     XML-based main configuration file parser class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CMainConfig.h"
#include "CBandwidthSettings.h"
#include "CTickRateSettings.h"
#include "Utils.h"
#include "ASE.h"
#include "CGame.h"
#include "CScriptDebugging.h"
#include "CResourceManager.h"
#include "CConsoleCommands.h"
#include "CHTTPD.h"
#include "CStaticFunctionDefinitions.h"

#define MTA_SERVER_CONF_TEMPLATE "mtaserver.conf.template"

extern CGame* g_pGame;

CBandwidthSettings* g_pBandwidthSettings = new CBandwidthSettings();
CTickRateSettings   g_TickRateSettings;

using namespace std;

// Used to identify <client_file> names
struct
{
    int         iBitNumber;
    const char* szRealFilename;
} const gtaDataFiles[] = {
    {1, "data/animgrp.dat"},   {3, "data/ar_stats.dat"}, {0, "data/carmods.dat"},   {5, "data/clothes.dat"},         {7, "data/default.dat"},
    {9, "data/default.ide"},   {11, "data/gta.dat"},     {25, "data/maps"},         {6, "data/object.dat"},          {13, "data/peds.ide"},
    {15, "data/pedstats.dat"}, {17, "data/txdcut.ide"},  {14, "data/vehicles.ide"}, {20, "data/weapon.dat"},         {4, "data/melee.dat"},
    {16, "data/water.dat"},    {18, "data/water1.dat"},  {2, "data/handling.cfg"},  {19, "models/coll/weapons.col"}, {21, "data/plants.dat"},
    {23, "data/furnitur.dat"}, {24, "data/procobj.dat"}, {8, "data/surface.dat"},   {12, "data/surfinfo.dat"},       {22, "anim/ped.ifp"},
    {26, "data/timecyc.dat"},
};

CMainConfig::CMainConfig(CConsole* pConsole) : CXMLConfig(NULL)
{
    m_pConsole = pConsole;
    m_pRootNode = NULL;
    m_pCommandLineParser = NULL;

    m_usServerPort = 0;
    m_uiHardMaxPlayers = 0;
    m_bHTTPEnabled = true;
    m_iAseMode = 0;
    m_iUpdateCycleDatagramsLimit = 4;
    m_iUpdateCycleMessagesLimit = 50;
    m_usHTTPPort = 0;
    m_ucHTTPDownloadType = HTTP_DOWNLOAD_DISABLED;
    m_iHTTPMaxConnectionsPerClient = 4;
    m_iHTTPThreadCount = 8;
    m_iHTTPDosThreshold = 20;
    m_iEnableClientChecks = -1;
    m_bJoinFloodProtectionEnabled = true;
    m_bScriptDebugLogEnabled = false;
    m_uiScriptDebugLogLevel = 0;
    m_bDontBroadcastLan = false;
    m_usFPSLimit = 36;
    m_uiVoiceSampleRate = 1;
    m_ucVoiceQuality = 4;
    m_bVoiceEnabled = false;
    m_uiVoiceBitrate = 0;
    m_strBandwidthReductionMode = "medium";
    m_iPendingWorkToDoSleepTime = -1;
    m_iNoWorkToDoSleepTime = -1;
    m_bThreadNetEnabled = true;
    m_iBackupInterval = 3;
    m_iBackupAmount = 5;
    m_bSyncMapElementData = true;
}

bool CMainConfig::Load()
{
    // Eventually destroy the previously loaded xml
    if (m_pFile)
    {
        delete m_pFile;
        m_pFile = NULL;
    }

    // Load the XML
    m_pFile = g_pServerInterface->GetXML()->CreateXML(GetFileName().c_str());
    if (!m_pFile)
    {
        CLogger::ErrorPrintf("Error loading config file\n");
        return false;
    }

    // Parse it
    if (!m_pFile->Parse())
    {
        SString strParseErrorDesc;
        m_pFile->GetLastError(strParseErrorDesc);
        CLogger::ErrorPrintf("Error parsing %s - %s\n", *ExtractFilename(GetFileName()), *strParseErrorDesc);
        return false;
    }

    // Grab the XML root node
    m_pRootNode = m_pFile->GetRootNode();
    if (!m_pRootNode)
    {
        CLogger::ErrorPrintf("Missing root node ('config')\n");
        return false;
    }

    if (AddMissingSettings())
    {
        Save();
    }

    // Name
    int iResult = GetString(m_pRootNode, "servername", m_strServerName, 1, 96);
    if (iResult == DOESNT_EXIST)
    {
        CLogger::ErrorPrintf("Server name not specified in config\n");
        return false;
    }
    else if (iResult == INVALID_VALUE)
    {
        CLogger::ErrorPrintf("Server name must be between 1 and 96 characters\n");
        return false;
    }

    // Strip spaces from beginning and end of server name
    m_strServerName = SString(m_strServerName).TrimStart(" ").TrimEnd(" ");

    // Grab the forced server ip(s)
    GetString(m_pRootNode, "serverip", m_strServerIP);
    m_strServerIP = SString(m_strServerIP).Replace(" ", "");
    if (m_strServerIP == "auto" || m_strServerIP == "any")
        m_strServerIP = "";

    // Grab the port
    int iTemp;
    iResult = GetInteger(m_pRootNode, "serverport", iTemp, 1, 65535);
    if (iResult == IS_SUCCESS)
    {
        m_usServerPort = static_cast<unsigned short>(iTemp);
    }
    else
    {
        if (iResult == DOESNT_EXIST)
            CLogger::ErrorPrintf("Server port not specified in config\n");
        else
            CLogger::ErrorPrintf("Server port must be between 1 and 65535\n");

        return false;
    }

    // Grab the max players
    iResult = GetInteger(m_pRootNode, "maxplayers", iTemp, 1, MAX_PLAYER_COUNT);
    if (iResult == IS_SUCCESS)
    {
        m_uiHardMaxPlayers = iTemp;
        m_uiSoftMaxPlayers = iTemp;
    }
    else
    {
        if (iResult == DOESNT_EXIST)
            CLogger::ErrorPrintf("Max players not specified in config\n");
        else
            CLogger::ErrorPrintf("Max players must be between 1 and %u\n", MAX_PLAYER_COUNT);

        return false;
    }

    // httpserver
    iResult = GetBoolean(m_pRootNode, "httpserver", m_bHTTPEnabled);
    if (iResult == INVALID_VALUE)
    {
        CLogger::LogPrint("WARNING: Invalid value specified in \"httpserver\" tag; defaulting to 1\n");
        m_bHTTPEnabled = true;
    }
    else if (iResult == DOESNT_EXIST)
    {
        m_bHTTPEnabled = true;
    }

    // HTTPD port
    iResult = GetInteger(m_pRootNode, "httpport", iTemp, 1, 65535);
    if (iResult == IS_SUCCESS)
    {
        m_usHTTPPort = static_cast<unsigned short>(iTemp);
    }
    else
    {
        if (iResult == DOESNT_EXIST)
            CLogger::ErrorPrintf("HTTP port is not specified in config\n");
        else
            CLogger::ErrorPrintf("HTTP server port must be between 1 and 65535\n");

        return false;
    }

    // HTTPD Download URL (if we want to host externally)
    if (GetString(m_pRootNode, "httpdownloadurl", m_strHTTPDownloadURL, 5) == IS_SUCCESS)
    {
        m_ucHTTPDownloadType = HTTP_DOWNLOAD_ENABLED_URL;
        m_strHTTPDownloadURL = SString(m_strHTTPDownloadURL).TrimEnd("/");
    }
    else
    {
        m_ucHTTPDownloadType = HTTP_DOWNLOAD_ENABLED_PORT;
        m_strHTTPDownloadURL = "";
    }

    // httpmaxconnectionsperclient
    GetInteger(m_pRootNode, "httpmaxconnectionsperclient", m_iHTTPMaxConnectionsPerClient, 1, 8);
    m_iHTTPMaxConnectionsPerClient = Clamp(1, m_iHTTPMaxConnectionsPerClient, 8);

    // httpthreadcount
    GetInteger(m_pRootNode, "httpthreadcount", m_iHTTPThreadCount, 1, 20);
    m_iHTTPThreadCount = Clamp(1, m_iHTTPThreadCount, 20);

    // httpdosthreshold
    GetInteger(m_pRootNode, "httpdosthreshold", m_iHTTPDosThreshold, 1, 10000);
    m_iHTTPDosThreshold = Clamp(1, m_iHTTPDosThreshold, 10000);

    // http_dos_exclude
    GetString(m_pRootNode, "http_dos_exclude", m_strHTTPDosExclude);

    // verifyclientsettings
    GetInteger(m_pRootNode, "verifyclientsettings", m_iEnableClientChecks);

    // Handle the <client_file> nodes
    CXMLNode*    pNode = NULL;
    unsigned int uiCurrentIndex = 0;
    do
    {
        // Grab the current script node
        pNode = m_pRootNode->FindSubNode("client_file", uiCurrentIndex++);
        if (pNode)
        {
            // Grab its "name" attribute
            CXMLAttribute* pAttribute = pNode->GetAttributes().Find("name");
            SString        strName = pAttribute ? pAttribute->GetValue() : "";
            strName = strName.Replace("\\", "/").ToLower();

            // Grab its "verify" attribute
            pAttribute = pNode->GetAttributes().Find("verify");
            SString strVerify = pAttribute ? pAttribute->GetValue() : "";
            bool    bVerify = strVerify == "true" || strVerify == "yes" || strVerify == "1";

            // Find bitnumber
            bool bFound = false;
            for (uint i = 0; i < NUMELMS(gtaDataFiles); i++)
            {
                if (strName == gtaDataFiles[i].szRealFilename)
                {
                    if (bVerify)
                        m_iEnableClientChecks |= 1 << gtaDataFiles[i].iBitNumber;
                    else
                        m_iEnableClientChecks &= ~(1 << gtaDataFiles[i].iBitNumber);
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
                CLogger::ErrorPrintf("Unknown client_file '%s'\n", *strName);
        }
    } while (pNode);

    // allow_gta3_img_mods
    SString strImgMods;
    GetString(m_pRootNode, "allow_gta3_img_mods", strImgMods);

    // hideac
    int iHideAC = 0;
    GetInteger(m_pRootNode, "hideac", iHideAC);

    {
        std::set<SString> disableACMap;
        std::set<SString> enableSDMap;

        {
            SString strDisableAC;
            GetString(m_pRootNode, "disableac", strDisableAC);
            std::vector<SString> tagACList;
            ReadCommaSeparatedList(strDisableAC, tagACList);
            for (std::vector<SString>::iterator it = tagACList.begin(); it != tagACList.end(); ++it)
                if (isdigit((uchar) * **it))
                {
                    MapInsert(disableACMap, *it);
                    MapInsert(m_DisableComboACMap, *it);
                }
        }

        // Add support for detections which default to disabled
        const char* defaultDisabledList[] = {"12", "14", "15", "16", "20", "22", "23", "28", "31", "32", "33", "34", "35", "36"};
        for (uint i = 0; i < NUMELMS(defaultDisabledList); i++)
        {
            MapInsert(m_DisableComboACMap, defaultDisabledList[i]);
        }

        {
            SString strEnableSD;
            GetString(m_pRootNode, "enablesd", strEnableSD);
            std::vector<SString> tagSDList;
            ReadCommaSeparatedList(strEnableSD, tagSDList);
            for (std::vector<SString>::iterator it = tagSDList.begin(); it != tagSDList.end(); ++it)
                if (isdigit((uchar) * **it))
                {
                    MapInsert(enableSDMap, *it);
                    MapRemove(m_DisableComboACMap, *it);
                }

            // Also save initial value in transient settings, so we can update the config without anyone knowing
            MapSet(m_TransientSettings, "enablesd", strEnableSD);
        }

        CArgMap argMap;
        for (std::set<SString>::iterator it = m_DisableComboACMap.begin(); it != m_DisableComboACMap.end(); ++it)
            argMap.Set(*it, "");
        SString strDisableComboACMap = argMap.ToString();

        argMap = CArgMap();
        for (std::set<SString>::iterator it = disableACMap.begin(); it != disableACMap.end(); ++it)
            argMap.Set(*it, "");
        SString strDisableACMap = argMap.ToString();

        argMap = CArgMap();
        for (std::set<SString>::iterator it = enableSDMap.begin(); it != enableSDMap.end(); ++it)
            argMap.Set(*it, "");
        SString strEnableSDMap = argMap.ToString();

        g_pNetServer->SetChecks(strDisableComboACMap, strDisableACMap, strEnableSDMap, m_iEnableClientChecks, iHideAC != 0, strImgMods);
    }

    {
        SString strEnable;
        GetString(m_pRootNode, "enable_diagnostic", strEnable);
        std::vector<SString> tagList;
        ReadCommaSeparatedList(strEnable, tagList);
        m_EnableDiagnosticMap = std::set<SString>(tagList.begin(), tagList.end());
    }

    // Grab the server password
    iResult = GetString(m_pRootNode, "password", m_strPassword, 1, 32);

    // Grab the server fps limit
    int iFPSTemp = 0;
    iResult = GetInteger(m_pRootNode, "fpslimit", iFPSTemp, 0, std::numeric_limits<short>::max());
    if (iResult == IS_SUCCESS)
    {
        if (iFPSTemp == 0 || iFPSTemp >= 25)
        {
            m_usFPSLimit = (unsigned short)iFPSTemp;
            SetInteger(m_pRootNode, "fpslimit", (int)m_usFPSLimit);
        }
    }

    // Grab whether or not voice is enabled
    iResult = GetInteger(m_pRootNode, "voice", iTemp, 0, 1);
    if (iResult == IS_SUCCESS)
    {
        m_bVoiceEnabled = iTemp ? true : false;
    }

    // Grab the Sample Rate for Voice
    iTemp = m_uiVoiceSampleRate;
    iResult = GetInteger(m_pRootNode, "voice_samplerate", iTemp, 0, 2);
    m_uiVoiceSampleRate = Clamp(0, iTemp, 2);

    // Grab the Quality for Voice
    iTemp = m_ucVoiceQuality;
    iResult = GetInteger(m_pRootNode, "voice_quality", iTemp, 0, 10);
    m_ucVoiceQuality = static_cast<unsigned char>(Clamp(0, iTemp, 10));

    // Grab the bitrate for Voice [optional]
    iResult = GetInteger(m_pRootNode, "voice_bitrate", iTemp);
    if (iResult == IS_SUCCESS)
    {
        m_uiVoiceBitrate = iTemp;
    }

    // Grab the serial verification
    /** ACHTUNG: Unsupported for release 1.0 (#4090)
    iResult = GetBoolean ( m_pRootNode, "verifyserials", m_bVerifySerials );
    if ( iResult == INVALID_VALUE )
    {
        m_bVerifySerials = true;
    }
    else if ( iResult == DOESNT_EXIST )
    */
    {
        m_bVerifySerials = false;
    }

    // Grab the server-id filename
    SString strIdFile = "server-id.keys";
    GetString(m_pRootNode, "idfile", strIdFile, 1);
    m_strIdFile = g_pServerInterface->GetModManager()->GetAbsolutePath(strIdFile);

    // Grab the server logfiles
    std::string strBuffer;
    if (GetString(m_pRootNode, "logfile", strBuffer, 1) == IS_SUCCESS)
        m_strLogFile = g_pServerInterface->GetModManager()->GetAbsolutePath(strBuffer.c_str());

    if (GetString(m_pRootNode, "authfile", strBuffer, 1) == IS_SUCCESS)
        m_strAuthFile = g_pServerInterface->GetModManager()->GetAbsolutePath(strBuffer.c_str());

    if (GetString(m_pRootNode, "dbfile", strBuffer, 1) == IS_SUCCESS)
        m_strDbLogFilename = g_pServerInterface->GetModManager()->GetAbsolutePath(strBuffer.c_str());
    else
        m_strDbLogFilename = g_pServerInterface->GetModManager()->GetAbsolutePath("logs/db.log");

    if (GetString(m_pRootNode, "loadstringfile", strBuffer, 1) == IS_SUCCESS)
        m_strLoadstringLogFilename = g_pServerInterface->GetModManager()->GetAbsolutePath(strBuffer.c_str());

    // Grab the server access control list
    if (GetString(m_pRootNode, "acl", strBuffer, 1, 255) == IS_SUCCESS)
    {
        m_strAccessControlListFile = g_pServerInterface->GetModManager()->GetAbsolutePath(strBuffer.c_str());
    }
    else
    {
        m_strAccessControlListFile = g_pServerInterface->GetModManager()->GetAbsolutePath("acl.xml");
    }

    // Grab the global databases path
    if (GetString(m_pRootNode, "global_databases_path", strBuffer, 1, 255) != IS_SUCCESS)
        strBuffer = "databases/global";
    if (!IsValidFilePath(strBuffer.c_str()) || strBuffer.empty())
    {
        CLogger::ErrorPrintf("global_databases_path is not valid. Defaulting to 'databases/global'\n");
        strBuffer = "databases/global";
    }
    m_strGlobalDatabasesPath = g_pServerInterface->GetModManager()->GetAbsolutePath(strBuffer.c_str());

    // Grab the system databases path
    if (GetString(m_pRootNode, "system_databases_path", strBuffer, 1, 255) != IS_SUCCESS)
        strBuffer = "databases/system";
    if (!IsValidFilePath(strBuffer.c_str()) || strBuffer.empty())
    {
        CLogger::ErrorPrintf("system_databases_path is not valid. Defaulting to 'databases/system'\n");
        strBuffer = "databases/system";
    }
    m_strSystemDatabasesPath = g_pServerInterface->GetModManager()->GetAbsolutePath(strBuffer.c_str());

    // Grab the backup path
    if (GetString(m_pRootNode, "backup_path", strBuffer, 1, 255) != IS_SUCCESS)
        strBuffer = "backups";
    if (!IsValidFilePath(strBuffer.c_str()) || strBuffer.empty())
    {
        CLogger::ErrorPrintf("backup_path is not valid. Defaulting to 'backups'\n");
        strBuffer = "backups";
    }
    m_strBackupPath = g_pServerInterface->GetModManager()->GetAbsolutePath(strBuffer.c_str());

    // Grab the backup interval
    GetInteger(m_pRootNode, "backup_interval", m_iBackupInterval);
    m_iBackupInterval = Clamp(0, m_iBackupInterval, 30);

    // Grab the backup count
    GetInteger(m_pRootNode, "backup_copies", m_iBackupAmount);
    m_iBackupAmount = Clamp(0, m_iBackupAmount, 100);

    // bandwidth_reduction
    GetString(m_pRootNode, "bandwidth_reduction", m_strBandwidthReductionMode);
    ApplyBandwidthReductionMode();

    // busy_sleep_time
    GetInteger(m_pRootNode, "busy_sleep_time", m_iPendingWorkToDoSleepTime);
    m_iPendingWorkToDoSleepTime = Clamp(-1, m_iPendingWorkToDoSleepTime, 50);

    // idle_sleep_time
    GetInteger(m_pRootNode, "idle_sleep_time", m_iNoWorkToDoSleepTime);
    m_iNoWorkToDoSleepTime = Clamp(-1, m_iNoWorkToDoSleepTime, 50);

    // threadnet - Default to on at startup
    m_bThreadNetEnabled = true;
    ApplyThreadNetEnabled();

    // auth_serial_groups
    SString strGroupList;
    if (GetString(m_pRootNode, "auth_serial_groups", strGroupList) != IS_SUCCESS)
    {
        // If not defined in conf file, then default to Admin
        strGroupList = "Admin";
    }
    ReadCommaSeparatedList(strGroupList, m_AuthSerialGroupList);

    // auth_serial_http
    if (GetBoolean(m_pRootNode, "auth_serial_http", m_bAuthSerialHttpEnabled) != IS_SUCCESS)
    {
        m_bAuthSerialHttpEnabled = true;
    }

    // auth_serial_http_ip_exceptions
    SString strIpsString;
    GetString(m_pRootNode, "auth_serial_http_ip_exceptions", strIpsString);
    ReadCommaSeparatedList(strIpsString, m_AuthSerialHttpIpExceptionList);

    // owner_email_address
    SString strEmailsString;
    GetString(m_pRootNode, "owner_email_address", strEmailsString);
    ReadCommaSeparatedList(strEmailsString, m_OwnerEmailAddressList);

    // Check settings in this list here
    const std::vector<SIntSetting>& settingList = GetIntSettingList();
    for (uint i = 0; i < settingList.size(); i++)
    {
        const SIntSetting& item = settingList[i];
        int                iValue = item.iDefault;
        GetInteger(m_pRootNode, item.szName, iValue);
        *item.pVariable = Clamp(item.iMin, iValue, item.iMax);
    }

    // Handle recently retired lightsync_rate
    if (m_pRootNode->FindSubNode("lightweight_sync_interval") == NULL)
    {
        GetInteger(m_pRootNode, "lightsync_rate", g_TickRateSettings.iLightSync);
        g_TickRateSettings.iLightSync = Clamp(200, g_TickRateSettings.iLightSync, 4000);
    }

    ApplyNetOptions();

    return true;
}

//
// Set those settings!
//
void CMainConfig::ApplyBandwidthReductionMode()
{
    if (m_strBandwidthReductionMode == "maximum")
    {
        g_pBandwidthSettings->SetMaximum();
    }
    else if (m_strBandwidthReductionMode == "medium")
    {
        g_pBandwidthSettings->SetMedium();
    }
    else
    {
        m_strBandwidthReductionMode = "none";
        g_pBandwidthSettings->SetNone();
    }
}

void CMainConfig::SetFakeLag(int iPacketLoss, int iExtraPing, int iExtraPingVary, int iKBPSLimit)
{
    m_NetOptions.netSim.bValid = true;
    m_NetOptions.netSim.iPacketLoss = iPacketLoss;
    m_NetOptions.netSim.iExtraPing = iExtraPing;
    m_NetOptions.netSim.iExtraPingVariance = iExtraPingVary;
    m_NetOptions.netSim.iKBPSLimit = iKBPSLimit;
    ApplyNetOptions();
}

void CMainConfig::ApplyNetOptions()
{
    m_NetOptions.netFilter.bValid = true;
    m_NetOptions.netFilter.bAutoFilter = m_bNetAutoFilter != 0;

    m_NetOptions.netOptimize.bValid = true;
    m_NetOptions.netOptimize.iUpdateCycleDatagramsLimit = m_iUpdateCycleDatagramsLimit;
    m_NetOptions.netOptimize.iUpdateCycleMessagesLimit = m_iUpdateCycleMessagesLimit;

    g_pNetServer->SetNetOptions(m_NetOptions);
}

void CMainConfig::ApplyThreadNetEnabled()
{
    CSimControl::EnableSimSystem(m_bThreadNetEnabled, false);
}

bool CMainConfig::LoadExtended()
{
    std::string strBuffer;
    int         iTemp = 0, iResult = 0;

    // minclientversion - Minimum client version or kick
    GetString(m_pRootNode, "minclientversion", m_strMinClientVersion);
    if (m_strMinClientVersion != "" && !IsValidVersionString(m_strMinClientVersion))
    {
        CLogger::LogPrint("WARNING: Invalid value specified in \"minclientversion\"\n");
        m_strMinClientVersion = "";
    }

    // recommendedclientversion - Minimum client version or spam
    GetString(m_pRootNode, "recommendedclientversion", m_strRecommendedClientVersion);
    if (m_strRecommendedClientVersion != "" && !IsValidVersionString(m_strRecommendedClientVersion))
    {
        CLogger::LogPrint("WARNING: Invalid value specified in \"recommendedclientversion\"\n");
        m_strRecommendedClientVersion = "";
    }

    // Grab the script debuglog
    if (GetString(m_pRootNode, "scriptdebuglogfile", strBuffer, 1, 255) == IS_SUCCESS)
    {
        m_strScriptDebugLogFile = g_pServerInterface->GetModManager()->GetAbsolutePath(strBuffer.c_str());
        m_bScriptDebugLogEnabled = true;
    }
    else
    {
        m_bScriptDebugLogEnabled = false;
    }

    // Grab the script debuglog level
    iResult = GetInteger(m_pRootNode, "scriptdebugloglevel", iTemp, 0, 3);
    if (iResult == IS_SUCCESS)
    {
        m_uiScriptDebugLogLevel = iTemp;
    }
    else
    {
        if (iResult == INVALID_VALUE)
        {
            CLogger::LogPrint("WARNING: Invalid value specified in \"scriptdebugloglevel\" tag; defaulting to 0\n");
        }

        m_uiScriptDebugLogLevel = 0;
    }

    iResult = GetInteger(m_pRootNode, "htmldebuglevel", iTemp, 0, 3);
    if (iResult == IS_SUCCESS)
    {
        g_pGame->GetScriptDebugging()->SetHTMLLogLevel(iTemp);
    }
    else
    {
        if (iResult == INVALID_VALUE)
        {
            CLogger::LogPrint("WARNING: Invalid value specified in \"htmldebuglevel\" tag; defaulting to 0\n");
        }

        g_pGame->GetScriptDebugging()->SetHTMLLogLevel(0);
    }

    // Handle the <module> nodes
    CXMLNode*    pNode = NULL;
    unsigned int uiCurrentIndex = 0;
    do
    {
        pNode = m_pRootNode->FindSubNode("module", uiCurrentIndex++);
        if (pNode)
        {
            CXMLAttribute* pAttribute = pNode->GetAttributes().Find("src");
            if (pAttribute)
            {
                std::string strBuffer = pAttribute->GetValue();
                SString     strFilename = PathJoin(g_pServerInterface->GetModManager()->GetServerPath(), SERVER_BIN_PATH_MOD, "modules", strBuffer);

                if (IsValidFilePath(strBuffer.c_str()))
                {
                    g_pGame->GetLuaManager()->GetLuaModuleManager()->LoadModule(strBuffer.c_str(), strFilename, false);
                }
            }
        }
    } while (pNode);

    // Handle the <resource> nodes
    pNode = NULL;
    uiCurrentIndex = 0;
    bool bFoundDefault = false;

    CLogger::SetMinLogLevel(LOGLEVEL_MEDIUM);
    CLogger::LogPrint("Starting resources...");
    CLogger::ProgressDotsBegin();

    do
    {
        if (g_pServerInterface->IsRequestingExit())
            return false;

        // Grab the current script node
        pNode = m_pRootNode->FindSubNode("resource", uiCurrentIndex++);
        if (pNode)
        {
            // Grab its "src" attribute
            CXMLAttribute* pAttribute = pNode->GetAttributes().Find("src");
            if (pAttribute)
            {
                // Grab the text in it and convert iwt to a path inside "scripts"
                std::string strBuffer = pAttribute->GetValue();

                CResource* loadedResource = g_pGame->GetResourceManager()->GetResource(strBuffer.c_str());
                if (!loadedResource)
                {
                    CLogger::ErrorPrintf("Couldn't find resource %s. Check it exists.\n", strBuffer.c_str());
                }
                else
                {
                    loadedResource->SetPersistent(true);

                    pAttribute = pNode->GetAttributes().Find("startup");
                    if (pAttribute)
                    {
                        std::string strStartup = pAttribute->GetValue();
                        if (strStartup.compare("true") == 0 || strStartup.compare("yes") == 0 || strStartup.compare("1") == 0)
                        {
                            if (loadedResource->Start(NULL, true))
                            {
                                CLogger::ProgressDotsUpdate();
                            }
                            else
                            {
                                CLogger::ErrorPrintf("Unable to start resource %s; %s\n", strBuffer.c_str(), loadedResource->GetFailureReason().c_str());
                            }
                        }
                    }

                    pAttribute = pNode->GetAttributes().Find("protected");
                    if (pAttribute)
                    {
                        std::string strProtected = pAttribute->GetValue();
                        if (strProtected.compare("true") == 0 || strProtected.compare("yes") == 0 || strProtected.compare("1") == 0)
                            loadedResource->SetProtected(true);
                    }

                    // Default resource
                    pAttribute = pNode->GetAttributes().Find("default");
                    if (pAttribute)
                    {
                        if (!bFoundDefault)
                        {
                            std::string strDefault = pAttribute->GetValue();
                            if (strDefault.compare("true") == 0 || strDefault.compare("yes") == 0 || strDefault.compare("1") == 0)
                            {
                                std::string strName = loadedResource->GetName();
                                if (!strName.empty())
                                {
                                    g_pGame->GetHTTPD()->SetDefaultResource(strName.c_str());
                                }

                                bFoundDefault = true;
                            }
                        }
                        else
                        {
                            CLogger::ErrorPrintf("More than one default resource specified!\n");
                        }
                    }
                }
            }
        }
    } while (pNode);

    CLogger::ProgressDotsEnd();
    CLogger::SetMinLogLevel(LOGLEVEL_LOW);

    // Register the commands
    RegisterCommand("start", CConsoleCommands::StartResource, false, "Usage: start <resource-name>\nStart a loaded resource eg: start admin");
    RegisterCommand("stop", CConsoleCommands::StopResource, false, "Usage: stop <resource-name>\nStop a resource eg: stop admin");
    RegisterCommand("stopall", CConsoleCommands::StopAllResources, false, "Stop all running resources");
    RegisterCommand("restart", CConsoleCommands::RestartResource, false, "Usage: restart <resource-name>\nRestarts a running resource eg: restart admin");
    RegisterCommand("refresh", CConsoleCommands::RefreshResources, false, "Refresh resource list to find new resources");
    RegisterCommand("refreshall", CConsoleCommands::RefreshAllResources, false, "Refresh resources and restart any changed resources");
    RegisterCommand("list", CConsoleCommands::ListResources, false, "Shows a list of resources");
    RegisterCommand("info", CConsoleCommands::ResourceInfo, false, "Usage: info <resource-name>\nGet info for a resource eg: info admin");
    RegisterCommand("upgrade", CConsoleCommands::UpgradeResources, false,
                    "Usage: upgrade [ all | <resource-name> ]\nPerform a basic upgrade of all resources.");
    RegisterCommand("check", CConsoleCommands::CheckResources, false,
                    "Usage: check [ all | <resource-name> ]\nChecks which files would be changed with upgrade command. Does not modify anything.");

    RegisterCommand("say", CConsoleCommands::Say, false, "Usage: say <text>\nShow a message to all players on the server eg: say hello");
    RegisterCommand("teamsay", CConsoleCommands::TeamSay, false, "Usage: teamsay <test>\nSend a message to all players on the same team");
    RegisterCommand("msg", CConsoleCommands::Msg, false, "Usage: msg <nick> <text>\nSend a message to a player eg: msg playername hello");
    RegisterCommand("me", CConsoleCommands::Me, false, "Usage: me <text>\nShow a message to all players on the server, with your nick prepended");
    RegisterCommand("nick", CConsoleCommands::Nick, false, "Usage: nick <old-nick> <new-nick>\nChange your ingame nickname");

    RegisterCommand("login", CConsoleCommands::LogIn, false, "Usage: login <accountname> <password>\nLogin to an account eg: login accountname password");
    RegisterCommand("logout", CConsoleCommands::LogOut, false, "Log out of the current account");
    RegisterCommand("chgmypass", CConsoleCommands::ChgMyPass, false, "Usage: chgmypass <oldpass> <newpass>\nChange your password eg: chgmypass oldpw newpw");

    RegisterCommand("addaccount", CConsoleCommands::AddAccount, false,
                    "Usage: addaccount <accountname> <password>\nAdd an account eg: addaccount accountname password");
    RegisterCommand("delaccount", CConsoleCommands::DelAccount, false, "Usage: delaccount <accountname>\nDelete an account eg: delaccount accountname");
    RegisterCommand("chgpass", CConsoleCommands::ChgPass, false,
                    "Usage: chgpass <accountname> <password>\nChange an accounts password eg: chgpass account newpw");
    RegisterCommand("shutdown", CConsoleCommands::Shutdown, false, "Usage: shutdown <reason>\nShutdown the server eg: shutdown put reason here");

    RegisterCommand("aexec", CConsoleCommands::AExec, false,
                    "Usage: aexec <nick> <command>\nForce a player to execute a command eg: aexec playername say hello");

    RegisterCommand("whois", CConsoleCommands::WhoIs, false,
                    "Usage: whois <nick>\nGet the IP of a player currently connected (use whowas for IP/serial/version)");

    RegisterCommand("debugscript", CConsoleCommands::DebugScript, false,
                    "Usage: debugscript <0-3>\nRemove (This does not work 'Incorrect client type for this command')");

    RegisterCommand("help", CConsoleCommands::Help, false, "");

    RegisterCommand("loadmodule", CConsoleCommands::LoadModule, false, "Usage: loadmodule <module-filename>\nLoad a module eg: loadmodule ml_sockets.dll");
    RegisterCommand("unloadmodule", CConsoleCommands::UnloadModule, false,
                    "Usage: unloadmodule <module-filename>\nUnload a module eg: unloadmodule ml_sockets.dll");
    RegisterCommand("reloadmodule", CConsoleCommands::ReloadModule, false,
                    "Usage: reloadmodule <module-filename>\nReload a module eg: reloadmodule ml_sockets.dll");

    RegisterCommand("ver", CConsoleCommands::Ver, false, "Get the MTA version");
    RegisterCommand("sver", CConsoleCommands::Ver, false, "Get the server MTA version");
    RegisterCommand("ase", CConsoleCommands::Ase, false, "See the amount of master server list queries");
    RegisterCommand("openports", CConsoleCommands::OpenPortsTest, false, "Test if server ports are open");

    RegisterCommand("debugdb", CConsoleCommands::SetDbLogLevel, false,
                    "Usage: debugdb <0-2>\nSet logging level for database functions. [0-Off  1-Errors only  2-All]");

    RegisterCommand("reloadbans", CConsoleCommands::ReloadBans, false, "Reloads all the bans from banlist.xml.");

    RegisterCommand("aclrequest", CConsoleCommands::AclRequest, false,
                    "Usage: aclrequest [ list | allow | deny ] <resource-name> [ <right> | all ]\nManage ACL requests from resources implementing <aclrequest> "
                    "in their meta.xml");
    RegisterCommand("authserial", CConsoleCommands::AuthorizeSerial, false,
                    "Usage: authserial <account-name> [list|removelast|httppass]\nManage serial authentication for an account.");
    RegisterCommand("reloadacl", CConsoleCommands::ReloadAcl, false, "Perform a simple ACL reload");
    RegisterCommand("debugjoinflood", CConsoleCommands::DebugJoinFlood, false, "Shows debug information regarding the join flood mitigation feature.");
    RegisterCommand("debuguptime", CConsoleCommands::DebugUpTime, false, "Shows how many days the server has been up and running.");
    RegisterCommand("sfakelag", CConsoleCommands::FakeLag, false,
                    "Usage: sfakelag <packet loss> <extra ping> <ping variance> [<KBPS limit>]\nOnly available if enabled in the mtaserver.conf file.\nAdds "
                    "artificial packet loss, ping, jitter and bandwidth limits to the server-client connections.");
    return true;
}

bool CMainConfig::Save()
{
    // If we have a file
    if (m_pFile && m_pRootNode)
    {
        // Save it
        if (m_pFile->Write())
            return true;
        CLogger::ErrorPrintf("Error saving '%s'\n", GetFileName().c_str());
    }

    // No file
    return false;
}

//
// Compare against default config and add missing nodes.
// Returns true if nodes were added.
//
bool CMainConfig::AddMissingSettings()
{
    // Only mtaserver.conf is currently supported
    if (!g_pGame->IsUsingMtaServerConf())
        return false;

    SString strTemplateFilename = PathJoin(g_pServerInterface->GetServerModPath(), "mtaserver.conf.template");

    if (!FileExists(strTemplateFilename))
        return false;

    CXMLFile* pFileTemplate = g_pServerInterface->GetXML()->CreateXML(strTemplateFilename);
    CXMLNode* pRootNodeTemplate = pFileTemplate && pFileTemplate->Parse() ? pFileTemplate->GetRootNode() : nullptr;
    if (!pRootNodeTemplate)
    {
        CLogger::ErrorPrintf("Can't parse '%s'\n", *strTemplateFilename);
        return false;
    }

    // Check that each item in the template also exists in the server config
    bool      bChanged = false;
    CXMLNode* pPrevNode = nullptr;
    for (auto it = pRootNodeTemplate->ChildrenBegin(); it != pRootNodeTemplate->ChildrenEnd(); ++it)
    {
        CXMLNode* pNodeTemplate = *it;
        SString   strNodeName = pNodeTemplate->GetTagName();
        CXMLNode* pNode = m_pRootNode->FindSubNode(strNodeName);
        if (!pNode)
        {
            CLogger::LogPrintf("Adding missing '%s' to mtaserver.conf\n", *strNodeName);
            SString strNodeValue = pNodeTemplate->GetTagContent();
            SString strNodeComment = pNodeTemplate->GetCommentText();
            pNode = m_pRootNode->CreateSubNode(strNodeName, pPrevNode);
            pNode->SetTagContent(strNodeValue);
            pNode->SetCommentText(strNodeComment, true);
            bChanged = true;
        }
        pPrevNode = pNode;
    }

    // Clean up
    g_pServerInterface->GetXML()->DeleteXML(pFileTemplate);
    FileDelete(strTemplateFilename);
    return bChanged;
}

bool CMainConfig::IsValidPassword(const char* szPassword)
{
    if (!szPassword)
        return false;

    // Test all characters for visibilty
    uint        uiUnsupportedIndex = 0;
    const char* szPtr = szPassword;
    while (*szPtr != 0)
    {
        if (*szPtr == ' ')
        {
            return false;
        }

        ++szPtr;
        ++uiUnsupportedIndex;
    }

    // Bigger than 32 chars?
    return (szPtr - szPassword <= 32);
}

bool CMainConfig::SetPassword(const char* szPassword, bool bSave)
{
    if (!IsValidPassword(szPassword))
        return false;

    m_strPassword = szPassword;

    if (bSave)
    {
        SetString(m_pRootNode, "password", szPassword);
        Save();
    }

    return true;
}

bool CMainConfig::SetFPSLimit(unsigned short usFPS, bool bSave)
{
    if (usFPS == 0 || (usFPS >= 25 && usFPS <= std::numeric_limits<short>::max()))
    {
        m_usFPSLimit = usFPS;
        if (bSave)
        {
            SetInteger(m_pRootNode, "fpslimit", usFPS);
            Save();
        }
        return true;
    }
    return false;
}

void CMainConfig::RegisterCommand(const char* szName, FCommandHandler* pFunction, bool bRestricted, const char* szConsoleHelpText)
{
    // Register the function with the given name and function pointer
    m_pConsole->AddCommand(pFunction, szName, bRestricted, szConsoleHelpText);
}

void CMainConfig::SetCommandLineParser(CCommandLineParser* pCommandLineParser)
{
    m_pCommandLineParser = pCommandLineParser;

    // Adjust max player limits for command line arguments
    uint uiMaxPlayers;
    if (m_pCommandLineParser && m_pCommandLineParser->GetMaxPlayers(uiMaxPlayers))
    {
        m_uiHardMaxPlayers = Clamp<uint>(1, uiMaxPlayers, MAX_PLAYER_COUNT);
        m_uiSoftMaxPlayers = uiMaxPlayers;
    }
}

SString CMainConfig::GetServerIP()
{
    std::string strServerIP;
    if (m_pCommandLineParser && m_pCommandLineParser->GetIP(strServerIP))
        return strServerIP;
    return SString(m_strServerIP).SplitLeft(",");
}

SString CMainConfig::GetServerIPList()
{
    std::string strServerIP;
    if (m_pCommandLineParser && m_pCommandLineParser->GetIP(strServerIP))
        return strServerIP;
    return m_strServerIP;
}

unsigned short CMainConfig::GetServerPort()
{
    unsigned short usPort;
    if (m_pCommandLineParser && m_pCommandLineParser->GetPort(usPort))
        return usPort;
    return m_usServerPort;
}

unsigned int CMainConfig::GetHardMaxPlayers()
{
    return m_uiHardMaxPlayers;
}

unsigned int CMainConfig::GetMaxPlayers()
{
    return std::min(GetHardMaxPlayers(), m_uiSoftMaxPlayers);
}

unsigned short CMainConfig::GetHTTPPort()
{
    unsigned short usHTTPPort;
    if (m_pCommandLineParser && m_pCommandLineParser->GetHTTPPort(usHTTPPort))
        return usHTTPPort;
    return m_usHTTPPort;
}

bool CMainConfig::IsVoiceEnabled()
{
    bool bDisabled;
    if (m_pCommandLineParser && m_pCommandLineParser->IsVoiceDisabled(bDisabled))
        return !bDisabled;
    return m_bVoiceEnabled;
}

int CMainConfig::GetPendingWorkToDoSleepTime()
{
    if (m_iPendingWorkToDoSleepTime != -1)
    {
        if (m_bThreadNetEnabled)
            return std::max(0, m_iPendingWorkToDoSleepTime);
        else
            return m_iPendingWorkToDoSleepTime;
    }

    // -1 means auto
    if (m_bThreadNetEnabled)
        return 20;
    else
        return 10;
}

int CMainConfig::GetNoWorkToDoSleepTime()
{
    if (m_iNoWorkToDoSleepTime != -1)
    {
        return std::max(10, m_iNoWorkToDoSleepTime);
    }

    // -1 means auto
    if (m_bThreadNetEnabled)
        return 40;
    else
        return 10;
}

void CMainConfig::NotifyDidBackup()
{
    m_bDidBackup = true;
}

bool CMainConfig::ShouldCompactInternalDatabases()
{
    return (m_iCompactInternalDatabases == 1 && m_bDidBackup) || m_iCompactInternalDatabases == 2;
}

//////////////////////////////////////////////////////////////////////
//
// Fetch multiple values for a named setting from the server config
//
//  <module src="module_test.dll" />
//  <resource src="admin" startup="1" protected="0" />
//
//////////////////////////////////////////////////////////////////////
bool CMainConfig::GetSettingTable(const SString& strName, CLuaArguments* outTable)
{
    if (strName == "module")
    {
        static const char* szAttribNames[] = {"src"};
        return GetSettingTable(strName, szAttribNames, NUMELMS(szAttribNames), outTable);
    }
    else if (strName == "resource")
    {
        static const char* szAttribNames[] = {"src", "startup", "protected", "default"};
        return GetSettingTable(strName, szAttribNames, NUMELMS(szAttribNames), outTable);
    }

    return false;
}

//////////////////////////////////////////////////////////////////////
//
// Fetch multiple values for a named setting from the server config
//
//  <module src="module_test.dll" />
//  <resource src="admin" startup="1" protected="0" />
//
//////////////////////////////////////////////////////////////////////
bool CMainConfig::GetSettingTable(const SString& strName, const char** szAttribNames, uint uiNumAttribNames, CLuaArguments* outTable)
{
    uint      uiXMLIndex = 0;
    uint      uiLuaIndex = 1;
    CXMLNode* pNode = NULL;
    do
    {
        // Grab the current script node
        pNode = m_pRootNode->FindSubNode(strName, uiXMLIndex++);
        if (pNode)
        {
            CLuaArguments   resultLine;
            CXMLAttributes& attributes = pNode->GetAttributes();
            for (uint i = 0; i < attributes.Count(); i++)
            {
                CXMLAttribute* pAttribute = attributes.Get(i);
                resultLine.PushString(pAttribute->GetName());
                resultLine.PushString(pAttribute->GetValue());
            }

            if (resultLine.IsNotEmpty())
            {
                outTable->PushNumber(uiLuaIndex++);
                outTable->PushTable(&resultLine);
            }
        }
    } while (pNode);

    return outTable->IsNotEmpty();
}

//////////////////////////////////////////////////////////////////////
//
// Fetch any single setting from the server config
//
//////////////////////////////////////////////////////////////////////
SString CMainConfig::GetSetting(const SString& strName)
{
    SString strValue;
    GetSetting(strName, strValue);
    return strValue;
}

//////////////////////////////////////////////////////////////////////
//
// Fetch any single setting from the server config
//
//////////////////////////////////////////////////////////////////////
bool CMainConfig::GetSetting(const SString& strName, SString& strValue)
{
    //
    // Fetch settings that may differ from the XML data
    //
    if (strName == "minclientversion")
    {
        strValue = m_strMinClientVersion;
        return true;
    }
    else if (strName == "recommendedclientversion")
    {
        strValue = m_strRecommendedClientVersion;
        return true;
    }
    else if (strName == "password")
    {
        strValue = GetPassword();
        return true;
    }
    else if (strName == "fpslimit")
    {
        strValue = SString("%d", GetFPSLimit());
        return true;
    }
    else if (strName == "networkencryption")
    {
        // Deprecated
        strValue = "1";
        return true;
    }
    else if (strName == "bandwidth_reduction")
    {
        strValue = m_strBandwidthReductionMode;
        return true;
    }
    else if (strName == "busy_sleep_time")
    {
        strValue = SString("%d", m_iPendingWorkToDoSleepTime);
        return true;
    }
    else if (strName == "idle_sleep_time")
    {
        strValue = SString("%d", m_iNoWorkToDoSleepTime);
        return true;
    }
    else if (strName == "verifyclientsettings")
    {
        strValue = SString("%d", m_iEnableClientChecks);
        return true;
    }
    else if (strName == "threadnet")
    {
        strValue = SString("%d", m_bThreadNetEnabled ? 1 : 0);
        return true;
    }
    else if (strName == "module" || strName == "resource")
    {
        return false;
    }
    else if (strName == "enable_diagnostic")
    {
        for (std::set<SString>::iterator iter = m_EnableDiagnosticMap.begin(); iter != m_EnableDiagnosticMap.end(); ++iter)
            strValue += *iter + ",";
        strValue = strValue.TrimEnd(",");
        return true;
    }
    else
    {
        // Check settings in this list here
        const std::vector<SIntSetting>& settingList = GetIntSettingList();
        for (uint i = 0; i < settingList.size(); i++)
        {
            const SIntSetting& item = settingList[i];
            if (strName == item.szName)
            {
                strValue = SString("%d", *item.pVariable);
                return true;
            }
        }

        // Check transient settings
        if (SString* pstrValue = MapFind(m_TransientSettings, strName))
        {
            strValue = *pstrValue;
            return true;
        }

        //
        // Everything else is read only, so can be fetched directly from the XML data
        //
        if (GetString(m_pRootNode, strName, strValue))
            return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////
//
// Put certain settings to the server config
//
//////////////////////////////////////////////////////////////////////
bool CMainConfig::SetSetting(const SString& strName, const SString& strValue, bool bSave)
{
    if (strName == "minclientversion")
    {
        if (strValue == "" || IsValidVersionString(strValue))
        {
            m_strMinClientVersion = strValue;
            if (bSave)
            {
                SetString(m_pRootNode, "minclientversion", m_strMinClientVersion);
                Save();
            }
            return true;
        }
    }
    else if (strName == "recommendedclientversion")
    {
        if (strValue == "" || IsValidVersionString(strValue))
        {
            m_strRecommendedClientVersion = strValue;
            if (bSave)
            {
                SetString(m_pRootNode, "recommendedclientversion", m_strRecommendedClientVersion);
                Save();
            }
            return true;
        }
    }
    else if (strName == "password")
    {
        return CStaticFunctionDefinitions::SetServerPassword(strValue, bSave);
    }
    else if (strName == "fpslimit")
    {
        return CStaticFunctionDefinitions::SetFPSLimit(atoi(strValue), bSave);
    }
    else if (strName == "networkencryption")
    {
        if (strValue == "0" || strValue == "1")
        {
            // Deprecated
            return true;
        }
    }
    else if (strName == "bandwidth_reduction")
    {
        if (strValue == "none" || strValue == "medium" || strValue == "maximum")
        {
            m_strBandwidthReductionMode = strValue;
            ApplyBandwidthReductionMode();
            if (bSave)
            {
                SetString(m_pRootNode, "bandwidth_reduction", m_strBandwidthReductionMode);
                Save();
            }
            return true;
        }
    }
    else if (strName == "busy_sleep_time")
    {
        int iSleepMs = atoi(strValue);
        if (iSleepMs >= -1 && iSleepMs <= 50)
        {
            m_iPendingWorkToDoSleepTime = iSleepMs;
            if (bSave)
            {
                SetString(m_pRootNode, "busy_sleep_time", SString("%d", m_iPendingWorkToDoSleepTime));
                Save();
            }
            return true;
        }
    }
    else if (strName == "idle_sleep_time")
    {
        int iSleepMs = atoi(strValue);
        if (iSleepMs >= -1 && iSleepMs <= 50)
        {
            m_iNoWorkToDoSleepTime = iSleepMs;
            if (bSave)
            {
                SetString(m_pRootNode, "idle_sleep_time", SString("%d", m_iNoWorkToDoSleepTime));
                Save();
            }
            return true;
        }
    }
    else if (strName == "threadnet")
    {
        if (strValue == "0" || strValue == "1")
        {
            m_bThreadNetEnabled = atoi(strValue) ? true : false;
            ApplyThreadNetEnabled();
            return true;
        }
    }
    else if (strName == "lightsync")
    {
        if (strValue == "0" || strValue == "1")
        {
            // Transient settings go in their own map, so they don't get saved
            MapSet(m_TransientSettings, "lightsync", strValue);
            g_pBandwidthSettings->bLightSyncEnabled = atoi(strValue) ? true : false;
            return true;
        }
    }
    else if (strName == "enablesd")
    {
        // 'enablesd' can only be added to, and then server has to be restarted

        // Get current setting as list of ids
        SString strCurSD;
        GetSetting("enablesd", strCurSD);
        std::vector<SString> curSDList;
        ReadCommaSeparatedList(strCurSD, curSDList);

        // Get new setting as as list of ids
        std::vector<SString> newSDList;
        ReadCommaSeparatedList(strValue, newSDList);

        // Merge
        std::set<uint> comboSDMap;
        for (std::vector<SString>::iterator it = curSDList.begin(); it != curSDList.end(); ++it)
            MapInsert(comboSDMap, atoi(**it));
        for (std::vector<SString>::iterator it = newSDList.begin(); it != newSDList.end(); ++it)
            MapInsert(comboSDMap, atoi(**it));

        // Make a string
        SString strComboResult;
        for (std::set<uint>::iterator it = comboSDMap.begin(); it != comboSDMap.end(); ++it)
        {
            uint uiId = *it;
            if (uiId)
            {
                if (!strComboResult.empty())
                    strComboResult += ",";
                strComboResult += SString("%d", uiId);
            }
        }

        // Save new setting
        SetString(m_pRootNode, "enablesd", strComboResult);
        Save();
        return true;
    }
    else if (strName == "enable_diagnostic")
    {
        if (true)
        {
            std::vector<SString> tagList;
            ReadCommaSeparatedList(strValue, tagList);
            m_EnableDiagnosticMap = std::set<SString>(tagList.begin(), tagList.end());

            if (bSave)
            {
                SetString(m_pRootNode, "enable_diagnostic", SString::Join(",", tagList));
                Save();
            }
            return true;
        }
    }

    // Check settings in this list here
    const std::vector<SIntSetting>& settingList = GetIntSettingList();
    for (uint i = 0; i < settingList.size(); i++)
    {
        const SIntSetting& item = settingList[i];
        if (item.bSettable && strName == item.szName)
        {
            int iValue = atoi(strValue);
            if (iValue >= item.iMin && iValue <= item.iMax)
            {
                *item.pVariable = iValue;
                if (item.bSavable && bSave)
                {
                    SetString(m_pRootNode, item.szName, SString("%d", *item.pVariable));
                    Save();
                }

                if (item.changeCallback)
                    (this->*item.changeCallback)();

                return true;
            }
        }
    }

    //
    // Everything else is read only, so can't be set
    //
    return false;
}

//////////////////////////////////////////////////////////////////////
//
// Put some int settings into an array for referencing
//
//////////////////////////////////////////////////////////////////////
const std::vector<SIntSetting>& CMainConfig::GetIntSettingList()
{
    static const SIntSetting settings[] = {
        // Set,  save,   min,    def,    max,    name,                                   variable,                                   callback
        {true, true, 50, 100, 4000, "player_sync_interval", &g_TickRateSettings.iPureSync, &CMainConfig::OnTickRateChange},
        {true, true, 50, 1500, 4000, "lightweight_sync_interval", &g_TickRateSettings.iLightSync, &CMainConfig::OnTickRateChange},
        {true, true, 50, 500, 4000, "camera_sync_interval", &g_TickRateSettings.iCamSync, &CMainConfig::OnTickRateChange},
        {true, true, 50, 400, 4000, "ped_sync_interval", &g_TickRateSettings.iPedSync, &CMainConfig::OnTickRateChange},
        {true, true, 50, 2000, 4000, "ped_far_sync_interval", &g_TickRateSettings.iPedFarSync, NULL},
        {true, true, 50, 400, 4000, "unoccupied_vehicle_sync_interval", &g_TickRateSettings.iUnoccupiedVehicle, &CMainConfig::OnTickRateChange},
        {true, true, 50, 100, 4000, "keysync_mouse_sync_interval", &g_TickRateSettings.iKeySyncRotation, &CMainConfig::OnTickRateChange},
        {true, true, 50, 100, 4000, "keysync_analog_sync_interval", &g_TickRateSettings.iKeySyncAnalogMove, &CMainConfig::OnTickRateChange},
        {true, true, 50, 100, 4000, "donkey_work_interval", &g_TickRateSettings.iNearListUpdate, &CMainConfig::OnTickRateChange},
        {true, true, 0, 0, 1, "bullet_sync", &m_bBulletSyncEnabled, &CMainConfig::OnTickRateChange},
        {true, true, 0, 0, 120, "vehext_percent", &m_iVehExtrapolatePercent, &CMainConfig::OnTickRateChange},
        {true, true, 0, 150, 500, "vehext_ping_limit", &m_iVehExtrapolatePingLimit, &CMainConfig::OnTickRateChange},
        {true, true, 0, 0, 1, "latency_reduction", &m_bUseAltPulseOrder, &CMainConfig::OnTickRateChange},
        {true, true, 0, 1, 2, "ase", &m_iAseMode, &CMainConfig::OnAseSettingChange},
        {true, true, 0, 0, 1, "donotbroadcastlan", &m_bDontBroadcastLan, &CMainConfig::OnAseSettingChange},
        {true, true, 0, 1, 1, "net_auto_filter", &m_bNetAutoFilter, &CMainConfig::ApplyNetOptions},
        {true, true, 1, 4, 100, "update_cycle_datagrams_limit", &m_iUpdateCycleDatagramsLimit, &CMainConfig::ApplyNetOptions},
        {true, true, 10, 50, 1000, "update_cycle_messages_limit", &m_iUpdateCycleMessagesLimit, &CMainConfig::ApplyNetOptions},
        {true, true, 50, 100, 400, "ped_syncer_distance", &g_TickRateSettings.iPedSyncerDistance, &CMainConfig::OnTickRateChange},
        {true, true, 50, 130, 400, "unoccupied_vehicle_syncer_distance", &g_TickRateSettings.iUnoccupiedVehicleSyncerDistance, &CMainConfig::OnTickRateChange},
        {true, true, 0, 30, 130, "vehicle_contact_sync_radius", &g_TickRateSettings.iVehicleContactSyncRadius, &CMainConfig::OnTickRateChange},
        {false, false, 0, 1, 2, "compact_internal_databases", &m_iCompactInternalDatabases, NULL},
        {true, true, 0, 1, 2, "minclientversion_auto_update", &m_iMinClientVersionAutoUpdate, NULL},
        {true, true, 0, 0, 100, "server_logic_fps_limit", &m_iServerLogicFpsLimit, NULL},
        {true, true, 0, 1, 1, "crash_dump_upload", &m_bCrashDumpUploadEnabled, NULL},
        {true, true, 0, 1, 1, "filter_duplicate_log_lines", &m_bFilterDuplicateLogLinesEnabled, NULL},
        {false, false, 0, 1, 1, "database_credentials_protection", &m_bDatabaseCredentialsProtectionEnabled, NULL},
        {false, false, 0, 0, 1, "fakelag", &m_bFakeLagCommandEnabled, NULL},
        {true, true, 50, 1000, 5000, "player_triggered_event_interval", &m_iPlayerTriggeredEventIntervalMs, &CMainConfig::OnPlayerTriggeredEventIntervalChange},
        {true, true, 1, 100, 1000, "max_player_triggered_events_per_interval", &m_iMaxPlayerTriggeredEventsPerInterval, &CMainConfig::OnPlayerTriggeredEventIntervalChange},
    };

    static std::vector<SIntSetting> settingsList;

    if (settingsList.empty())
        for (uint i = 0; i < NUMELMS(settings); i++)
            settingsList.push_back(settings[i]);

    return settingsList;
}

//////////////////////////////////////////////////////////////////////
//
// Settings change callback
//
//////////////////////////////////////////////////////////////////////
void CMainConfig::OnTickRateChange()
{
    CStaticFunctionDefinitions::SendSyncIntervals();
    g_pGame->SendSyncSettings();
    g_pGame->CalculateMinClientRequirement();
}

void CMainConfig::OnAseSettingChange()
{
    g_pGame->ApplyAseSetting();
}

void CGame::ApplyAseSetting()
{
    if (!m_pMainConfig->GetAseLanListenEnabled())
        SAFE_DELETE(m_pLanBroadcast);

    bool bInternetEnabled = m_pMainConfig->GetAseInternetListenEnabled();
    bool bLanEnabled = m_pMainConfig->GetAseLanListenEnabled();
    m_pASE->SetPortEnabled(bInternetEnabled, bLanEnabled);

    if (m_pMainConfig->GetAseLanListenEnabled())
    {
        if (!m_pLanBroadcast)
            m_pLanBroadcast = m_pASE->InitLan();
    }
}

void CMainConfig::OnPlayerTriggeredEventIntervalChange()
{
    g_pGame->ApplyPlayerTriggeredEventIntervalChange();
}

void CGame::ApplyPlayerTriggeredEventIntervalChange()
{
    m_iClientTriggeredEventsIntervalMs = m_pMainConfig->GetPlayerTriggeredEventInterval();
    m_iMaxClientTriggeredEventsPerInterval = m_pMainConfig->GetMaxPlayerTriggeredEventsPerInterval();
}
