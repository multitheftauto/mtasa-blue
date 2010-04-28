/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CLuaPacket.h
*  PURPOSE:     Lua packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CLUAPACKET_H
#define __PACKETS_CLUAPACKET_H

#include "CPacket.h"

class CLuaPacket : public CPacket
{
public:
                                    CLuaPacket          ( unsigned char ucActionID, NetBitStreamInterface& BitStream ) : m_ucActionID ( ucActionID ), m_BitStream ( BitStream ) {};

    inline ePacketID                GetPacketID         ( void ) const  { return PACKET_ID_LUA; };
    inline unsigned long            GetFlags            ( void ) const  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                            Write               ( NetBitStreamInterface& BitStream ) const;

private:
    unsigned char                   m_ucActionID;
    NetBitStreamInterface&          m_BitStream;
};

#endif
