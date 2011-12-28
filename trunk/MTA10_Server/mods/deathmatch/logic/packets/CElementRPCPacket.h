/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CElementRPCPacket.h
*  PURPOSE:     Lua element RPC packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CPacket.h"

class CElementRPCPacket : public CPacket
{
public:
                                    CElementRPCPacket   ( CElement* pSourceElement,
                                                          unsigned char ucActionID,
                                                          NetBitStreamInterface& BitStream )
                                                          : m_ucActionID ( ucActionID )
                                                          , m_BitStream ( BitStream )
                                                          , m_pSourceElement ( pSourceElement )
                                                        {};

    inline ePacketID                GetPacketID         ( void ) const  { return PACKET_ID_LUA_ELEMENT_RPC; };
    inline NetServerPacketOrdering  GetPacketOrdering   ( void ) const  { return PACKET_ORDERING_LUA; }
    inline unsigned long            GetFlags            ( void ) const  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                            Write               ( NetBitStreamInterface& BitStream ) const;

private:
    unsigned char                   m_ucActionID;
    NetBitStreamInterface&          m_BitStream;
    CElement*                       m_pSourceElement;
};
