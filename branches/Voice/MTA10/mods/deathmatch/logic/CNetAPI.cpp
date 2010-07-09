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
            SPositionSync position ( false );

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
                BitStream.Read ( &position );

                // And rotation
                SRotationDegreesSync rotation ( false );
                BitStream.Read ( &rotation );

                // Remember that position
                m_vecLastReturnPosition = position.data.vecPosition;
                m_vecLastReturnRotation = rotation.data.vecRotation;
                m_bVehicleLastReturn = true;
            }
            else
            {
                // Read out the position
                BitStream.Read ( &position );
                
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
                m_vecLastReturnPosition = position.data.vecPosition;
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
    m_Interpolator.Push ( vecPosition, ulCurrentTime );
}


bool CNetAPI::GetInterpolation ( CVector& vecPosition, unsigned short usLatency )
{
    unsigned long ulInterTime = CClientTime::GetTime () - usLatency;
    return m_Interpolator.Evaluate ( ulInterTime, &vecPosition );
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
            if ( IsPureSyncNeeded () && !g_pClientGame->IsDownloadingBigPacket () )
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
                if ( IsSmallKeySyncNeeded ( pPlayer ) && !g_pClientGame->IsDownloadingBigPacket () )
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

            // Time to freeze because of lack of return sync?
            if ( !g_pClientGame->IsDownloadingBigPacket () &&
                    ( m_bStoredReturnSync ) &&
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
                int iPosX = g_pCore->GetGraphics ()->GetViewportWidth () / 2;             // Half way across
                int iPosY = g_pCore->GetGraphics ()->GetViewportHeight () * 45 / 100;     // 45/100 down
                g_pCore->GetGraphics ()->DrawText ( iPosX, iPosY, iPosX, iPosY, COLOR_ARGB ( 255, 255, 0, 0 ), "*** NETWORK TROUBLE ***", 2.0f, 2.0f, DT_NOCLIP | DT_CENTER );
            }
        }

        if ( pPlayer )
        {
            // Do camera sync even if player is dead
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
        }
    }
}


bool CNetAPI::IsSmallKeySyncNeeded ( CClientPed* pPlayerModel )
{
    // Grab the current and the old controllerstate
    CControllerState ControllerState, LastControllerState;
    pPlayerModel->GetControllerState ( ControllerState );
    pPlayerModel->GetLastControllerState ( LastControllerState );

    bool bPlaneRudderSync = false;
    CClientVehicle* pVehicle = pPlayerModel->GetOccupiedVehicle ();
    if ( pVehicle && 
         ( pVehicle->GetVehicleType () == CLIENTVEHICLE_PLANE ||
           pVehicle->GetVehicleType () == CLIENTVEHICLE_HELI ) )
    {
        bPlaneRudderSync = ( ControllerState.LeftShoulder2 != LastControllerState.LeftShoulder2 ||
                             ControllerState.RightShoulder2 != LastControllerState.RightShoulder2 );
    }

    // Compare the parts we sync
    return ( bPlaneRudderSync ||
             ControllerState.LeftShoulder1 != LastControllerState.LeftShoulder1 ||
             ControllerState.RightShoulder1 != LastControllerState.RightShoulder1 ||
             ControllerState.ButtonSquare != LastControllerState.ButtonSquare ||
             ControllerState.ButtonCross != LastControllerState.ButtonCross ||
             ControllerState.ButtonCircle != LastControllerState.ButtonCircle ||
             ControllerState.ButtonTriangle != LastControllerState.ButtonTriangle ||
             ControllerState.ShockButtonL != LastControllerState.ShockButtonL ||
             ControllerState.m_bPedWalk != LastControllerState.m_bPedWalk );
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
    // Grab the old controller state
    CControllerState LastControllerState;
    pPlayer->GetLastControllerState ( LastControllerState );

    // Grab the current controller state
    CControllerState CurrentControllerState;
    pPlayer->GetControllerState ( CurrentControllerState );

    // Read out the small keysync data
    CControllerState ControllerState;
    ReadSmallKeysync ( ControllerState, LastControllerState, BitStream );

    // We don't sync the direction keys in keysync, so we set them
    // to the current ones.
    ControllerState.LeftStickX = CurrentControllerState.LeftStickX;
    ControllerState.LeftStickY = CurrentControllerState.LeftStickY;

    // Flags
    SKeysyncFlags flags;
    BitStream.Read ( &flags );

    // Grab the occupied vehicle
    CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();

    // If he's shooting
    if ( ControllerState.ButtonCircle )
    {
        // Read out his current weapon slot
        SWeaponSlotSync slot;
        BitStream.Read ( &slot );
        unsigned int uiSlot = slot.data.uiSlot;
        CWeapon* pWeapon = pPlayer->GetWeapon ( static_cast < eWeaponSlot > ( uiSlot ) );

        // Is the current weapon a goggle (44 or 45) or a camera (43), detonator (40), don't apply the fire key
        if ( uiSlot == 11 || uiSlot == 12 || ( pWeapon && pWeapon->GetType () == 43 ) )
            ControllerState.ButtonCircle = 0;

        if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {
            unsigned char ucCurrentWeaponType = 0;
            float fWeaponRange = 1.6f;

            if ( pWeapon )
            {
                ucCurrentWeaponType = pWeapon->GetType ();
                fWeaponRange = pWeapon->GetInfo ()->GetWeaponRange ();
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

            // Read the weapon aim data
            SWeaponAimSync aim ( fWeaponRange );
            BitStream.Read ( &aim );

            // Read out the driveby direction
            unsigned char ucDriveByAim;
            BitStream.Read ( ucDriveByAim );

            // Set the aim data (immediately if in vehicle, otherwize delayed/interpolated)
            if ( pVehicle )
            {
                pPlayer->SetAimingData ( TICK_RATE, aim.data.vecTarget, aim.data.fArm, 0.0f, ucDriveByAim, &(aim.data.vecOrigin), false );
            }
            else
            {
                pPlayer->SetTargetTarget ( TICK_RATE, aim.data.vecOrigin, aim.data.vecTarget );
                pPlayer->SetAimInterpolated ( TICK_RATE, pPlayer->GetCurrentRotation (), aim.data.fArm, flags.data.bAkimboTargetUp, ucDriveByAim );
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
    if ( pVehicle && flags.data.bSyncingVehicle )
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

        if ( pVehicle->GetVehicleType () == CLIENTVEHICLE_PLANE ||
             pVehicle->GetVehicleType () == CLIENTVEHICLE_HELI )
        {
            ControllerState.LeftShoulder2 = 255 * BitStream.ReadBit ();
            ControllerState.RightShoulder2 = 255 * BitStream.ReadBit ();
        }

        // Apply the new keysync data immediately
        pPlayer->SetChoking ( false );
    }
    else
    {
        // null out the crouch key or it will conflict with the crouch syncing
        ControllerState.ShockButtonL = 0;
        pPlayer->Duck ( flags.data.bIsDucked );   
        pPlayer->SetChoking ( flags.data.bIsChoking );       
    }

    pPlayer->SetControllerState ( ControllerState );

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
    CClientVehicle* pVehicle = pPlayerModel->GetOccupiedVehicle ();

    // Write them to the bitstream
    WriteSmallKeysync ( ControllerState, LastControllerState, BitStream );

    // Flags
    SKeysyncFlags flags;
    flags.data.bIsDucked        = ( pPlayerModel->IsDucked () == true );
    flags.data.bIsChoking       = ( pPlayerModel->IsChoking () == true );
    flags.data.bAkimboTargetUp  = ( g_pMultiplayer->GetAkimboTargetUp () == true );
    flags.data.bSyncingVehicle  = ( pVehicle != NULL && pPlayerModel->GetOccupiedVehicleSeat () == 0 );

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
                ammo.data.usAmmoInClip = static_cast < unsigned short > ( pPlayerWeapon->GetAmmoInClip () );
                BitStream.Write ( &ammo );

                // Write the aim data
                SWeaponAimSync aim ( pPlayerWeapon->GetInfo ()->GetWeaponRange () );
                if ( pVehicle )
                    pPlayerModel->GetShotData ( &aim.data.vecOrigin, &aim.data.vecTarget, NULL, NULL, &aim.data.fArm );
                else
                    pPlayerModel->GetShotData ( &aim.data.vecOrigin, &aim.data.vecTarget, NULL, NULL, NULL, &aim.data.fArm );
                BitStream.Write ( &aim );

                // Write the driveby direction
                CShotSyncData* pShotsyncData = g_pMultiplayer->GetLocalShotSyncData ();
                BitStream.Write ( pShotsyncData->m_cInVehicleAimDirection );
            }
        }
        else
        {
            BitStream.WriteBit ( false );
        }
    }

    // Eventually write vehicle specific stuff.
    if ( flags.data.bSyncingVehicle )
    {
        WriteSmallVehicleSpecific ( pVehicle, BitStream );

        CVehicleUpgrades * pUpgrades = pVehicle->GetUpgrades ();
        if ( pUpgrades )
        {
            if ( pUpgrades->HasUpgrade ( 1087 ) ) // Hydraulics?
            {
                BitStream.Write ( ControllerState.RightStickX );
                BitStream.Write ( ControllerState.RightStickY );
            }
        }

        if ( pVehicle->GetVehicleType () == CLIENTVEHICLE_PLANE ||
             pVehicle->GetVehicleType () == CLIENTVEHICLE_HELI )
        {
            BitStream.WriteBit ( ControllerState.LeftShoulder2 != 0 );
            BitStream.WriteBit ( ControllerState.RightShoulder2 != 0 );
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
    pPlayer->SetPing ( usLatency );

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
    SPositionSync position ( false );
    BitStream.Read ( &position );

    // If the players in contact with an object/vehicle, make that the origin
    if ( pContactEntity )
    {
        CVector vecTempPos;
        pContactEntity->GetPosition ( vecTempPos );
        position.data.vecPosition += vecTempPos;
    }

    // Player rotation
    SPedRotationSync rotation;
    BitStream.Read ( &rotation );

    // Move speed vector
    if ( flags.data.bSyncingVelocity )
    {
        SVelocitySync velocity;
        if ( BitStream.Read ( &velocity ) )
            pPlayer->SetMoveSpeed ( velocity.data.vecVelocity );
    }

    // Player health
    SPlayerHealthSync health;
    BitStream.Read ( &health );
    pPlayer->SetHealth ( health.data.fValue );
    pPlayer->LockHealth ( health.data.fValue );

    // Player armor
    SPlayerArmorSync armor;
    BitStream.Read ( &armor );
    pPlayer->SetArmor ( armor.data.fValue );
    pPlayer->LockArmor ( armor.data.fValue );

    // Read out the camera rotation
    float fCameraRotation;
    BitStream.Read ( fCameraRotation );    

    // Current weapon id
    if ( flags.data.bHasAWeapon )
    {
        SWeaponSlotSync slot;
        BitStream.Read ( &slot );

        unsigned int uiSlot = slot.data.uiSlot;
        CWeapon* pWeapon = pPlayer->GetWeapon ( static_cast < eWeaponSlot > ( uiSlot ) );

        // Is the current weapon goggles (44 or 45) or a camera (43), or a detonator (40), don't apply the fire key
        if ( uiSlot == 11 || uiSlot == 12 || ( pWeapon && pWeapon->GetType () == 43 ) )
            ControllerState.ButtonCircle = 0;

        if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {            
     
            unsigned char ucCurrentWeapon = 0;
            float fWeaponRange = 0.01f;
            if ( pWeapon )
            {
                ucCurrentWeapon = pWeapon->GetType ();
                fWeaponRange = pWeapon->GetInfo ()->GetWeaponRange ();
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
            SWeaponAimSync aim ( fWeaponRange, ( ControllerState.RightShoulder1 || ControllerState.ButtonCircle ) );
            BitStream.Read ( &aim );

            // Interpolate the aiming
            pPlayer->SetAimInterpolated ( TICK_RATE, rotation.data.fRotation, aim.data.fArm, flags.data.bAkimboTargetUp, 0 );

            // Read the aim data only if he's shooting or aiming
            if ( aim.isFull() )
            {
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
        position.data.vecPosition -= vecTempPos;
    }

    // If the player is working on leaving a vehicle, don't set any target position
    if ( pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_NONE ||
         pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_GETTING_IN ||
         pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_JACKING )
    {
        pPlayer->SetTargetPosition ( position.data.vecPosition, TICK_RATE, pContactEntity );
        pPlayer->SetCurrentRotation ( rotation.data.fRotation );
    }

    // Set move speed, controller state and camera rotation + duck state
    pPlayer->SetControllerState ( ControllerState );
    pPlayer->SetCameraRotation ( fCameraRotation );
    pPlayer->Duck ( flags.data.bIsDucked );
    pPlayer->SetChoking ( flags.data.bIsChoking );
    pPlayer->SetOnFire ( flags.data.bIsOnFire );
    pPlayer->SetStealthAiming ( flags.data.bStealthAiming );

    // Remember now as the last puresync time
    pPlayer->SetLastPuresyncTime ( CClientTime::GetTime () );
    pPlayer->SetLastPuresyncPosition ( position.data.vecPosition );
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
    flags.data.bWearsGoogles    = ( pPlayerModel->IsWearingGoggles ( true ) == true );
    flags.data.bHasContact      = bInContact;
    flags.data.bIsChoking       = ( pPlayerModel->IsChoking () == true );
    flags.data.bAkimboTargetUp  = ( g_pMultiplayer->GetAkimboTargetUp () == true );
    flags.data.bIsOnFire        = ( pPlayerModel->IsOnFire () == true );
    flags.data.bHasAWeapon      = ( pPlayerWeapon != NULL );
    flags.data.bSyncingVelocity = ( !flags.data.bIsOnGround || ( pPlayerModel->GetPlayerSyncCount () % 4 ) == 0 );
    flags.data.bStealthAiming   = ( pPlayerModel->IsStealthAiming () == true );

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

    SPositionSync position ( false );
    position.data.vecPosition = vecPosition;
    BitStream.Write ( &position );

    // Player rotation
    SPedRotationSync rotation;
    rotation.data.fRotation = pPlayerModel->GetCurrentRotation ();
    BitStream.Write ( &rotation );

    // Move speed vector
    if ( flags.data.bSyncingVelocity )
    {
        SVelocitySync velocity;
        pPlayerModel->GetMoveSpeed ( velocity.data.vecVelocity );
        BitStream.Write ( &velocity );
    }

    // Player health sync (scaled from 0.0f-200.0f to 0-255 to save three bytes).
    // Scale goes up to 200.0f because having max stats gives you the double of health.
    SPlayerHealthSync health;
    health.data.fValue = pPlayerModel->GetHealth ();
    BitStream.Write ( &health );

    // Player armor (scaled from 0.0f-100.0f to 0-255 to save three bytes)
    SPlayerArmorSync armor;
    armor.data.fValue = pPlayerModel->GetArmor ();
    BitStream.Write ( &armor );

    // Write the camera rotation
    BitStream.Write ( g_pGame->GetCamera ()->GetCameraRotation () );

    if ( flags.data.bHasAWeapon )
    {
        if ( BitStream.Version () >= 0x0d )
        {
            unsigned char ucWeaponType = pPlayerWeapon->GetType ();
            BitStream.Write ( ucWeaponType );
        }

        // Write the weapon slot
        unsigned int uiSlot = pPlayerWeapon->GetSlot ();
        SWeaponSlotSync slot;
        slot.data.uiSlot = uiSlot;
        BitStream.Write ( &slot );

        if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {
            // Write the ammo states
            SWeaponAmmoSync ammo ( pPlayerWeapon->GetType (), true, true );
            ammo.data.usAmmoInClip = static_cast < unsigned short > ( pPlayerWeapon->GetAmmoInClip () );
            ammo.data.usTotalAmmo = static_cast < unsigned short > ( pPlayerWeapon->GetAmmoTotal () );
            BitStream.Write ( &ammo );

            // Sync aim data
            CShotSyncData* pShotsyncData = g_pMultiplayer->GetLocalShotSyncData ();
            SWeaponAimSync aim ( 0.0f, ( ControllerState.RightShoulder1 || ControllerState.ButtonCircle ) );
            aim.data.fArm = pShotsyncData->m_fArmDirectionY;

            // Write the vectors data only if he's aiming or shooting
            if ( ControllerState.RightShoulder1 || ControllerState.ButtonCircle )
            {
                pPlayerModel->GetShotData ( &(aim.data.vecOrigin), &(aim.data.vecTarget) );
            }
            BitStream.Write ( &aim );
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
        weaponType.data.ucWeaponType = g_pClientGame->GetDamageWeapon ();
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
    pPlayer->SetPing ( usLatency );

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
        SPositionSync position ( false );
        BitStream.Read ( &position );

        SRotationDegreesSync rotation;
        BitStream.Read ( &rotation );

        // Read out the movespeed
        SVelocitySync velocity;
        BitStream.Read ( &velocity );

        // Read out the turnspeed
        SVelocitySync turnSpeed;
        BitStream.Read ( &turnSpeed );

        // Read out the vehicle health
        SVehicleHealthSync health;
        BitStream.Read ( &health );
        pVehicle->SetHealth ( health.data.fValue );

        // Set the target position and rotation
        pVehicle->SetTargetPosition ( position.data.vecPosition, TICK_RATE );
        pVehicle->SetTargetRotation ( rotation.data.vecRotation, TICK_RATE );

        // Apply the correct move and turnspeed
        pVehicle->SetMoveSpeed ( velocity.data.vecVelocity );
        pVehicle->SetTurnSpeed ( turnSpeed.data.vecVelocity );
    }

    // Player health
    SPlayerHealthSync health;
    BitStream.Read ( &health );
    pPlayer->SetHealth ( health.data.fValue );
    pPlayer->LockHealth ( health.data.fValue );

    // Player armor
    SPlayerArmorSync armor;
    BitStream.Read ( &armor );
    pPlayer->SetArmor ( armor.data.fValue );
    pPlayer->LockArmor ( armor.data.fValue );

    // Vehicle flags
    SVehiclePuresyncFlags flags;
    BitStream.Read ( &flags );

    // Set flag stuff
    pPlayer->SetWearingGoggles ( flags.data.bIsWearingGoggles );

    // Sirene states
    int iModelID = pVehicle->GetModel ();
    if ( uiSeat == 0 )
    {
        if ( CClientVehicleManager::HasSirens ( iModelID ) )
        {
            pVehicle->SetSirenOrAlarmActive ( flags.data.bIsSirenOrAlarmActive );
        }

        // Smoke trail
        if ( CClientVehicleManager::HasSmokeTrail ( iModelID ) )
        {
            pVehicle->SetSmokeTrailEnabled ( flags.data.bIsSmokeTrailEnabled );
        }

        // Landing gear vehicles
        if ( CClientVehicleManager::HasLandingGears ( iModelID ) )
        {
            pVehicle->SetLandingGearDown ( flags.data.bIsLandingGearDown );
        }

        // Derailed state
        if ( pVehicle->GetVehicleType() == CLIENTVEHICLE_TRAIN )
        {
            pVehicle->SetDerailed ( flags.data.bIsLandingGearDown );
        }

        // Heli search light
        if ( CClientVehicleManager::HasSearchLight ( iModelID ) )
        {
            pVehicle->SetHeliSearchLightVisible ( flags.data.bIsHeliSearchLightVisible );
        }
    }

    if ( flags.data.bHasAWeapon )
    {
        SWeaponSlotSync slot;
        BitStream.Read ( &slot );

        unsigned int uiSlot = slot.data.uiSlot;
        CWeapon* pWeapon = pPlayer->GetWeapon ( static_cast < eWeaponSlot > ( uiSlot ) );

        // Is the current weapon a goggle (44 or 45) or a camera (43), or a detonator (40), don't apply the fire key
        if ( uiSlot == 11 || uiSlot == 12 || ( pWeapon && pWeapon->GetType () == 43 ) )
            ControllerState.ButtonCircle = 0;

        if ( flags.data.bIsDoingGangDriveby && CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {

            unsigned char ucCurrentWeapon = 0;
            float fWeaponRange = 0.01f;
            if ( pWeapon )
            {
                ucCurrentWeapon = pWeapon->GetType ();
                fWeaponRange = pWeapon->GetInfo ()->GetWeaponRange ();
            }

            // Read out the weapon ammo
            SWeaponAmmoSync ammo ( ucCurrentWeapon, false, true );
            BitStream.Read ( &ammo );
            unsigned short usWeaponAmmo = ammo.data.usAmmoInClip;

            if ( pWeapon )
            {
                CWeapon* pPlayerWeapon = pPlayer->GiveWeapon ( static_cast < eWeaponType > ( ucCurrentWeapon ), usWeaponAmmo );
                if ( pPlayerWeapon )
                {
                    pPlayerWeapon->SetAsCurrentWeapon ();
                    pPlayerWeapon->SetAmmoTotal ( 9999 );
                    pPlayerWeapon->SetAmmoInClip ( usWeaponAmmo );
                }
            }

            // Read out the aim directions
            SWeaponAimSync aim ( fWeaponRange, true );
            BitStream.Read ( &aim );

            // Read out the driveby direction
            SDrivebyDirectionSync driveby;
            BitStream.Read ( &driveby );

            // Set the aiming
            pPlayer->SetAimingData ( TICK_RATE, aim.data.vecTarget, aim.data.fArm, 0.0f, driveby.data.ucDirection, &aim.data.vecOrigin, false );
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

    // Read out vehicle specific data if he's the driver
    if ( uiSeat == 0 )
    {
        ReadFullVehicleSpecific ( pVehicle, BitStream );
    }

    // Gang driveby
    if ( flags.data.bIsDoingGangDriveby &&
         ( pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_GETTING_OUT ||
           pPlayer->GetVehicleInOutState () == VEHICLE_INOUT_GETTING_JACKED ) )
    {
        // Don't set the ped doing gang driveby if it's leaving the vehicle
        flags.data.bIsDoingGangDriveby = false;
    }
    pPlayer->SetDoingGangDriveby ( flags.data.bIsDoingGangDriveby );

    // Read the vehicle_look_left and vehicle_look_right control states
    // if it's an aircraft.
    if ( flags.data.bIsAircraft )
    {
        ControllerState.LeftShoulder2 = BitStream.ReadBit () * 255;
        ControllerState.RightShoulder2 = BitStream.ReadBit () * 255;
    }

    pPlayer->SetControllerState ( ControllerState );

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
    SPositionSync position ( false );
    position.data.vecPosition = vecPosition;
    BitStream.Write ( &position );

    // Grab the occupied vehicle seat. Send this only if we're driver.
    unsigned int uiSeat = pPlayerModel->GetOccupiedVehicleSeat ();
    BitStream.Write ( ( unsigned char ) uiSeat );
    if ( uiSeat == 0 )
    {
        // Write the rotation in degrees
        SRotationDegreesSync rotation;
        pVehicle->GetRotationDegrees ( rotation.data.vecRotation );
        BitStream.Write ( &rotation );

        // Write the movespeed/turnspeed
        CVector vecMoveSpeed, vecTurnSpeed;

        // Jax: if the vehicles streamed out, make sure it stays still for everyone else
        if ( pVehicle->GetGameVehicle () )
        {
            pVehicle->GetMoveSpeed ( vecMoveSpeed ); 
            pVehicle->GetTurnSpeed ( vecTurnSpeed ); 
        }

        SVelocitySync velocity;
        velocity.data.vecVelocity = vecMoveSpeed;
        BitStream.Write ( &velocity );

        SVelocitySync turnSpeed;
        turnSpeed.data.vecVelocity = vecTurnSpeed;
        BitStream.Write ( &turnSpeed );

        // Write the health
        SVehicleHealthSync health;
        health.data.fValue = pVehicle->GetHealth ();
        BitStream.Write ( &health );

        // Write the trailer chain
        CClientVehicle* pTrailer = pVehicle->GetRealTowedVehicle ();
        while ( pTrailer )
        {
            BitStream.WriteBit ( true );
            BitStream.WriteCompressed ( pTrailer->GetID () );

            // Write the position and rotation
            CVector vecTrailerPosition, vecTrailerRotationDegrees;

            // Write the matrix
            pTrailer->GetPosition ( vecTrailerPosition );
            pTrailer->GetRotationDegrees ( vecTrailerRotationDegrees );

            SPositionSync trailerPosition ( false );
            trailerPosition.data.vecPosition = vecTrailerPosition;
            BitStream.Write ( &trailerPosition );

            SRotationDegreesSync trailerRotation;
            trailerRotation.data.vecRotation = vecTrailerRotationDegrees;
            BitStream.Write ( &trailerRotation );

            // Get the next towed vehicle
            pTrailer = pTrailer->GetTowedVehicle ();
        }

        // End of our trailer chain
        BitStream.WriteBit ( false );
    }

    // Player health sync (scaled from 0.0f-200.0f to 0-255 to save three bytes).
    // Scale goes up to 200.0f because having max stats gives you the double of health.
    SPlayerHealthSync health;
    health.data.fValue = pPlayerModel->GetHealth ();
    BitStream.Write ( &health );

    // Player armor (scaled from 0.0f-100.0f to 0-255 to save three bytes)
    SPlayerArmorSync armor;
    armor.data.fValue = pPlayerModel->GetArmor ();
    BitStream.Write ( &armor );

    // Get the player weapon
    CWeapon * pPlayerWeapon = pPlayerModel->GetWeapon();

    // Flags
    SVehiclePuresyncFlags flags;
    flags.data.bIsWearingGoggles = pPlayerModel->IsWearingGoggles ( true );
    flags.data.bIsDoingGangDriveby = pPlayerModel->IsDoingGangDriveby ();
    flags.data.bIsSirenOrAlarmActive = pVehicle->IsSirenOrAlarmActive ();
    flags.data.bIsSmokeTrailEnabled = pVehicle->IsSmokeTrailEnabled ();
    flags.data.bIsLandingGearDown = pVehicle->IsLandingGearDown ();
    flags.data.bIsOnGround = pVehicle->IsOnGround ();
    flags.data.bIsInWater = pVehicle->IsInWater ();
    flags.data.bIsDerailed = pVehicle->IsDerailed ();
    flags.data.bIsAircraft = ( pVehicle->GetVehicleType () == CLIENTVEHICLE_PLANE ||
                               pVehicle->GetVehicleType () == CLIENTVEHICLE_HELI );
    flags.data.bHasAWeapon = ( pPlayerWeapon != NULL );
    flags.data.bIsHeliSearchLightVisible = pVehicle->IsHeliSearchLightVisible ();


    // Write the flags
    BitStream.Write ( &flags );

    if ( flags.data.bHasAWeapon )
    {
        // Write the weapon slot
        unsigned int uiSlot = pPlayerWeapon->GetSlot ();
        SWeaponSlotSync slot;
        slot.data.uiSlot = uiSlot;
        BitStream.Write ( &slot );

        if ( flags.data.bIsDoingGangDriveby && CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {
            // Write the ammo states
            SWeaponAmmoSync ammo ( pPlayerWeapon->GetType (), false, true );
            ammo.data.usAmmoInClip = static_cast < unsigned short > ( pPlayerWeapon->GetAmmoInClip () );
            BitStream.Write ( &ammo );

            // Sync aim data
            SWeaponAimSync aim ( 0.0f, true );
            pPlayerModel->GetShotData ( &(aim.data.vecOrigin), &(aim.data.vecTarget), NULL, NULL, &(aim.data.fArm) );
            BitStream.Write ( &aim );

            // Sync driveby direction
            CShotSyncData* pShotsyncData = g_pMultiplayer->GetLocalShotSyncData ();
            SDrivebyDirectionSync driveby;
            driveby.data.ucDirection = static_cast < unsigned char > ( pShotsyncData->m_cInVehicleAimDirection );
            BitStream.Write ( &driveby );
        }
    }

    // Write vehicle specific stuff if we're driver
    if ( uiSeat == 0 )
    {
        WriteFullVehicleSpecific ( pVehicle, BitStream );
    }

    // Write vehicle_look_left and vehicle_look_right control states when
    // it's an aircraft.
    if ( flags.data.bIsAircraft )
    {
        BitStream.WriteBit ( ControllerState.LeftShoulder2 != 0 );
        BitStream.WriteBit ( ControllerState.RightShoulder2 != 0 );
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
        SVehicleSpecific vehicle;
        BitStream.Read ( &vehicle );

        pVehicle->SetTurretRotation ( vehicle.data.fTurretX, vehicle.data.fTurretY );
    }
}


void CNetAPI::WriteSmallVehicleSpecific ( CClientVehicle* pVehicle, NetBitStreamInterface& BitStream )
{
    // Turret states
    int iModelID = pVehicle->GetModel ();
    if ( CClientVehicleManager::HasTurret ( iModelID ) )
    {
        SVehicleSpecific vehicle;
        pVehicle->GetTurretRotation ( vehicle.data.fTurretX, vehicle.data.fTurretY );

        BitStream.Write ( &vehicle );
    }
}


void CNetAPI::ReadFullVehicleSpecific ( CClientVehicle* pVehicle, NetBitStreamInterface& BitStream )
{
    // Turret states
    int iModelID = pVehicle->GetModel ();
    if ( CClientVehicleManager::HasTurret ( iModelID ) )
    {
        SVehicleSpecific vehicle;
        BitStream.Read ( &vehicle );

        pVehicle->SetTurretRotation ( vehicle.data.fTurretX, vehicle.data.fTurretY );
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
        SVehicleSpecific vehicle;
        pVehicle->GetTurretRotation ( vehicle.data.fTurretX, vehicle.data.fTurretY );

        BitStream.Write ( &vehicle );
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
    BitStream.WriteBit ( bFixed );
    if ( bFixed )
    {
        // Write our position
        SPositionSync position ( false );
        pCamera->GetPosition ( position.data.vecPosition );
        BitStream.Write ( &position );

        // Write our lookAt
        SPositionSync lookAt ( false );
        pCamera->GetTarget ( lookAt.data.vecPosition );
        BitStream.Write ( &lookAt );
    }
    else
    {
        // Write our target
        CClientPlayer * pPlayer = pCamera->GetFocusedPlayer ();
        if ( !pPlayer ) pPlayer = g_pClientGame->GetLocalPlayer ();

        BitStream.WriteCompressed ( pPlayer->GetID () );
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
            int iLength = pBitStream->GetNumberOfBitsUsed ();
            while ( iLength > 8 )
            {
                pBitStream->Read ( ucTemp );
                pRPCBitStream->Write ( ucTemp );
                iLength -= 8;
            }
            if ( iLength > 0 )
            {
                pBitStream->ReadBits ( &ucTemp, iLength );
                pRPCBitStream->WriteBits ( &ucTemp, iLength );
            }
            pBitStream->ResetReadPointer ();
        }

        g_pNet->SendPacket ( PACKET_ID_RPC, pRPCBitStream, PACKET_PRIORITY_MEDIUM, PACKET_RELIABILITY_RELIABLE_ORDERED, packetOrdering );
        g_pNet->DeallocateNetBitStream ( pRPCBitStream );
    }
}
