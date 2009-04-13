/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CUnoccupiedVehicleSync.cpp
*  PURPOSE:     Unoccupied vehicle sync manager
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

extern CClientGame* g_pClientGame;

#define UNOCCUPIED_VEHICLE_SYNC_RATE 1000

CUnoccupiedVehicleSync::CUnoccupiedVehicleSync ( CClientVehicleManager* pVehicleManager )
{
    m_pVehicleManager = pVehicleManager;
    m_ulLastSyncTime = 0;
}


CUnoccupiedVehicleSync::~CUnoccupiedVehicleSync ( void )
{

}


bool CUnoccupiedVehicleSync::ProcessPacket ( unsigned char ucPacketID, NetBitStreamInterface& BitStream )
{
    switch ( ucPacketID )
    {
        case PACKET_ID_UNOCCUPIED_VEHICLE_STARTSYNC:
        {
            Packet_UnoccupiedVehicleStartSync ( BitStream );
            return true;
        }

        case PACKET_ID_UNOCCUPIED_VEHICLE_STOPSYNC:
        {
            Packet_UnoccupiedVehicleStopSync ( BitStream );
            return true;
        }

        case PACKET_ID_UNOCCUPIED_VEHICLE_SYNC:
        {
            Packet_UnoccupiedVehicleSync ( BitStream );
            return true;
        }
    }

    return false;
}


void CUnoccupiedVehicleSync::DoPulse ( void )
{
    // Check all our vehicles for damage
    UpdateDamageModels ();

    // Has it been long enough since our last state's sync?
    unsigned long ulCurrentTime = CClientTime::GetTime ();
    if ( ulCurrentTime >= m_ulLastSyncTime + UNOCCUPIED_VEHICLE_SYNC_RATE )
    {
        UpdateStates ();
        m_ulLastSyncTime = ulCurrentTime;
    }
}


void CUnoccupiedVehicleSync::AddVehicle ( CDeathmatchVehicle* pVehicle )
{
    m_List.push_front ( pVehicle );
    pVehicle->SetIsSyncing ( true );
}


void CUnoccupiedVehicleSync::RemoveVehicle ( CDeathmatchVehicle* pVehicle )
{
    if ( !m_List.empty() ) m_List.remove ( pVehicle );
    pVehicle->SetIsSyncing ( false );
}


void CUnoccupiedVehicleSync::ClearVehicles ( void )
{
    // Mark all vehicles as 'not syncing'
    list < CDeathmatchVehicle* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        (*iter)->SetIsSyncing ( false );
    }

    // Clear the list
    m_List.clear ();
}


bool CUnoccupiedVehicleSync::Exists ( CDeathmatchVehicle * pVehicle )
{
    list < CDeathmatchVehicle* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pVehicle )
            return true;
    }
    return false;
}


void CUnoccupiedVehicleSync::Packet_UnoccupiedVehicleStartSync ( NetBitStreamInterface& BitStream )
{
    // Read out the vehicle id
    ElementID ID;
    if ( BitStream.Read ( ID ) )
    {
        // Grab the vehicle
        CDeathmatchVehicle* pVehicle = static_cast < CDeathmatchVehicle* > ( m_pVehicleManager->Get ( ID ) );
        if ( pVehicle )
        {
            // Read out and set its current state, so we start off sync'd with the server
            
            // Read out the position
            CVector vecPosition;
            BitStream.Read ( vecPosition.fX );
            BitStream.Read ( vecPosition.fY );
            BitStream.Read ( vecPosition.fZ );

            // And rotation
            CVector vecRotationDegrees;
            BitStream.Read ( vecRotationDegrees.fX );
            BitStream.Read ( vecRotationDegrees.fY );
            BitStream.Read ( vecRotationDegrees.fZ );

            // And the move and turn speed
            CVector vecMoveSpeed;
            BitStream.Read ( vecMoveSpeed.fX );
            BitStream.Read ( vecMoveSpeed.fY );
            BitStream.Read ( vecMoveSpeed.fZ );

            CVector vecTurnSpeed;
            BitStream.Read ( vecTurnSpeed.fX );
            BitStream.Read ( vecTurnSpeed.fY );
            BitStream.Read ( vecTurnSpeed.fZ );

            // And health
            float fHealth;
            BitStream.Read ( fHealth );

            // Set data for interpolation
            pVehicle->SetTargetPosition ( vecPosition );
            pVehicle->SetTargetRotation ( vecRotationDegrees );
            pVehicle->SetMoveSpeed ( vecMoveSpeed );
            pVehicle->SetTurnSpeed ( vecTurnSpeed );

            // Set the new health
            pVehicle->SetHealth ( fHealth );

            // Start syncing it
            AddVehicle ( pVehicle );

#ifdef MTA_DEBUG
            pVehicle->m_pLastSyncer = g_pClientGame->GetLocalPlayer ();
            pVehicle->m_ulLastSyncTime = GetTickCount ();
            pVehicle->m_szLastSyncType = "unoccupied-start";
#endif
        }
    }
}


void CUnoccupiedVehicleSync::Packet_UnoccupiedVehicleStopSync ( NetBitStreamInterface& BitStream )
{
    // Read out the vehicle id
    ElementID ID;
    if ( BitStream.Read ( ID ) )
    {
        // Grab the vehicle
        CDeathmatchVehicle* pVehicle = static_cast < CDeathmatchVehicle* > ( m_pVehicleManager->Get ( ID ) );
        if ( pVehicle )
        {
            // Stop syncing it
            RemoveVehicle ( pVehicle );
        }
    }
}


void CUnoccupiedVehicleSync::Packet_UnoccupiedVehicleSync ( NetBitStreamInterface& BitStream )
{
    // While we're not out of vehicles
    while ( BitStream.GetNumberOfUnreadBits () > 32 )
    {
        // Read out the vehicle id
        ElementID ID;
        if ( BitStream.Read ( ID ) )
        {
            // Read out the sync time context. See CClientEntity for documentation on that.
            unsigned char ucSyncTimeContext = 0;
            BitStream.Read ( ucSyncTimeContext );

            unsigned short usFlags = 0;
            BitStream.Read ( usFlags );

            // Read out the position
            CVector vecPosition, vecRotationDegrees, vecMoveSpeed, vecTurnSpeed;
            float fHealth;

            if ( usFlags & 0x01 )
            {
                BitStream.Read ( vecPosition.fX );
                BitStream.Read ( vecPosition.fY );
                BitStream.Read ( vecPosition.fZ );
            }

            // And rotation
            if ( usFlags & 0x02 )
            {
                BitStream.Read ( vecRotationDegrees.fX );
                BitStream.Read ( vecRotationDegrees.fY );
                BitStream.Read ( vecRotationDegrees.fZ );
            }

            // And the move and turn speed
            if ( usFlags & 0x04 )
            {
                BitStream.Read ( vecMoveSpeed.fX );
                BitStream.Read ( vecMoveSpeed.fY );
                BitStream.Read ( vecMoveSpeed.fZ );
            }

            if ( usFlags & 0x08 )
            {
                BitStream.Read ( vecTurnSpeed.fX );
                BitStream.Read ( vecTurnSpeed.fY );
                BitStream.Read ( vecTurnSpeed.fZ );
            }

            // And health
            if ( usFlags & 0x10 )
            {
                BitStream.Read ( fHealth );
            }

            CClientVehicle* pVehicle = m_pVehicleManager->Get ( ID );
            if ( pVehicle && pVehicle->CanUpdateSync ( ucSyncTimeContext ) )
            {
                if ( usFlags & 0x01 )pVehicle->SetTargetPosition ( vecPosition );
                if ( usFlags & 0x02 ) pVehicle->SetTargetRotation ( vecRotationDegrees );
                if ( usFlags & 0x04 ) pVehicle->SetMoveSpeed ( vecMoveSpeed );
                if ( usFlags & 0x08 ) pVehicle->SetTurnSpeed ( vecTurnSpeed );
                if ( usFlags & 0x10 ) pVehicle->SetHealth ( fHealth );
                pVehicle->SetEngineOn ( ( usFlags & 0x40 ) ? true : false );
                if ( pVehicle->GetVehicleType() == CLIENTVEHICLE_TRAIN )
                    pVehicle->SetDerailed ( ( usFlags & 0x80 ) ? true : false );
                pVehicle->SetInWater( ( usFlags & 0x100 ) ? true : false );
#ifdef MTA_DEBUG
				pVehicle->m_pLastSyncer = NULL;
				pVehicle->m_ulLastSyncTime = GetTickCount ();
				pVehicle->m_szLastSyncType = "unoccupied";
#endif
            }
        }
        else break;
    }
}


void CUnoccupiedVehicleSync::UpdateDamageModels ( void )
{
    // Got any items?
    if ( m_List.size () > 0 )
    {
        list < CDeathmatchVehicle* > ::iterator iter = m_List.begin ();
        for ( ; iter != m_List.end (); iter++ )
        {
            // Sync its damage model changes if neccessary
            (*iter)->SyncDamageModel ();
        }           
    }
}


void CUnoccupiedVehicleSync::UpdateStates ( void )
{
    // Got any items?
    if ( m_List.size () > 0 )
    {
        // Create a packet
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
        if ( pBitStream )
        {
            // Write each vehicle to it
            list < CDeathmatchVehicle* > ::iterator iter = m_List.begin ();
            for ( ; iter != m_List.end (); iter++ )
            {
                WriteVehicleInformation ( pBitStream, *iter );
            }

            // Send and destroy the packet
            g_pNet->SendPacket ( PACKET_ID_UNOCCUPIED_VEHICLE_SYNC, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED );
            g_pNet->DeallocateNetBitStream ( pBitStream );
        }
    }
}


void CUnoccupiedVehicleSync::WriteVehicleInformation ( NetBitStreamInterface* pBitStream, CDeathmatchVehicle* pVehicle )
{
    CVector vecPosition, vecRotation, vecMoveSpeed, vecTurnSpeed;
    pVehicle->GetPosition ( vecPosition );
    pVehicle->GetRotationDegrees ( vecRotation );
    if ( pVehicle->GetGameVehicle () )
    {
        pVehicle->GetMoveSpeed ( vecMoveSpeed );
        pVehicle->GetTurnSpeed ( vecTurnSpeed );
    }

    ElementID Trailer = NULL;
    CClientVehicle* pTrailer = pVehicle->GetRealTowedVehicle ();
    if ( pTrailer )
        Trailer = pTrailer->GetID ();
    else
        Trailer = static_cast < ElementID > ( INVALID_ELEMENT_ID );

    unsigned short usFlags = 0;
    if ( pVehicle->m_LastSyncedData->vecPosition != vecPosition ) usFlags |= 0x01;
    if ( pVehicle->m_LastSyncedData->vecRotation != vecRotation ) usFlags |= 0x02;
    if ( pVehicle->m_LastSyncedData->vecMoveSpeed != vecMoveSpeed ) usFlags |= 0x04;
    if ( pVehicle->m_LastSyncedData->vecTurnSpeed != vecTurnSpeed ) usFlags |= 0x08;
    if ( pVehicle->m_LastSyncedData->fHealth != pVehicle->GetHealth() ) usFlags |= 0x010;
    if ( pVehicle->m_LastSyncedData->Trailer != Trailer ) usFlags |= 0x020;
    if ( pVehicle->IsEngineOn () ) usFlags |= 0x040;
    if ( pVehicle->IsDerailed () ) usFlags |= 0x080;
    if ( pVehicle->IsOnWater () ) usFlags |= 0x100; 

    // If nothing has changed we dont sync the vehicle
    if ( usFlags == 0 ) return;

    // Write the vehicle id
    pBitStream->Write ( pVehicle->GetID () );

    // Write the sync time context
    pBitStream->Write ( pVehicle->GetSyncTimeContext () );

    // Write flags
    pBitStream->Write ( usFlags );

    // Write it
    if ( usFlags & 0x01 )
    {
        pBitStream->Write ( vecPosition.fX );
        pBitStream->Write ( vecPosition.fY );
        pBitStream->Write ( vecPosition.fZ );
        pVehicle->m_LastSyncedData->vecPosition = vecPosition;
    }

    if ( usFlags & 0x02 )
    {
        pBitStream->Write ( vecRotation.fX );
        pBitStream->Write ( vecRotation.fY );
        pBitStream->Write ( vecRotation.fZ );
        pVehicle->m_LastSyncedData->vecRotation = vecRotation;
    }

    // Write movespeed
    if ( usFlags & 0x04 )
    {
        pBitStream->Write ( vecMoveSpeed.fX );
        pBitStream->Write ( vecMoveSpeed.fY );
        pBitStream->Write ( vecMoveSpeed.fZ );
        pVehicle->m_LastSyncedData->vecMoveSpeed = vecMoveSpeed;
    }

    if ( usFlags & 0x08 )
    {
        pBitStream->Write ( vecTurnSpeed.fX );
        pBitStream->Write ( vecTurnSpeed.fY );
        pBitStream->Write ( vecTurnSpeed.fZ );
        pVehicle->m_LastSyncedData->vecTurnSpeed = vecTurnSpeed;
    }

    // And health
    if ( usFlags & 0x10 )
    {
        pBitStream->Write ( pVehicle->GetHealth () );
        pVehicle->m_LastSyncedData->fHealth = pVehicle->GetHealth();
    }

    // And trailer
    if ( usFlags & 0x20 )
    {
        pBitStream->Write ( Trailer );
        pVehicle->m_LastSyncedData->Trailer = Trailer;
    }
}
