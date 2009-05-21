/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPedSyncPacket.cpp
*  PURPOSE:     Ped synchronization packet class
*  DEVELOPERS:  lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPedSyncPacket::~CPedSyncPacket ( void )
{
    vector < SyncData* > ::const_iterator iter = m_Syncs.begin ();
    for ( ; iter != m_Syncs.end (); iter++ )
    {
        delete *iter;
    }
    m_Syncs.clear ();
}


bool CPedSyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // While we're not out of bytes
    while ( BitStream.GetNumberOfUnreadBits () > 32 )
    {
        // Read out the sync data
        SyncData* pData = new SyncData;
        pData->bSend = false;

        BitStream.Read ( pData->Model );

        // Read the sync time context
        BitStream.Read ( pData->ucSyncTimeContext );

        unsigned char ucFlags = 0;
        BitStream.Read ( ucFlags );
        pData->ucFlags = ucFlags;

        // Did we recieve position?
        if ( ucFlags & 0x01 )
        {
            BitStream.Read ( pData->vecPosition.fX );
            BitStream.Read ( pData->vecPosition.fY );
            BitStream.Read ( pData->vecPosition.fZ );
        }

        // Rotation
        if ( ucFlags & 0x02 )
        {
            BitStream.Read ( pData->fRotation );
        }

        // Velocity
        if ( ucFlags & 0x04 )
        {
            BitStream.Read ( pData->vecVelocity.fX );
            BitStream.Read ( pData->vecVelocity.fY );
            BitStream.Read ( pData->vecVelocity.fZ );
        }

        // Health and armour
        if ( ucFlags & 0x08 ) BitStream.Read ( pData->fHealth );
        if ( ucFlags & 0x10 ) BitStream.Read ( pData->fArmor );

        // Add it to our list. We no longer check if it's valid here
        // because CPedSync does and it won't write bad ID's
        // back to clients.
        m_Syncs.push_back ( pData );
    }

    return m_Syncs.size () > 0;
}


bool CPedSyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // While we're not out of syncs to write
    bool bSent = false;
    vector < SyncData* > ::const_iterator iter = m_Syncs.begin ();
    for ( ; iter != m_Syncs.end (); iter++ )
    {
        // If we're not supposed to ignore the packet
        SyncData* pData = *iter;
        if ( pData->bSend )
        {
            // Write vehicle ID
            BitStream.Write ( pData->Model );

            // Write the sync time context
            BitStream.Write ( pData->ucSyncTimeContext );

            BitStream.Write ( pData->ucFlags );

            // Position and rotation
            if ( pData->ucFlags & 0x01 )
            {
                BitStream.Write ( pData->vecPosition.fX );
                BitStream.Write ( pData->vecPosition.fY );
                BitStream.Write ( pData->vecPosition.fZ );
            }

            if ( pData->ucFlags & 0x02 )
            {
                BitStream.Write ( pData->fRotation );
            }

            // Velocity
            if ( pData->ucFlags & 0x04 )
            {
                BitStream.Write ( pData->vecVelocity.fX );
                BitStream.Write ( pData->vecVelocity.fY );
                BitStream.Write ( pData->vecVelocity.fZ );
            }

            // Health and armour
            if ( pData->ucFlags & 0x08 ) BitStream.Write ( pData->fHealth );
            if ( pData->ucFlags & 0x10 ) BitStream.Write ( pData->fArmor );

            // We've sent atleast one sync
            bSent = true;
        }
    }

    return bSent;
}
