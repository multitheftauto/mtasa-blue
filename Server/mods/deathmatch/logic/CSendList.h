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
    using BaseContainer = std::unordered_map<unsigned short, std::vector<CPlayer*>>;

public:
    CSendList() = default;

    // From sequencial container
    template<class Container>
    CSendList(Container&& players) { Insert(std::forward<Container>(players)); }

    CSendList(CSendList&&) = default;
    CSendList(const CSendList&) = default;

    void Insert(CPlayer* pPlayer);

    // Insert from a sequential container(including a set)
    template<typename Container,
        typename = std::enable_if_t<std::is_same_v<std::remove_cv_t<typename Container::value_type>, CPlayer*>>>
    void Insert(const Container& players)
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
        if (BaseContainer::mapped_type* group = GetAPlayersGroup(pPlayer))
        {
            if (const auto it = std::find(group->begin(), group->end(), pPlayer); it != group->end())
            {
                group->erase(it);
                if (group->empty())
                    erase(pPlayer->GetBitStreamVersion()); // Delete this group as it contains no players
                return true;
            }
        }
        return false;
    }

    const BaseContainer::mapped_type* GetPlayerGroupByBitStream(unsigned short bsver) const
    {
        if (const auto it = find(bsver); it != end())
            return &it->second;
        return nullptr;
    }

    BaseContainer::mapped_type* GetPlayerGroupByBitStream(unsigned short bsver)
    { 
        // As seen in Scott Mayers book.. Thanks stackoverflow!
        return const_cast<BaseContainer::mapped_type*>(const_cast<const CSendList*>(this)->GetPlayerGroupByBitStream(bsver));
    }

    // Return a list where players thave the same bitstream version as the player
    BaseContainer::mapped_type* GetAPlayersGroup(CPlayer* pPlayer);
    const BaseContainer::mapped_type* GetAPlayersGroup(CPlayer* pPlayer) const;

    // Wrappers (we dont want to expose non-const iterators)
    bool Empty() const noexcept { return empty(); }

    BaseContainer::const_iterator begin() const noexcept { return BaseContainer::begin(); }
    BaseContainer::const_iterator end() const noexcept { return BaseContainer::end(); }
};
