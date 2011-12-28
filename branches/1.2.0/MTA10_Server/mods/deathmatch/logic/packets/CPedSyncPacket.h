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
            
    inline ePacketID                GetPacketID                     ( void ) const                  { return PACKET_ID_PED_SYNC; };
    inline NetServerPacketOrdering  GetPacketOrdering               ( void ) const                  { return PACKET_ORDERING_OTHERSYNC; }
    inline unsigned long            GetFlags                        ( void ) const                  { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };

    bool                    Read                                    ( NetBitStreamInterface& BitStream );
    bool                    Write                                   ( NetBitStreamInterface& BitStream ) const;

    inline std::vector < SyncData* > ::const_iterator     IterBegin ( void )                        { return m_Syncs.begin (); };
    inline std::vector < SyncData* > ::const_iterator     IterEnd   ( void )                        { return m_Syncs.end (); };

    std::vector < SyncData* > m_Syncs;
};

#endif
