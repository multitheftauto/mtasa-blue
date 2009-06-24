/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPedDamagePacket.h
*  PURPOSE:     Ped damage packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPLAYERDAMAGEPACKET_H
#define __CPLAYERDAMAGEPACKET_H

#include "CPacket.h"
#include <CVector.h>

class CPed;
typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;

class CPlayerDamagePacket : public CPacket
{
public:
                            CPlayerDamagePacket         ( void );
                            CPlayerDamagePacket         ( CPed * pPed, AssocGroupId animGroup, AnimationId animID );

    inline ePacketID        GetPacketID                 ( void ) const                      { return PACKET_ID_PLAYER_DAMAGE; };
    inline unsigned long    GetFlags                    ( void ) const                      { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Read                        ( NetBitStreamInterface& BitStream );
    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

    ElementID               m_PlayerID;
    AssocGroupId            m_AnimGroup;
    AnimationId             m_AnimID;
};

#endif
