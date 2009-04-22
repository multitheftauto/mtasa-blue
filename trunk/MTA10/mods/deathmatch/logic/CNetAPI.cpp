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
            if ( BitStream.Read ( PlayerID ) )
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
            if ( BitStream.Read ( PlayerID ) )
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
            // Flags
            unsigned char ucFlags;
            BitStream.Read ( ucFlags );

            // Grab the in vehicle flag
            bool bInVehicle = ucFlags & 0x01;

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
    BitStream.Read ( usLatency );
    pPlayer->SetLatency ( usLatency + g_pNet->GetPing () );

    // Grab the old controller state
    CControllerState LastControllerState;
    pPlayer->GetLastControllerState ( LastControllerState );

    // Read out the small keysync data
    CControllerState ControllerState;
    ReadSmallKeysync ( ControllerState, LastControllerState, BitStream );

    // Flags
    unsigned char ucFlags;
    BitStream.Read ( ucFlags );

    // Decode the flags
    bool bDucked = ( ucFlags & 0x01 ) ? true:false;;
    bool bChoking = ( ucFlags & 0x02 ) ? true:false;;
    bool bAimAkimboUp = ( ucFlags & 0x04 ) ? true:false;;

    // Grab the occupied vehicle
    CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
    bool bSyncingVehicle = pVehicle && pPlayer->GetOccupiedVehicleSeat () == 0;

    // If he's shooting
    if ( ControllerState.ButtonCircle )
    {
        // Read out his current weapon
        unsigned char ucCurrentWeaponSlot;
        unsigned char ucCurrentWeaponType;

        BitStream.Read ( ucCurrentWeaponSlot );
        BitStream.Read ( ucCurrentWeaponType );

        if ( ucCurrentWeaponType != 0 )
        {
            // Is the current weapon a goggle (44 or 45) or a camera (43), detonator (40), don't apply the fire key
            if ( ucCurrentWeaponType == 44 || ucCurrentWeaponType == 45 || ucCurrentWeaponType == 43 || ucCurrentWeaponType == 40 )
            {
                ControllerState.ButtonCircle = 0;
            }

            // Read out the weapon ammo
            unsigned short usWeaponAmmo = 0;
            BitStream.Read ( usWeaponAmmo );

            // Valid current weapon id? Add it to the change weapon queue
            if ( CClientPickupManager::IsValidWeaponID ( ucCurrentWeaponType ) )
            {
                if ( pVehicle )
                {

                }
                else
                {
                    pPlayer->AddChangeWeapon ( TICK_RATE, ucCurrentWeaponType, usWeaponAmmo );
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
                    pPlayer->AddChangeWeapon ( TICK_RATE, 0, 0 );
                }
            }

			// Make sure that if he doesn't have an akimbo weapon his hands up state is false
			if ( !IsWeaponIDAkimbo ( ucCurrentWeaponType ) )
			{
				bAimAkimboUp = false;
			}

            // Read out the aim directions
            float fArmX, fArmY;
			BitStream.Read ( fArmX );
			BitStream.Read ( fArmY );

            CVector vecSource;
            BitStream.Read ( vecSource.fX );
            BitStream.Read ( vecSource.fY );
            BitStream.Read ( vecSource.fZ );

            // Read out the target vector and set it
            CVector vecTemp;
            BitStream.Read ( vecTemp.fX );
            BitStream.Read ( vecTemp.fY );
            BitStream.Read ( vecTemp.fZ );

            // Read out the driveby direction
            unsigned char ucDriveByAim;
            BitStream.Read ( ucDriveByAim );

            // Set the aim data (immediately if in vehicle, otherwize delayed/interpolated)
            if ( pVehicle )
            {
                pPlayer->SetAimingData ( TICK_RATE, vecTemp, fArmX, fArmY, ucDriveByAim, &vecSource, false );
            }
            else
            {
                pPlayer->SetTargetTarget ( TICK_RATE, vecSource, vecTemp );
                pPlayer->SetAimInterpolated ( TICK_RATE, fArmX, fArmY, bAimAkimboUp, ucDriveByAim );
            }
        }
        else
        {
            pPlayer->AddChangeWeapon ( TICK_RATE, 0, 0 );
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
        pPlayer->Duck ( bDucked );   
        pPlayer->SetChoking ( bChoking );       
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
    unsigned char ucFlags = 0;
    ucFlags |= pPlayerModel->IsDucked () ? 1:0;
    ucFlags |= pPlayerModel->IsChoking () << 1;
    ucFlags |= g_pMultiplayer->GetAkimboTargetUp () << 2;

    // Write the flags
    BitStream.Write ( ucFlags );

    // Are we shooting?
    if ( ControllerState.ButtonCircle )
    {
        // Grab the current weapon
        CWeapon * pPlayerWeapon = pPlayerModel->GetWeapon ();
        if ( pPlayerWeapon )
        {
            // Write the type
            unsigned char ucSlot = static_cast < unsigned char > ( pPlayerWeapon->GetSlot () );
            unsigned char ucType = static_cast < unsigned char > ( pPlayerWeapon->GetType () );

            BitStream.Write ( ucSlot );
            BitStream.Write ( ucType );

            if ( ucType != 0 )
            {
                 // Write the clip ammo
                unsigned short usAmmoInClip = static_cast < unsigned short > ( pPlayerWeapon->GetAmmoInClip () );
                BitStream.Write ( usAmmoInClip );

                // Grab his aim directions and sync them if he's not using an akimbo
				CShotSyncData* pShotsyncData = g_pMultiplayer->GetLocalShotSyncData ();
				BitStream.Write ( pShotsyncData->m_fArmDirectionX );
				BitStream.Write ( pShotsyncData->m_fArmDirectionY );

                CVector vecOrigin, vecTarget;
                pPlayerModel->GetShotData ( &vecOrigin, &vecTarget );

                // Write the origin
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
            BitStream.Write ( static_cast < unsigned char > ( 0 ) ); // Slot
            BitStream.Write ( static_cast < unsigned char > ( 0 ) ); // Type
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
    BitStream.Read ( usLatency );
    pPlayer->SetLatency ( usLatency + g_pNet->GetPing () );

    // Read out the keysync data
    CControllerState ControllerState;
    ReadFullKeysync ( ControllerState, BitStream );

    // Read out puresync flags
    unsigned short usFlags = 0;
    BitStream.Read ( ( unsigned short )usFlags );

    // Convert them to bools
    bool bInWater = ( usFlags & 0x01 ) ? true:false;
    bool bOnGround = ( usFlags & 0x02 ) ? true:false;
    bool bHasJetPack = ( usFlags & 0x04 ) ? true:false;
    bool bDucked = ( usFlags & 0x08 ) ? true:false;
    bool bWearingGoggles = ( usFlags & 0x10 ) ? true:false;
    bool bInContact = ( usFlags & 0x20 ) ? true:false;
    bool bIsChoking = ( usFlags & 0x40 ) ? true:false;
    bool bAimAkimboUp = ( usFlags & 0x80 ) ? true:false;
    bool bIsOnFire = ( usFlags & 0x100 ) ? true:false;

    // Set the jetpack and google states
    if ( bHasJetPack != pPlayer->HasJetPack () )
        pPlayer->SetHasJetPack ( bHasJetPack );
    pPlayer->SetWearingGoggles ( bWearingGoggles );

    CClientEntity* pContactEntity = NULL;
    if ( bInContact )
    {
        ElementID Temp;
        BitStream.Read ( Temp );
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
    CVector vecMoveSpeed;
    BitStream.Read ( vecMoveSpeed.fX );
    BitStream.Read ( vecMoveSpeed.fY );
    BitStream.Read ( vecMoveSpeed.fZ );    

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
    unsigned char ucCurrentWeapon = 0;
    BitStream.Read ( ucCurrentWeapon );
    if ( ucCurrentWeapon != 0 )
    {
        // Is the current weapon a goggle (44 or 45) or a camera (43), or a detonator (40), don't apply the fire key
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
            pPlayer->AddChangeWeapon ( TICK_RATE, ucCurrentWeapon, usWeaponAmmo );
        }
        else
        {
            pPlayer->AddChangeWeapon ( TICK_RATE, 0, 0 );
        }

		// Make sure that if he doesn't have an akimbo weapon his hands up state is false
		if ( !IsWeaponIDAkimbo ( ucCurrentWeapon ) )
		{
			bAimAkimboUp = false;
		}

        // Read out the aim directions
        float fArmX, fArmY;
		BitStream.Read ( fArmX );
		BitStream.Read ( fArmY );

        // Read out source vector
        CVector vecSource;
        BitStream.Read ( vecSource.fX );
        BitStream.Read ( vecSource.fY );
        BitStream.Read ( vecSource.fZ );

        // Read out the target vector and set it
        CVector vecTemp;
        BitStream.Read ( vecTemp.fX );
        BitStream.Read ( vecTemp.fY );
        BitStream.Read ( vecTemp.fZ );

        // Interpolate the aiming
        pPlayer->SetAimInterpolated ( TICK_RATE, fArmX, fArmY, bAimAkimboUp, 0 );

        // Interpolate the source/target vectors
        pPlayer->SetTargetTarget ( TICK_RATE, vecSource, vecTemp );
    }
    else
    {
        // Make him empty-handed
        pPlayer->SetCurrentWeaponSlot ( static_cast < eWeaponSlot > ( 0 ) );
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
    pPlayer->SetMoveSpeed ( vecMoveSpeed );
    pPlayer->SetControllerState ( ControllerState );
    pPlayer->SetCameraRotation ( fCameraRotation );
    pPlayer->Duck ( bDucked );
    pPlayer->SetChoking ( bIsChoking );
    pPlayer->SetOnFire ( bIsOnFire );

    // Remember now as the last puresync time
    pPlayer->SetLastPuresyncTime ( CClientTime::GetTime () );
    pPlayer->SetLastPuresyncPosition ( vecPos );
    pPlayer->IncrementPlayerSync ();
}


void CNetAPI::WritePlayerPuresync ( CClientPed* pPlayerModel, NetBitStreamInterface& BitStream )
{
    // Write our sync context.
    BitStream.Write ( pPlayerModel->GetSyncTimeContext () );

    // Write the full player keys
    CControllerState ControllerState;
    pPlayerModel->GetControllerState ( ControllerState );
    WriteFullKeysync ( ControllerState, BitStream );

    CClientEntity* pContactEntity = pPlayerModel->GetContactEntity ();
    bool bInContact = ( pContactEntity && pContactEntity->GetID () != INVALID_ELEMENT_ID );

    unsigned short usFlags = 0;
    if ( pPlayerModel->IsInWater () ) usFlags |= 0x01;
    if ( pPlayerModel->IsOnGround () ) usFlags |= 0x02;
    if ( pPlayerModel->HasJetPack () ) usFlags |= 0x04;
    if ( pPlayerModel->IsDucked () ) usFlags |= 0x08;
    if ( pPlayerModel->IsWearingGoggles () ) usFlags |= 0x10;
    if ( bInContact ) usFlags |= 0x20;
    if ( pPlayerModel->IsChoking () ) usFlags |= 0x40;
    if ( g_pMultiplayer->GetAkimboTargetUp () ) usFlags |= 0x80;
    if ( pPlayerModel->IsOnFire () ) usFlags |= 0x100;
    BitStream.Write ( (unsigned short) usFlags );

    // Player position
    CVector vecActualPosition;
    pPlayerModel->GetPosition ( vecActualPosition );
    CVector vecPosition = vecActualPosition;
    
    // If the player is in contact with a object/vehicle, make that the origin    
    if ( bInContact )
    {
        BitStream.Write ( pContactEntity->GetID () );

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
    CVector vecMoveSpeed;
    pPlayerModel->GetMoveSpeed ( vecMoveSpeed );

    BitStream.Write ( vecMoveSpeed.fX );
    BitStream.Write ( vecMoveSpeed.fY );
    BitStream.Write ( vecMoveSpeed.fZ );

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

    // Grab the current weapon
    CWeapon * pPlayerWeapon = pPlayerModel->GetWeapon();
    if ( pPlayerWeapon )
    {
        // Write the weapon slot
        unsigned char ucWeaponSlot = static_cast < unsigned char > ( pPlayerWeapon->GetSlot () );
        unsigned char ucWeaponType = static_cast < unsigned char > ( pPlayerWeapon->GetType () );
        BitStream.Write ( ucWeaponSlot );
		BitStream.Write ( ucWeaponType );

        if ( ucWeaponType != 0 )
        {
            // Write the ammo states
            unsigned short usAmmoInClip = static_cast < unsigned short > ( pPlayerWeapon->GetAmmoInClip () );
            BitStream.Write ( usAmmoInClip );
            unsigned short usTotalAmmo = static_cast < unsigned short > ( pPlayerWeapon->GetAmmoTotal () );
            BitStream.Write ( usTotalAmmo );

			CShotSyncData* pShotsyncData = g_pMultiplayer->GetLocalShotSyncData ();
			BitStream.Write ( pShotsyncData->m_fArmDirectionX );
			BitStream.Write ( pShotsyncData->m_fArmDirectionY );

            // Grab the shot origin and target.
            CVector vecOrigin, vecTarget;

            pPlayerModel->GetShotData ( &vecOrigin, &vecTarget );

            // Write the source vector
            BitStream.Write ( vecOrigin.fX );
            BitStream.Write ( vecOrigin.fY );
            BitStream.Write ( vecOrigin.fZ );

            // Write the gun's target vector
            BitStream.Write ( vecTarget.fX );
            BitStream.Write ( vecTarget.fY );
            BitStream.Write ( vecTarget.fZ );
        }
    }
    else
    {
        BitStream.Write ( static_cast < unsigned char > ( 0 ) );
        BitStream.Write ( static_cast < unsigned char > ( 0 ) );
    }

    // Write our damage info if different from last time
    ElementID DamagerID = RESERVED_ELEMENT_ID;
    if ( !g_pClientGame->GetDamageSent () )
    {
        g_pClientGame->SetDamageSent ( true );

        DamagerID = g_pClientGame->GetDamagerID ();
    }
    BitStream.Write ( DamagerID );
    if ( DamagerID != RESERVED_ELEMENT_ID )
    {
        BitStream.Write ( g_pClientGame->GetDamageWeapon () );
        BitStream.Write ( g_pClientGame->GetDamageBodyPiece () );
    }    

    // Write the sent position to the interpolator
    AddInterpolation ( vecActualPosition );
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
    BitStream.Read ( usLatency );
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
        pPlayer->SetCurrentWeaponSlot ( static_cast < eWeaponSlot > ( 0 ) );
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
    // Read out the byte with keysyncs
    unsigned char ucKeys;
    if ( !BitStream.Read ( ucKeys ) )
    {
        return false;
    }

    // Put the result into the controllerstate
    if ( ( ucKeys & 0x01 ) ? true:false )
        ControllerState.LeftShoulder1 = 255;
    else
        ControllerState.LeftShoulder1 = 0;

    if ( ( ucKeys & 0x02 ) ? true:false )
        ControllerState.RightShoulder1 = 255;
    else
        ControllerState.RightShoulder1 = 0;

    if ( ( ucKeys & 0x04 ) ? true:false )
        ControllerState.ButtonSquare = 255;
    else
        ControllerState.ButtonSquare = 0;

    if ( ( ucKeys & 0x08 ) ? true:false )
        ControllerState.ButtonCross = 255;
    else
        ControllerState.ButtonCross = 0;

    if ( ( ucKeys & 0x10 ) ? true:false )
        ControllerState.ButtonCircle = 255;
    else
        ControllerState.ButtonCircle = 0;

    if ( ( ucKeys & 0x20 ) ? true:false )
        ControllerState.ButtonTriangle = 255;
    else
        ControllerState.ButtonTriangle = 0;

    if ( ( ucKeys & 0x40 ) ? true:false )
        ControllerState.ShockButtonL = 255;
    else
        ControllerState.ShockButtonL = 0;

    if ( ( ucKeys & 0x80 ) ? true:false )
        ControllerState.m_bPedWalk = 255;
    else
        ControllerState.m_bPedWalk = 0;

    // Read out a byte indicating if left or right key changed
    // TODO: Move this out so that these bools come from an another byte with free bits in it
    unsigned char ucChangeFlags;
    if ( !BitStream.Read ( ucChangeFlags ) )
    {
        return false;
    }

    // Read out changed flags
    bool bLeftStickXChanged = ( ucChangeFlags & 0x01 ) ? true:false;
    bool bLeftStickYChanged = ( ucChangeFlags & 0x02 ) ? true:false;

    // Left stick X changed?
    if ( bLeftStickXChanged )
    {
        // Read out stick X
	    if ( !BitStream.Read ( ControllerState.LeftStickX ) )
        {
            return false;
        }
    }
    else
    {
	    ControllerState.LeftStickX = LastControllerState.LeftStickX;
    }

    // Left stick X changed?
    if ( bLeftStickYChanged )
    {
        // Read out stick Y
	    if ( !BitStream.Read ( ControllerState.LeftStickY ) )
        {
            return false;
        }
    }
    else
    {
	    ControllerState.LeftStickY = LastControllerState.LeftStickY;
    }

    // Success
    return true;
}


void CNetAPI::WriteSmallKeysync ( const CControllerState& ControllerState, const CControllerState& LastControllerState, NetBitStreamInterface& BitStream )
{
    // Put the controllerstate bools into a key byte
    unsigned char ucKeys = 0;
    ucKeys |= ( ControllerState.LeftShoulder1 ? true:false ) ? 1:0;          // Action / Secondary-Fire
    ucKeys |= ( ControllerState.RightShoulder1 ? true:false ) << 1;          // Aim-Weapon / Handbrake
    ucKeys |= ( ControllerState.ButtonSquare ? true:false ) << 2;            // Jump / Reverse
    ucKeys |= ( ControllerState.ButtonCross ? true:false ) << 3;             // Sprint / Accelerate
    ucKeys |= ( ControllerState.ButtonCircle ? true:false ) << 4;            // Fire // Fire
    ucKeys |= ( ControllerState.ButtonTriangle ? true:false ) << 5;          // Enter/Exit/Special-Attack / Enter/exit
    ucKeys |= ( ControllerState.ShockButtonL ? true:false ) << 6;            // Crouch / Horn
    ucKeys |= ( ControllerState.m_bPedWalk ? true:false ) << 7;              // Walk / -

    // Write it
    BitStream.Write ( ucKeys );

    // Did the leftstick x/y's change?
    bool bLeftStickXChange = ( ControllerState.LeftStickX != LastControllerState.LeftStickX );
    bool bLeftStickYChange = ( ControllerState.LeftStickY != LastControllerState.LeftStickY );

    // Put it in a byte and write it
    unsigned char ucChanged = 0;
    ucChanged |= bLeftStickXChange ? 1:0;
    ucChanged |= bLeftStickYChange << 1;

    // Write the byte
    BitStream.Write ( ucChanged );

    // Write the left stick X
    if ( bLeftStickXChange )
    {
	    BitStream.Write ( ControllerState.LeftStickX );
    }

    // Write the left stick Y
    if ( bLeftStickYChange )
    {
	    BitStream.Write( ControllerState.LeftStickY );
    }
}


bool CNetAPI::ReadFullKeysync ( CControllerState& ControllerState, NetBitStreamInterface& BitStream )
{
    // Read out the byte with keysyncs
    unsigned char ucKeys;
    if ( !BitStream.Read ( ucKeys ) )
    {
        return false;
    }

    // Put the result into the controllerstate
    if ( ( ucKeys & 0x01 ) ? true:false )
        ControllerState.LeftShoulder1 = 255;
    else
        ControllerState.LeftShoulder1 = 0;

    if ( ( ucKeys & 0x02 ) ? true:false )
        ControllerState.RightShoulder1 = 255;
    else
        ControllerState.RightShoulder1 = 0;

    if ( ( ucKeys & 0x04 ) ? true:false )
        ControllerState.ButtonSquare = 255;
    else
        ControllerState.ButtonSquare = 0;

    if ( ( ucKeys & 0x08 ) ? true:false )
        ControllerState.ButtonCross = 255;
    else
        ControllerState.ButtonCross = 0;

    if ( ( ucKeys & 0x10 ) ? true:false )
        ControllerState.ButtonCircle = 255;
    else
        ControllerState.ButtonCircle = 0;

    if ( ( ucKeys & 0x20 ) ? true:false )
        ControllerState.ButtonTriangle = 255;
    else
        ControllerState.ButtonTriangle = 0;

    if ( ( ucKeys & 0x40 ) ? true:false )
        ControllerState.ShockButtonL = 255;
    else
        ControllerState.ShockButtonL = 0;

    if ( ( ucKeys & 0x80 ) ? true:false )
        ControllerState.m_bPedWalk = 255;
    else
        ControllerState.m_bPedWalk = 0;

    // Apply leftstick X and Y
    return ( BitStream.Read ( ControllerState.LeftStickX ) &&
	         BitStream.Read ( ControllerState.LeftStickY ) );
}


void CNetAPI::WriteFullKeysync ( const CControllerState& ControllerState, NetBitStreamInterface& BitStream )
{
    // Put the controllerstate bools into a key byte
    unsigned char ucKeys = 0;
    ucKeys |= ( ControllerState.LeftShoulder1 ? true:false ) ? 1:0;
    ucKeys |= ( ControllerState.RightShoulder1 ? true:false ) << 1;
    ucKeys |= ( ControllerState.ButtonSquare ? true:false ) << 2;
    ucKeys |= ( ControllerState.ButtonCross ? true:false ) << 3;
    ucKeys |= ( ControllerState.ButtonCircle ? true:false ) << 4;
    ucKeys |= ( ControllerState.ButtonTriangle ? true:false ) << 5;
    ucKeys |= ( ControllerState.ShockButtonL ? true:false ) << 6;
    ucKeys |= ( ControllerState.m_bPedWalk ? true:false ) << 7;

    // Write it
    BitStream.Write ( ucKeys );

    // Write the left stick X and Y
	BitStream.Write ( ControllerState.LeftStickX );
	BitStream.Write ( ControllerState.LeftStickY );
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

        g_pNet->SendPacket ( PACKET_ID_RPC, pRPCBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED, packetOrdering );
        g_pNet->DeallocateNetBitStream ( pRPCBitStream );
    }
}