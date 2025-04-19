/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBanManager.h
 *  PURPOSE:     Ban manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClient.h"
#include "CPlayerManager.h"

class CBanManager
{
public:
    CBanManager();
    virtual ~CBanManager();

    void DoPulse();

    CBan* AddBan(CPlayer* pPlayer, const SString& strBanner = "Console", const SString& strReason = "", time_t tTimeOfUnban = 0);
    CBan* AddBan(const SString& strIP, const SString& strBanner = "Console", const SString& strReason = "", time_t tTimeOfUnban = 0);

    CBan* AddSerialBan(CPlayer* pPlayer, CClient* pBanner = NULL, const SString& strReason = "", time_t tTimeOfUnban = 0);
    CBan* AddSerialBan(const SString& strSerial, CClient* pBanner = NULL, const SString& strReason = "", time_t tTimeOfUnban = 0);

    CBan* AddAccountBan(CPlayer* pPlayer, CClient* pBanner = NULL, const SString& strReason = "", time_t tTimeOfUnban = 0);
    CBan* AddAccountBan(const SString& szAccount, CClient* pBanner = NULL, const SString& strReason = "", time_t tTimeOfUnban = 0);

    CBan* AddBan(const SString& strBanner = "Console", const SString& strReason = "", time_t tTimeOfUnban = 0);

    CBan* GetBanFromScriptID(uint uiScriptID);

    bool  IsSpecificallyBanned(const char* szIP);
    bool  IsSerialBanned(const char* szSerial);
    bool  IsAccountBanned(const char* szAccount);
    CBan* GetBanFromAccount(const char* szAccount);
    void  RemoveBan(CBan* pBan);

    CBan* GetBanFromSerial(const char* szSerial);
    CBan* GetBanFromIP(const char* szIP);

    unsigned int GetBansWithNick(const char* szNick);
    unsigned int GetBansWithBanner(const char* szBanner);

    bool        LoadBanList();
    bool        ReloadBanList();
    void        SaveBanList();
    void        SafeSetValue(CXMLNode* pNode, const char* szKey, const std::string& strValue);
    void        SafeSetValue(CXMLNode* pNode, const char* szKey, unsigned int);
    std::string SafeGetValue(CXMLNode* pNode, const char* szKey);
    bool        IsValidIP(const char* szIP);
    static void SetBansModified() { ms_bSaveRequired = true; }

    list<CBan*>::const_iterator IterBegin() { return m_BanManager.begin(); };
    list<CBan*>::const_iterator IterEnd() { return m_BanManager.end(); };

private:
    SString m_strPath;

    CMappedList<CBan*> m_BanManager;
    std::set<CBan*>    m_BansBeingDeleted;

    time_t m_tUpdate;

    bool        IsValidIPPart(const char* szIP);
    bool        m_bAllowSave;
    static bool ms_bSaveRequired;
};
