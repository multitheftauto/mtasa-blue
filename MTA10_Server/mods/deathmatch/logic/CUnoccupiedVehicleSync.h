/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CUnoccupiedVehicleSync.h
*  PURPOSE:     Unoccupied vehicle entity synchronization class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CUNOCCUPIEDVEHICLESYNC_H
#define __CUNOCCUPIEDVEHICLESYNC_H

#include "CPlayerManager.h"
#include "CVehicleManager.h"
#include "packets/CUnoccupiedVehicleSyncPacket.h"
#define MIN_ROTATION_DIFF   0.1

class CUnoccupiedVehicleSync
{
public:
                            CUnoccupiedVehicleSync              ( CPlayerManager* pPlayerManager, CVehicleManager* pVehicleManager );

    void                    DoPulse                             ( void );
    bool                    ProcessPacket                       ( CPacket& Packet );

    void                    OverrideSyncer                      ( CVehicle* pVehicle, CPlayer* pPlayer );

private:
    void                    Update                              ( unsigned long ulCurrentTime );
    void                    UpdateVehicle                       ( CVehicle* pVehicle );
    void                    FindSyncer                          ( CVehicle* pVehicle );
    CPlayer*                FindPlayerCloseToVehicle            ( CVehicle* pVehicle, float fMaxDistance );

    void                    StartSync                           ( CPlayer* pPlayer, CVehicle* pVehicle );
    void                    StopSync                            ( CVehicle* pVehicle );

    void                    Packet_UnoccupiedVehicleSync        ( CUnoccupiedVehicleSyncPacket& Packet );

    CPlayerManager*         m_pPlayerManager;
    CVehicleManager*        m_pVehicleManager;

    unsigned long           m_ulLastSweepTime;
};

#endif
