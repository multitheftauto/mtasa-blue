/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CObjectSyncPacket.h
*  PURPOSE:     Header for object sync packet class
*  DEVELOPERS:  Stanislav Izmalkov <izstas@live.ru>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_COBJECTSYNCPACKET_H
#define __PACKETS_COBJECTSYNCPACKET_H

#include "CPacket.h"
#include <CVector.h>
#include <vector>

class CObjectSyncPacket : public CPacket
{
public:
    struct SyncData
    {
        bool                    bSend;
        ElementID               ID;
        unsigned char           ucSyncTimeContext;
        unsigned char           ucFlags;
        CVector                 vecPosition;
        CVector                 vecRotation;
        float                   fHealth;
    };

public:
                            ~CObjectSyncPacket                      ( void );
            
    inline ePacketID                GetPacketID                     ( void ) const                  { return PACKET_ID_OBJECT_SYNC; };
    inline unsigned long            GetFlags                        ( void ) const                  { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool                    Read                                    ( NetBitStreamInterface& BitStream );
    bool                    Write                                   ( NetBitStreamInterface& BitStream ) const;

    inline std::vector < SyncData* > ::const_iterator     IterBegin ( void )                        { return m_Syncs.begin (); };
    inline std::vector < SyncData* > ::const_iterator     IterEnd   ( void )                        { return m_Syncs.end (); };

    std::vector < SyncData* > m_Syncs;
};

#endif
