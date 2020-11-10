/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBanManager.cpp
 *  PURPOSE:     Ban manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
bool CBanManager::ms_bSaveRequired = false;

CBanManager::CBanManager() :
    m_strBanListXMLPath(g_pServerInterface->GetModManager()->GetAbsolutePath(FILENAME_BANLIST)),
    m_NextUpdateTime(0),
    m_bAllowSave(false)
{
}

CBanManager::~CBanManager()
{
    SaveBanList();
}

void CBanManager::DoPulse()
{
    for (auto iter = m_Bans.begin(); iter != m_Bans.end(); iter++)
        if (iter->IsBeingDeleted()) // Should we delete it now?
            iter = m_Bans.erase(iter); // Okayyy

    // Perhaps process unbans?
    time_t timeNow = time(NULL);
    if (timeNow > m_NextUpdateTime)
    {
        for (auto iter = m_Bans.begin(); iter != m_Bans.end(); iter++)
        {
            auto& ban = *iter;
            const auto tunban = ban.GetTimeOfUnban();

            // Banned forever?
            if (tunban <= 0)
                continue;

            // Has it expired?
            if (timeNow < ban.GetTimeOfUnban())
                continue;

            CLuaArguments Arguments;
            Arguments.PushBan(&ban);
            g_pGame->GetMapManager()->GetRootElement()->CallEvent("onUnban", Arguments);

            iter = m_Bans.erase(iter); // Make sure nothing blows up..
        }

        m_NextUpdateTime = timeNow + 1;
    }

    // And now, maybe save it all :D
    if (ms_bSaveRequired)
        SaveBanList();
}

CBan* CBanManager::AddBan(CPlayer* pPlayer, const SString& strBanner, const SString& strReason, time_t tTimeOfUnban)
{
    if (pPlayer)
    {
        SString strIP = pPlayer->GetSourceIP();

        if (IsValidIP(strIP) && !IsSpecificallyBanned(strIP))
        {
            CBan* pBan = AddBan(strBanner, strReason, tTimeOfUnban);
            pBan->SetNick(pPlayer->GetNick());
            pBan->SetIP(strIP);
            return pBan;
        }
    }
    return NULL;
}

CBan* CBanManager::AddBan(const SString& strIP, const SString& strBanner, const SString& strReason, time_t tTimeOfUnban)
{
    if (IsValidIP(strIP.c_str()) && !IsSpecificallyBanned(strIP.c_str()))
    {
        CBan* pBan = AddBan(strBanner, strReason, tTimeOfUnban);
        pBan->SetIP(strIP);
        return pBan;
    }

    return NULL;
}

CBan* CBanManager::AddSerialBan(CPlayer* pPlayer, CClient* pBanner, const SString& strReason, time_t tTimeOfUnban)
{
    if (pPlayer)
    {
        if (!pPlayer->GetSerial().empty() && !IsSerialBanned(pPlayer->GetSerial().c_str()))
        {
            CBan* pBan = AddBan(pBanner->GetNick(), strReason, tTimeOfUnban);
            pBan->SetNick(pPlayer->GetNick());
            pBan->SetSerial(pPlayer->GetSerial());
            return pBan;
        }
    }

    return NULL;
}

CBan* CBanManager::AddSerialBan(const SString& strSerial, CClient* pBanner, const SString& strReason, time_t tTimeOfUnban)
{
    if (/*IsValidSerial ( szSerial ) &&*/ !IsSerialBanned(strSerial.c_str()))
    {
        CBan* pBan = AddBan(pBanner->GetNick(), strReason, tTimeOfUnban);
        pBan->SetSerial(strSerial);
        return pBan;
    }

    return NULL;
}

CBan* CBanManager::AddAccountBan(CPlayer* pPlayer, CClient* pBanner, const SString& strReason, time_t tTimeOfUnban)
{
    if (pPlayer)
    {
        if (!pPlayer->GetSerialUser().empty() && !IsAccountBanned(pPlayer->GetSerialUser().c_str()))
        {
            CBan* pBan = AddBan(pBanner->GetNick(), strReason, tTimeOfUnban);
            pBan->SetNick(pPlayer->GetNick());
            pBan->SetAccount(pPlayer->GetSerialUser());
            return pBan;
        }
    }

    return NULL;
}

CBan* CBanManager::AddAccountBan(const SString& strAccount, CClient* pBanner, const SString& strReason, time_t tTimeOfUnban)
{
    if (!IsAccountBanned(strAccount.c_str()))
    {
        CBan* pBan = AddBan(pBanner->GetNick(), strReason, tTimeOfUnban);
        pBan->SetSerial(strAccount);
        return pBan;
    }

    return NULL;
}

CBan* CBanManager::AddBan(const SString& strBanner, const SString& strReason, time_t tTimeOfUnban)
{
    // Create the ban and assign its values
    CBan& ban = m_Bans.emplace_back();

    ban.SetTimeOfBan(time(NULL));
    ban.SetTimeOfUnban(tTimeOfUnban);
    ban.SetReason(strReason);
    ban.SetBanner(strBanner);

    return &ban;
}

bool CBanManager::IsSerialBanned(std::string_view serial)
{
    return (bool)GetBanFromSerial(serial);
}

bool CBanManager::IsAccountBanned(std::string_view account)
{
    return (bool)GetBanFromAccount(account);
}

bool CBanManager::IsSpecificallyBanned(std::string_view IP)
{
    return FindIf([=](const CBan& ban) {
        return ban.GetIP() == IP; });
}

// Include wildcard checks
CBan* CBanManager::GetBanFromIP(std::string_view ip)
{
    return FindIf([=](const CBan& ban) {
        const auto& bannedIP = ban.GetIP();
        if (bannedIP.find("*") != std::string::npos) // Wildcard ban?
        {
            for (uint i = 0; i < 17; i++)
            {
                char a = ip[i];
                char b = bannedIP[i];
                if (a == b)
                {
                    if (a == 0)
                        return true;            // Full match
                }
                else
                    return b == '*';
            }
        }
        else
            return bannedIP == ip;
        return false;
    });
}

CBan* CBanManager::GetBanFromScriptID(uint uiScriptID)
{
    CBan* pBan = (CBan*)CIdArray::FindEntry(uiScriptID, EIdClass::BAN);
    dassert(!pBan || ListContains(m_Bans, pBan));
    return pBan;
}

CBan* CBanManager::GetBanFromAccount(std::string_view account)
{
    return FindIf([=](const CBan& ban) {
        return ban.GetAccount() == account; });
}

CBan* CBanManager::GetBanFromSerial(std::string_view serial)
{
    return FindIf([=](const CBan& ban) {
        return ban.GetSerial() == serial; });
}

unsigned int CBanManager::GetBansWithNick(std::string_view nick)
{
    return CountIf([=](const CBan& ban) {
        return ban.GetNick() == nick; });
}

unsigned int CBanManager::GetBansWithBanner(std::string_view banner)
{
    return CountIf([=](const CBan& ban) {
        return ban.GetBanner() == banner; });
}

bool CBanManager::LoadBanList()
{
    m_bAllowSave = true;

    // Create the XML
    std::unique_ptr<CXMLFile> pFile(g_pServerInterface->GetXML()->CreateXML(m_strBanListXMLPath));
    if (!pFile)
    {
        CLogger::ErrorPrintf("Banlist: load error: failed creating XML. Check if the file exists.\n");
        return false;
    }

    // Parse it
    if (!pFile->Parse())
    {
        if (FileExists(m_strBanListXMLPath))
            CLogger::ErrorPrintf("Banlist: load error: Error parsing\n");
        return false;
    }

    // Grab the XML root node
    CXMLNode* pRootNode = pFile->GetRootNode();
    if (!pRootNode)
        pRootNode = pFile->CreateRootNode("banlist");

    // Is the rootnode's name <banlist>?
    if (pRootNode->GetTagName() == std::string_view{ "banlist" })
    {
        CLogger::ErrorPrintf("Banlist: load error: Wrong root node name (Got: '%s', expected 'banlist')\n", pRootNode->GetTagName().c_str());
        return false;
    }

    const auto timeNow = time(NULL);
    for (auto it = pRootNode->ChildrenBegin(); it != pRootNode->ChildrenEnd(); it++)
    {
        CXMLNode* pNode = *it;

        // Helper function
        auto SafeGetValue = [pNode](const char* attrName) mutable {
            if (CXMLAttribute* pAttribute = pNode->GetAttributes().Find(attrName))
                return pAttribute->GetValue();
            return std::string{};
        };

        const std::string& ip = SafeGetValue("ip");
        const std::string& serial = SafeGetValue("serial");
        const std::string& account = SafeGetValue("account");

        if (ip.empty() && serial.empty() && account.empty())
            continue;

        CBan& ban = m_Bans.emplace_back();

        if (IsValidIP(ip.c_str()))
            ban.SetIP(ip);
        ban.SetAccount(account);
        ban.SetSerial(serial);
        ban.SetBanner(SafeGetValue("banner"));
        ban.SetNick(SafeGetValue("nick"));
        ban.SetReason(SafeGetValue("reason"));

        // Try to read and set ban time and unban time
        try
        {
            auto ReadAndSet = [&](const char* what, auto setter) mutable {
                if (const std::string& time = SafeGetValue(what); !time.empty())
                    setter((time_t)std::stoll(time));
                else // Empty
                    setter(timeNow);
            };
            ReadAndSet("time", [&](time_t value) mutable { ban.SetTimeOfBan(value); });
            ReadAndSet("unban", [&](time_t value) mutable { ban.SetTimeOfBan(value); });
        }
        catch (const std::exception& e)
        {
            CLogger::ErrorPrintf("Banlist: load error: failed parsing attribute `time` or `unban`. Error: %s", e.what());
        }
    }

    ms_bSaveRequired = false;
    return true;
}

bool CBanManager::ReloadBanList()
{
    // Flush any pending saves - This is ok because reloadbans is for loading manual changes to banlist.xml
    // and manual changes are subject to being overwritten by server actions at any time.
    if (ms_bSaveRequired)
        SaveBanList();

    for (CBan& ban : m_Bans)
        ban.SetBeingDeleted(); // Set all bans as being deleted (Will get deleted in DoPulse)

    return LoadBanList();
}

void CBanManager::SaveBanList()
{
    // Only allow save after a load was attempted
    if (!m_bAllowSave)
        return;

    ms_bSaveRequired = false;

    // Create the XML
    std::unique_ptr<CXMLFile> pFile(g_pServerInterface->GetXML()->CreateXML(m_strBanListXMLPath));
    if (pFile)
    {
        CLogger::ErrorPrintf("Banlist: save error: failed creating XML. Check if the file exists.\n");
        return;
    }

    // create the root node again as you are outputting all the bans again not just new ones
    CXMLNode* pRootNode = pFile->CreateRootNode("banlist");
    if (!pRootNode)
    {
        CLogger::ErrorPrintf("Banlist: save error: failed creating rootnode\n");
        return;
    }

    // Iterate the ban list adding it to the XML tree
    for (CBan& ban : m_Bans)
    {
        CXMLNode* pNode = pRootNode->CreateSubNode("ban");
        if (!pNode)
            continue;

        // Helper functions
        auto CreateAttrAndSet = [pNode](const char* attrName, const auto& value) mutable {
            CXMLAttribute* pAttribute = pNode->GetAttributes().Create(attrName);
            if (pAttribute)
                pAttribute->SetValue(value);
        };

        auto SafeSetValueString = [&](const char* attrName, const std::string& value) {
            if (!value.empty())
                CreateAttrAndSet(attrName, value.c_str());
        };

        auto SafeSetValueInt = [&](const char* attrName, unsigned int value) {
            if (value)
                CreateAttrAndSet(attrName, value);
        };

        SafeSetValueString("nick", ban.GetNick());
        SafeSetValueString("ip", ban.GetIP());
        SafeSetValueString("serial", ban.GetSerial());
        SafeSetValueString("account", ban.GetAccount());
        SafeSetValueString("banner", ban.GetBanner());
        SafeSetValueString("reason", ban.GetReason());

        // These two will work fine until 03:14:07 on Tuesday, 19 January 2038
        SafeSetValueInt("time", (unsigned int)ban.GetTimeOfBan());
        SafeSetValueInt("unban", (unsigned int)ban.GetTimeOfUnban());
    }

    // Write the XML file
    if (!pFile->Write())
        CLogger::ErrorPrintf("Banlist: save error: failed saving '%s'\n", FILENAME_BANLIST);

    // Loading the ban list into memory modifies this. We need to set it to false again
    ms_bSaveRequired = false;
}

bool CBanManager::IsValidIP(const char* szIP)
{
    char strIP[256] = {'\0'};
    strncpy(strIP, szIP, 255);
    strIP[255] = '\0';

    char* szIP1 = strtok(strIP, ".");
    char* szIP2 = strtok(NULL, ".");
    char* szIP3 = strtok(NULL, ".");
    char* szIP4 = strtok(NULL, "\r");

    if (szIP1 && szIP2 && szIP3 && szIP4)
    {
        if (IsValidIPPart(szIP1) && IsValidIPPart(szIP2) && IsValidIPPart(szIP3) && IsValidIPPart(szIP4))
            return true;
    }

    return false;
}

bool CBanManager::IsValidIPPart(const char* szIP)
{
    if (IsNumericString(szIP))
    {
        int iIP = atoi(szIP);
        if (iIP >= 0 && iIP < 256)
            return true;
    }
    else if (strcmp(szIP, "*") == 0)
        return true;

    return false;
}
