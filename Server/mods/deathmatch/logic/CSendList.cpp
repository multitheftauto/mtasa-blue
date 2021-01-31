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

#include "StdInc.h"
#include "CPlayer.h"
#include <CSendList.h>
#include <numeric>

void CSendList::Insert(CPlayer* pPlayer)
{
    BaseContainer::mapped_type& group = (*this)[pPlayer->GetBitStreamVersion()];

    // Check if the player is already in the group. If he is, something went wrong.
    dassert(std::find(group.begin(), group.end(), pPlayer) == group.end());

    // Insert him after the check
    group.push_back(pPlayer);
}

size_t CSendList::CountJoined() const noexcept
{
    return std::accumulate(begin(), end(), 0,
        [](const auto& count, const auto& pair) { return count + pair.second.size(); }
    );
}

auto CSendList::GetAPlayersGroup(CPlayer* pPlayer) -> BaseContainer::mapped_type*
{
    return GetPlayerGroupByBitStream(pPlayer->GetBitStreamVersion()); 
}

auto CSendList::GetAPlayersGroup(CPlayer* pPlayer) const -> const BaseContainer::mapped_type*
{
    return GetPlayerGroupByBitStream(pPlayer->GetBitStreamVersion()); 
}
