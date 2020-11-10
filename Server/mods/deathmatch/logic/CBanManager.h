/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBanManager.h
 *  PURPOSE:     Ban manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClient.h"
#include "CPlayerManager.h"

#include <list>
#include <string_view>


class CBanManager
{
    using BanList_t = std::list<CBan>;
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

    bool  IsSpecificallyBanned(std::string_view ip) const noexcept;
    bool  IsSerialBanned(std::string_view serial) const noexcept;
    bool  IsAccountBanned(std::string_view account) const noexcept;

    CBan* GetBanFromIP(std::string_view ip) const noexcept;
    CBan* GetBanFromScriptID(uint uiScriptID) const noexcept;
    CBan* GetBanFromAccount(std::string_view account) const noexcept;
    CBan* GetBanFromSerial(std::string_view serial) const noexcept;

    unsigned int GetBansWithNick(std::string_view szNick) const noexcept;
    unsigned int GetBansWithBanner(std::string_view szBanner) const noexcept;

    void  RemoveBan(CBan* pBan);

    bool        LoadBanList();
    bool        ReloadBanList();
    void        SaveBanList();
    static bool IsValidIP(const char* szIP);
    static void SetBansModified() { ms_bSaveRequired = true; }

    size_t GetCount() const noexcept { return m_Bans.size(); }

    // Iterate thru bans that aren't currently being deleted
    // Even in the class itself use this, not a range based for loop!
    template<class Functor_t>
    void IterBans(const Functor_t& f) const noexcept
    {
        for (CBan& ban : m_Bans)
            if (!ban.IsBeingDeleted())
                f(ban);
    }

    // Find a ban based on a predicate
    template<class Pred_t>
    CBan* FindIf(const Pred_t& predicate) const noexcept
    {
        for (const CBan& ban : m_Bans)
            if (!ban.IsBeingDeleted())
                if (predicate(ban))
                    return &ban;
        return nullptr;
    }

    // Like std::count_if
    template<class Pred_t>
    size_t CountIf(const Pred_t& predicate) const noexcept
    {
        size_t i = 0;
        for (const CBan& ban : m_Bans)
            if (!ban.IsBeingDeleted())
                if (predicate(ban))
                    i++;
        return i;
    }

private:
    SString m_strBanListXMLPath;

    BanList_t m_Bans;

    time_t m_NextUpdateTime;

    bool        m_bAllowSave;
    static bool ms_bSaveRequired;
};
