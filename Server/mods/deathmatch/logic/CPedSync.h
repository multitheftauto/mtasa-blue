/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPedSync.h
 *  PURPOSE:     Ped entity synchronization class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPlayerManager.h"
#include "CPedManager.h"
#include "packets/CPedSyncPacket.h"

class CPedSync
{
public:
    CPedSync(CPlayerManager* pPlayerManager, CPedManager* pPedManager);

    void DoPulse();
    bool ProcessPacket(CPacket& Packet);

    void OverrideSyncer(CPed* pPed, CPlayer* pPlayer, bool bPersist = false);

    void SetSyncerAsPersistent(bool bPersist) { m_bSyncPersist = bPersist; };
    bool IsSyncerPersistent() { return m_bSyncPersist; };

private:
    void     UpdateAllSyncer();
    void     UpdateSyncer(CPed* pPed);
    void     UpdateNearPlayersList();
    void     FindSyncer(CPed* pPed);
    CPlayer* FindPlayerCloseToPed(CPed* pPed, float fMaxDistance);

    void StartSync(CPlayer* pPlayer, CPed* pPed);
    void StopSync(CPed* pPed);

    void Packet_PedSync(CPedSyncPacket& Packet);

    CPlayerManager* m_pPlayerManager;
    CPedManager*    m_pPedManager;

    bool m_bSyncPersist = false;

    CElapsedTime m_UpdateSyncerTimer;
    CElapsedTime m_UpdateNearListTimer;
};
