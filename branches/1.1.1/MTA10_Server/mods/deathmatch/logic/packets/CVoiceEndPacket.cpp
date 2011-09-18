/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVoiceEndPacket.cpp
*  PURPOSE:     Voice end packet class
*  DEVELOPERS:  Philip Farquharson <philip@philipfarquharson.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVoiceEndPacket::CVoiceEndPacket ( void )
{
    m_PlayerID = INVALID_ELEMENT_ID;
}

CVoiceEndPacket::~CVoiceEndPacket ( )
{

}


ePacketID CVoiceEndPacket::GetPacketID ( void ) const
{
    return static_cast < ePacketID > ( PACKET_ID_VOICE_END );
}


unsigned long CVoiceEndPacket::GetFlags ( void ) const
{
    return 0;
}


bool CVoiceEndPacket::Read ( NetBitStreamInterface& BitStream )
{
    return true;
}


bool CVoiceEndPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    return false;
}
