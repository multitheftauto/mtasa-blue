/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     An optimized container of players grouped by
 *               bitstream version.
 *               For painless and fast DoBroadcast's.
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
class CSendList;

#pragma once

#include <vector>
#include <unordered_map>
#include <type_traits>

class CPlayer;

class CSendList : protected std::unordered_map<unsigned short, std::vector<CPlayer*>>
{
public:
    using BaseCont_t = std::unordered_map<unsigned short, std::vector<CPlayer*>>;

public:
    CSendList() = default;

    // From sequencial container
    template<class Cont_t>
    CSendList(Cont_t&& players) { Insert(std::forward<Cont_t>(players)); }

    CSendList(CSendList&&) = default;
    CSendList(const CSendList&) = default;

    void Insert(CPlayer* pPlayer);

    // Insert from a sequencial container(including a set)
    template<typename Cont_t,
        typename = std::enable_if_t<std::is_same_v<std::remove_cv_t<typename Cont_t::value_type>, CPlayer*>>>
    void Insert(const Cont_t& players)
    {
        for (auto pPlayer : players)
            Insert(pPlayer);
    }

    size_t CountUniqueVersions() const noexcept { return size(); }

    size_t CountJoined() const noexcept;

    bool Contains(CPlayer* pPlayer) const
    {
        if (const auto group = GetAPlayersGroup(pPlayer))
            return std::find(group->begin(), group->end(), pPlayer) != group->end();
        return false;
    }

    bool Erase(CPlayer* pPlayer) noexcept
    {
        if (BaseCont_t::mapped_type* group = GetAPlayersGroup(pPlayer))
        {
            if (const auto it = std::find(group->begin(), group->end(), pPlayer); it != group->end())
            {
                group->erase(it);
                return true;
            }
        }
        return false;
    }

    const BaseCont_t::mapped_type* GetPlayerGroupByBitStream(unsigned short bsver) const
    {
        if (const auto it = find(bsver); it != end())
            return &it->second;
        return nullptr;
    }

    BaseCont_t::mapped_type* GetPlayerGroupByBitStream(unsigned short bsver)
    { 
        // As seen in Scott Mayers book.. Thanks stackoverflow!
        return const_cast<BaseCont_t::mapped_type*>(const_cast<const CSendList*>(this)->GetPlayerGroupByBitStream(bsver));
    }

    // Return a list where players thave the same bitstream version as the player
    BaseCont_t::mapped_type* GetAPlayersGroup(CPlayer* pPlayer);
    const BaseCont_t::mapped_type* GetAPlayersGroup(CPlayer* pPlayer) const;

    // Wrappers (we dont want to expose non-const iterators)
    bool Empty() const noexcept { return empty(); }

    BaseCont_t::const_iterator begin() const noexcept { return BaseCont_t::begin(); }
    BaseCont_t::const_iterator end() const noexcept { return BaseCont_t::end(); }
};
