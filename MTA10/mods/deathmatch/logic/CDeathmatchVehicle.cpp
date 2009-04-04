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

#include <StdInc.h>

CDeathmatchVehicle::CDeathmatchVehicle ( CClientManager* pManager, CUnoccupiedVehicleSync* pUnoccupiedVehicleSync, ElementID ID, unsigned short usVehicleModel ) : CClientVehicle ( pManager, ID, usVehicleModel )
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
    // Grab the door status
    unsigned char ucDoorStates [ MAX_DOORS ], ucWheelStates [ MAX_WHEELS ],
                  ucPanelStates [ MAX_PANELS ], ucLightStates [ MAX_LIGHTS ];

    for ( int i = 0; i < MAX_DOORS; i++ ) ucDoorStates [i] = GetDoorStatus ( i );
    for ( int i = 0; i < MAX_WHEELS; i++ ) ucWheelStates [i] = GetWheelStatus ( i );
    for ( int i = 0; i < MAX_PANELS; i++ ) ucPanelStates [i] = GetPanelStatus ( i );
    for ( int i = 0; i < MAX_LIGHTS; i++ ) ucLightStates [i] = GetLightStatus ( i );

    // Something has changed?
    if ( memcmp ( m_ucLastDoorStates, ucDoorStates, sizeof ( m_ucLastDoorStates ) ) != 0 ||
         memcmp ( m_ucLastWheelStates, ucWheelStates, sizeof ( m_ucLastWheelStates ) ) != 0 ||
         memcmp ( m_ucLastPanelStates, ucPanelStates, sizeof ( m_ucLastPanelStates ) ) != 0 ||
         memcmp ( m_ucLastLightStates, ucLightStates, sizeof ( m_ucLastLightStates ) ) != 0 )
    {
        // Set the last state to current
        memcpy ( m_ucLastDoorStates, ucDoorStates, sizeof ( m_ucLastDoorStates ) );
        memcpy ( m_ucLastWheelStates, ucWheelStates, sizeof ( m_ucLastWheelStates ) );
        memcpy ( m_ucLastPanelStates, ucPanelStates, sizeof ( m_ucLastPanelStates ) );
        memcpy ( m_ucLastLightStates, ucLightStates, sizeof ( m_ucLastLightStates ) );

        // Sync it
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
        if ( pBitStream )
        {
            // Write the vehicle id and the damage model data
            pBitStream->Write ( m_ID );
            pBitStream->Write ( (char*) ucDoorStates, MAX_DOORS );
            pBitStream->Write ( (char*) ucWheelStates, MAX_WHEELS );
            pBitStream->Write ( (char*) ucPanelStates, MAX_PANELS );
            pBitStream->Write ( (char*) ucLightStates, MAX_LIGHTS );

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
