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
    friend class CBanManager;

    CBan() :
        m_uiScriptID(CIdArray::PopUniqueId(this, EIdClass::BAN))
    {
        CBanManager::SetBansModified();
    }

    // I'm lazy to implement these, but feel free to do so if it's needed
    // Tip: Use the copy-swap idiom:
    // https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    CBan(const CBan&) = delete;
    CBan(CBan&&) = delete;
    
    ~CBan()
    {
        CIdArray::PushUniqueId(this, EIdClass::BAN, m_uiScriptID);
        CBanManager::SetBansModified();
    }

    // Same as above
    CBan& operator=(const CBan&) = delete;
    CBan& operator=(CBan&&) = delete;
public:
    const std::string& GetIP() const noexcept { return m_strIP; };
    void               SetIP(std::string strIP) noexcept
    {
        CBanManager::SetBansModified();
        m_strIP = std::move(strIP);
    };

    const std::string& GetNick() const noexcept { return m_strNick; };
    void               SetNick(std::string strNick) noexcept
    {
        CBanManager::SetBansModified();
        m_strNick = std::move(strNick);
    };

    const std::string& GetBanner() const noexcept { return m_strBanner; };
    void               SetBanner(std::string strBanner) noexcept
    {
        CBanManager::SetBansModified();
        m_strBanner = std::move(strBanner);
    };

    const std::string& GetReason() const noexcept { return m_strReason; };
    void               SetReason(std::string strReason) noexcept
    {
        CBanManager::SetBansModified();
        m_strReason = std::move(strReason);
    };

    const time_t GetTimeOfBan() const noexcept { return m_tTimeOfBan; };
    void         SetTimeOfBan(time_t tTimeOfBan) noexcept
    {
        CBanManager::SetBansModified();
        m_tTimeOfBan = tTimeOfBan;
    };

    time_t GetTimeOfUnban() const noexcept { return m_tTimeOfUnban; };
    void   SetTimeOfUnban(time_t tTimeOfUnban) noexcept
    {
        CBanManager::SetBansModified();
        m_tTimeOfUnban = tTimeOfUnban;
    };

    const std::string& GetSerial() const noexcept { return m_strSerial; };
    void               SetSerial(std::string strSerial) noexcept
    {
        CBanManager::SetBansModified();
        m_strSerial = std::move(strSerial);
    };

    const std::string& GetAccount() const noexcept { return m_strAccount; };
    void               SetAccount(std::string strAccount) noexcept
    {
        CBanManager::SetBansModified();
        m_strAccount = std::move(strAccount);
    };

    time_t  GetBanTimeRemaining() const noexcept;
    SString GetDurationDesc() const noexcept;
    SString GetReasonText() const noexcept;
    uint    GetScriptID() const noexcept { return m_uiScriptID; }
    bool    IsBeingDeleted() const noexcept { return m_bBeingDeleted; }
    void    SetBeingDeleted() noexcept { m_bBeingDeleted = true; }

private:
    std::string m_strIP{};
    std::string m_strNick{};
    std::string m_strBanner{};
    std::string m_strReason{};
    std::string m_strSerial{};
    std::string m_strAccount{};
    time_t      m_tTimeOfBan = 0;
    time_t      m_tTimeOfUnban = 0;
    uint        m_uiScriptID = INVALID_ARRAY_ID;
    bool        m_bBeingDeleted = false;
};
