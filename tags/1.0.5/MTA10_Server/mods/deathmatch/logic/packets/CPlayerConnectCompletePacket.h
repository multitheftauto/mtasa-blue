/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerConnectCompletePacket.h
*  PURPOSE:     Player connect complete packet class
*  DEVELOPERS:  Derek Abdine <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERCONNECTCOMPLETEPACKET_H
#define __PACKETS_CPLAYERCONNECTCOMPLETEPACKET_H

#include "../../Config.h"
#include "CPacket.h"

#define MAX_CONN_TEXT_LEN 128

class CPlayerConnectCompletePacket : public CPacket
{
public:
                            CPlayerConnectCompletePacket    ( void );

    inline ePacketID        GetPacketID                     ( void ) const      { return static_cast < ePacketID > ( PACKET_ID_SERVER_JOIN_COMPLETE ); };
    inline unsigned long    GetFlags                        ( void ) const      { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                           ( NetBitStreamInterface& BitStream ) const;
    bool                    Read                            ( NetBitStreamInterface& bitStream );
private:
    char m_szConnText[MAX_CONN_TEXT_LEN];
};

#endif
