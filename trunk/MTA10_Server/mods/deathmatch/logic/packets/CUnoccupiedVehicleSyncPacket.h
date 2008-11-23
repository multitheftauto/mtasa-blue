/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CUnoccupiedVehicleSyncPacket.h
*  PURPOSE:     Unoccupied vehicle synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CUNOCCUPIEDVEHICLESYNCPACKET_H
#define __CUNOCCUPIEDVEHICLESYNCPACKET_H

#include <CVector.h>
#include "CPacket.h"
#include <vector>

using namespace std;

class CUnoccupiedVehicleSyncPacket : public CPacket
{
public:
    struct SyncData
    {
        bool                    bSend;
        ElementID               Model;
        unsigned char           ucFlags;
        unsigned char           ucSyncTimeContext;
        CVector                 vecPosition;
        CVector                 vecRotationDegrees;
        CVector                 vecVelocity;
        CVector                 vecTurnSpeed;
        float                   fHealth;
        ElementID               TrailerID;
    };

public:
                            CUnoccupiedVehicleSyncPacket            ( void )                        {};
                            ~CUnoccupiedVehicleSyncPacket           ( void );
            
    inline ePacketID        GetPacketID                             ( void ) const                  { return PACKET_ID_UNOCCUPIED_VEHICLE_SYNC; };
    inline unsigned long    GetFlags                                ( void ) const                  { return PACKET_LOW_PRIORITY; };

    bool                    Read                                    ( NetServerBitStreamInterface& BitStream );
    bool                    Write                                   ( NetServerBitStreamInterface& BitStream ) const;

    inline vector < SyncData* > ::const_iterator     IterBegin       ( void )                        { return m_Syncs.begin (); };
    inline vector < SyncData* > ::const_iterator     IterEnd         ( void )                        { return m_Syncs.end (); };

    vector < SyncData* >     m_Syncs;
};

#endif
