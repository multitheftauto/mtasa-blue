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

CVoiceEndPacket::CVoiceEndPacket ( CPlayer* pPlayer )
{
    m_PlayerID = INVALID_ELEMENT_ID;
    if ( pPlayer )
        SetSourceElement ( pPlayer );
}

CVoiceEndPacket::~CVoiceEndPacket ( )
{

}


bool CVoiceEndPacket::Read ( NetBitStreamInterface& BitStream )
{
    return true;
}


bool CVoiceEndPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    if ( m_pSourceElement )
    {
        // Write the source player
        ElementID ID = m_pSourceElement->GetID();
        BitStream.Write ( ID );
        return true;
    }
    return false;
}
