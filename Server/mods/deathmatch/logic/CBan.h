/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBan.h
 *  PURPOSE:     Ban descriptor class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CCommon.h"
#include "CBanManager.h"
#include <string>

class CBan
{
public:
    CBan();
    ~CBan();

    const std::string& GetIP() { return m_strIP; };
    void               SetIP(const std::string& strIP)
    {
        CBanManager::SetBansModified();
        m_strIP = strIP;
    };

    const std::string& GetNick() { return m_strNick; };
    void               SetNick(const std::string& strNick)
    {
        CBanManager::SetBansModified();
        m_strNick = strNick;
    };

    const std::string& GetBanner() { return m_strBanner; };
    void               SetBanner(const std::string& strBanner)
    {
        CBanManager::SetBansModified();
        m_strBanner = strBanner;
    };

    const std::string& GetReason() { return m_strReason; };
    void               SetReason(const std::string& strReason)
    {
        CBanManager::SetBansModified();
        m_strReason = strReason;
    };

    const time_t GetTimeOfBan() { return m_tTimeOfBan; };
    void         SetTimeOfBan(time_t tTimeOfBan)
    {
        CBanManager::SetBansModified();
        m_tTimeOfBan = tTimeOfBan;
    };

    time_t GetTimeOfUnban() { return m_tTimeOfUnban; };
    void   SetTimeOfUnban(time_t tTimeOfUnban)
    {
        CBanManager::SetBansModified();
        m_tTimeOfUnban = tTimeOfUnban;
    };

    const std::string& GetSerial() { return m_strSerial; };
    void               SetSerial(const std::string& strSerial)
    {
        CBanManager::SetBansModified();
        m_strSerial = strSerial;
    };

    const std::string& GetAccount() { return m_strAccount; };
    void               SetAccount(const std::string& strAccount)
    {
        CBanManager::SetBansModified();
        m_strAccount = strAccount;
    };

    time_t  GetBanTimeRemaining();
    SString GetDurationDesc();
    SString GetReasonText() const;
    uint    GetScriptID() const { return m_uiScriptID; }
    bool    IsBeingDeleted() const { return m_bBeingDeleted; }
    void    SetBeingDeleted() { m_bBeingDeleted = true; }

private:
    std::string m_strIP;
    std::string m_strNick;
    std::string m_strBanner;
    std::string m_strReason;
    std::string m_strSerial;
    std::string m_strAccount;
    time_t      m_tTimeOfBan;
    time_t      m_tTimeOfUnban;
    uint        m_uiScriptID;
    bool        m_bBeingDeleted;
};
