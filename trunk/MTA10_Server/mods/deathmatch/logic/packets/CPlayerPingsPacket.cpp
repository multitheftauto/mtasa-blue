/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerPingsPacket.cpp
*  PURPOSE:     Player pinging packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CPlayerPingsPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    // Got any players?
    if ( m_List.size () > 0 )
    {
        // Write each player's id/ping to the packet
        list < CPlayer* > ::const_iterator iter;
        for ( iter = m_List.begin (); iter != m_List.end (); iter++ )
        {
            // Grab the ping?
            unsigned int uiPing = (*iter)->GetPing ();

            // Divide the ping by 4 to fit it into a byte
            unsigned short usPing = 0;
            if ( uiPing > 999 )
            {
                usPing = 999;
            }
            else
            {
                usPing = static_cast < unsigned short > ( uiPing );
            }

            // Player ID
            ElementID PlayerID = (*iter)->GetID ();
            BitStream.Write ( PlayerID );

            // Write the ping
            BitStream.Write ( usPing );
        }

        return true;
    }

    return false;
}
