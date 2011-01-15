/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CObjectSync.cpp
*  PURPOSE:     Object sync class
*  DEVELOPERS:  Stanislav Izmalkov <izstas@live.ru>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

using std::list;

#define OBJECT_SYNC_RATE 500

CObjectSync::CObjectSync ( CClientObjectManager* pObjectManager )
{
    m_pObjectManager = pObjectManager;
    m_ulLastSyncTime = 0;
}


bool CObjectSync::ProcessPacket ( unsigned char ucPacketID, NetBitStreamInterface& BitStream )
{
    switch ( ucPacketID )
    {
        case PACKET_ID_OBJECT_STARTSYNC:
        {
            Packet_ObjectStartSync ( BitStream );
            return true;
        }

        case PACKET_ID_OBJECT_STOPSYNC:
        {
            Packet_ObjectStopSync ( BitStream );
            return true;
        }

        case PACKET_ID_OBJECT_SYNC:
        {
            Packet_ObjectSync ( BitStream );
            return true;
        }
    }

    return false;
}


void CObjectSync::DoPulse ( void )
{
    // Has it been long enough since our last state's sync?
    unsigned long ulCurrentTime = CClientTime::GetTime ();
    if ( ulCurrentTime >= m_ulLastSyncTime + OBJECT_SYNC_RATE )
    {
        Sync ();
        m_ulLastSyncTime = ulCurrentTime;
    }
}


void CObjectSync::AddObject ( CDeathmatchObject* pObject )
{
    m_List.push_front ( pObject );
}


void CObjectSync::RemoveObject ( CDeathmatchObject* pObject )
{
    if ( !m_List.empty () ) m_List.remove ( pObject );
}


void CObjectSync::ClearObjects ( void )
{
    m_List.clear ();
}


bool CObjectSync::Exists ( CDeathmatchObject* pObject )
{
    return m_List.Contains ( pObject );
}


void CObjectSync::Packet_ObjectStartSync ( NetBitStreamInterface& BitStream )
{
    // Read out the ID
    ElementID ID;
    if ( BitStream.ReadCompressed ( ID ) )
    {
        // Grab the object
        CDeathmatchObject* pObject = static_cast < CDeathmatchObject* > ( m_pObjectManager->Get ( ID ) );
        if ( pObject )
        {
            // Read out the position and rotation
            SPositionSync position;
            SRotationRadiansSync rotation;
            if ( BitStream.Read ( &position ) && BitStream.Read ( &rotation ) )
            {
                pObject->SetOrientation ( position.data.vecPosition, rotation.data.vecRotation );
            }
            // No velocity due to issue #3522

            // Read out the health
            SObjectHealthSync health;
            if ( BitStream.Read ( &health ) )
            {
                pObject->SetHealth ( health.data.fValue );
            }

            AddObject ( pObject );
        }
    }
}


void CObjectSync::Packet_ObjectStopSync ( NetBitStreamInterface& BitStream )
{
    // Read out the ID
    ElementID ID;
    if ( BitStream.ReadCompressed ( ID ) )
    {
        // Grab the object
        CDeathmatchObject* pObject = static_cast < CDeathmatchObject* > ( m_pObjectManager->Get ( ID ) );
        if ( pObject )
        {
            RemoveObject ( pObject );
        }
    }
}


void CObjectSync::Packet_ObjectSync ( NetBitStreamInterface& BitStream )
{
    // While we're not out of bytes
    while ( BitStream.GetNumberOfUnreadBits () > 8 )
    {
        // Read out the ID
        ElementID ID;
        if ( !BitStream.ReadCompressed ( ID ) )
            return;

        // Read out the sync time context. See CClientEntity for documentation on that.
        unsigned char ucSyncTimeContext;
        if ( !BitStream.Read ( ucSyncTimeContext ) )
            return;
            
        // Read out flags
        SIntegerSync < unsigned char, 3 > flags ( 0 );
        if ( !BitStream.Read ( &flags ) )
            return;
                    
        // Read out the position if we need
        SPositionSync position;
        if ( flags & 0x1 )
        {
            if ( !BitStream.Read ( &position ) )
                return;
        }

        // Read out the rotation
        SRotationRadiansSync rotation;
        if ( flags & 0x2 )
        {
            if ( !BitStream.Read ( &rotation ) )
                return;
        }

        // Read out the health
        SObjectHealthSync health;
        if ( flags & 0x4 )
        {
            if ( !BitStream.Read ( &health ) )
                return;
        }

        // Grab the object
        CDeathmatchObject* pObject = static_cast < CDeathmatchObject* > ( m_pObjectManager->Get ( ID ) );
        // Only update the sync if this packet is from the same context
        if ( pObject && pObject->CanUpdateSync ( ucSyncTimeContext ) )
        {
            if ( flags & 0x1 ) pObject->SetPosition ( position.data.vecPosition );
            if ( flags & 0x2 ) pObject->SetRotationRadians ( rotation.data.vecRotation );
            if ( flags & 0x4 ) pObject->SetHealth ( health.data.fValue );
        }
    }
}


void CObjectSync::Sync ( void )
{
    // Got any items?
    if ( m_List.size () > 0 )
    {
        // Write each object to packet
        CBitStream bitStream;
        list < CDeathmatchObject* > ::const_iterator iter = m_List.begin ();
        for ( ; iter != m_List.end (); iter++ )
        {
            WriteObjectInformation ( bitStream.pBitStream, *iter );
        }

        // Send the packet
        g_pNet->SendPacket ( PACKET_ID_OBJECT_SYNC, bitStream.pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED );
    }
}


void CObjectSync::WriteObjectInformation ( NetBitStreamInterface* pBitStream, CDeathmatchObject* pObject )
{
    unsigned char ucFlags = 0;

    // What's changed?
    CVector vecPosition, vecRotation;
    pObject->GetPosition ( vecPosition );
    pObject->GetRotationRadians ( vecRotation );

    if ( vecPosition != pObject->m_LastSyncedData.vecPosition ) ucFlags |= 0x1;
    if ( vecRotation != pObject->m_LastSyncedData.vecRotation ) ucFlags |= 0x2;
    if ( pObject->GetHealth () != pObject->m_LastSyncedData.fHealth ) ucFlags |= 0x4;

    // Don't sync if nothing changed
    if ( ucFlags == 0 )
        return;

    // Write the ID
    pBitStream->WriteCompressed ( pObject->GetID () );

    // Write the sync time context
    pBitStream->Write ( pObject->GetSyncTimeContext () );

    // Write flags
    SIntegerSync < unsigned char, 3 > flags ( ucFlags );
    pBitStream->Write ( &flags );

    // Write changed stuff
    // Position
    if ( ucFlags & 0x1 )
    {
        SPositionSync position;
        pObject->GetPosition ( position.data.vecPosition );
        pBitStream->Write ( &position );
        pObject->m_LastSyncedData.vecPosition = position.data.vecPosition;
    }

    // Rotation
    if ( ucFlags & 0x2 )
    {
        SRotationRadiansSync rotation;
        pObject->GetRotationRadians ( rotation.data.vecRotation );
        pBitStream->Write ( &rotation );
        pObject->m_LastSyncedData.vecRotation = rotation.data.vecRotation;
    }

    // Health
    if ( ucFlags & 0x4 )
    {
        SObjectHealthSync health;
        health.data.fValue = pObject->GetHealth ();
        pBitStream->Write ( &health );
        pObject->m_LastSyncedData.fHealth = health.data.fValue;
    }
}
