/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CDestroySatchelsPacket.cpp
*  PURPOSE:     Satchel destruction packet class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CDestroySatchelsPacket::CDestroySatchelsPacket ( void )
{
}


bool CDestroySatchelsPacket::Read ( NetBitStreamInterface& BitStream )
{
    return true;
}


bool CDestroySatchelsPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the source player.
    if ( m_pSourceElement )
    {
        BitStream.Write ( m_pSourceElement->GetID () );

    }
    else
        return false;

    return true;
}
