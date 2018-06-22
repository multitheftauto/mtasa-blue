/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBan.h
 *  PURPOSE:     Ban descriptor class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CCommon.h"
#include <string>

class CBan
{
public:
    CBan(void);
    ~CBan(void);

    const std::string& GetIP(void) { return m_strIP; };
    void               SetIP(const std::string& strIP)
    {
        CBanManager::SetBansModified();
        m_strIP = strIP;
    };

    const std::string& GetNick(void) { return m_strNick; };
    void               SetNick(const std::string& strNick)
    {
        CBanManager::SetBansModified();
        m_strNick = strNick;
    };

    const std::string& GetBanner(void) { return m_strBanner; };
    void               SetBanner(const std::string& strBanner)
    {
        CBanManager::SetBansModified();
        m_strBanner = strBanner;
    };

    const std::string& GetReason(void) { return m_strReason; };
    void               SetReason(const std::string& strReason)
    {
        CBanManager::SetBansModified();
        m_strReason = strReason;
    };

    const time_t GetTimeOfBan(void) { return m_tTimeOfBan; };
    void         SetTimeOfBan(time_t tTimeOfBan)
    {
        CBanManager::SetBansModified();
        m_tTimeOfBan = tTimeOfBan;
    };

    time_t GetTimeOfUnban(void) { return m_tTimeOfUnban; };
    void   SetTimeOfUnban(time_t tTimeOfUnban)
    {
        CBanManager::SetBansModified();
        m_tTimeOfUnban = tTimeOfUnban;
    };

    const std::string& GetSerial(void) { return m_strSerial; };
    void               SetSerial(const std::string& strSerial)
    {
        CBanManager::SetBansModified();
        m_strSerial = strSerial;
    };

    const std::string& GetAccount(void) { return m_strAccount; };
    void               SetAccount(const std::string& strAccount)
    {
        CBanManager::SetBansModified();
        m_strAccount = strAccount;
    };

    time_t  GetBanTimeRemaining(void);
    SString GetDurationDesc(void);
    SString GetReasonText(void) const;
    uint    GetScriptID(void) const { return m_uiScriptID; }
    bool    IsBeingDeleted(void) const { return m_bBeingDeleted; }
    void    SetBeingDeleted(void) { m_bBeingDeleted = true; }

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
