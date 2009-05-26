/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CUnoccupiedVehicleSyncPacket.cpp
*  PURPOSE:     Unoccupied vehicle synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CUnoccupiedVehicleSyncPacket::~CUnoccupiedVehicleSyncPacket ( void )
{
    m_Syncs.clear ();
}


bool CUnoccupiedVehicleSyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // While we're not out of bytes
    while ( BitStream.GetNumberOfUnreadBits () > 0 )
    {
        // Read out the sync data
        SyncData data;
        data.bSend = false;

        SUnoccupiedVehicleSync vehicle;
        BitStream.Read ( &vehicle );
        data.syncStructure = vehicle;

        // Add it to our list. We no longer check if it's valid here
        // because CUnoccupiedVehicleSync does and it won't write bad ID's
        // back to clients.
        m_Syncs.push_back ( data );
    }

    return m_Syncs.size () > 0;
}


bool CUnoccupiedVehicleSyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // While we're not out of syncs to write
    bool bSent = false;
    vector < SyncData > ::const_iterator iter = m_Syncs.begin ();
    for ( ; iter != m_Syncs.end (); ++iter )
    {
        // If we're not supposed to ignore the packet
        const SyncData* data = &( *iter );

        if ( data->bSend )
        {
            BitStream.Write ( &(data->syncStructure) );

            // We've sent atleast one sync
            bSent = true;
        }
    }

    return bSent;
}
