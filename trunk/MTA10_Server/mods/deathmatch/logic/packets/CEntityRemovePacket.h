/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CEntityRemovePacket.h
*  PURPOSE:     Entity remove packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CENTITYREMOVEPACKET_H
#define __PACKETS_CENTITYREMOVEPACKET_H

#include "CPacket.h"
#include <vector>

using namespace std;

class CEntityRemovePacket : public CPacket
{
public:
    inline ePacketID            GetPacketID                 ( void ) const                  { return PACKET_ID_ENTITY_REMOVE; };
    inline unsigned long        GetFlags                    ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                        Write                       ( NetServerBitStreamInterface& BitStream ) const;

    inline void                 Add                         ( class CElement* pElement )    { m_List.push_back ( pElement ); };
    inline void                 Clear                       ( void )                        { m_List.clear (); };

private:
    vector < class CElement* >  m_List;
};

#endif
