/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerPlayerEntity.h
 *  PURPOSE:     Per-player entity linking class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <map>

#include "CElement.h"
#include "packets/CPacket.h"

class CPerPlayerEntity : public CElement
{
    friend class CElement;

public:
    CPerPlayerEntity(CElement* pParent);
    ~CPerPlayerEntity();

    bool IsEntity() { return true; }
    bool IsPerPlayerEntity() { return true; };

    bool Sync(bool bSync);
    bool IsSynced() { return m_bIsSynced; };
    void SetIsSynced(bool bIsSynced) { m_bIsSynced = bIsSynced; };

    void OnReferencedSubtreeAdd(CElement* pElement);
    void OnReferencedSubtreeRemove(CElement* pElement);
    void UpdatePerPlayer();

    bool AddVisibleToReference(CElement* pElement);
    bool RemoveVisibleToReference(CElement* pElement);
    void ClearVisibleToReferences();
    bool IsVisibleToElement(CElement* pElement);

    bool IsVisibleToPlayer(CPlayer& Player);

    const std::set<CPlayer*>& GetPlayersList() { return m_Players; }

    static void StaticOnPlayerDelete(CPlayer* pPlayer);
    void        OnPlayerDelete(CPlayer* pPlayer);

protected:
    virtual void CreateEntity(CPlayer* pPlayer);
    virtual void DestroyEntity(CPlayer* pPlayer);

    void BroadcastOnlyVisible(const CPacket& Packet);
    bool m_bIsSynced;

private:
    void RemoveIdenticalEntries(std::set<class CPlayer*>& List1, std::set<class CPlayer*>& List2);

    bool SetElementVisibility(CElement* pElement, bool bVisible);

    void UpdatePlayersBelow(CElement* pElement);
    void HidePlayersBelow(CElement* pElement);
    void SyncPlayerVisibility(class CPlayer* pPlayer);

    void AddPlayerReference(class CPlayer* pPlayer);
    void RemovePlayerReference(class CPlayer* pPlayer);

    std::map<CElement*, bool> m_ElementVisibility;

    std::set<CPlayer*> m_PlayersAdded;
    std::set<CPlayer*> m_PlayersRemoved;
    std::set<CPlayer*> m_Players;

    static std::set<CPerPlayerEntity*> ms_AllPerPlayerEntityMap;
};
