/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPlayerManager.cpp
 *  PURPOSE:     Player entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"

#define REMOTE_PLAYER_CONNECTION_TROUBLE_TIME 6000

using std::list;
using std::vector;

CClientPlayerManager::CClientPlayerManager(CClientManager* pManager)
{
    // Initialize members
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
    m_pLocalPlayer = NULL;
}

CClientPlayerManager::~CClientPlayerManager()
{
    // Destroy all players
    DeleteAll();
}

void CClientPlayerManager::DoPulse()
{
    unsigned long                          ulCurrentTime = CClientTime::GetTime();
    CClientPlayer*                         pPlayer = NULL;
    vector<CClientPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); ++iter)
    {
        pPlayer = *iter;

        if (!pPlayer->IsLocalPlayer())
        {
            // Pulse voice data if voice is enabled
            if (g_pClientGame->GetVoiceRecorder()->IsEnabled() && pPlayer->GetVoice())
                pPlayer->GetVoice()->DoPulse();

            // Flag him with connection error if its been too long since last puresync and force his position
            unsigned long ulLastPuresyncTime = pPlayer->GetLastPuresyncTime();
            bool          bHasConnectionTrouble = (ulLastPuresyncTime != 0 && ulCurrentTime >= ulLastPuresyncTime + REMOTE_PLAYER_CONNECTION_TROUBLE_TIME);
            if (bHasConnectionTrouble && !g_pClientGame->IsDownloadingBigPacket() && !pPlayer->IsDeadOnNetwork())
            {
                pPlayer->SetHasConnectionTrouble(true);

                // Reset his controller so he doesn't get stuck shooting or something
                CControllerState State;
                memset(&State, 0, sizeof(CControllerState));
                pPlayer->SetControllerState(State);

                // Grab his vehicle if any and force the position to where he was last sync
                CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle();
                if (pVehicle)
                {
                    // Is he driving the vehicle?
                    if (pPlayer->GetOccupiedVehicleSeat() == 0)
                    {
                        // Force his position to where he was last sync
                        pVehicle->SetPosition(pPlayer->GetLastPuresyncPosition());
                        pVehicle->SetMoveSpeed(CVector(0, 0, 0));
                        pVehicle->SetTurnSpeed(CVector(0, 0, 0));
                        pPlayer->ResetInterpolation();
                    }
                }
                else
                {
                    // Force his position to where he was last sync
                    pPlayer->SetPosition(pPlayer->GetLastPuresyncPosition());
                    pPlayer->ResetInterpolation();
                    pPlayer->SetMoveSpeed(CVector(0, 0, 0));
                    pPlayer->ResetInterpolation();
                }
            }
            else
            {
                pPlayer->SetHasConnectionTrouble(false);
            }
        }
    }
}

void CClientPlayerManager::DeleteAll()
{
    // Delete all the players
    m_bCanRemoveFromList = false;
    vector<CClientPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_Players.clear();
    m_bCanRemoveFromList = true;
}

CClientPlayer* CClientPlayerManager::Get(ElementID ID)
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTPLAYER)
    {
        return static_cast<CClientPlayer*>(pEntity);
    }

    return NULL;
}

CClientPlayer* CClientPlayerManager::Get(const char* szNick, bool bCaseSensitive)
{
    assert(szNick);

    // Find a player with a matching nick in the list
    vector<CClientPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        const char* szPtr = (*iter)->GetNick();
        if (szPtr)
        {
            if (bCaseSensitive && strcmp(szNick, szPtr) == 0 || !bCaseSensitive && stricmp(szNick, szPtr) == 0)
            {
                return *iter;
            }
        }
    }

    // If we didn't find it, return NULL
    return NULL;
}

CClientPlayer* CClientPlayerManager::Get(CPlayerPed* pPlayer, bool bValidatePointer)
{
    if (!pPlayer)
        return NULL;

    if (bValidatePointer)
    {
        vector<CClientPlayer*>::const_iterator iter = m_Players.begin();
        for (; iter != m_Players.end(); iter++)
        {
            if ((*iter)->GetGamePlayer() == pPlayer)
            {
                return *iter;
            }
        }
    }
    else
    {
        CClientPed* pPed = reinterpret_cast<CClientPed*>(pPlayer->GetStoredPointer());
        if (pPed->GetType() == CCLIENTPLAYER)
        {
            return static_cast<CClientPlayer*>(pPed);
        }
    }

    return NULL;
}

bool CClientPlayerManager::Exists(CClientPlayer* pPlayer)
{
    return m_Players.Contains(pPlayer);
}

bool CClientPlayerManager::IsPlayerLimitReached()
{
    return g_pGame->GetPools()->GetPedCount() >= 110;

    return true;
}

bool CClientPlayerManager::IsValidModel(unsigned long ulModel)
{
    if (ulModel < static_cast<unsigned long>(g_pGame->GetBaseIDforTXD()))
    {
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(ulModel);
        return pModelInfo && pModelInfo->IsPlayerModel();
    }
    return false;
}

void CClientPlayerManager::ResetAll()
{
    vector<CClientPlayer*>::const_iterator iter = m_Players.begin();
    for (; iter != m_Players.end(); iter++)
    {
        CClientPlayer* pPlayer = *iter;
        if (pPlayer)
        {
            pPlayer->Reset();
        }
    }
}

void CClientPlayerManager::RemoveFromList(CClientPlayer* pPlayer)
{
    if (m_bCanRemoveFromList)
    {
        m_Players.remove(pPlayer);
    }
}
