/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CObjectSyncPacket.cpp
*  PURPOSE:     Header for object sync packet class
*  DEVELOPERS:  Stanislav Izmalkov <izstas@live.ru>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CObjectSyncPacket::~CObjectSyncPacket ( void )
{
    vector < SyncData* > ::const_iterator iter = m_Syncs.begin ();
    for ( ; iter != m_Syncs.end (); iter++ )
    {
        delete *iter;
    }
    m_Syncs.clear ();
}


bool CObjectSyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // While we're not out of bytes
    while ( BitStream.GetNumberOfUnreadBits () > 8 )
    {
        // Read out the sync data
        SyncData* pData = new SyncData;
        pData->bSend = false;

        // Read out the ID
        if ( !BitStream.ReadCompressed ( pData->ID ) )
            return false;

        // Read the sync time context
        if ( !BitStream.Read ( pData->ucSyncTimeContext ) )
            return false;

        // Read out flags
        SIntegerSync < unsigned char, 3 > flags;
        if ( !BitStream.Read ( &flags ) )
            return false;
        pData->ucFlags = flags;

        // Read out the position if we need
        if ( flags & 0x1 )
        {
            SPositionSync position;
            if ( !BitStream.Read ( &position ) )
                return false;
            pData->vecPosition = position.data.vecPosition;
        }

        // Read out the rotation
        if ( flags & 0x2 )
        {
            SRotationRadiansSync rotation;
            if ( !BitStream.Read ( &rotation ) )
                return false;
            pData->vecRotation = rotation.data.vecRotation;
        }

        // Read out the health
        if ( flags & 0x4 )
        {
            SObjectHealthSync health;
            if ( !BitStream.Read ( &health ) )
                return false;
            pData->fHealth = health.data.fValue;
        }

        // Add it to our list
        m_Syncs.push_back ( pData );
    }

    return m_Syncs.size () > 0;
}


bool CObjectSyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    bool bSent = false;
    vector < SyncData* > ::const_iterator iter = m_Syncs.begin ();
    // Write syncs
    for ( ; iter != m_Syncs.end (); iter++ )
    {
        SyncData* pData = *iter;
        // If we're not supposed to ignore the packet
        if ( pData->bSend )
        {
            // Write the ID
            BitStream.WriteCompressed ( pData->ID );

            // Write the sync time context
            BitStream.Write ( pData->ucSyncTimeContext );

            // Write flags
            SIntegerSync < unsigned char, 3 > flags ( pData->ucFlags );
            BitStream.Write ( &flags );

            // Write position if we need
            if ( flags & 0x1 )
            {
                SPositionSync position;
                position.data.vecPosition = pData->vecPosition;
                BitStream.Write ( &position );
            }

            // Write rotation
            if ( flags & 0x2 )
            {
                SRotationRadiansSync rotation;
                rotation.data.vecRotation = pData->vecRotation;
                BitStream.Write ( &rotation );
            }

            // Write health
            if ( flags & 0x4 )
            {
                SObjectHealthSync health;
                health.data.fValue = pData->fHealth;
            }

            // We've sent atleast one sync
            bSent = true;
        }
    }

    return bSent;
}
