/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerScreenShotPacket.cpp
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CPlayerScreenShotPacket::Read ( NetBitStreamInterface& BitStream )
{
    CPlayer* pPlayer = GetSourcePlayer ();
    if ( !pPlayer )
        return false;

    bool bHasGrabTime = false;
    uint uiServerGrabTime;

    // Read status
    BitStream.Read ( m_ucStatus );

    if ( m_ucStatus != 1 )
    {
        // minimized or disabled
        bHasGrabTime = true;
        BitStream.Read ( uiServerGrabTime );
        BitStream.ReadString ( m_strResourceName );
        if ( !BitStream.ReadString ( m_strTag ) )
            return false;
    }
    else
    if ( m_ucStatus == 1 )
    {
        // Read info
        BitStream.Read ( m_usScreenShotId );
        BitStream.Read ( m_usPartNumber );

        // Read data
        ushort usNumBytes = 0;
        if ( !BitStream.Read ( usNumBytes ) )
            return false;

        m_buffer.SetSize ( usNumBytes );
        if ( !BitStream.Read ( m_buffer.GetData (), m_buffer.GetSize () ) )
            return false;

        // Read more info if first part
        if ( m_usPartNumber == 0 )
        {
            bHasGrabTime = true;
            BitStream.Read ( uiServerGrabTime );
            BitStream.Read ( m_uiTotalBytes );
            BitStream.Read ( m_usTotalParts );
            BitStream.ReadString ( m_strResourceName );
            if ( !BitStream.ReadString ( m_strTag ) )
                return false;
        }
    }

    // Fixup grab time
    if ( bHasGrabTime )
    {
        uiServerGrabTime += pPlayer->GetPing () / 2;
        uint uiTimeSinceGrab = GetTickCount32 () - uiServerGrabTime;
        m_llServerGrabTime = GetTickCount64_ () - uiTimeSinceGrab;
    }

    return true;
}
