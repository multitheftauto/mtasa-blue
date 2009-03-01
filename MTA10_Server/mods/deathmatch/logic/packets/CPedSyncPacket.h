/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPedSyncPacket.h
*  PURPOSE:     Ped synchronization packet class
*  DEVELOPERS:  lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPEDSYNCPACKET_H
#define __PACKETS_CPEDSYNCPACKET_H

#include <CVector.h>
#include "CPacket.h"
#include <vector>

using namespace std;

class CPedSyncPacket : public CPacket
{
public:
    struct SyncData
    {
        bool                    bSend;
        ElementID               Model;
        unsigned char           ucFlags;
        unsigned char           ucSyncTimeContext;
        CVector                 vecPosition;
        float                   fRotation;
        CVector                 vecVelocity;
        float                   fHealth;
        float                   fArmor;
    };

public:
                            CPedSyncPacket                          ( void )                        {};
                            ~CPedSyncPacket                         ( void );
            
    inline ePacketID        GetPacketID                             ( void ) const                  { return PACKET_ID_PED_SYNC; };
    inline unsigned long    GetFlags                                ( void ) const                  { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };

    bool                    Read                                    ( NetServerBitStreamInterface& BitStream );
    bool                    Write                                   ( NetServerBitStreamInterface& BitStream ) const;

    inline vector < SyncData* > ::const_iterator     IterBegin       ( void )                        { return m_Syncs.begin (); };
    inline vector < SyncData* > ::const_iterator     IterEnd         ( void )                        { return m_Syncs.end (); };

    vector < SyncData* >     m_Syncs;
};

#endif
