/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CEntityAddPacket.h
*  PURPOSE:     Entity add packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               Kevin Whiteside <>
*               Cecill Etheredge <>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CEntityAddPacket;

#ifndef __PACKETS_CENTITYADDPACKET_H
#define __PACKETS_CENTITYADDPACKET_H

#include "../packets/CPacket.h"
#include <vector>
#include "../CGame.h"

extern CGame* g_pGame;

class CEntityAddPacket : public CPacket
{
public:
    inline ePacketID                GetPacketID                 ( void ) const                  { return PACKET_ID_ENTITY_ADD; };
    inline unsigned long            GetFlags                    ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                            Write                       ( NetServerBitStreamInterface& BitStream ) const;

    void                            Add                         ( class CElement* pElement );
    inline void                     Clear                       ( void )                        { m_Entities.clear (); };

private:
    std::vector < class CElement* > m_Entities;
};

#endif
