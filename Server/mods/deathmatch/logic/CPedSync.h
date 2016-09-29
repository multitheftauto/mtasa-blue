/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPedSync.h
*  PURPOSE:     Ped entity synchronization class
*  DEVELOPERS:  lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPEDSYNC_H
#define __CPEDSYNC_H

#include "CPlayerManager.h"
#include "CPedManager.h"
#include "packets/CPedSyncPacket.h"

class CPedSync
{
public:
                            CPedSync                            ( CPlayerManager* pPlayerManager, CPedManager* pPedManager );

    void                    DoPulse                             ( void );
    bool                    ProcessPacket                       ( CPacket& Packet );

    void                    OverrideSyncer                      ( CPed* pPed, CPlayer* pPlayer );

private:
    void                    Update                              ( void );
    void                    UpdatePed                           ( CPed* pPed );
    void                    FindSyncer                          ( CPed* pPed );
    CPlayer*                FindPlayerCloseToPed                ( CPed* pPed, float fMaxDistance );

    void                    StartSync                           ( CPlayer* pPlayer, CPed* pPed );
    void                    StopSync                            ( CPed* pPed );

    void                    Packet_PedSync                      ( CPedSyncPacket& Packet );

    CPlayerManager*         m_pPlayerManager;
    CPedManager*            m_pPedManager;

    CElapsedTime            m_UpdateTimer;
};

#endif
