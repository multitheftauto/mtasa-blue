/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPedSync.h
*  PURPOSE:     Header for ped sync class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPEDSYNC_H
#define __CPEDSYNC_H

#include <CClientCommon.h>
#include "CClientVehicle.h"

class CPedSync
{
public:
                                    CPedSync                            ( CClientPedManager* pPedManager );
                                    ~CPedSync                           ( void );

    bool                            ProcessPacket                       ( unsigned char ucPacketID, NetBitStreamInterface& bitStream );
    void                            DoPulse                             ( void );

    void                            AddPed                              ( CClientPed* pPed );
    void                            RemovePed                           ( CClientPed* pPed );
    void                            ClearPeds                           ( void );

    inline std::list < CClientPed* > ::const_iterator
                                    IterBegin                           ( void )    { return m_List.begin (); };
    inline std::list < CClientPed* > ::const_iterator
                                    IterEnd                             ( void )    { return m_List.end (); };

    bool                            Exists                              ( CClientPed * pPed );
private:
    void                            Packet_PedStartSync                 ( NetBitStreamInterface& BitStream );
    void                            Packet_PedStopSync                  ( NetBitStreamInterface& BitStream );
    void                            Packet_PedSync                      ( NetBitStreamInterface& BitStream );

    void                            Update                              ( void );
    void                            WritePedInformation                 ( NetBitStreamInterface* pBitStream, CClientPed* pPed );

    CClientPedManager*              m_pPedManager;
    CMappedList < CClientPed* >     m_List;
    unsigned long                   m_ulLastSyncTime;
};

#endif
