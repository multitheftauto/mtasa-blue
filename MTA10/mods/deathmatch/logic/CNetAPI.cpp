/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CNetAPI.cpp
*  PURPOSE:     Packet processing class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Kent Simon <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include <net/SyncStructures.h>

extern CClientGame* g_pClientGame;


CNetAPI::CNetAPI ( CClientManager* pManager )
{
    // Init
    m_pManager = pManager;
    m_pPlayerManager = pManager->GetPlayerManager ();
    m_pVehicleManager = pManager->GetVehicleManager ();

    m_ulLastPuresyncTime = 0;
    m_ulLastSyncReturnTime = 0;
    m_bLastSentCameraMode = true;       // start out in fixed mode
    m_pLastSentCameraTarget = NULL;
    m_ulLastCameraSyncTime = 0;
    m_bStoredReturnSync = false;
}


bool CNetAPI::ProcessPacket ( unsigned char bytePacketID, NetBitStreamInterface& BitStream ) 
{
    switch ( bytePacketID )
    { 
        case PACKET_ID_PLAYER_PURESYNC:
        {
            // Read out the player ID
            ElementID PlayerID;
            if ( BitStream.ReadCompressed ( PlayerID ) )
            {
                // Grab the player
                CClientPlayer* pPlayer = m_pPlayerManager->Get ( PlayerID );
                if ( pPlayer )
                {
                    // Read out and apply the puresync data
                    ReadPlayerPuresync ( pPlayer, BitStream );
                }
            }

            return true;
        }

        case PACKET_ID_PLAYER_VEHICLE_PURESYNC:
        {
            // Read out the player ID
            ElementID PlayerID;
            if ( BitStream.Read ( PlayerID ) )
            {
                // Grab the player
                CClientPlayer* pPlayer = m_pPlayerManager->Get ( PlayerID );
                if ( pPlayer )
                {
                    // Grab the player vehicle
                    CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
                    if ( pVehicle ) 
                    {
                        // Read out the vehicle puresync data
                        ReadVehiclePuresync ( pPlayer, pVehicle, BitStream );
                    }
                }
            }

            return true;
        }

        case PACKET_ID_PLAYER_KEYSYNC:
        {
            // Read out the player ID
            ElementID PlayerID;
            if ( BitStream.ReadCompressed ( PlayerID ) )
            {
                // Grab the player
                CClientPlayer* pPlayer = m_pPlayerManager->Get ( PlayerID );
                if ( pPlayer )
                {
                    // Read out the keysync data
                    ReadKeysync ( pPlayer, BitStream );
                }
            }

            return true;
        }
        
        case PACKET_ID_RETURN_SYNC:
        {
            // Grab the in vehicle flag
            bool bInVehicle = BitStream.ReadBit ();

            // Are we in a vehicle?
            if ( bInVehicle )
            {
                CClientVehicle * pVehicle = m_pPlayerManager->GetLocalPlayer ()->GetOccupiedVehicle ();
#ifdef MTA_DEBUG
                if ( m_pPlayerManager->GetLocalPlayer ()->GetOccupiedVehicleSeat () == 0 )
                {               
                    if ( pVehicle )
                    {
                        pVehicle->m_pLastSyncer = m_pPlayerManager->GetLocalPlayer ();
                        pVehicle->m_ulLastSyncTime = GetTickCount ();
                        pVehicle->m_szLastSyncType = "pure";
                    }
                }
#endif

                // Read out position
                CVector vecPosition;
                BitStream.Read ( vecPosition.fX );
                BitStream.Read ( vecPosition.fY );
                BitStream.Read ( vecPosition.fZ );

                // And rotation
                CVector vecRotationDegrees;
                BitStream.Read ( vecRotationDegrees.fX );
                BitStream.Read ( vecRotationDegrees.fY );
                BitStream.Read ( vecRotationDegrees.fZ );

                // Remember that position
                m_vecLastReturnPosition = vecPosition;
                m_vecLastReturnRotation = vecRotationDegrees;
                m_bVehicleLastReturn = true;
            }
            else
            {
                // Read out the position
                CVector vecPosition;
                BitStream.Read ( vecPosition.fX );
                BitStream.Read ( vecPosition.fY );
                BitStream.Read ( vecPosition.fZ );
                
                /* Test code:
                CClientPlayer* pLocalPlayer = m_pPlayerManager->GetLocalPlayer ();
                CVector vecCurrentPosition;
                pLocalPlayer->GetPosition ( vecCurrentPosition );
                // Are we 'x' distance away from our server position?
                if ( DistanceBetweenPoints3D ( vecCurrentPosition, vecPosition ) > 5.0f )
                {
                    // Move us back to our server position to stay in-sync
                    pLocalPlayer->SetPosition ( vecPosition );
                }*/

                // Remember that position
                m_vecLastReturnPosition = vecPosition;
                m_vecLastReturnRotation = CVector ( 0.0f, 0.0f, 0.0f );
                m_bVehicleLastReturn = false;
            }

            // Remember the last return sync time
            m_ulLastSyncReturnTime = CClientTime::GetTime ();
            m_bStoredReturnSync = true;

            return true;
        }
    }

    // We couldn't handle it
    return false;
}


void CNetAPI::ResetReturnPosition ( void )
{
    m_bStoredReturnSync = false;

    CClientPlayer* pPlayer = m_pPlayerManager->GetLocalPlayer ();
    if ( pPlayer )
    {
        m_bStoredReturnSync = true;
        m_ulLastSyncReturnTime = CClientTime::GetTime ();
        m_ulLastPuresyncTime = 0;

        CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            pVehicle->GetPosition ( m_vecLastReturnPosition );
            pVehicle->GetRotationDegrees ( m_vecLastReturnRotation );
            m_bVehicleLastReturn = true;
        }
        else
        {
            pPlayer->GetPosition ( m_vecLastReturnPosition );
            m_vecLastReturnRotation = CVector ( 0.0f, 0.0f, 0.0f );
            m_bVehicleLastReturn = false;
        }
    }
}


void CNetAPI::AddInterpolation ( const CVector& vecPosition )
{
    // Store our current position for interpolation purposes
    unsigned long ulCurrentTime = CClientTime::GetTime ();
    m_Interpolator.Push ( vecPosition.fX, vecPosition.fY, vecPosition.fZ, ulCurrentTime );
    CVector vecTemp;
    if ( m_Interpolator.GetOldestEntry ( vecTemp ) < ulCurrentTime - 2500 )
    {
        m_Interpolator.Pop ();
    }
}


void CNetAPI::GetInterpolation ( CVector& vecPosition, unsigned short usLatency )
{
    unsigned long ulInterTime = CClientTime::GetTime () - usLatency;
    m_Interpolator.Evaluate ( ulInterTime, vecPosition );
}


bool CNetAPI::IsWeaponIDAkimbo ( unsigned char ucWeaponID )
{
	return ( ucWeaponID == 22 ||
		     ucWeaponID == 26 ||
			 ucWeaponID == 28 ||
			 ucWeaponID == 32 );
}


bool CNetAPI::IsDriveByWeapon ( unsigned char ucID )
{
    return ( ( ucID >= 22 && ucID <= 33 ) || ucID == 38 );
}


void CNetAPI::DoPulse ( void )
{
    // If we're ingame
    if ( m_pManager->IsGameLoaded () )
    {
        // Grab the local player
        CClientPlayer* pPlayer = m_pPlayerManager->GetLocalPlayer ();
        if ( pPlayer && !pPlayer->IsDeadOnNetwork () )
        {
            unsigned long ulCurrentTime = CClientTime::GetTime ();

            // Grab the player vehicle
            CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();

			// Record local data in the packet recorder
			m_pManager->GetPacketRecorder ()->RecordLocalData ( pPlayer );

            // We should do a puresync?
            if ( IsPureSyncNeeded () )
            {
                // Are in a vehicle?
                if ( pVehicle )
                {
                    // Send a puresync packet
                    NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
                    if ( pBitStream )
                    {
                        // Write our data
                        WriteVehiclePuresync ( pPlayer, pVehicle, *pBitStream );

                        // Send the packet and destroy it
                        g_pNet->SendPacket( PACKET_ID_PLAYER_VEHICLE_PURESYNC, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED, PACKET_ORDERING_GAME );
                        g_pNet->DeallocateNetBitStream ( pBitStream );
                    }

                    // Sync its damage model too
                    static_cast < CDeathmatchVehicle* > ( pVehicle )->SyncDamageModel ();
                }
                else
                {
                    // Send a puresync only if we're not doing anything vehicle related
                    // except getting in and jacking.
                    if ( pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_NONE ||
                         pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_GETTING_IN ||
                         pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_JACKING )
                    {                             
                        // Send a puresync packet
                        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
                        if ( pBitStream )
                        {
                            // Write our data
                            WritePlayerPuresync ( pPlayer, *pBitStream );

                            // Send the packet and destroy it
                            g_pNet->SendPacket( PACKET_ID_PLAYER_PURESYNC, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED, PACKET_ORDERING_GAME );
                            g_pNet->DeallocateNetBitStream ( pBitStream );
                        }
                    }
                }
            }
            else
            {
                // We should do a keysync?
                if ( IsSmallKeySyncNeeded ( pPlayer ) )
                {
                    // Send a keysync packet
                    NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
                    if ( pBitStream )
                    {
                        // Write the generic keysync data
                        WriteKeysync ( pPlayer, *pBitStream );

                        // Send the packet
                        g_pNet->SendPacket ( PACKET_ID_PLAYER_KEYSYNC, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED );
                        g_pNet->DeallocateNetBitStream ( pBitStream );
                    }
                }
            }

            if ( IsCameraSyncNeeded () )
            {
                // Send a camera-sync packet
                NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
                if ( pBitStream )
                {
                    // Write our data
                    WriteCameraSync ( *pBitStream );

                    // Send the packet and destroy it
                    g_pNet->SendPacket ( PACKET_ID_CAMERA_SYNC, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE_ORDERED, PACKET_ORDERING_GAME );
                    g_pNet->DeallocateNetBitStream ( pBitStream );
                }
            }

            // Time to freeze because of lack of return sync?
            if ( ( m_bStoredReturnSync ) &&
                    ( m_ulLastPuresyncTime != 0 ) &&
                    ( m_ulLastSyncReturnTime != 0 ) &&
                    ( ulCurrentTime <= m_ulLastPuresyncTime + 5000 ) &&
                    ( ulCurrentTime >= m_ulLastSyncReturnTime + 10000 ) &&
                    ( !g_pClientGame->IsGettingIntoVehicle () ) )
            {
                // No vehicle or vehicle in seat 0?
                if ( !pVehicle || pPlayer->GetOccupiedVehicleSeat () == 0 )
                {
                    // Disable our controls (swap with a blank controller state)
                    g_pCore->GetGame ()->GetPad ()->SetCurrentControllerState ( &CControllerState () );

                    // Grab our vehicle
                    CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
                    if ( pVehicle )
                    {
                        // Freeze us at the last position
                        pVehicle->SetMoveSpeed ( CVector ( 0, 0, 0 ) );
                        pVehicle->SetTurnSpeed ( CVector ( 0, 0, 0 ) );
                        if ( m_bVehicleLastReturn )
                        {
                            pVehicle->SetPosition ( m_vecLastReturnPosition );
                            pVehicle->SetRotationDegrees ( m_vecLastReturnRotation );
                        }
                    }
                    else
                    {
                        // Freeze us at the last position
                        pPlayer->SetMoveSpeed ( CVector ( 0, 0, 0 ) );
                        pPlayer->SetPosition ( m_vecLastReturnPosition );
                    }
                }

                // Display network trouble
                m_pManager->GetDisplayManager ()->DrawText2D ( "*** NETWORK TROUBLE ***", CVector ( 0.30f, 0.45f, 0 ), 2.0f, 0xFFFF0000 );
            }
        }
    }
}


bool CNetAPI::IsSmallKeySyncNeeded ( CClientPed* pPlayerModel )
{
    // Grab the current and the old controllerstate
    CControllerState ControllerState, LastControllerState;
    pPlayerModel->GetControllerState ( ControllerState );
    pPlayerModel->GetLastControllerState ( LastControllerState );

    // Compare the parts we sync
    return ( ControllerState.LeftShoulder1 != LastControllerState.LeftShoulder1 ||
             ControllerState.RightShoulder1 != LastControllerState.RightShoulder1 ||
             ControllerState.ButtonSquare != LastControllerState.ButtonSquare ||
             ControllerState.ButtonCross != LastControllerState.ButtonCross ||
             ControllerState.ButtonCircle != LastControllerState.ButtonCircle ||
             ControllerState.ButtonTriangle != LastControllerState.ButtonTriangle ||
             ControllerState.ShockButtonL != LastControllerState.ShockButtonL ||
             ControllerState.m_bPedWalk != LastControllerState.m_bPedWalk ||
             ControllerState.LeftStickX != LastControllerState.LeftStickX ||
             ControllerState.LeftStickY != LastControllerState.LeftStickY );
}


bool CNetAPI::IsPureSyncNeeded ( void )
{
    unsigned long ulCurrentTime = CClientTime::GetTime ();
    if ( ulCurrentTime >= m_ulLastPuresyncTime + TICK_RATE )
    {
        m_ulLastPuresyncTime = ulCurrentTime;
        return true;
    }

    return false;
}


bool CNetAPI::IsCameraSyncNeeded ()
{
    CClientCamera * pCamera = m_pManager->GetCamera ();
    if ( pCamera->IsInFixedMode () )
    {
        CVector vecPosition, vecLookAt;
        pCamera->GetPosition ( vecPosition );
        pCamera->GetTarget ( vecLookAt );
        // Is the camera at a different place?
        if ( m_vecLastSentCameraPosition != vecPosition || m_vecLastSentCameraLookAt != vecLookAt )
        {
            // Has it been long enough since our last sync?
            unsigned long ulCurrentTime = CClientTime::GetTime ();
            if ( ulCurrentTime >= m_ulLastCameraSyncTime + CAM_SYNC_RATE )
            {
                m_ulLastCameraSyncTime = ulCurrentTime;
                m_bLastSentCameraMode = true;
                m_vecLastSentCameraPosition = vecPosition;
                m_vecLastSentCameraLookAt = vecLookAt;
                
                return true;
            }
        }
    }
    else
    {
        // We're in player mode.
        if ( m_bLastSentCameraMode == true ||
             pCamera->GetFocusedPlayer () != m_pLastSentCameraTarget )
        {
            // Something changed (mode has become "player", or different target)
            // Has it been long enough since our last sync?
            unsigned long ulCurrentTime = CClientTime::GetTime ();
            if ( ulCurrentTime >= m_ulLastCameraSyncTime + CAM_SYNC_RATE )
            {
                m_ulLastCameraSyncTime = ulCurrentTime;
                m_bLastSentCameraMode = false;
                m_pLastSentCameraTarget = pCamera->GetFocusedPlayer ();
                return true;
            }
        }
    }

    return false;
}

void CNetAPI::ReadKeysync ( CClientPlayer* pPlayer, NetBitStreamInterface& BitStream )
{
    // Read out the time it took for the packet to go from the remote client to the server and to us
    unsigned short usLatency;
    BitStream.ReadCompressed ( usLatency );
    pPlayer->SetLatency ( usLatency + g_pNet->GetPing () );

    // Grab the old controller state
    CControllerState LastControllerState;
    pPlayer->GetLastControllerState ( LastControllerState );

    // Read out the small keysync data
    CControllerState ControllerState;
    ReadSmallKeysync ( ControllerState, LastControllerState, BitStream );

    // Flags
    SKeysyncFlags flags;
    BitStream.Read ( &flags );

    // Grab the occupied vehicle
    CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
    bool bSyncingVehicle = pVehicle && pPlayer->GetOccupiedVehicleSeat () == 0;

    // If he's shooting
    if ( ControllerState.ButtonCircle )
    {
        // Read out his current weapon slot
        SWeaponSlotSync slot;
        BitStream.Read ( &slot );
        unsigned int uiSlot = slot.data.uiSlot;

        if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {
            CWeapon* pWeapon = pPlayer->GetWeapon ( static_cast < eWeaponSlot > ( uiSlot ) );
            unsigned char ucCurrentWeaponType = 0;
            float fWeaponRange = 1.6f;

            if ( pWeapon )
            {
                ucCurrentWeaponType = pWeapon->GetType ();
                fWeaponRange = pWeapon->GetInfo ()->GetWeaponRange ();
            }

            // Is the current weapon a goggle (44 or 45) or a camera (43), detonator (40), don't apply the fire key
            if ( ucCurrentWeaponType == 44 || ucCurrentWeaponType == 45 || ucCurrentWeaponType == 43 || ucCurrentWeaponType == 40 )
            {
                ControllerState.ButtonCircle = 0;
            }

            // Read out the weapon ammo
            SWeaponAmmoSync ammo ( ucCurrentWeaponType, false, true );
            BitStream.Read ( &ammo );
            unsigned short usWeaponAmmo = ammo.data.usAmmoInClip;

            // Valid current weapon id? Add it to the change weapon queue
            if ( CClientPickupManager::IsValidWeaponID ( ucCurrentWeaponType ) )
            {
                if ( pVehicle )
                {
                    // TODO?
                }
                else
                {
                    pPlayer->AddChangeWeapon ( TICK_RATE, static_cast < eWeaponSlot > ( uiSlot ), usWeaponAmmo );
                }
            }
            else
            {
                if ( pVehicle )
                {
                    pPlayer->RemoveAllWeapons ();
                }
                else
                {
                    pPlayer->AddChangeWeapon ( TICK_RATE, WEAPONSLOT_TYPE_UNARMED, 1 );
                }
            }

			// Make sure that if he doesn't have an akimbo weapon his hands up state is false
			if ( !IsWeaponIDAkimbo ( ucCurrentWeaponType ) )
			{
                flags.data.bAkimboTargetUp = false;
			}

            // Read out the aim directions
            float fArmX, fArmY;
			BitStream.Read ( fArmX );
			BitStream.Read ( fArmY );

            // Read the weapon aim data
            SWeaponAimSync aim ( fWeaponRange );
            BitStream.Read ( &aim );

            // Read out the driveby direction
            unsigned char ucDriveByAim;
            BitStream.Read ( ucDriveByAim );

            // Set the aim data (immediately if in vehicle, otherwize delayed/interpolated)
            if ( pVehicle )
            {
                pPlayer->SetAimingData ( TICK_RATE, aim.data.vecTarget, fArmX, fArmY, ucDriveByAim, &(aim.data.vecOrigin), false );
            }
            else
            {
                pPlayer->SetTargetTarget ( TICK_RATE, aim.data.vecOrigin, aim.data.vecTarget );
                pPlayer->SetAimInterpolated ( TICK_RATE, fArmX, fArmY, flags.data.bAkimboTargetUp, ucDriveByAim );
            }
        }
        else if ( uiSlot != 0 )
        {
            pPlayer->AddChangeWeapon ( TICK_RATE, static_cast < eWeaponSlot > ( uiSlot ), 1 );
        }
        else
        {
            pPlayer->SetCurrentWeaponSlot ( WEAPONSLOT_TYPE_UNARMED );
        }
    }

    // Are we syncing a vehicle?
    if ( bSyncingVehicle )
    {
        // Eventually read vehicle specific keysync data
        ReadSmallVehicleSpecific ( pVehicle, BitStream );

        if ( pVehicle->GetUpgrades ()->HasUpgrade ( 1087 ) ) // Hydraulics?
        {
            short sRightStickX, sRightStickY;
            BitStream.Read ( sRightStickX );
            BitStream.Read ( sRightStickY );

            ControllerState.RightStickX = sRightStickX;
            ControllerState.RightStickY = sRightStickY;
        }

        // Jax: temp fix for rhino firing, CPlayerInfo::m_LastTimeBigGunFired needs to be context-switched
        if ( pVehicle->GetModel () == VT_RHINO )
        {
            ControllerState.ButtonCircle = 0;
        }

        // Apply the new keysync data immediately
        pPlayer->SetControllerState ( ControllerState );
        pPlayer->SetChoking ( false );
    }
    else
    {
        // null out the crouch key or it will conflict with the crouch syncing
        ControllerState.ShockButtonL = 0;
        pPlayer->SetControllerState ( ControllerState );
        pPlayer->Duck ( flags.data.bIsDucked );   
        pPlayer->SetChoking ( flags.data.bIsChoking );       
    }

    // Increment keysync counter
    pPlayer->IncrementKeySync ();
}


void CNetAPI::WriteKeysync ( CClientPed* pPlayerModel, NetBitStreamInterface& BitStream )
{
    // Grab the local controllerstates
    CControllerState ControllerState;
    CControllerState LastControllerState;
    pPlayerModel->GetControllerState ( ControllerState );
    pPlayerModel->GetLastControllerState ( LastControllerState );

    // Write them to the bitstream
    WriteSmallKeysync ( ControllerState, LastControllerState, BitStream );

    // Flags
    SKeysyncFlags flags;
    flags.data.bIsDucked        = ( pPlayerModel->IsDucked () == true );
    flags.data.bIsChoking       = ( pPlayerModel->IsChoking () == true );
    flags.data.bAkimboTargetUp  = ( g_pMultiplayer->GetAkimboTargetUp () == true );

    // Write the flags
    BitStream.Write ( &flags );

    // Are we shooting?
    if ( ControllerState.ButtonCircle )
    {
        // Grab the current weapon
        CWeapon * pPlayerWeapon = pPlayerModel->GetWeapon ();
        if ( pPlayerWeapon )
        {
            BitStream.WriteBit ( true );

            // Write the type
            unsigned int uiSlot = pPlayerWeapon->GetSlot ();
            SWeaponSlotSync slot;
            slot.data.uiSlot = uiSlot;
            BitStream.Write ( &slot );

            if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
            {
                 // Write the clip ammo
                SWeaponAmmoSync ammo ( pPlayerWeapon->GetType (), false, true );
                ammo.data.usAmmoInClip = pPlayerWeapon->GetAmmoInClip ();
                BitStream.Write ( &ammo );

                // Grab his aim directions and sync them if he's not using an akimbo
				CShotSyncData* pShotsyncData = g_pMultiplayer->GetLocalShotSyncData ();
				BitStream.Write ( pShotsyncData->m_fArmDirectionX );
				BitStream.Write ( pShotsyncData->m_fArmDirectionY );

                // Write the aim data
                SWeaponAimSync aim ( pPlayerWeapon->GetInfo ()->GetWeaponRange () );
                pPlayerModel->GetShotData ( &aim.data.vecOrigin, &aim.data.vecTarget );
                BitStream.Write ( &aim );

                // Write the driveby direction
                BitStream.Write ( pShotsyncData->m_cInVehicleAimDirection );
            }
        }
        else
        {
            BitStream.WriteBit ( false );
        }
    }

    // Grab the occupied vehicle. Eventually write vehicle specific stuff.
    CClientVehicle* pVehicle = pPlayerModel->GetOccupiedVehicle ();
    if ( pVehicle && pPlayerModel->GetOccupiedVehicleSeat () == 0 )
    {
        WriteSmallVehicleSpecific ( pVehicle, BitStream );

        // Jax: pVehicle was corrupted here (seemingly a global crash)
        CVehicleUpgrades * pUpgrades = pVehicle->GetUpgrades ();
        if ( pUpgrades )
        {
            if ( pUpgrades->HasUpgrade ( 1087 ) ) // Hydraulics?
            {
                BitStream.Write ( ControllerState.RightStickX );
                BitStream.Write ( ControllerState.RightStickY );
            }
        }
    }
}


void CNetAPI::ReadPlayerPuresync ( CClientPlayer* pPlayer, NetBitStreamInterface& BitStream )
{
    // Read out the sync time context. See CClientEntity for documentation on that.
    unsigned char ucSyncTimeContext = 0;
    BitStream.Read ( ucSyncTimeContext );

    // Only update the sync if this packet is from the same context.
    if ( !pPlayer->CanUpdateSync ( ucSyncTimeContext ) )
    {
        return;
    }

    // Read out the time it took for the packet to go from the remote client to the server and to us
    unsigned short usLatency;
    BitStream.ReadCompressed ( usLatency );
    pPlayer->SetLatency ( usLatency + g_pNet->GetPing () );

    // Read out the keysync data
    CControllerState ControllerState;
    ReadFullKeysync ( ControllerState, BitStream );

    // Read out puresync flags
    SPlayerPuresyncFlags flags;
    BitStream.Read ( &flags );

    // Set the jetpack and google states
    if ( flags.data.bHasJetPack != pPlayer->HasJetPack () )
        pPlayer->SetHasJetPack ( flags.data.bHasJetPack );
    pPlayer->SetWearingGoggles ( flags.data.bWearsGoogles );

    CClientEntity* pContactEntity = NULL;
    if ( flags.data.bHasContact )
    {
        ElementID Temp;
        BitStream.ReadCompressed ( Temp );
        pContactEntity = CElementIDs::GetElement ( Temp );
    }
        
    // Player position
    CVector vecPos;
    BitStream.Read ( vecPos.fX );
    BitStream.Read ( vecPos.fY );
    BitStream.Read ( vecPos.fZ );

    // If the players in contact with an object/vehicle, make that the origin
    if ( pContactEntity )
    {
        CVector vecTempPos;
        pContactEntity->GetPosition ( vecTempPos );
        vecPos += vecTempPos;
    }

    // Player rotation
    float fRotation = 0.0f;
    BitStream.Read ( fRotation );

    // Move speed vector
    if ( flags.data.bSyncingVelocity )
    {
        SVelocitySync velocity;
        if ( BitStream.Read ( &velocity ) )
            pPlayer->SetMoveSpeed ( velocity.data.vecVelocity );
    }

    // Health
    unsigned char ucHealth;
    BitStream.Read ( ucHealth );
    float fHealth = static_cast < float > ( ucHealth ) / 1.25f;    
    pPlayer->SetHealth ( fHealth );
    pPlayer->LockHealth ( fHealth );

    // Armor
    unsigned char ucArmor;
    BitStream.Read ( ucArmor );
    float fArmor = static_cast < float > ( ucArmor ) / 1.25f;
	pPlayer->SetArmor ( fArmor );
    pPlayer->LockArmor ( fArmor );

    // Read out the camera rotation
    float fCameraRotation;
    BitStream.Read ( fCameraRotation );    

    // Current weapon id
    if ( flags.data.bHasAWeapon )
    {
        SWeaponSlotSync slot;
        BitStream.Read ( &slot );

        unsigned int uiSlot = slot.data.uiSlot;

        if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {
            CWeapon* pWeapon = pPlayer->GetWeapon ( static_cast < eWeaponSlot > ( uiSlot ) );
            unsigned char ucCurrentWeapon = 0;
            float fWeaponRange = 0.01f;
            if ( pWeapon )
            {
                ucCurrentWeapon = pWeapon->GetType ();
                fWeaponRange = pWeapon->GetInfo ()->GetWeaponRange ();
            }

            // Is the current weapon a goggle (44 or 45) or a camera (43), or a detonator (40), don't apply the fire key
            if ( ucCurrentWeapon == 44 || ucCurrentWeapon == 45 || ucCurrentWeapon == 43 || ucCurrentWeapon == 40 )
            {
                ControllerState.ButtonCircle = 0;
            }

            // Read out the weapon ammo
            SWeaponAmmoSync ammo ( ucCurrentWeapon, false, true );
            BitStream.Read ( &ammo );
            unsigned short usWeaponAmmo = ammo.data.usAmmoInClip;

            // Valid current weapon id?
            if ( CClientPickupManager::IsValidWeaponID ( ucCurrentWeapon ) )
            {
                pPlayer->AddChangeWeapon ( TICK_RATE, static_cast < eWeaponSlot > ( uiSlot ), usWeaponAmmo );
            }
            else
            {
                pPlayer->AddChangeWeapon ( TICK_RATE, WEAPONSLOT_TYPE_UNARMED, 0 );
            }

    		// Make sure that if he doesn't have an akimbo weapon his hands up state is false
	    	if ( !IsWeaponIDAkimbo ( ucCurrentWeapon ) )
		    {
                flags.data.bAkimboTargetUp = false;
		    }

            // Read out the aim directions
            float fArmX, fArmY;
		    BitStream.Read ( fArmX );
		    BitStream.Read ( fArmY );

            // Interpolate the aiming
            pPlayer->SetAimInterpolated ( TICK_RATE, fArmX, fArmY, flags.data.bAkimboTargetUp, 0 );

            // Read the aim data only if he's shooting or aiming
            if ( ControllerState.RightShoulder1 || ControllerState.ButtonCircle )
            {
                SWeaponAimSync aim ( fWeaponRange );
                BitStream.Read ( &aim );
                // Interpolate the source/target vectors
                pPlayer->SetTargetTarget ( TICK_RATE, aim.data.vecOrigin, aim.data.vecTarget );
            }
        }
        else
        {
            pPlayer->SetCurrentWeaponSlot ( static_cast < eWeaponSlot > ( slot.data.uiSlot ) );
        }
    }
    else
    {
        // Make him empty-handed
        pPlayer->SetCurrentWeaponSlot ( WEAPONSLOT_TYPE_UNARMED );
    }

     // null out the crouch bit or it'll conflict with the crouched syncing
    ControllerState.ShockButtonL = 0;    
    
    // If the players in contact with an object/vehicle, revert to contact position
    if ( pContactEntity )
    {
        CVector vecTempPos;
        pContactEntity->GetPosition ( vecTempPos );
        vecPos -= vecTempPos;
    }

    // If the player is working on leaving a vehicle, don't set any target position
    if ( pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_NONE ||
         pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_GETTING_IN ||
         pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_JACKING )
    {
        pPlayer->SetTargetPosition ( vecPos, pContactEntity );
        pPlayer->SetTargetRotation ( fRotation );
    }

    // Set move speed, controller state and camera rotation + duck state
    pPlayer->SetControllerState ( ControllerState );
    pPlayer->SetCameraRotation ( fCameraRotation );
    pPlayer->Duck ( flags.data.bIsDucked );
    pPlayer->SetChoking ( flags.data.bIsChoking );
    pPlayer->SetOnFire ( flags.data.bIsOnFire );

    // Remember now as the last puresync time
    pPlayer->SetLastPuresyncTime ( CClientTime::GetTime () );
    pPlayer->SetLastPuresyncPosition ( vecPos );
    pPlayer->IncrementPlayerSync ();
}


void CNetAPI::WritePlayerPuresync ( CClientPlayer* pPlayerModel, NetBitStreamInterface& BitStream )
{
    // Write our sync context.
    BitStream.Write ( pPlayerModel->GetSyncTimeContext () );

    // Write the full player keys
    CControllerState ControllerState;
    pPlayerModel->GetControllerState ( ControllerState );
    WriteFullKeysync ( ControllerState, BitStream );

    // Get the contact entity
    CClientEntity* pContactEntity = pPlayerModel->GetContactEntity ();
    bool bInContact = ( pContactEntity && pContactEntity->GetID () != INVALID_ELEMENT_ID && !pContactEntity->IsLocalEntity() );

    // Grab the current weapon
    CWeapon * pPlayerWeapon = pPlayerModel->GetWeapon();

    // Write the flags
    SPlayerPuresyncFlags flags;
    flags.data.bIsInWater       = ( pPlayerModel->IsInWater () == true );
    flags.data.bIsOnGround      = ( pPlayerModel->IsOnGround () == true );
    flags.data.bHasJetPack      = ( pPlayerModel->HasJetPack () == true );
    flags.data.bIsDucked        = ( pPlayerModel->IsDucked () == true );
    flags.data.bWearsGoogles    = ( pPlayerModel->IsWearingGoggles () == true );
    flags.data.bHasContact      = bInContact;
    flags.data.bIsChoking       = ( pPlayerModel->IsChoking () == true );
    flags.data.bAkimboTargetUp  = ( g_pMultiplayer->GetAkimboTargetUp () == true );
    flags.data.bIsOnFire        = ( pPlayerModel->IsOnFire () == true );
    flags.data.bHasAWeapon      = ( pPlayerWeapon != NULL );
    flags.data.bSyncingVelocity = ( !flags.data.bIsOnGround || ( pPlayerModel->GetPlayerSyncCount () % 4 ) == 0 );

    if ( pPlayerWeapon->GetSlot () > 15 )
        flags.data.bHasAWeapon = false;

    BitStream.Write ( &flags );

    // Player position
    CVector vecActualPosition;
    pPlayerModel->GetPosition ( vecActualPosition );
    CVector vecPosition = vecActualPosition;
    
    // If the player is in contact with a object/vehicle, make that the origin    
    if ( bInContact )
    {
        BitStream.WriteCompressed ( pContactEntity->GetID () );

        CVector vecOrigin;
        pContactEntity->GetPosition ( vecOrigin );
        vecPosition -= vecOrigin;
    }

    BitStream.Write ( vecPosition.fX );
    BitStream.Write ( vecPosition.fY );
    BitStream.Write ( vecPosition.fZ );

    // Player rotation
    float fCurrentRotation = pPlayerModel->GetCurrentRotation ();
    BitStream.Write ( fCurrentRotation );

    // Move speed vector
    if ( flags.data.bSyncingVelocity )
    {
        SVelocitySync velocity;
        pPlayerModel->GetMoveSpeed ( velocity.data.vecVelocity );
        BitStream.Write ( &velocity );
    }

    // Health (scaled from 0.0f-100.0f to 0-250 to save three bytes)
    float fHealth = pPlayerModel->GetHealth ();
    unsigned char ucHealth = static_cast < unsigned char > ( 1.25f * fHealth );
    // Make sure its atleast 1 if we aren't quite dead
    if ( ucHealth == 0 && fHealth > 0.0f )
        ucHealth = 1;
    BitStream.Write ( ucHealth );

    // Armor (scaled from 0.0f-100.0f to 0-250 to save three bytes)
    unsigned char ucArmor = static_cast < unsigned char > ( 1.25f * pPlayerModel->GetArmor () );
    BitStream.Write ( ucArmor );

    // Write the camera rotation
    BitStream.Write ( g_pGame->GetCamera ()->GetCameraRotation () );

    if ( flags.data.bHasAWeapon )
    {
        // Write the weapon slot
        unsigned int uiSlot = pPlayerWeapon->GetSlot ();
        SWeaponSlotSync slot;
        slot.data.uiSlot = uiSlot;
        BitStream.Write ( &slot );

        if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {
            // Write the ammo states
            SWeaponAmmoSync ammo ( pPlayerWeapon->GetType (), true, true );
            ammo.data.usAmmoInClip = pPlayerWeapon->GetAmmoInClip ();
            ammo.data.usTotalAmmo = pPlayerWeapon->GetAmmoTotal ();
            BitStream.Write ( &ammo );

			CShotSyncData* pShotsyncData = g_pMultiplayer->GetLocalShotSyncData ();
			BitStream.Write ( pShotsyncData->m_fArmDirectionX );
			BitStream.Write ( pShotsyncData->m_fArmDirectionY );

            // Write the aim data only if he's aiming or shooting
            if ( ControllerState.RightShoulder1 || ControllerState.ButtonCircle )
            {
                SWeaponAimSync aim ( 0.0f );
                pPlayerModel->GetShotData ( &(aim.data.vecOrigin), &(aim.data.vecTarget) );
                BitStream.Write ( &aim );
            }
        }
    }

    // Write our damage info if different from last time
    ElementID DamagerID = RESERVED_ELEMENT_ID;
    if ( !g_pClientGame->GetDamageSent () )
    {
        g_pClientGame->SetDamageSent ( true );

        DamagerID = g_pClientGame->GetDamagerID ();
    }
    if ( DamagerID != RESERVED_ELEMENT_ID )
    {
        BitStream.WriteBit ( true );
        BitStream.WriteCompressed ( DamagerID );
        
        SWeaponTypeSync weaponType;
        weaponType.data.uiWeaponType = g_pClientGame->GetDamageWeapon ();
        BitStream.Write ( &weaponType );

        SBodypartSync bodypart;
        bodypart.data.uiBodypart = g_pClientGame->GetDamageBodyPiece ();
        BitStream.Write ( &bodypart );
    }    
    else
        BitStream.WriteBit ( false );

    // Write the sent position to the interpolator
    AddInterpolation ( vecActualPosition );

    // Increment the puresync count
    pPlayerModel->IncrementPlayerSync ();
}


void CNetAPI::ReadVehiclePuresync ( CClientPlayer* pPlayer, CClientVehicle* pVehicle, NetBitStreamInterface& BitStream )
{
    // Read out the sync time context. See CClientEntity for documentation on that.
    unsigned char ucSyncTimeContext = 0;
    BitStream.Read ( ucSyncTimeContext );

    // Only update the sync if this packet is from the same context.
    if ( !pPlayer->CanUpdateSync ( ucSyncTimeContext ) )
    {
#ifdef MTA_DEBUG
        g_pCore->GetConsole ()->Printf ( "ignoring vehicle sync: %u", ucSyncTimeContext );
#endif
        return;
    }    

    // Read out the time it took for the packet to go from the remote client to the server and to us
    unsigned short usLatency;
    BitStream.ReadCompressed ( usLatency );
    pPlayer->SetLatency ( usLatency + g_pNet->GetPing () );

    // Read out the keysync
    CControllerState ControllerState;
    ReadFullKeysync ( ControllerState, BitStream );
    
    // Jax: temp fix for rhino firing, CPlayerInfo::m_LastTimeBigGunFired needs to be context-switched
    if ( pVehicle->GetModel () == VT_RHINO )
    {
        ControllerState.ButtonCircle = 0;
    }
    pPlayer->SetControllerState ( ControllerState );

    // Grab the vehicle seat the player is in. Only read out vehicle position stuff if he's the driver.
    unsigned int uiSeat = pPlayer->GetOccupiedVehicleSeat ();
    if ( uiSeat == 0 )
    {
#ifdef MTA_DEBUG
        if ( pVehicle )
        {
            pVehicle->m_pLastSyncer = pPlayer;
            pVehicle->m_ulLastSyncTime = GetTickCount ();
            pVehicle->m_szLastSyncType = "pure";
        }
#endif

        // Read out vehicle position and rotation
        CVector vecPosition;
        CVector vecRotationDegrees;

        // Read out the vehicle matrix
        BitStream.Read ( vecPosition.fX );
        BitStream.Read ( vecPosition.fY );
        BitStream.Read ( vecPosition.fZ );

        BitStream.Read ( vecRotationDegrees.fX );
        BitStream.Read ( vecRotationDegrees.fY );
        BitStream.Read ( vecRotationDegrees.fZ );     

        // Read out the movespeed
        CVector vecMoveSpeed;
        BitStream.Read ( vecMoveSpeed.fX );
        BitStream.Read ( vecMoveSpeed.fY );
        BitStream.Read ( vecMoveSpeed.fZ );

        // Read out the turnspeed
        CVector vecTurnSpeed;
        BitStream.Read ( vecTurnSpeed.fX );
        BitStream.Read ( vecTurnSpeed.fY );
        BitStream.Read ( vecTurnSpeed.fZ );        

        // Read out the vehicle health
        float fHealth;
        BitStream.Read ( fHealth ); 
        pVehicle->SetHealth ( fHealth );

        // Set the target position and rotation
        pVehicle->SetTargetPosition ( vecPosition );
        pVehicle->SetTargetRotation ( vecRotationDegrees );

        // Apply the correct move and turnspeed
        pVehicle->SetMoveSpeed ( vecMoveSpeed );
        pVehicle->SetTurnSpeed ( vecTurnSpeed );
    }

    // Player health
    unsigned char ucPlayerHealth;
    BitStream.Read ( ucPlayerHealth );
    float fPlayerHealth = static_cast < float > ( ucPlayerHealth ) / 1.25f;
    pPlayer->SetHealth ( fPlayerHealth );
    pPlayer->LockHealth ( fPlayerHealth );

    // Player armor
    unsigned char ucArmor;
    BitStream.Read ( ucArmor );
    float fArmor = static_cast < float > ( ucArmor ) / 1.25f;
    pPlayer->SetArmor ( fArmor );

    // Vehicle flags
    unsigned char ucFlags;
    BitStream.Read ( ucFlags );

    // Decode the vehicle flags
    bool bWearingGoggles = ( ucFlags & 0x01 ) ? true:false;
    bool bDoingGangDriveby = ( ucFlags & 0x02 ) ? true:false;
    bool bSireneActive = ( ucFlags & 0x04 ) ? true:false;
    bool bSmokeTrail = ( ucFlags & 0x08 ) ? true:false;
    bool bLandingGearDown = ( ucFlags & 0x10 ) ? true:false;
    bool bIsOnGround = ( ucFlags & 0x20 ) ? true:false;
    bool bInWater = ( ucFlags & 0x40 ) ? true:false;
    bool bDerailed = ( ucFlags & 0x80 ) ? true:false;

    // Set flag stuff
    pPlayer->SetWearingGoggles ( bWearingGoggles );

    // Sirene states
    int iModelID = pVehicle->GetModel ();
    if ( CClientVehicleManager::HasSirens ( iModelID ) )
    {
        pVehicle->SetSirenOrAlarmActive ( bSireneActive );
    }

    // Smoke trail
    if ( CClientVehicleManager::HasSmokeTrail ( iModelID ) )
    {
        pVehicle->SetSmokeTrailEnabled ( bSmokeTrail );
    }

    // Landing gear vehicles
    if ( CClientVehicleManager::HasLandingGears ( iModelID ) )
    {
        pVehicle->SetLandingGearDown ( bLandingGearDown );
    }

    // Derailed state
    if ( pVehicle->GetVehicleType() == CLIENTVEHICLE_TRAIN )
    {
        pVehicle->SetDerailed ( bDerailed );
    }

    // Current weapon id
    unsigned char ucCurrentWeapon = 0;
    BitStream.Read ( ucCurrentWeapon );
    if ( ucCurrentWeapon != 0 )
    {
        // Is the current weapon a goggle (44 or 45) or a camera (43), or detonator (40), don't apply the fire key
        if ( ucCurrentWeapon == 44 || ucCurrentWeapon == 45 || ucCurrentWeapon == 43 || ucCurrentWeapon == 40 )
        {
            ControllerState.ButtonCircle = 0;
        }

        // Read out the weapon ammo
        unsigned short usWeaponAmmo;
        BitStream.Read ( usWeaponAmmo );

        // Valid current weapon id?
        if ( CClientPickupManager::IsValidWeaponID ( ucCurrentWeapon ) )
        {
            // Grab the current weapon the player has
            CWeapon* pPlayerWeapon = pPlayer->GetWeapon ();
            eWeaponType eCurrentWeapon = static_cast < eWeaponType > ( ucCurrentWeapon );
            if ( ( pPlayerWeapon && pPlayerWeapon->GetType () != eCurrentWeapon ) || !pPlayerWeapon )
            {
                pPlayerWeapon = pPlayer->GiveWeapon ( eCurrentWeapon, usWeaponAmmo );
                if ( pPlayerWeapon )
                {
                    pPlayerWeapon->SetAsCurrentWeapon ();
                }
            }

            // Give it unlimited ammo and set the ammo in clip
            if ( pPlayerWeapon )
            {
                pPlayerWeapon->SetAmmoTotal ( 9999 );
                pPlayerWeapon->SetAmmoInClip ( usWeaponAmmo );
            }
        }

        // Read out the aim directions
        float fArmX, fArmY;
        BitStream.Read ( fArmX );
        BitStream.Read ( fArmY );

        // Read out the origin vector
        CVector vecOrigin;
        BitStream.Read ( vecOrigin.fX );
        BitStream.Read ( vecOrigin.fY );
        BitStream.Read ( vecOrigin.fZ );

        // Read out the target vector
        CVector vecTarget;
        BitStream.Read ( vecTarget.fX );
        BitStream.Read ( vecTarget.fY );
        BitStream.Read ( vecTarget.fZ );

        // Read out the driveby direction
        unsigned char ucDriveByAim;
        BitStream.Read ( ucDriveByAim );

        // Set the aiming data
        pPlayer->SetAimingData ( TICK_RATE, vecTarget, fArmX, fArmY, ucDriveByAim, &vecOrigin, false );
    }
    else
    {
        // Make him empty-handed
        pPlayer->SetCurrentWeaponSlot ( WEAPONSLOT_TYPE_UNARMED );
    }

    // Read out vehicle specific data if he's the driver
    if ( uiSeat == 0 )
    {
        ReadFullVehicleSpecific ( pVehicle, BitStream );
    }

    pPlayer->SetDoingGangDriveby ( bDoingGangDriveby );

    // Remember now as the last puresync time
    CVector vecPosition;
    pVehicle->GetPosition ( vecPosition );
    pPlayer->SetLastPuresyncPosition ( vecPosition );
    pPlayer->SetLastPuresyncTime ( CClientTime::GetTime () );
    pPlayer->IncrementVehicleSync ();
}


void CNetAPI::WriteVehiclePuresync ( CClientPed* pPlayerModel, CClientVehicle* pVehicle, NetBitStreamInterface& BitStream )
{
    // Write our sync context.
    BitStream.Write ( pPlayerModel->GetSyncTimeContext () );

    // Write the controller state
    CControllerState ControllerState;
    pPlayerModel->GetControllerState ( ControllerState );
    WriteFullKeysync ( ControllerState, BitStream );

    // Grab the vehicle position
    CVector vecPosition;
    pVehicle->GetPosition ( vecPosition );

    // Write it
    BitStream.Write ( vecPosition.fX );
    BitStream.Write ( vecPosition.fY );
    BitStream.Write ( vecPosition.fZ );

    // Grab the occupied vehicle seat. Send this only if we're driver.
    unsigned int uiSeat = pPlayerModel->GetOccupiedVehicleSeat ();
    BitStream.Write ( ( unsigned char ) uiSeat );
    if ( uiSeat == 0 )
    {
        // Write the rotation in degrees
        CVector vecRotation;
        pVehicle->GetRotationDegrees ( vecRotation );

        // Write it
        BitStream.Write ( vecRotation.fX );
        BitStream.Write ( vecRotation.fY );
        BitStream.Write ( vecRotation.fZ );

        // Write the movespeed/turnspeed
        CVector vecMoveSpeed, vecTurnSpeed;

        // Jax: if the vehicles streamed out, make sure it stays still for everyone else
        if ( pVehicle->GetGameVehicle () )
        {
            pVehicle->GetMoveSpeed ( vecMoveSpeed ); 
            pVehicle->GetTurnSpeed ( vecTurnSpeed ); 
        }

        BitStream.Write ( vecMoveSpeed.fX );
        BitStream.Write ( vecMoveSpeed.fY );
        BitStream.Write ( vecMoveSpeed.fZ );       

        BitStream.Write ( vecTurnSpeed.fX );
        BitStream.Write ( vecTurnSpeed.fY );
        BitStream.Write ( vecTurnSpeed.fZ );

        // Write the health
        BitStream.Write ( pVehicle->GetHealth () );

        // Write the trailer chain
        CClientVehicle* pTrailer = pVehicle->GetRealTowedVehicle ();
        while ( pTrailer )
        {
            BitStream.Write ( pTrailer->GetID () );

            // Write the position and rotation
            CVector vecTrailerPosition, vecTrailerRotationDegrees;

            // Write the matrix
            pTrailer->GetPosition ( vecTrailerPosition );
            pTrailer->GetRotationDegrees ( vecTrailerRotationDegrees );

            BitStream.Write ( vecTrailerPosition.fX );
            BitStream.Write ( vecTrailerPosition.fY );
            BitStream.Write ( vecTrailerPosition.fZ );

            BitStream.Write ( vecTrailerRotationDegrees.fX );
            BitStream.Write ( vecTrailerRotationDegrees.fY );
            BitStream.Write ( vecTrailerRotationDegrees.fZ );

            // Get the next towed vehicle
            pTrailer = pTrailer->GetTowedVehicle ();
        }

        // End of our trailer chain
        BitStream.Write ( static_cast < ElementID > ( INVALID_ELEMENT_ID ) );
    }

    // Health (scaled from 0.0f-100.0f to 0-250 to save three bytes)
    float fHealth = pPlayerModel->GetHealth ();
    unsigned char ucHealth = static_cast < unsigned char > ( 1.25f * fHealth );
    // Make sure its atleast 1 if we aren't quite dead
    if ( ucHealth == 0 && fHealth > 0.0f ) ucHealth = 1;
    BitStream.Write ( ucHealth );

    // Player armor (scaled from 0.0f-100.0f to 0-250 to save three bytes)
    unsigned char ucArmor = static_cast < unsigned char > ( 1.25f * pPlayerModel->GetArmor () );
    BitStream.Write ( ucArmor );

    // Flags
    unsigned char ucFlags = 0;
    if ( pPlayerModel->IsWearingGoggles () ) ucFlags |= 0x01;
    if ( pPlayerModel->IsDoingGangDriveby () ) ucFlags |= 0x02;
    if ( pVehicle->IsSirenOrAlarmActive () ) ucFlags |= 0x04;
    if ( pVehicle->IsSmokeTrailEnabled () ) ucFlags |= 0x08;
    if ( pVehicle->IsLandingGearDown () ) ucFlags |= 0x10;
    if ( pVehicle->IsOnGround () ) ucFlags |= 0x20;
    if ( pVehicle->IsInWater () ) ucFlags |= 0x40;
    if ( pVehicle->IsDerailed () ) ucFlags |= 0x80;

    // Write the flags
    BitStream.Write ( ucFlags );

    // Grab the current weapon
    CWeapon * pPlayerWeapon = pPlayerModel->GetWeapon();
    if ( pPlayerWeapon )
    {
        // Write the weapon type
        unsigned char ucWeaponSlot = static_cast < unsigned char > ( pPlayerWeapon->GetSlot () );
        unsigned char ucWeaponType = static_cast < unsigned char > ( pPlayerWeapon->GetType () );
        BitStream.Write ( ucWeaponSlot );
        if ( ucWeaponType != 0 )
        {
            // Write the ammo in clip
            unsigned short usAmmoInClip = static_cast < unsigned short > ( pPlayerWeapon->GetAmmoInClip () );
            BitStream.Write ( usAmmoInClip );

            // Grab his aim directions and sync them
            CShotSyncData* pShotsyncData = g_pMultiplayer->GetLocalShotSyncData ();
            BitStream.Write ( pShotsyncData->m_fArmDirectionX );
            BitStream.Write ( pShotsyncData->m_fArmDirectionY );

            CVector vecOrigin, vecTarget;
            pPlayerModel->GetShotData ( &vecOrigin, &vecTarget );  

            // Write the gun's origin vector
            BitStream.Write ( vecOrigin.fX );
            BitStream.Write ( vecOrigin.fY );
            BitStream.Write ( vecOrigin.fZ );

            // Write the gun's target vector
            BitStream.Write ( vecTarget.fX );
            BitStream.Write ( vecTarget.fY );
            BitStream.Write ( vecTarget.fZ );

            // Write the driveby direction
            BitStream.Write ( pShotsyncData->m_cInVehicleAimDirection );
        }
    }
    else
    {
        BitStream.Write ( static_cast < unsigned char > ( 0 ) );
    }

    // Write vehicle specific stuff if we're driver
    if ( uiSeat == 0 )
    {
        WriteFullVehicleSpecific ( pVehicle, BitStream );
    }

    // Write the sent position to the interpolator
    AddInterpolation ( vecPosition );
}


bool CNetAPI::ReadSmallKeysync ( CControllerState& ControllerState, const CControllerState& LastControllerState, NetBitStreamInterface& BitStream )
{
    SSmallKeysyncSync keys;
    if ( !BitStream.Read ( &keys ) )
        return false;

    // Put the result into the controllerstate
    ControllerState.LeftShoulder1   = 255 * keys.data.bLeftShoulder1;
    ControllerState.RightShoulder1  = 255 * keys.data.bRightShoulder1;
    ControllerState.ButtonSquare    = 255 * keys.data.bButtonSquare;
    ControllerState.ButtonCross     = 255 * keys.data.bButtonCross;
    ControllerState.ButtonCircle    = 255 * keys.data.bButtonCircle;
    ControllerState.ButtonTriangle  = 255 * keys.data.bButtonTriangle;
    ControllerState.ShockButtonL    = 255 * keys.data.bShockButtonL;
    ControllerState.m_bPedWalk      = 255 * keys.data.bPedWalk;

    if ( keys.data.bLeftStickXChanged )
        ControllerState.LeftStickX  = keys.data.sLeftStickX;
    else
        ControllerState.LeftStickX  = LastControllerState.LeftStickX;

    if ( keys.data.bLeftStickYChanged )
        ControllerState.LeftStickY  = keys.data.sLeftStickY;
    else
        ControllerState.LeftStickY  = LastControllerState.LeftStickY;

    return true;
}


void CNetAPI::WriteSmallKeysync ( const CControllerState& ControllerState, const CControllerState& LastControllerState, NetBitStreamInterface& BitStream )
{
    SSmallKeysyncSync keys;
    keys.data.bLeftShoulder1    = ( ControllerState.LeftShoulder1 != 0 );       // Action / Secondary-Fire
    keys.data.bRightShoulder1   = ( ControllerState.RightShoulder1 != 0 );      // Aim-Weapon / Handbrake
    keys.data.bButtonSquare     = ( ControllerState.ButtonSquare != 0 );        // Jump / Reverse
    keys.data.bButtonCross      = ( ControllerState.ButtonCross != 0 );         // Sprint / Accelerate
    keys.data.bButtonCircle     = ( ControllerState.ButtonCircle != 0 );        // Fire // Fire
    keys.data.bButtonTriangle   = ( ControllerState.ButtonTriangle != 0 );      // Enter/Exit/Special-Attack / Enter/exit
    keys.data.bShockButtonL     = ( ControllerState.ShockButtonL != 0 );        // Crouch / Horn
    keys.data.bPedWalk          = ( ControllerState.m_bPedWalk != 0 );          // Walk / -

    keys.data.bLeftStickXChanged    = ( ControllerState.LeftStickX != LastControllerState.LeftStickX );
    keys.data.bLeftStickYChanged    = ( ControllerState.LeftStickY != LastControllerState.LeftStickY );
    keys.data.sLeftStickX           = ControllerState.LeftStickX;
    keys.data.sLeftStickY           = ControllerState.LeftStickY;

    // Write it
    BitStream.Write ( &keys );
}


bool CNetAPI::ReadFullKeysync ( CControllerState& ControllerState, NetBitStreamInterface& BitStream )
{
    // Read the key sync
    SFullKeysyncSync keys;
    if ( !BitStream.Read ( &keys ) )
        return false;

    // Put the result into the controllerstate
    ControllerState.LeftShoulder1   = 255 * keys.data.bLeftShoulder1;
    ControllerState.RightShoulder1  = 255 * keys.data.bRightShoulder1;
    ControllerState.ButtonSquare    = 255 * keys.data.bButtonSquare;
    ControllerState.ButtonCross     = 255 * keys.data.bButtonCross;
    ControllerState.ButtonCircle    = 255 * keys.data.bButtonCircle;
    ControllerState.ButtonTriangle  = 255 * keys.data.bButtonTriangle;
    ControllerState.ShockButtonL    = 255 * keys.data.bShockButtonL;
    ControllerState.m_bPedWalk      = 255 * keys.data.bPedWalk;

    ControllerState.LeftStickX      = keys.data.sLeftStickX;
    ControllerState.LeftStickY      = keys.data.sLeftStickY;

    return true;
}


void CNetAPI::WriteFullKeysync ( const CControllerState& ControllerState, NetBitStreamInterface& BitStream )
{
    // Put the controllerstate bools into a key byte
    SFullKeysyncSync keys;
    keys.data.bLeftShoulder1    = ( ControllerState.LeftShoulder1 != 0 );
    keys.data.bRightShoulder1   = ( ControllerState.RightShoulder1 != 0 );
    keys.data.bButtonSquare     = ( ControllerState.ButtonSquare != 0 );
    keys.data.bButtonCross      = ( ControllerState.ButtonCross != 0 );
    keys.data.bButtonCircle     = ( ControllerState.ButtonCircle != 0 );
    keys.data.bButtonTriangle   = ( ControllerState.ButtonTriangle != 0 );
    keys.data.bShockButtonL     = ( ControllerState.ShockButtonL != 0 );
    keys.data.bPedWalk          = ( ControllerState.m_bPedWalk != 0 );
    keys.data.sLeftStickX       = ControllerState.LeftStickX;
    keys.data.sLeftStickY       = ControllerState.LeftStickY;

    // Write it
    BitStream.Write ( &keys );
}


void CNetAPI::ReadSmallVehicleSpecific ( CClientVehicle* pVehicle, NetBitStreamInterface& BitStream )
{
    // Turret states
    int iModelID = pVehicle->GetModel ();
    if ( CClientVehicleManager::HasTurret ( iModelID ) )
    {
        // Read out the turret position
        float fHorizontal;
        float fVertical;
        BitStream.Read ( fHorizontal );
        BitStream.Read ( fVertical );
        
        // Set it
        pVehicle->SetTurretRotation ( fHorizontal, fVertical );
    }
}


void CNetAPI::WriteSmallVehicleSpecific ( CClientVehicle* pVehicle, NetBitStreamInterface& BitStream )
{
    // Turret states
    int iModelID = pVehicle->GetModel ();
    if ( CClientVehicleManager::HasTurret ( iModelID ) )
    {
        // Grab the turret position
        float fHorizontal;
        float fVertical;
        pVehicle->GetTurretRotation ( fHorizontal, fVertical );

        // Write it
        BitStream.Write ( fHorizontal );
        BitStream.Write ( fVertical );
    }
}


void CNetAPI::ReadFullVehicleSpecific ( CClientVehicle* pVehicle, NetBitStreamInterface& BitStream )
{
    // Turret states
    int iModelID = pVehicle->GetModel ();
    if ( CClientVehicleManager::HasTurret ( iModelID ) )
    {
        // Read out the turret position
        float fHorizontal;
        float fVertical;
        BitStream.Read ( fHorizontal );
        BitStream.Read ( fVertical );
        
        // Set it
        pVehicle->SetTurretRotation ( fHorizontal, fVertical );
    }

    // Adjustable property
    if ( CClientVehicleManager::HasAdjustableProperty ( iModelID ) )
    {
        unsigned short usAdjustableProperty;
        if ( BitStream.Read ( usAdjustableProperty ) )
        {
            pVehicle->SetAdjustablePropertyValue ( usAdjustableProperty );
        }
    }
}


void CNetAPI::WriteFullVehicleSpecific ( CClientVehicle* pVehicle, NetBitStreamInterface& BitStream )
{
    // Turret states
    int iModelID = pVehicle->GetModel ();
    if ( CClientVehicleManager::HasTurret ( iModelID ) )
    {
        // Grab the turret position
        float fHorizontal;
        float fVertical;
        pVehicle->GetTurretRotation ( fHorizontal, fVertical );

        // Write it
        BitStream.Write ( fHorizontal );
        BitStream.Write ( fVertical );
    }

    // Adjustable property
    if ( CClientVehicleManager::HasAdjustableProperty ( iModelID ) )
    {
        BitStream.Write ( pVehicle->GetAdjustablePropertyValue () );
    }
}


void CNetAPI::WriteCameraSync ( NetBitStreamInterface& BitStream )
{
    CClientCamera * pCamera = m_pManager->GetCamera ();

    // Are we in fixed mode?
    bool bFixed = pCamera->IsInFixedMode ();
    BitStream.Write ( ( unsigned char ) ( ( bFixed ) ? 1 : 0 ) );
    if ( bFixed )
    {
        // Write our position
        CVector vecPosition;
        pCamera->GetPosition ( vecPosition );
        BitStream.Write ( vecPosition.fX );
        BitStream.Write ( vecPosition.fY );
        BitStream.Write ( vecPosition.fZ );

        // Write our lookAt
        CVector vecLookAt;
        pCamera->GetTarget ( vecLookAt );
        BitStream.Write ( vecLookAt.fX );
        BitStream.Write ( vecLookAt.fY );
        BitStream.Write ( vecLookAt.fZ );
    }
    else
    {
        // Write our target
        CClientPlayer * pPlayer = pCamera->GetFocusedPlayer ();
        if ( !pPlayer ) pPlayer = g_pClientGame->GetLocalPlayer ();

        BitStream.Write ( pPlayer->GetID () );
    }
}


void CNetAPI::RPC ( eServerRPCFunctions ID, NetBitStreamInterface * pBitStream, NetPacketOrdering packetOrdering )
{
    NetBitStreamInterface* pRPCBitStream = g_pNet->AllocateNetBitStream ();
    if ( pRPCBitStream )
    {
        // Write the rpc ID
        pRPCBitStream->Write ( ( unsigned char ) ID );

        if ( pBitStream )
        {
            // Copy each byte from the bitstream we have to this one
            unsigned char ucTemp;
            int iLength = pBitStream->GetNumberOfBytesUsed ();
            for ( int i = 0; i < iLength; i++ )
            {
                pBitStream->Read ( ucTemp );
                pRPCBitStream->Write ( ucTemp );
            }
            pBitStream->ResetReadPointer ();
        }

        g_pNet->SendPacket ( PACKET_ID_RPC, pRPCBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE_ORDERED, packetOrdering );
        g_pNet->DeallocateNetBitStream ( pRPCBitStream );
    }
}