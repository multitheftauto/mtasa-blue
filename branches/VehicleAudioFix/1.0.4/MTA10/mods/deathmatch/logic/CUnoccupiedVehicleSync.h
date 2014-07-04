/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CUnoccupiedVehicleSync.h
*  PURPOSE:     Header for unoccupied vehicle sync class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CUNOCCUPIEDVEHICLESYNC_H
#define __CUNOCCUPIEDVEHICLESYNC_H

#include <CClientCommon.h>
#include "CDeathmatchVehicle.h"

class CUnoccupiedVehicleSync
{
public:
                                    CUnoccupiedVehicleSync              ( CClientVehicleManager* pVehicleManager );
                                    ~CUnoccupiedVehicleSync             ( void );

    bool                            ProcessPacket                       ( unsigned char ucPacketID, NetBitStreamInterface& bitStream );
    void                            DoPulse                             ( void );

    void                            AddVehicle                          ( CDeathmatchVehicle* pVehicle );
    void                            RemoveVehicle                       ( CDeathmatchVehicle* pVehicle );
    void                            ClearVehicles                       ( void );

    inline std::list < CDeathmatchVehicle* > ::const_iterator
                                    IterBegin                           ( void )    { return m_List.begin (); };
    inline std::list < CDeathmatchVehicle* > ::const_iterator
                                    IterEnd                             ( void )    { return m_List.end (); };

    bool                            Exists                              ( CDeathmatchVehicle * pVehicle );
private:
    void                            Packet_UnoccupiedVehicleStartSync   ( NetBitStreamInterface& BitStream );
    void                            Packet_UnoccupiedVehicleStopSync    ( NetBitStreamInterface& BitStream );
    void                            Packet_UnoccupiedVehicleSync        ( NetBitStreamInterface& BitStream );

    void                            UpdateDamageModels                  ( void );
    void                            UpdateStates                        ( void );
    bool                            WriteVehicleInformation             ( NetBitStreamInterface* pBitStream, CDeathmatchVehicle* pVehicle );

    CClientVehicleManager*          m_pVehicleManager;
    CMappedList < CDeathmatchVehicle* > m_List;
    unsigned long                   m_ulLastSyncTime;
};

#endif
