/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CCommandPacket.h
*  PURPOSE:     Command packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CCOMMANDPACKET_H
#define __PACKETS_CCOMMANDPACKET_H

#include "CPacket.h"

class CCommandPacket : public CPacket
{
public:
    inline                  CCommandPacket          ( void )                { m_strCommand = ""; };

    inline ePacketID        GetPacketID             ( void ) const          { return static_cast < ePacketID > ( PACKET_ID_COMMAND ); };
    inline unsigned long    GetFlags                ( void ) const          { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Read                    ( NetBitStreamInterface& BitStream );

    inline const char*      GetCommand              ( void ) const          { return m_strCommand.c_str(); };

private:
    std::string             m_strCommand;
};

#endif
