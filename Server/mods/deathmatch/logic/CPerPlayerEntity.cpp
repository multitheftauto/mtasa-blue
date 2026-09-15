/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerPlayerEntity.cpp
 *  PURPOSE:     Per-player entity linking class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPerPlayerEntity.h"
#include "CGame.h"
#include "CMapManager.h"
#include "packets/CEntityAddPacket.h"

std::set<CPerPlayerEntity*> CPerPlayerEntity::ms_AllPerPlayerEntityMap;

CPerPlayerEntity::CPerPlayerEntity(CElement* pParent) : CElement(pParent)
{
    MapInsert(ms_AllPerPlayerEntityMap, this);
    m_bIsSynced = false;
    AddVisibleToReference(g_pGame->GetMapManager()->GetRootElement());
};

CPerPlayerEntity::~CPerPlayerEntity()
{
    // Unreference us from what we're referencing
    for (std::map<CElement*, bool>::const_iterator iter = m_ElementVisibility.begin(); iter != m_ElementVisibility.end(); iter++)
    {
        iter->first->m_ElementReferenced.remove(this);
    }
    MapRemove(ms_AllPerPlayerEntityMap, this);
}

bool CPerPlayerEntity::Sync(bool bSync)
{
    // Are we getting synced but not already synced or vice versa?
    if (bSync != m_bIsSynced)
    {
        // Create it for everyone we're visible if it's synced, otherwise destroy
        if (bSync)
        {
            m_bIsSynced = true;
            CreateEntity(NULL);
        }
        else
        {
            DestroyEntity(NULL);
            m_bIsSynced = false;
        }
    }

    return true;
}

void CPerPlayerEntity::OnReferencedSubtreeAdd(CElement* pElement)
{
    assert(pElement);

    // Add all players below that item to our list
    UpdatePlayersBelow(pElement);
}

void CPerPlayerEntity::OnReferencedSubtreeRemove(CElement* pElement)
{
    assert(pElement);

    // Remove all players below that item from our list
    HidePlayersBelow(pElement);
}

void CPerPlayerEntity::UpdatePerPlayer()
{
    if (m_PlayersAdded.empty() && m_PlayersRemoved.empty())  // This check reduces cpu usage when loading large maps (due to recursion)
        return;

    // Remove entries that match in both added and removed lists
    RemoveIdenticalEntries(m_PlayersAdded, m_PlayersRemoved);

    // Delete us for every player in our deleted list
    std::set<CPlayer*>::const_iterator iter = m_PlayersRemoved.begin();
    for (; iter != m_PlayersRemoved.end(); iter++)
    {
        DestroyEntity(*iter);
    }

    // Add us for every player in our added list
    iter = m_PlayersAdded.begin();
    for (; iter != m_PlayersAdded.end(); iter++)
    {
        CreateEntity(*iter);
    }

    // Clear both lists
    m_PlayersAdded.clear();
    m_PlayersRemoved.clear();
}

bool CPerPlayerEntity::AddVisibleToReference(CElement* pElement)
{
    return SetElementVisibility(pElement, true);
}

bool CPerPlayerEntity::RemoveVisibleToReference(CElement* pElement)
{
    return SetElementVisibility(pElement, false);
}

bool CPerPlayerEntity::SetElementVisibility(CElement* pElement, bool bVisible)
{
    assert(pElement);

    std::map<CElement*, bool>::iterator iter = m_ElementVisibility.find(pElement);
    if (iter != m_ElementVisibility.end())
    {
        // Nothing to do if the value is already what we were asked to set
        if (iter->second == bVisible)
            return false;

        iter->second = bVisible;
    }
    else
    {
        m_ElementVisibility[pElement] = bVisible;

        pElement->m_ElementReferenced.push_back(this);
    }

    UpdatePlayersBelow(pElement);
    UpdatePerPlayerEntities();

    return true;
}

void CPerPlayerEntity::ClearVisibleToReferences()
{
    CElement* pRoot = g_pGame->GetMapManager()->GetRootElement();
    assert(pRoot);

    for (std::map<CElement*, bool>::iterator iter = m_ElementVisibility.begin(); iter != m_ElementVisibility.end(); iter++)
    {
        iter->first->m_ElementReferenced.remove(this);
    }
    m_ElementVisibility.clear();

    // Restore the default visibility, which is being visible to everyone
    if (pRoot)
    {
        m_ElementVisibility[pRoot] = true;
        pRoot->m_ElementReferenced.push_back(this);

        UpdatePlayersBelow(pRoot);
        UpdatePerPlayerEntities();
    }
}

bool CPerPlayerEntity::IsVisibleToElement(CElement* pElement)
{
    for (CElement* pCurrent = pElement; pCurrent; pCurrent = pCurrent->GetParentEntity())
    {
        std::map<CElement*, bool>::const_iterator iter = m_ElementVisibility.find(pCurrent);
        if (iter != m_ElementVisibility.end())
            return iter->second;
    }

    return false;
}

bool CPerPlayerEntity::IsVisibleToPlayer(CPlayer& Player)
{
    // Return true if we're visible to the given player
    return MapContains(m_Players, &Player);
}

void CPerPlayerEntity::CreateEntity(CPlayer* pPlayer)
{
    // Are we visible?
    if (m_bIsSynced)
    {
        // Create the add entity packet
        CEntityAddPacket Packet;
        Packet.Add(this);

        // Send it to the player if available, if not everyone
        if (pPlayer)
        {
            // Only send it to him if we can
            if (!pPlayer->DoNotSendEntities())
            {
                // CLogger::DebugPrintf ( "Created %u (%s) for %s\n", GetID (), GetName (), pPlayer->GetNick () );
                pPlayer->Send(Packet);
            }
        }
        else
        {
            // CLogger::DebugPrintf ( "Created %u (%s) for everyone (%u)\n", GetID (), GetName (), m_Players.size () );
            BroadcastOnlyVisible(Packet);
        }
    }
}

void CPerPlayerEntity::DestroyEntity(CPlayer* pPlayer)
{
    // Are we visible?
    if (m_bIsSynced)
    {
        // Create the remove entity packet
        CEntityRemovePacket Packet;
        Packet.Add(this);

        // Send it to the player if available, if not everyone
        if (pPlayer)
        {
            // Only send it to him if we can
            if (!pPlayer->DoNotSendEntities())
            {
                pPlayer->Send(Packet);
                // CLogger::DebugPrintf ( "Destroyed %u (%s) for %s\n", GetID (), GetName (), pPlayer->GetNick () );
            }
        }
        else
        {
            // CLogger::DebugPrintf ( "Destroyed %u (%s) for everyone (%u)\n", GetID (), GetName (), m_Players.size () );
            BroadcastOnlyVisible(Packet);
        }
    }
}

void CPerPlayerEntity::BroadcastOnlyVisible(const CPacket& Packet)
{
    // Are we synced? (if not we're not visible to anybody)
    if (!m_bIsSynced)
        return;

    CPlayerManager* playerManager = g_pGame->GetPlayerManager();
    CSendList       sendList;

    for (std::set<CPlayer*>::iterator iter = m_Players.begin(); iter != m_Players.end(); /*manual increment*/)
    {
        CPlayer* player = *iter;

        if (!playerManager->Exists(player))
        {
            // Why does this happen?
            // CLogger::ErrorPrintf("CPerPlayerEntity removed invalid player from list: %08x", player);
            iter = m_Players.erase(iter);
        }
        else
        {
            ++iter;

            if (player->IsLeavingServer())
                continue;
        }

        sendList.push_back(player);
    }

    CPlayerManager::Broadcast(Packet, sendList);
}

void CPerPlayerEntity::RemoveIdenticalEntries(std::set<CPlayer*>& List1, std::set<CPlayer*>& List2)
{
    std::vector<CPlayer*> dupList;

    // Make list of dups
    for (std::set<CPlayer*>::iterator it = List1.begin(); it != List1.end(); it++)
        if (MapContains(List2, *it))
            dupList.push_back(*it);

    // Remove dups from both lists
    for (std::vector<CPlayer*>::iterator it = dupList.begin(); it != dupList.end(); it++)
    {
        MapRemove(List1, *it);
        MapRemove(List2, *it);
    }
}

void CPerPlayerEntity::UpdatePlayersBelow(CElement* pElement)
{
    assert(pElement);

    // Is this a player?
    if (IS_PLAYER(pElement))
    {
        SyncPlayerVisibility(static_cast<CPlayer*>(pElement));
    }

    // Call ourself on all its children elements
    CChildListType ::const_iterator iterChildren = pElement->IterBegin();
    for (; iterChildren != pElement->IterEnd(); iterChildren++)
    {
        CElement* pChild = *iterChildren;
        if (pChild->CountChildren() || IS_PLAYER(pChild))  // This check reduces cpu usage when loading large maps (due to recursion)
            UpdatePlayersBelow(pChild);
    }
}

void CPerPlayerEntity::HidePlayersBelow(CElement* pElement)
{
    assert(pElement);

    // Is this a player?
    if (IS_PLAYER(pElement))
    {
        CPlayer* pPlayer = static_cast<CPlayer*>(pElement);
        if (IsVisibleToPlayer(*pPlayer))
        {
            RemovePlayerReference(pPlayer);
            MapInsert(m_PlayersRemoved, pPlayer);
        }
    }

    // Call ourself on all our children
    CChildListType ::const_iterator iterChildren = pElement->IterBegin();
    for (; iterChildren != pElement->IterEnd(); iterChildren++)
    {
        CElement* pChild = *iterChildren;
        if (pChild->CountChildren() || IS_PLAYER(pChild))  // This check reduces cpu usage when loading large maps (due to recursion)
            HidePlayersBelow(pChild);
    }
}

void CPerPlayerEntity::SyncPlayerVisibility(CPlayer* pPlayer)
{
    const bool bVisible = IsVisibleToElement(pPlayer);

    if (bVisible == IsVisibleToPlayer(*pPlayer))
        return;

    if (bVisible)
    {
        AddPlayerReference(pPlayer);
        MapInsert(m_PlayersAdded, pPlayer);
    }
    else
    {
        RemovePlayerReference(pPlayer);
        MapInsert(m_PlayersRemoved, pPlayer);
    }
}

void CPerPlayerEntity::AddPlayerReference(CPlayer* pPlayer)
{
    if (g_pGame->GetPlayerManager()->Exists(pPlayer))
        MapInsert(m_Players, pPlayer);
    else
        CLogger::ErrorPrintf("CPerPlayerEntity tried to add reference for non existing player: %08x\n", pPlayer);
}

void CPerPlayerEntity::RemovePlayerReference(CPlayer* pPlayer)
{
    MapRemove(m_Players, pPlayer);
}

//
// Hacks to stop crash
//
void CPerPlayerEntity::StaticOnPlayerDelete(CPlayer* pPlayer)
{
    for (std::set<CPerPlayerEntity*>::iterator iter = ms_AllPerPlayerEntityMap.begin(); iter != ms_AllPerPlayerEntityMap.end(); ++iter)
    {
        (*iter)->OnPlayerDelete(pPlayer);
    }
}

void CPerPlayerEntity::OnPlayerDelete(CPlayer* pPlayer)
{
    /* Caz - Debug code disabled because it is being fixed by removing them from the map and the error is misleading users*/
    // SString strStatus;
    if (MapContains(m_Players, pPlayer))
    {
        // strStatus += "m_Players ";
        MapRemove(m_Players, pPlayer);
    }

    if (MapContains(m_PlayersAdded, pPlayer))
    {
        // strStatus += "m_PlayersAdded ";
        MapRemove(m_PlayersAdded, pPlayer);
    }

    if (MapContains(m_PlayersRemoved, pPlayer))
    {
        // strStatus += "m_PlayersRemoved ";
        MapRemove(m_PlayersRemoved, pPlayer);
    }

    /*if ( !strStatus.empty() )
    {
        CLogger::ErrorPrintf( "CPerPlayerEntity problem: %s\n", *strStatus );
    }*/
}
