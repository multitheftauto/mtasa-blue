/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDeathmatchVehicle.cpp
*  PURPOSE:     Vehicle element interface
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

CDeathmatchVehicle::CDeathmatchVehicle ( CClientManager* pManager, CUnoccupiedVehicleSync* pUnoccupiedVehicleSync, ElementID ID, unsigned short usVehicleModel ) : ClassInit ( this ), CClientVehicle ( pManager, ID, usVehicleModel )
{
    m_pUnoccupiedVehicleSync = pUnoccupiedVehicleSync;
    GetInitialDoorStates ( m_ucLastDoorStates );
    memset ( m_ucLastWheelStates, 0, sizeof ( m_ucLastWheelStates ) );
    memset ( m_ucLastPanelStates, 0, sizeof ( m_ucLastPanelStates ) );
    memset ( m_ucLastLightStates, 0, sizeof ( m_ucLastLightStates ) );
    m_bIsSyncing = false;

    SetIsSyncing ( false );
}


CDeathmatchVehicle::~CDeathmatchVehicle ( void )
{
    if ( m_bIsSyncing && m_pUnoccupiedVehicleSync )
    {
        m_pUnoccupiedVehicleSync->RemoveVehicle ( this );
    }
}


void CDeathmatchVehicle::SetIsSyncing ( bool bIsSyncing )
{
    m_bIsSyncing = bIsSyncing;
    SetSyncUnoccupiedDamage( m_bIsSyncing );
}


bool CDeathmatchVehicle::SyncDamageModel ( void )
{
    SVehicleDamageSync damage ( true, true, true, true, true );
    bool bChanges = false;

    // Copy current door states to the sync structure and mark those that changed
    for ( unsigned int i = 0; i < MAX_DOORS; ++i )
    {
        damage.data.ucDoorStates [ i ] = GetDoorStatus ( i );
        if ( damage.data.ucDoorStates [ i ] != m_ucLastDoorStates [ i ] )
        {
            bChanges = true;
            damage.data.bDoorStatesChanged [ i ] = true;
        }
        else
            damage.data.bDoorStatesChanged [ i ] = false;
    }
    // Copy current wheel states to the sync structure and mark those that changed
    for ( unsigned int i = 0; i < MAX_WHEELS; ++i )
    {
        damage.data.ucWheelStates [ i ] = GetWheelStatus ( i );
        if ( damage.data.ucWheelStates [ i ] != m_ucLastWheelStates [ i ] )
        {
            bChanges = true;
            damage.data.bWheelStatesChanged [ i ] = true;
        }
        else
            damage.data.bWheelStatesChanged [ i ] = false;
    }
    // Copy current panel states to the sync structure and mark those that changed
    for ( unsigned int i = 0; i < MAX_PANELS; ++i )
    {
        damage.data.ucPanelStates [ i ] = GetPanelStatus ( i );
        if ( damage.data.ucPanelStates [ i ] != m_ucLastPanelStates [ i ] )
        {
            bChanges = true;
            damage.data.bPanelStatesChanged [ i ] = true;
        }
        else
            damage.data.bPanelStatesChanged [ i ] = false;
    }
    // Copy current light states to the sync structure and mark those that changed
    for ( unsigned int i = 0; i < MAX_LIGHTS; ++i )
    {
        damage.data.ucLightStates [ i ] = GetLightStatus ( i );
        if ( damage.data.ucLightStates [ i ] != m_ucLastLightStates [ i ] )
        {
            bChanges = true;
            damage.data.bLightStatesChanged [ i ] = true;
        }
        else
            damage.data.bLightStatesChanged [ i ] = false;
    }

    // Something has changed?
    if ( bChanges )
    {
        // Set the last state to current
        memcpy ( m_ucLastDoorStates,  damage.data.ucDoorStates,  sizeof ( m_ucLastDoorStates ) );
        memcpy ( m_ucLastWheelStates, damage.data.ucWheelStates, sizeof ( m_ucLastWheelStates ) );
        memcpy ( m_ucLastPanelStates, damage.data.ucPanelStates, sizeof ( m_ucLastPanelStates ) );
        memcpy ( m_ucLastLightStates, damage.data.ucLightStates, sizeof ( m_ucLastLightStates ) );

        // Sync it
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
        if ( pBitStream )
        {
            // Write the vehicle id and the damage model data
            pBitStream->Write ( m_ID );
            pBitStream->Write ( &damage );

            // Send and delete it
            g_pNet->SendPacket ( PACKET_ID_VEHICLE_DAMAGE_SYNC, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE_ORDERED );
            g_pNet->DeallocateNetBitStream ( pBitStream );
        }

        return true;
    }

    return false;
}


void CDeathmatchVehicle::ResetDamageModelSync ( void )
{
    for ( int i = 0; i < MAX_DOORS; i++ ) m_ucLastDoorStates [i] = GetDoorStatus ( i );
    for ( int i = 0; i < MAX_WHEELS; i++ ) m_ucLastWheelStates [i] = GetWheelStatus ( i );
    for ( int i = 0; i < MAX_PANELS; i++ ) m_ucLastPanelStates [i] = GetPanelStatus ( i );
    for ( int i = 0; i < MAX_LIGHTS; i++ ) m_ucLastLightStates [i] = GetLightStatus ( i );
}
