/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPed.cpp
*  PURPOSE:     Ped entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Fedor Sinev <fedorsinev@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using std::vector;

extern CClientGame* g_pClientGame;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define INVALID_VALUE   0xFFFFFFFF

#define PED_INTERPOLATION_WARP_THRESHOLD            5   // Minimal threshold
#define PED_INTERPOLATION_WARP_THRESHOLD_FOR_SPEED  5   // Units to increment the threshold per speed unit

enum eAnimGroups
{    
    ANIM_GROUP_GOGGLES=32,
    ANIM_GROUP_STEALTH_KN=87,
};

enum eAnimIDs
{
    ANIM_ID_WALK_CIVI=0,
    ANIM_ID_RUN_CIVI,
    ANIM_ID_SPRINT_PANIC,
    ANIM_ID_IDLE_STANCE,
    ANIM_ID_WEAPON_CROUCH=55,
    ANIM_ID_GOGGLES_ON=224,
    ANIM_ID_STEALTH_AIM=347,
};

#define STEALTH_KILL_RANGE 2.5f

struct SBodyPartName
{
    const char* szName;
};

static const SFixedArray < SBodyPartName, 10 > BodyPartNames =
{ {
    {"Unknown"}, {"Unknown"}, {"Unknown"}, {"Torso"}, {"Ass"},
    {"Left Arm"}, {"Right Arm"}, {"Left Leg"}, {"Right Leg"}, {"Head"}
} };


CClientPed::CClientPed ( CClientManager* pManager, unsigned long ulModelID, ElementID ID ) : ClassInit ( this ), CClientStreamElement ( pManager->GetPlayerStreamer (), ID ), CAntiCheatModule ( pManager->GetAntiCheat () )
{
    SetTypeName ( "ped" );

    // Init
    Init ( pManager, ulModelID, false );

    // Add it to our ped manager
    pManager->GetPedManager ()->AddToList ( this );
}


CClientPed::CClientPed ( CClientManager* pManager, unsigned long ulModelID, ElementID ID, bool bIsLocalPlayer ) : ClassInit ( this ), CClientStreamElement ( pManager->GetPlayerStreamer (), ID ), CAntiCheatModule ( pManager->GetAntiCheat () )
{
    // Init
    Init ( pManager, ulModelID, bIsLocalPlayer );

    // Add it to our ped manager
    pManager->GetPedManager ()->AddToList ( this );
}


void CClientPed::Init ( CClientManager* pManager, unsigned long ulModelID, bool bIsLocalPlayer )
{
    CClientEntityRefManager::AddEntityRefs ( ENTITY_REF_DEBUG ( this, "CClientPed" ), &m_pOccupiedVehicle, &m_pOccupyingVehicle, &m_pTargetedEntity, &m_pCurrentContactEntity, &m_pBulletImpactEntity, &m_interp.pTargetOriginSource, NULL );

    // Init members
    m_pManager = pManager;

    // Store the model we're going to use
    m_ulModel = ulModelID;
    m_pModelInfo = g_pGame->GetModelInfo ( ulModelID );

    m_bIsLocalPlayer = bIsLocalPlayer;
    m_pPad = g_pGame->GetPad ();

    m_pRequester = pManager->GetModelRequestManager ();

    m_iVehicleInOutState = VEHICLE_INOUT_NONE;
    m_pPlayerPed = NULL;
    m_pTaskManager = NULL;
    m_pOccupiedVehicle = NULL;
    m_pOccupyingVehicle = NULL;
    //m_uiOccupyingSeat = 0;
    m_uiOccupiedVehicleSeat = 0xFF;
    m_bHealthLocked = false;
    m_bDontChangeRadio = false;
    m_bArmorLocked = false;
    m_ulLastOnScreenTime = 0;
    m_pLoadedModelInfo = NULL;
    m_pOutOfVehicleWeaponSlot = WEAPONSLOT_MAX; // WEAPONSLOT_MAX = invalid
    m_bRadioOn = false;
    m_ucRadioChannel = 1;
    m_fBeginAimX = 0.0f;
    m_fBeginAimY = 0.0f;
    m_fTargetAimX = 0.0f;
    m_fTargetAimY = 0.0f;
    m_ulBeginAimTime = 0;
    m_ulTargetAimTime = 0;
    m_ulBeginRotationTime = 0;
    m_ulEndRotationTime = 0;
    m_fBeginRotation = 0.0f;
    m_fTargetRotationA = 0.0f;
    m_fBeginCameraRotation = 0.0f;
    m_fTargetCameraRotation = 0.0f;
    m_ulBeginTarget = 0;
    m_ulEndTarget = 0;
    m_bForceGettingIn = false;
    m_bForceGettingOut = false;
    m_ucLeavingDoor = 0xFF;
    m_bDucked = false;
    m_bWearingGoggles = false;
    m_bVisible = true;
    m_bUsesCollision = true;
    m_fHealth = 100.0f;
    m_fArmor = 0.0f;
    m_bWorldIgnored = false;
    m_fCurrentRotation = 0.0f;
    m_fMoveSpeed = 0.0f;
    m_bCanBeKnockedOffBike = true;
    RemoveAllWeapons ();        // Set all our weapon values to unarmed
    m_bHasJetPack = false;
    m_FightingStyle = STYLE_GRAB_KICK;
    m_MoveAnim = MOVE_PLAYER;
    m_ucAlpha = 255;
    m_fTargetRotation = 0.0f;
    m_bTargetAkimboUp = false;
    m_bIsChoking = false;
    m_ulLastTimeAimed = 0;
    m_ulLastTimeBeganCrouch = 0;
    m_ulLastTimeBeganStand = 0; //Standing after crouching
    m_ulLastTimeMovedWhileCrouched = 0; //Moved while crouching
    m_bRecreatingModel = false;
    m_pCurrentContactEntity = NULL;
    m_bSunbathing = false;
    m_bDestroyingSatchels = false;
    m_bDoingGangDriveby = false;
    m_pAnimationBlock = NULL;
    m_bRequestedAnimation = false;
    m_iTimeAnimation = -1;
    m_bLoopAnimation = false;    
    m_bUpdatePositionAnimation = false;
    m_bInterruptableAnimation = false;
    m_bFreezeLastFrameAnimation = true;
    m_bHeadless = false;
    m_bFrozen = false;
    m_bFrozenWaitingForGroundToLoad = false;
    m_fGroundCheckTolerance = 0.f;
    m_fObjectsAroundTolerance = 0.f;
    m_iLoadAllModelsCounter = 0;
    m_bIsOnFire = false;
    m_LastSyncedData = new SLastSyncedPedData;
    m_bSpeechEnabled = true;
    m_bStealthAiming = false;
    m_fLighting = 0.0f;
    m_bBulletImpactData = false;
    m_ucEnteringDoor = 0xFF;
    m_ucLeavingDoor = 0xFF;

    // Time based interpolation
    m_interp.pTargetOriginSource = NULL;
    m_interp.bHadOriginSource = false;
    m_interp.pos.ulFinishTime = 0;
   
    m_pClothes = new CClientPlayerClothes ( this );

    // Our default clothes
    m_pClothes->DefaultClothes ( false );

    // Movement state names for Lua
    m_MovementStateNames[MOVEMENTSTATE_STAND]       =    "stand";
    m_MovementStateNames[MOVEMENTSTATE_WALK]        =    "walk";
    m_MovementStateNames[MOVEMENTSTATE_POWERWALK]   =    "powerwalk";
    m_MovementStateNames[MOVEMENTSTATE_JOG]         =    "jog";
    m_MovementStateNames[MOVEMENTSTATE_SPRINT]      =    "sprint";
    m_MovementStateNames[MOVEMENTSTATE_CROUCH]      =    "crouch";
    //These two are inactive for now
    m_MovementStateNames[MOVEMENTSTATE_CRAWL]       =    "crawl";
    m_MovementStateNames[MOVEMENTSTATE_ROLL]        =    "roll";

    // Create the player model
    if ( m_bIsLocalPlayer )
    {
        // Init shotsync data stuff to 0
        m_remoteDataStorage = NULL;
        m_shotSyncData = g_pMultiplayer->GetLocalShotSyncData ();
        m_currentControllerState = NULL;
        m_lastControllerState = NULL;
        m_stats = NULL;

        // Init the local player
        _CreateLocalModel ();

        // Give full health, no armor, no weapons and put him at a safe location
        SetHealth ( GetMaxHealth () );
        SetArmor ( 0 );
        RemoveAllWeapons ();
        SetPosition ( CVector ( 2488.562f, -1662.40f, 23.335f ) );
    }
    else
    {
        // Add our shotsync data
        m_remoteDataStorage = g_pMultiplayer->CreateRemoteDataStorage ();
        m_remoteDataStorage->SetProcessPlayerWeapon ( true );
        m_shotSyncData = m_remoteDataStorage->ShotSyncData ( );
        m_currentControllerState = m_remoteDataStorage->CurrentControllerState ( );
        m_lastControllerState = m_remoteDataStorage->LastControllerState ( );
        m_stats = m_remoteDataStorage->Stats ( );        
        // ### remember if you want to set Int flags, subtract STATS_OFFSET from the enum ID ###

        SetStat ( MAX_HEALTH, 569.0f );     // Default max_health stat

        SetArmor ( 0.0f );
    }
    m_eStoredWeaponID = (eWeaponType)0;
    m_dwStoredAmmo = 0;
    m_dwStoredClipAmmo = 0;
    m_bStoredCurrentWeapon = false;
}


CClientPed::~CClientPed ( void )
{
    // Remove from the ped manager
    m_pManager->GetPedManager ()->RemoveFromList ( this );

    // Unreference us from stuff
    m_pManager->UnreferenceEntity ( this );

    // Remove our linked contact entity
    if ( m_pCurrentContactEntity )
    {
        m_pCurrentContactEntity->RemoveContact ( this );
        m_pCurrentContactEntity = NULL;
    }

    // Make sure we're not requesting any model
    m_pRequester->Cancel ( this, false );

    // Detach us from eventual entities
    AttachTo ( NULL );

    // Remove all our projectiles
    RemoveAllProjectiles ();

    // If this is the local player, give the player full health and put him at a safe location
    if ( m_bIsLocalPlayer )
    {
        SetHealth ( GetMaxHealth () );
        SetPosition ( CVector ( 2488.562f, -1662.40f, 23.335f ) );
        SetInterior ( 0 );
        SetDimension ( 0 );
        SetVoice ( "PED_TYPE_PLAYER", "VOICE_PLY_CR" );
        m_pClothes->DefaultClothes ( true );
        SetCanBeKnockedOffBike ( true );
        SetHeadless ( false );
    }
    else
    {
        // Remove our shotsync data
        g_pMultiplayer->RemoveRemoteDataStorage ( m_pPlayerPed );
        g_pMultiplayer->DestroyRemoteDataStorage ( m_remoteDataStorage );
        m_remoteDataStorage = NULL;
    }

    // We have a player model?
    if ( m_pPlayerPed )
    {
        // Destroy the player model
        if ( m_bIsLocalPlayer )
        {
            _DestroyLocalModel ();
        }
        else
        {
            _DestroyModel ();
        }
    }

    // Delete our clothes
    delete m_pClothes;
    m_pClothes = NULL;

    // Remove us from any occupied vehicle
    CClientVehicle::UnpairPedAndVehicle ( this );

    // Delete delayed sync data
    list < SDelayedSyncData* > ::iterator iter = m_SyncBuffer.begin ();
    for ( ; iter != m_SyncBuffer.end () ; iter++ )
    {
        delete *iter;
    }

    m_SyncBuffer.clear ();

    if ( m_interp.pTargetOriginSource )
    {
        m_interp.pTargetOriginSource->RemoveOriginSourceUser ( this );
        m_interp.pTargetOriginSource = NULL;
    }

    g_pClientGame->GetPedSync()->RemovePed ( this );

    CClientEntityRefManager::RemoveEntityRefs ( 0, &m_pOccupiedVehicle, &m_pOccupyingVehicle, &m_pTargetedEntity, &m_pCurrentContactEntity, &m_pBulletImpactEntity, &m_interp.pTargetOriginSource, NULL );
}   

void CClientPed::SetStat ( unsigned short usStat, float fValue )
{
    if ( m_bIsLocalPlayer )
    {
        if ( usStat < MAX_INT_FLOAT_STATS )
            g_pGame->GetStats()->SetStatValue ( usStat, fValue );
        g_pMultiplayer->SetLocalStatValue ( usStat, fValue );
    }
    else
    {
        if ( usStat < MAX_FLOAT_STATS )
            m_stats->StatTypesFloat [ usStat ] = fValue;
        else if ( usStat >= STATS_OFFSET && usStat < MAX_INT_FLOAT_STATS )
            m_stats->StatTypesInt [ usStat - STATS_OFFSET ] = (int)fValue;
    }
}

float CClientPed::GetStat ( unsigned short usStat )
{
    if ( m_bIsLocalPlayer )
    {
        return g_pGame->GetStats()->GetStatValue ( usStat );
    }
    else
    {
        if ( usStat < MAX_FLOAT_STATS )
            return m_stats->StatTypesFloat [ usStat ];
        else if ( usStat >= STATS_OFFSET && usStat < MAX_INT_FLOAT_STATS )
            return (float)m_stats->StatTypesInt [ usStat - STATS_OFFSET ];
        else
            return 0.0f;
    }
}


void CClientPed::ResetStats ( void )
{
    // stats
    for ( unsigned short us = 0 ; us <= NUM_PLAYER_STATS ; us++ )
    {
        if ( us == MAX_HEALTH )
        {
            SetStat ( us, 569.0f );
        }
        else
        {
            SetStat ( us, 0.0f );
        }
    }
}


bool CClientPed::GetMatrix ( CMatrix& Matrix ) const
{
    // Are we frozen?
    if ( IsFrozen () )
    {
        Matrix = m_matFrozen;
    }    
    else
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->GetMatrix ( &Matrix );
    }
    else
    {
        Matrix = m_Matrix;
    }

    return true;
}


bool CClientPed::SetMatrix ( const CMatrix& Matrix )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetMatrix ( const_cast < CMatrix* > ( &Matrix ) );
    }

    if ( m_Matrix.vPos != Matrix.vPos )
    {
        UpdateStreamPosition ( Matrix.vPos );
    }
    m_Matrix = Matrix;
    m_matFrozen = Matrix;

    return true;
}


void CClientPed::GetPosition ( CVector& vecPosition ) const
{
    CClientVehicle * pVehicle = const_cast < CClientPed * > ( this )->GetRealOccupiedVehicle ();

    // Are we frozen?
    if ( IsFrozen () )
    {
        vecPosition = m_matFrozen.vPos;
    }    
    // Streamed in?
    else if ( m_pPlayerPed )
    {
        vecPosition = *m_pPlayerPed->GetPosition ();
    }
    // In a vehicle?    
    else if ( pVehicle )
    {
        pVehicle->GetPosition ( vecPosition );
    }
    // Attached to an entity?
    else if ( m_pAttachedToEntity )
    {
        m_pAttachedToEntity->GetPosition ( vecPosition );
        vecPosition += m_vecAttachedPosition;
    }
    // None of the above?
    else
    {
        vecPosition = m_Matrix.vPos;
    }
}


void CClientPed::SetPosition ( const CVector& vecPosition, bool bResetInterpolation )
{
    // We have a player ped?
    if ( m_pPlayerPed )
    {
        // Don't set the actual position if we're in a vehicle
        if ( !GetRealOccupiedVehicle () )
        {
            // Set it only if we're not in a vehicle or not working on getting in/out
            if ( !m_pOccupiedVehicle || GetVehicleInOutState () != VEHICLE_INOUT_GETTING_OUT )
            {
                // Is this the local player?
                if ( m_bIsLocalPlayer )
                {
                    // If move is big enough, do ground checks
                    float DistanceMoved = ( m_Matrix.vPos - vecPosition ).Length ();
                    if ( DistanceMoved > 50 && !IsFrozen () )
                        SetFrozenWaitingForGroundToLoad ( true );
                }

                // Set the real position
                m_pPlayerPed->SetPosition ( const_cast < CVector* > ( &vecPosition ) );
            }
        }
    }

    // Have we moved to a different position?
    if ( m_Matrix.vPos != vecPosition )
    {
        // Store our new position
        m_Matrix.vPos = vecPosition;
        m_matFrozen.vPos = vecPosition;

        // Update our streaming position
        UpdateStreamPosition ( vecPosition );
    }

    if ( bResetInterpolation )
        RemoveTargetPosition ();
}


void CClientPed::SetInterior ( unsigned char ucInterior )
{
    CEntity * pEntity = GetGameEntity ();
    if ( pEntity )
    {
        pEntity->SetAreaCode ( ucInterior );
    }

    // If local player
    if ( m_bIsLocalPlayer )
    {
        // If our camera is in the same world as the player, move it
        if ( g_pGame->GetWorld ()->GetCurrentArea() == m_ucInterior )
        {
            g_pGame->GetWorld ()->SetCurrentArea ( ucInterior );
        }
    }

    m_ucInterior = ucInterior;
}


void CClientPed::Teleport ( const CVector& vecPosition )
{
    // We have a player ped?
    if ( m_pPlayerPed )
    {
        // Don't set the actual position if we're in a vehicle
        if ( !GetRealOccupiedVehicle () )
        {
            // Set it only if we're not in a vehicle or not working on getting in/out
            if ( !m_pOccupiedVehicle || GetVehicleInOutState () != VEHICLE_INOUT_GETTING_OUT )
            {
                // Is this the local player?
                if ( m_bIsLocalPlayer )
                {
                    // If move is big enough, do ground checks
                    float DistanceMoved = ( m_Matrix.vPos - vecPosition ).Length ();
                    if ( DistanceMoved > 50 && !IsFrozen () )
                        SetFrozenWaitingForGroundToLoad ( true );
                }

                // Set the real position
                m_pPlayerPed->Teleport ( vecPosition.fX, vecPosition.fY, vecPosition.fZ );
            }
        }
    }

    // Have we moved to a different position?
    if ( m_Matrix.vPos != vecPosition )
    {
        // Store our new position
        m_Matrix.vPos = vecPosition;
        m_matFrozen.vPos = vecPosition;

        // Update our streaming position
        UpdateStreamPosition ( vecPosition );
    }
}


void CClientPed::GetRotationDegrees ( CVector& vecRotation ) const
{
    // Grab our rotations in radians
    GetRotationRadians ( vecRotation );

    // Convert it to degrees
    vecRotation.fX = vecRotation.fX * 180.0f / 3.1415926535897932384626433832795f;
    vecRotation.fY = vecRotation.fY * 180.0f / 3.1415926535897932384626433832795f;
    vecRotation.fZ = vecRotation.fZ * 180.0f / 3.1415926535897932384626433832795f;
}


void CClientPed::GetRotationRadians ( CVector& vecRotation ) const
{
    CMatrix matTemp;
    GetMatrix ( matTemp );
    g_pMultiplayer->ConvertMatrixToEulerAngles ( matTemp, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
}


void CClientPed::SetRotationDegrees ( const CVector& vecRotation )
{
    // Convert from degrees to radians
    CVector vecTemp;
    vecTemp.fX = vecRotation.fX * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fY = vecRotation.fY * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fZ = vecRotation.fZ * 3.1415926535897932384626433832795f / 180.0f;

    // Set the rotation as radians
    SetRotationRadians ( vecTemp );

    // HACK: set again the z rotation to work on ground
    SetCurrentRotation ( vecTemp.fZ );
    if ( !IS_PLAYER ( this ) )
        SetCameraRotation ( vecTemp.fZ );
}


void CClientPed::SetRotationRadians ( const CVector& vecRotation )
{
    // Grab the matrix, apply the rotation to it and set it again
    CMatrix matTemp;
    GetMatrix ( matTemp );
    g_pMultiplayer->ConvertEulerAnglesToMatrix ( matTemp, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    SetMatrix ( matTemp );
}


void CClientPed::Spawn ( const CVector& vecPosition,
                         float fRotation,
                         unsigned short usModel,
                         unsigned char ucInterior )
{
    // Remove us from our car
    RemoveFromVehicle ();    
    SetVehicleInOutState ( VEHICLE_INOUT_NONE );

    // Wait for ground
    if ( m_bIsLocalPlayer )
    {
        SetFrozenWaitingForGroundToLoad ( true );
        m_iLoadAllModelsCounter = 10;
    }

    // Remove any animation
    KillAnimation ();

    // Give him the correct model
    SetModel ( usModel );

    // Detach from any entities
    AttachTo ( NULL );

    // Restore our health before any resurrection calls (::SetHealth/SetInitialState)
    // So we don't get recreated more than once
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetInitialState ();
        m_fHealth = GetMaxHealth ();
        m_pPlayerPed->SetHealth ( m_fHealth );
        m_bUsesCollision = true;
    } else {
        // Remote ped health/armor was locked during Kill, so make sure it's unlocked
        UnlockHealth ();
        UnlockArmor ();
    }

    // Set some states
    SetFrozen ( false );
    Teleport ( vecPosition );
    SetCurrentRotation ( fRotation );
    SetHealth ( GetMaxHealth () );
    RemoveAllWeapons ();
    SetArmor ( 0 );
    ResetInterpolation ();
    SetHasJetPack ( false );
    SetMoveSpeed ( CVector () );
    SetInterior ( ucInterior );
    SetFootBloodEnabled( false );
}

void CClientPed::ResetInterpolation ( void )
{
    m_ulBeginRotationTime = 0;
    m_ulEndRotationTime = 0;
    m_ulBeginAimTime = 0;
    m_ulTargetAimTime = 0;

    RemoveTargetPosition ();
}


float CClientPed::GetCameraRotation ( void )
{
    // Local player
    if ( m_bIsLocalPlayer )
    {
        return g_pGame->GetCamera ()->GetCameraRotation ();
    }
    else
    {
        // Get the keypad
        return m_remoteDataStorage->GetCameraRotation ();
    }
}


void CClientPed::SetCameraRotation ( float fRotation )
{
    // Local player
    if ( m_bIsLocalPlayer )
    {
        // Jax: only has an effect if CMultiplayer::SetCustomCameraRotationEnabled is called
        g_pMultiplayer->SetLocalCameraRotation ( fRotation );
    }
    else
    {
        // Get the keypad
        m_remoteDataStorage->SetCameraRotation ( fRotation );
    }
}

void CClientPed::GetMoveSpeed ( CVector& vecMoveSpeed ) const
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->GetMoveSpeed ( &vecMoveSpeed );
    }
    else
    {
        vecMoveSpeed = m_vecMoveSpeed;
    }
}


void CClientPed::SetMoveSpeed ( const CVector& vecMoveSpeed )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetMoveSpeed ( const_cast < CVector* > ( &vecMoveSpeed ) );
    }
    m_vecMoveSpeed = vecMoveSpeed;
}


void CClientPed::GetTurnSpeed ( CVector& vecTurnSpeed ) const
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->GetTurnSpeed ( &vecTurnSpeed );
    }
    else
    {
        vecTurnSpeed = m_vecTurnSpeed;
    }
}


void CClientPed::SetTurnSpeed ( const CVector& vecTurnSpeed )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetTurnSpeed ( const_cast < CVector* > ( &vecTurnSpeed ) );
    }
    m_vecTurnSpeed = vecTurnSpeed;
}


void CClientPed::GetControllerState ( CControllerState& ControllerState )
{
    // Local player
    if ( m_bIsLocalPlayer )
    {
        m_pPad->GetCurrentControllerState ( &ControllerState );

        // Fix for GTA bug allowing to drive bikes and boats with engine off (3437)
        CClientVehicle* pVehicle = GetRealOccupiedVehicle();
        if ( pVehicle && !pVehicle->IsEngineOn() )
        {
            ControllerState.ButtonCross = 0;
        }
    }
    else
    {
        ControllerState = *m_currentControllerState;
    }
}


void CClientPed::GetLastControllerState ( CControllerState& ControllerState )
{
    // Local player
    if ( m_bIsLocalPlayer )
    {
        m_pPad->GetLastControllerState ( &ControllerState );
    }
    else
    {
        ControllerState = *m_lastControllerState;
    }
}


void CClientPed::SetControllerState ( const CControllerState& ControllerState )
{
    // Local player
    if ( m_bIsLocalPlayer )
    {
        // Put the current keystate in the old keystate
        CControllerState csOld;
        m_pPad->GetCurrentControllerState ( &csOld );
        m_pPad->SetLastControllerState ( &csOld );

        // Set the new current keystate
        m_pPad->SetCurrentControllerState ( const_cast < CControllerState* > ( &ControllerState ) );
    }
    else
    {
        // Put the current into the old keystates
        memcpy ( m_lastControllerState, m_currentControllerState, sizeof ( CControllerState ) );

        // Set the new current keystate
        memcpy ( m_currentControllerState, &ControllerState, sizeof ( CControllerState ) );       
    }
}


void CClientPed::AddKeysync ( unsigned long ulDelay, const CControllerState& ControllerState, bool bDucking )
{
    if ( !m_bIsLocalPlayer )
    {
        SDelayedSyncData* pData = new SDelayedSyncData;
        pData->ulTime = CClientTime::GetTime () + ulDelay;
        pData->ucType = DELAYEDSYNC_KEYSYNC;
        pData->State = ControllerState;
        pData->bDucking = bDucking;

        m_SyncBuffer.push_back ( pData );

        if ( !IsStreamedIn () )
            UpdateKeysync ( true );
    }
}


void CClientPed::AddChangeWeapon ( unsigned long ulDelay, eWeaponSlot slot, unsigned short usWeaponAmmo )
{
    if ( !m_bIsLocalPlayer )
    {
        SDelayedSyncData* pData = new SDelayedSyncData;
        pData->ulTime = CClientTime::GetTime () + ulDelay;
        pData->ucType = DELAYEDSYNC_CHANGEWEAPON;
        pData->slot = slot;
        pData->usWeaponAmmo = usWeaponAmmo;

        m_SyncBuffer.push_back ( pData );

        if ( !IsStreamedIn () )
            UpdateKeysync ( true );
    }
}


void CClientPed::AddMoveSpeed ( unsigned long ulDelay, const CVector& vecMoveSpeed )
{
    if ( !m_bIsLocalPlayer )
    {
        SDelayedSyncData* pData = new SDelayedSyncData;
        pData->ulTime = CClientTime::GetTime () + ulDelay;
        pData->ucType = DELAYEDSYNC_MOVESPEED;
        pData->vecTarget = vecMoveSpeed;

        m_SyncBuffer.push_back ( pData );

        if ( !IsStreamedIn () )
            UpdateKeysync ( true );
    }
}


void CClientPed::SetTargetTarget ( unsigned long ulDelay, const CVector& vecSource, const CVector& vecTarget )
{
    if ( !m_bIsLocalPlayer )
    {
        m_ulBeginTarget = CClientTime::GetTime ();
        m_ulEndTarget = m_ulBeginTarget + ulDelay;
        m_vecBeginSource = m_shotSyncData->m_vecShotOrigin;
        m_vecBeginTarget = m_shotSyncData->m_vecShotTarget;
        m_vecTargetSource = vecSource;
        m_vecTargetTarget = vecTarget;

        // Grab the radius of the target circle
        float fRadius = DistanceBetweenPoints3D ( m_vecTargetSource, m_vecTargetTarget );

        // Grab the angle of the source vector and the angle of the target vector relative to the source vector that applies
        m_vecBeginTargetAngle.fX = acos ( ( m_vecBeginTarget.fX - m_vecBeginSource.fX ) / fRadius );
        m_vecBeginTargetAngle.fY = acos ( ( m_vecBeginTarget.fY - m_vecBeginSource.fY ) / fRadius );
        m_vecBeginTargetAngle.fZ = acos ( ( m_vecBeginTarget.fZ - m_vecBeginSource.fZ ) / fRadius );
        m_vecTargetTargetAngle.fX = acos ( ( m_vecTargetTarget.fX - m_vecTargetSource.fX ) / fRadius );
        m_vecTargetTargetAngle.fY = acos ( ( m_vecTargetTarget.fY - m_vecTargetSource.fY ) / fRadius );
        m_vecTargetTargetAngle.fZ = acos ( ( m_vecTargetTarget.fZ - m_vecTargetSource.fZ ) / fRadius );

        // Grab the angle to interpolate and make sure it's below pi and above -pi (shortest path of interpolation)
        m_vecTargetInterpolateAngle = m_vecTargetTargetAngle - m_vecBeginTargetAngle;

        if ( m_vecTargetInterpolateAngle.fX >= PI )
            m_vecTargetInterpolateAngle.fX -= 2 * PI;
        else if ( m_vecTargetInterpolateAngle.fX <= -PI )
            m_vecTargetInterpolateAngle.fX += 2 * PI;

        if ( m_vecTargetInterpolateAngle.fY >= PI )
            m_vecTargetInterpolateAngle.fY -= 2 * PI;
        else if ( m_vecTargetInterpolateAngle.fY <= -PI )
            m_vecTargetInterpolateAngle.fY += 2 * PI;

        if ( m_vecTargetInterpolateAngle.fZ >= PI )
            m_vecTargetInterpolateAngle.fZ -= 2 * PI;
        else if ( m_vecTargetInterpolateAngle.fZ <= -PI )
            m_vecTargetInterpolateAngle.fZ += 2 * PI;
    }
}


bool CClientPed::SetModel ( unsigned long ulModel, bool bTemp )
{
    // Valid model?
    if ( CClientPlayerManager::IsValidModel ( ulModel ) )
    {
        // Different model from what we have now?
        if ( m_ulModel != ulModel )
        {
            if ( bTemp )
                m_ulStoredModel = m_ulModel;

            // Set the model we're changing to
            m_ulModel = ulModel;
            m_pModelInfo = g_pGame->GetModelInfo ( ulModel );
            UpdateSpatialData ();

            // Are we loaded?
            if ( m_pPlayerPed )
            {
                // Request the model
                if ( m_pRequester->Request ( static_cast < unsigned short > ( ulModel ), this ) )
                {
                    m_pModelInfo->MakeCustomModel ( );
                    // Change the model immediately if it was loaded
                    _ChangeModel ();
                }
            }
        }

        return true;
    }

    return false;
}


bool CClientPed::GetCanBeKnockedOffBike ( void )
{
    if ( m_pPlayerPed )
    {
        return !m_pPlayerPed->GetCantBeKnockedOffBike ();
    }
    return m_bCanBeKnockedOffBike;
}


void CClientPed::SetCanBeKnockedOffBike ( bool bCanBeKnockedOffBike )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetCantBeKnockedOffBike ( ( bCanBeKnockedOffBike ) ? BIKE_KNOCK_OFF_DEFAULT : BIKE_KNOCK_OFF_NEVER );
    }
    m_bCanBeKnockedOffBike = bCanBeKnockedOffBike;
}


CVector* CClientPed::GetBonePosition ( eBone bone, CVector& vecPosition ) const
{
    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetBonePosition ( bone, &vecPosition );
    }

    return NULL;
}


CVector* CClientPed::GetTransformedBonePosition ( eBone bone, CVector& vecPosition ) const
{
    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetTransformedBonePosition ( bone, &vecPosition );
    }

    return NULL;
}


CClientVehicle* CClientPed::GetRealOccupiedVehicle ( void )
{
    if ( m_pPlayerPed )
    {
        // Grab the player in the vehicle using the game interface
        CVehicle* pGameVehicle = m_pPlayerPed->GetVehicle ();
        if ( pGameVehicle )
        {
            // Return the CClientVehicle for it
            return (CClientVehicle*) pGameVehicle->GetStoredPointer ();
        }
    }

    // No occupied vehicle
    return NULL;
}


CClientVehicle* CClientPed::GetClosestVehicleInRange ( bool bGetPositionFromClosestDoor, bool bCheckDriverDoor, bool bCheckPassengerDoors, bool bCheckStreamedOutVehicles, unsigned int* uiClosestDoor, CVector* pClosestDoorPosition, float fWithinRange )
{
    if ( bGetPositionFromClosestDoor )
    {
        if ( !m_pPlayerPed || ( !bCheckDriverDoor && !bCheckPassengerDoors ) )
            return NULL;
    }

    CClientVehicle* pVehicle = NULL;
    int iClosestDoor = 0;
    CVector vecClosestDoorPosition;

    CVector vecPosition;
    GetPosition ( vecPosition );

    float fClosestDistance = 0.0f;
    CVector vecVehiclePosition;
    CClientVehicle* pTempVehicle = NULL;
    vector < CClientVehicle * > ::const_iterator iter, listEnd;
    if ( bCheckStreamedOutVehicles )
    {
        iter = m_pManager->GetVehicleManager ()->IterBegin ();
        listEnd = m_pManager->GetVehicleManager ()->IterEnd ();
    }
    else
    {
        iter = m_pManager->GetVehicleManager ()->StreamedBegin ();
        listEnd = m_pManager->GetVehicleManager ()->StreamedEnd ();
    }
    for ( ; iter != listEnd; iter++ )
    {            
        pTempVehicle = *iter;
        CVehicle* pGameVehicle = pTempVehicle->GetGameVehicle ();
        
        if ( !pGameVehicle && bGetPositionFromClosestDoor ) continue;

        // Should we take the position from the closest door instead of center of vehicle
        if ( bGetPositionFromClosestDoor && pTempVehicle->GetModel() != VT_RCBARON )
        {
            // Get the closest front-door
            CVector vecFrontPos;
            int iFrontDoor = 0;
            g_pGame->GetCarEnterExit ()->GetNearestCarDoor ( m_pPlayerPed, pGameVehicle, &vecFrontPos, &iFrontDoor );
        
            // Get the closest passenger-door
            CVector vecPassengerPos;
            int iPassengerDoor;
            g_pGame->GetCarEnterExit ()->GetNearestCarPassengerDoor ( m_pPlayerPed, pGameVehicle, &vecPassengerPos, &iPassengerDoor, false, false, false );

            if ( bCheckDriverDoor && !bCheckPassengerDoors )
            {
                iClosestDoor = iFrontDoor;
                vecClosestDoorPosition = vecVehiclePosition = vecFrontPos;
            }
            else
            {
                iClosestDoor = iPassengerDoor;
                vecClosestDoorPosition = vecVehiclePosition = vecPassengerPos;
            }
            if ( bCheckDriverDoor )
            {
                // If they're different, find the closest
                if ( iFrontDoor != iPassengerDoor && iPassengerDoor < 2 )
                {
                    float fDistanceFromFront = DistanceBetweenPoints3D ( vecPosition, vecFrontPos );
                    float fDistanceFromPassenger = DistanceBetweenPoints3D ( vecPosition, vecPassengerPos );
                    if ( fDistanceFromPassenger < fDistanceFromFront )
                    {
                        iClosestDoor = iPassengerDoor;
                        vecClosestDoorPosition = vecVehiclePosition = vecPassengerPos;
                    }
                }
            }
        }
        else
        {
            pTempVehicle->GetPosition ( vecVehiclePosition );
        }

        float fDistance = DistanceBetweenPoints3D ( vecPosition, vecVehiclePosition );
        if ( fDistance <= fWithinRange )
        {
            if ( pVehicle == NULL || fDistance < fClosestDistance )
            {
                pVehicle = pTempVehicle;
                fClosestDistance = fDistance;

                if ( uiClosestDoor )
                {
                    // Get the actual door id
                    switch ( iClosestDoor )
                    {
                        case 10:
                            iClosestDoor = 0;
                            break;
                        case 8:
                            iClosestDoor = 1;
                            break;
                        case 11:
                            iClosestDoor = 2;
                            break;
                        case 9:
                            iClosestDoor = 3;
                            break;
                    }
                    *uiClosestDoor = static_cast < unsigned int > ( iClosestDoor );
                }
                if ( pClosestDoorPosition )
                    *pClosestDoorPosition = vecClosestDoorPosition;
            }
        }
    }

    return pVehicle;
}


bool CClientPed::GetClosestDoor ( CClientVehicle* pVehicle, bool bCheckDriverDoor, bool bCheckPassengerDoors, unsigned int& uiClosestDoor, CVector* pClosestDoorPosition )
{
    if ( !bCheckDriverDoor && !bCheckPassengerDoors )
        return false;

    int iClosestDoor;
    CVector vecClosestDoorPosition;

    CVector vecPosition;
    GetPosition ( vecPosition );

    CVehicle* pGameVehicle = pVehicle->GetGameVehicle ();
    if ( pGameVehicle )
    {
        if ( m_pPlayerPed )
        {
            // Get the closest front-door
            CVector vecFrontPos;
            int iFrontDoor;
            g_pGame->GetCarEnterExit ()->GetNearestCarDoor ( m_pPlayerPed, pGameVehicle, &vecFrontPos, &iFrontDoor );
        
            // Get the closest passenger-door
            CVector vecPassengerPos;
            int iPassengerDoor;
            g_pGame->GetCarEnterExit ()->GetNearestCarPassengerDoor ( m_pPlayerPed, pGameVehicle, &vecPassengerPos, &iPassengerDoor, false, false, false );

            if ( bCheckDriverDoor && !bCheckPassengerDoors )
            {
                iClosestDoor = iFrontDoor;
                vecClosestDoorPosition = vecFrontPos;
            }
            else
            {
                iClosestDoor = iPassengerDoor;
                vecClosestDoorPosition = vecPassengerPos;
            }
            if ( bCheckDriverDoor )
            {
                // If they're different, find the closest
                if ( iFrontDoor != iPassengerDoor )
                {
                    float fDistanceFromFront = DistanceBetweenPoints3D ( vecPosition, vecFrontPos );
                    float fDistanceFromPassenger = DistanceBetweenPoints3D ( vecPosition, vecPassengerPos );
                    if ( fDistanceFromPassenger < fDistanceFromFront )
                    {
                        iClosestDoor = iPassengerDoor;
                        vecClosestDoorPosition = vecPassengerPos;
                    }
                }
            }
            // Get the actual door id
            switch ( iClosestDoor )
            {
                case 10:
                    uiClosestDoor = 0;
                    break;
                case 8:
                    uiClosestDoor = 1;
                    break;
                case 11:
                    uiClosestDoor = 2;
                    break;
                case 9:
                    uiClosestDoor = 3;
                    break;
            }
        }
        if ( pClosestDoorPosition )
            *pClosestDoorPosition = vecClosestDoorPosition;

        return true;
    }
    return false;
}


void CClientPed::GetOutOfVehicle ( unsigned char ucDoor )
{
    if ( ucDoor != 0xFF )
        m_ucLeavingDoor = ucDoor + 2;
    else
        m_ucLeavingDoor = 0xFF;
    m_bForceGettingOut = true;

    // Get the current vehicle you're in
    CClientVehicle* pVehicle = GetRealOccupiedVehicle ();
    if ( pVehicle )
    {
        //m_pOccupyingVehicle = pVehicle;
        if ( m_pPlayerPed )
        {
            CVehicle* pGameVehicle = pVehicle->m_pVehicle;
            
            if ( pGameVehicle )
            {
                CTaskComplexLeaveCar * pOutTask = g_pGame->GetTasks ()->CreateTaskComplexLeaveCar ( pGameVehicle, m_ucLeavingDoor );
                if ( pOutTask )
                {
                    pOutTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PRIMARY, true );

                    // Turn off the radio if local player
                    if ( m_bIsLocalPlayer )
                    {
                        StopRadio ();
                    }
                }
            }

            if ( m_ucLeavingDoor != 0xFF )
                pVehicle->AllowDoorRatioSetting ( m_ucLeavingDoor, false );
        }
    }

    ResetInterpolation ();
    ResetToOutOfVehicleWeapon();
}


void CClientPed::GetIntoVehicle ( CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor )
{
    // TODO: add checks to ensure we don't try to use the wrong seats for bikes etc
    // Eventually remove us from a previous vehicle
    RemoveFromVehicle ();

    // Do it
    _GetIntoVehicle ( pVehicle, uiSeat, ucDoor );
    m_uiOccupiedVehicleSeat = uiSeat;
    m_ucEnteringDoor = ucDoor;
    m_bForceGettingIn = true;
}


void CClientPed::WarpIntoVehicle ( CClientVehicle* pVehicle, unsigned int uiSeat )
{
    // Ensure vehicle model is loaded
    CModelInfo* pModelInfo = pVehicle->GetModelInfo();
    if ( g_pGame->IsASyncLoadingEnabled () && !pModelInfo->IsLoaded () )
    {
        if ( pVehicle->IsStreamedIn () )
        {
            pModelInfo->Request ( BLOCKING, "CClientPed::WarpIntoVehicle" );
        }
    }

    // Transfer WaitingForGroundToLoad state to vehicle
    if ( m_bIsLocalPlayer )
    {
        if ( IsFrozenWaitingForGroundToLoad () )
        {
            SetFrozenWaitingForGroundToLoad ( false );
            pVehicle->SetFrozenWaitingForGroundToLoad ( true );
        }
        CVector vecPosition;
        GetPosition ( vecPosition );
        CVector vecVehiclePosition;
        pVehicle->GetPosition ( vecVehiclePosition );
        float fDist = ( vecPosition - vecVehiclePosition ).Length ();
        if ( fDist > 50 && !pVehicle->IsFrozen () )
        {
            pVehicle->SetFrozenWaitingForGroundToLoad ( true );
        }
    }


    // Remove some tasks so we don't get any weird results
    SetChoking ( false );
    SetHasJetPack ( false );
    SetSunbathing ( false );
    KillAnimation ();

    // Eventually remove us from a previous vehicle
    RemoveFromVehicle ();
    //m_uiOccupyingSeat = uiSeat;
    m_bForceGettingIn = false;
    m_bForceGettingOut = false;
    m_ucLeavingDoor = 0xFF;

    // Store our current seat
    if ( m_pPlayerPed ) m_pPlayerPed->SetOccupiedSeat ( ( unsigned char ) uiSeat );

    // Driverseat
    if ( uiSeat == 0 )
    {       
        // Force the vehicle we're warping into to be streamed in
        // if the local player is entering it. This is so we don't
        // get screwed up with camera not following and similar issues.
        if ( m_bIsLocalPlayer )
        {
            pVehicle->AddStreamReference ();
            pVehicle->SetSwingingDoorsAllowed ( true );
        }

        // Warp the player into the car's driverseat
        CVehicle* pGameVehicle = pVehicle->m_pVehicle;
        if ( pGameVehicle )
        {
            // Warp him in
            InternalWarpIntoVehicle ( pGameVehicle );
        }

        // Update the vehicle and us so we know we've occupied it
        CClientVehicle::SetPedOccupiedVehicle( this, pVehicle, 0, 0xFF );
    }
    else
    {
        // Passenger seat
        unsigned char ucSeat = CClientVehicleManager::ConvertIndexToGameSeat ( pVehicle->m_usModel, uiSeat );
        if ( ucSeat != 0 && ucSeat != 0xFF )
        {
            if ( m_pPlayerPed )
            {
                // Force the vehicle we're warping into to be streamed in
                // if the local player is entering it. This is so we don't
                // get screwed up with camera not following and similar issues.
                if ( m_bIsLocalPlayer )
                {
                    pVehicle->AddStreamReference ();
                }

                // Warp the player into the car's driverseat
                CVehicle* pGameVehicle = pVehicle->m_pVehicle;
                if ( pGameVehicle )
                {
                    // Reset whatever task
                    m_pTaskManager->RemoveTask ( TASK_PRIORITY_PRIMARY );

                    // Create a task to warp the player in and execute it
                    CTaskSimpleCarSetPedInAsPassenger* pInTask = g_pGame->GetTasks ()->CreateTaskSimpleCarSetPedInAsPassenger ( pGameVehicle, ucSeat );
                    if ( pInTask )
                    {
                        pInTask->SetIsWarpingPedIntoCar ();
                        pInTask->ProcessPed ( m_pPlayerPed );
                        pInTask->Destroy ();
                    }
                }
            }

            // Update us so we know we've occupied it
            CClientVehicle::SetPedOccupiedVehicle( this, pVehicle, uiSeat, 0xFF );
        }
        else
            return;
    }

    // Turn on the radio if local player and it's not already on.
    if ( m_bIsLocalPlayer )
    {
        StartRadio ();
    }

    RemoveTargetPosition ();

    // Make peds stream in when they warp to a vehicle
    if ( pVehicle )
    {
        CVector vecInVehiclePosition;
        GetPosition ( vecInVehiclePosition );
        UpdateStreamPosition ( vecInVehiclePosition );
        if ( pVehicle->IsStreamedIn () )
            StreamIn ( true );
    }
}

void CClientPed::ResetToOutOfVehicleWeapon ( void )
{
    if ( m_pOutOfVehicleWeaponSlot != WEAPONSLOT_MAX )
    {
        // Jax: I think this should be left up to scripting
        //SetCurrentWeaponSlot ( m_pOutOfVehicleWeaponSlot );
        m_pOutOfVehicleWeaponSlot = WEAPONSLOT_MAX;
    }
}


CClientVehicle * CClientPed::RemoveFromVehicle ( bool bIgnoreIfGettingOut )
{
    SetDoingGangDriveby ( false );

    // Reset any enter/exit tasks
    if ( IsEnteringVehicle () )
    {
        m_pTaskManager->RemoveTask ( TASK_PRIORITY_DEFAULT );
    }

    // Get the current vehicle you're in
    CClientVehicle* pVehicle = GetRealOccupiedVehicle ();
    if ( !pVehicle )
    {
        pVehicle = GetOccupiedVehicle ();
        if ( !pVehicle )
        {
            pVehicle = m_pOccupyingVehicle;
        }
    }

    if ( pVehicle )
    {
        pVehicle->SetSwingingDoorsAllowed ( false );

        // Warp the player out of the vehicle
        CVehicle* pGameVehicle = pVehicle->m_pVehicle;
        if ( pGameVehicle )
        {
            // Jax: this should be safe, doesn't remove the player if he's getting dragged out already (fix for getting stuck on back after being jacked)
            if ( !bIgnoreIfGettingOut || ( !IsGettingOutOfVehicle () ) )
            {
                // Warp the player out
                InternalRemoveFromVehicle ( pGameVehicle );
            }
        }        

        CClientVehicle::UnpairPedAndVehicle( this );

        if ( m_bIsLocalPlayer )
        {
            pVehicle->RemoveStreamReference ();
        }
    }

    // Reset the interpolation so we won't move from the last known spot to where we exit
    ResetInterpolation ();

    // Local player?
    if ( m_bIsLocalPlayer )
    {
        // Stop the radio
        StopRadio ();
    }

    // And in our class
    CClientVehicle::UnpairPedAndVehicle( this );
    assert ( m_pOccupiedVehicle == NULL );
    assert ( m_pOccupyingVehicle == NULL );
    assert ( m_uiOccupiedVehicleSeat = 0xFF );

    m_bForceGettingIn = false;
    m_bForceGettingOut = false;
    m_ucLeavingDoor = 0xFF;

    return pVehicle;
}


bool CClientPed::IsVisible ( void )
{
    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->IsVisible ();
    }
    return m_bVisible;
}


void CClientPed::SetVisible ( bool bVisible )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetVisible ( bVisible );
    }
    m_bVisible = bVisible;
}


bool CClientPed::GetUsesCollision ( void )
{
    /*
    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetUsesCollision ();
    }*/
    return m_bUsesCollision;
}


void CClientPed::SetUsesCollision ( bool bUsesCollision )
{
    // The game changes this bool frequently, so we shouldn't set it every frame
    if ( bUsesCollision != m_bUsesCollision )
    {
        if ( m_pPlayerPed )
        {
            m_pPlayerPed->SetUsesCollision ( bUsesCollision );
        }
        m_bUsesCollision = bUsesCollision;
    }
}


float CClientPed::GetMaxHealth ( void )
{
    // TODO: Verify this formula

    // Grab his player health stat
    float fStat = GetStat ( MAX_HEALTH );

    // Do a linear interpolation to get how much health this would allow
    // Assumes: 100 health = 569 stat, 200 health = 1000 stat.
    float fMaxHealth = 100.0f + ( 100.0f / 431.0f * ( fStat - 569.0f ) );

    // Return the max health. Make sure it can't be below 1
    if ( fMaxHealth < 1.0f ) fMaxHealth = 1.0f;
    return fMaxHealth;
}


float CClientPed::GetHealth ( void )
{
    if ( m_bHealthLocked ) return m_fHealth;

    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetHealth ();
    }
    return m_fHealth;
}


void CClientPed::SetHealth ( float fHealth )
{
    // If our health is locked, dont allow any change
    if ( m_bHealthLocked ) return;

    InternalSetHealth ( fHealth );
    m_fHealth = fHealth;
}


void CClientPed::InternalSetHealth ( float fHealth )
{
    if ( m_pPlayerPed )
    {
        // If the player is dead, call grab the current vehicle he's in, respawn and put back into the vehicle
        if ( m_pPlayerPed->GetHealth () <= 0.0f && fHealth > 0.0f )
        {
            // Grab the vehicle and eventually warp out of it
            CClientVehicle* pVehicle = GetOccupiedVehicle ();
            unsigned int uiVehicleSeat = m_uiOccupiedVehicleSeat;
            RemoveFromVehicle ();

            // If it's the local player, call respawn
            if ( m_bIsLocalPlayer )
            {
                Respawn ( NULL, false, true );
            }
            else
            {
                // Recreate the player
                ReCreateModel ();
            }
            
            // If the vehicle existed, warp the player back in
            if ( pVehicle )
            {
                WarpIntoVehicle ( pVehicle, uiVehicleSeat );
            }
        }

        // Recheck we have a ped, ReCreateModel might destroy it
        if ( m_pPlayerPed )
        {
            // Set the new health
            m_pPlayerPed->SetHealth ( fHealth );
        }
    }
}


float CClientPed::GetArmor ( void )
{
    if ( m_bArmorLocked ) return m_fArmor;

    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetArmor ();
    }
    return m_fArmor;
}


void CClientPed::SetArmor ( float fArmor )
{
    // If our armor is locked, dont allow any change
    if ( m_bArmorLocked ) return;

    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetArmor ( fArmor );
    }
    m_fArmor = fArmor;
}


void CClientPed::LockHealth ( float fHealth )
{
    m_bHealthLocked = true;
    m_fHealth = fHealth;
}


void CClientPed::LockArmor ( float fArmor )
{
    m_bArmorLocked = true;
    m_fArmor = fArmor;
}


bool CClientPed::IsDying ( void )
{
    if ( m_pPlayerPed )
    {
        CTask * pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
        if ( pTask )
        {
            if ( pTask->GetTaskType () == TASK_SIMPLE_DIE ||
                 pTask->GetTaskType () == TASK_SIMPLE_DROWN ||
                 pTask->GetTaskType () == TASK_SIMPLE_DIE_IN_CAR ||
                 pTask->GetTaskType () == TASK_COMPLEX_DIE_IN_CAR ||
                 pTask->GetTaskType () == TASK_SIMPLE_DROWN_IN_CAR ||
                 pTask->GetTaskType () == TASK_COMPLEX_DIE )
            {
                return true;
            }
        }
    }
    return false;
}


bool CClientPed::IsDead ( void )
{
    if ( m_pPlayerPed )
    {
        CTask * pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
        if ( pTask )
        {
            if ( pTask->GetTaskType () == TASK_SIMPLE_DEAD )
            {
                return true;
            }
        }
    }
    return false;
}

void CClientPed::Kill ( eWeaponType weaponType, unsigned char ucBodypart, bool bStealth, bool bSetDirectlyDead, AssocGroupId animGroup, AnimationId animID )
{
    // Don't change task if already dead or dying
    if ( m_pPlayerPed && !IsDead () && !IsDying () )
    {        
        // Do we have the in_water task?
        CTask * pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
        if ( pTask && pTask->GetTaskType () == TASK_COMPLEX_IN_WATER )
        {
            // Kill the task
            pTask->MakeAbortable ( m_pPlayerPed, ABORT_PRIORITY_URGENT, NULL );
        }

        if ( bSetDirectlyDead )
        {
            // TODO: Avoid the animation, try to make it go directly to the last animation frame.
            pTask = g_pGame->GetTasks ()->CreateTaskSimpleDead ( GetTickCount32(), true );
            if ( pTask )
            {
                pTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_DEFAULT );
            }
        }
        else if ( bStealth )
        {
            pTask = g_pGame->GetTasks ()->CreateTaskSimpleStealthKill ( false, m_pPlayerPed, 87 );
            if ( pTask )
            {
                pTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PRIMARY );
            }
        }
        else
        {
            pTask = g_pGame->GetTasks ()->CreateTaskComplexDie ( weaponType, animGroup, animID, 4.0f, 1.0f );
            if ( pTask )
            {
                pTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
            }
        }
    }
    if ( m_bIsLocalPlayer )
    {
        SetHealth ( 0.0f );
        SetArmor ( 0.0f );
    }
    else
    {
        LockHealth ( 0.0f );
        LockArmor ( 0.0f );
    }

    // Silently remove the ped satchels
    DestroySatchelCharges ( false, true );

    // Stop pressing buttons
    SetControllerState ( CControllerState () );
}


void CClientPed::StealthKill ( CClientPed * pPed )
{
    if ( m_pPlayerPed )
    {
        CPlayerPed * pPlayerPed = pPed->GetGamePlayer ();
        if ( pPlayerPed )
        {
            CTask * pTask = g_pGame->GetTasks ()->CreateTaskSimpleStealthKill ( true, pPlayerPed, 87 );
            if ( pTask )
            {
                pTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PRIMARY );
            }
        }
    }
}


void CClientPed::SetFrozen ( bool bFrozen )
{
    if ( m_bFrozen != bFrozen )
    {
        m_bFrozen = bFrozen;

        if ( bFrozen )
        {
            if ( m_pTaskManager )
            {
                m_pTaskManager->RemoveTask ( TASK_PRIORITY_PRIMARY );
                m_pTaskManager->RemoveTask ( TASK_PRIORITY_EVENT_RESPONSE_TEMP );
                m_pTaskManager->RemoveTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
                m_pTaskManager->RemoveTask ( TASK_PRIORITY_PHYSICAL_RESPONSE );
            }

            if ( m_pPlayerPed )
            {
                m_pPlayerPed->GetMatrix ( &m_matFrozen );
            }
            else
            {
                m_matFrozen = m_Matrix;
            }
        }
    }
}


bool CClientPed::IsFrozenWaitingForGroundToLoad ( void ) const
{
    return m_bFrozenWaitingForGroundToLoad;
}


void CClientPed::SetFrozenWaitingForGroundToLoad ( bool bFrozen )
{
    if ( !g_pGame->IsASyncLoadingEnabled ( true ) )
        return;

    if ( m_bFrozenWaitingForGroundToLoad != bFrozen )
    {
        m_bFrozenWaitingForGroundToLoad = bFrozen;

        if ( bFrozen )
        {
            g_pGame->SuspendASyncLoading ( true );

            m_fGroundCheckTolerance = 0.f;
            m_fObjectsAroundTolerance = -1.f;
/*
            if ( m_pTaskManager )
            {
                m_pTaskManager->RemoveTask ( TASK_PRIORITY_PRIMARY );
                m_pTaskManager->RemoveTask ( TASK_PRIORITY_EVENT_RESPONSE_TEMP );
                m_pTaskManager->RemoveTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
                m_pTaskManager->RemoveTask ( TASK_PRIORITY_PHYSICAL_RESPONSE );
            }
*/
            if ( m_pPlayerPed )
            {
                m_pPlayerPed->GetMatrix ( &m_matFrozen );
            }
            else
            {
                m_matFrozen = m_Matrix;
            }
        }
        else
        {
            g_pGame->SuspendASyncLoading ( false );
        }
    }
}


CWeapon * CClientPed::GiveWeapon ( eWeaponType weaponType, unsigned int uiAmmo )
{   
    // Multiply ammo with 10 if flamethrower to get the numbers correct.
    if ( weaponType == WEAPONTYPE_FLAMETHROWER ) uiAmmo *= 10;

    CWeapon* pWeapon = NULL;
    if ( m_pPlayerPed )
    {   
        CWeaponInfo* pWeaponInfo = NULL;
        // Grab our current ammo in clip
        pWeapon = GetWeapon ( weaponType );
        unsigned int uiPreviousAmmoTotal = 0, uiPreviousAmmoInClip = 0;
        eWeaponSkill weaponSkill = WEAPONSKILL_STD;
        if ( pWeapon )
        {
            uiPreviousAmmoTotal = pWeapon->GetAmmoTotal ();
            uiPreviousAmmoInClip = pWeapon->GetAmmoInClip ();
        }

        if ( weaponType >= WEAPONTYPE_PISTOL && weaponType <= WEAPONTYPE_TEC9 )
        {
            float fSkill = GetStat ( g_pGame->GetStats ()->GetSkillStatIndex ( weaponType ) );
            weaponSkill = g_pGame->GetWeaponStatManager ( )->GetWeaponSkillFromSkillLevel ( weaponType, fSkill );            
        }

        pWeapon = m_pPlayerPed->GiveWeapon ( weaponType, uiAmmo, weaponSkill );
        
        // Restore clip ammo?
        if ( uiPreviousAmmoInClip )
        {
            pWeapon->SetAmmoTotal ( uiPreviousAmmoTotal + uiAmmo );
            pWeapon->SetAmmoInClip ( uiPreviousAmmoInClip );
        }            
    }

    CWeaponInfo* pInfo = NULL;
    if ( weaponType >= eWeaponType::WEAPONTYPE_PISTOL && weaponType <= WEAPONTYPE_TEC9  )
    {
        float fStat = GetStat ( g_pGame->GetStats ( )->GetSkillStatIndex ( weaponType ) );
        eWeaponSkill weaponSkill = g_pGame->GetWeaponStatManager ( )->GetWeaponSkillFromSkillLevel ( weaponType, fStat );
        pInfo = g_pGame->GetWeaponInfo ( weaponType, weaponSkill );
    }
    else
    {
        pInfo = g_pGame->GetWeaponInfo ( weaponType );
    }
    if ( pInfo )
    {
        m_CurrentWeaponSlot = pInfo->GetSlot ();
        m_WeaponTypes [ m_CurrentWeaponSlot ] = weaponType;
    }

    return pWeapon;
}

bool CClientPed::SetCurrentWeaponSlot ( eWeaponSlot weaponSlot )
{
    if ( weaponSlot < WEAPONSLOT_MAX )
    {
        if ( weaponSlot != GetCurrentWeaponSlot () )
        {
            if ( m_pPlayerPed )
            {
                if ( weaponSlot == WEAPONSLOT_TYPE_UNARMED )
                {
                    eWeaponSlot currentSlot = GetCurrentWeaponSlot ();
                    CWeapon * oldWeapon = GetWeapon ( currentSlot );
                    DWORD ammoInClip = oldWeapon->GetAmmoInClip();
                    DWORD ammoInTotal = oldWeapon->GetAmmoTotal();
                    eWeaponType weaponType = oldWeapon->GetType();
                    RemoveWeapon ( oldWeapon->GetType() );

                    m_pPlayerPed->SetCurrentWeaponSlot ( WEAPONSLOT_TYPE_UNARMED );

                    CWeapon * newWeapon = GiveWeapon(weaponType, ammoInTotal);
                    newWeapon->SetAmmoInClip(ammoInClip);
                    newWeapon->SetAmmoTotal(ammoInTotal);

                    // Don't allow doing gang driveby while unarmed
                    if ( IsDoingGangDriveby () )
                        SetDoingGangDriveby ( false );
                }
                else
                {
                    // Make sure we have a weapon and some ammo on this slot
                    CWeapon * pWeapon = GetWeapon ( weaponSlot );
                    if ( pWeapon && pWeapon->GetAmmoTotal () )
                    {
                        m_pPlayerPed->SetCurrentWeaponSlot ( weaponSlot );
                        m_CurrentWeaponSlot = weaponSlot;
                        return true;
                    }
                }
            }
            else
            {
                m_CurrentWeaponSlot = weaponSlot;
                return true;
            }
        }
    }
    return false;
}


eWeaponSlot CClientPed::GetCurrentWeaponSlot ( void )
{
    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetCurrentWeaponSlot ();
    }
    return m_CurrentWeaponSlot;
}


eWeaponType CClientPed::GetCurrentWeaponType ( void )
{
    if ( m_pPlayerPed )
    {
        CWeapon* pWeapon = GetWeapon ( GetCurrentWeaponSlot () );
        if ( pWeapon )
        {
            return pWeapon->GetType ();
        }
    }    
    return WEAPONTYPE_UNARMED;
}


CWeapon* CClientPed::GetWeapon ( void )
{
    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetWeapon ( m_pPlayerPed->GetCurrentWeaponSlot () );
    }
    return NULL;
}


void CClientPed::RemoveWeapon ( eWeaponType weaponType )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->ClearWeapon ( weaponType );
        m_pPlayerPed->RemoveWeaponModel ( weaponType );

        // Set whatever weapon to 0 ammo so we don't keep it anymore
        CWeapon* pWeapon = GetWeapon ( weaponType );
        if ( pWeapon )
        {
            pWeapon->SetType ( WEAPONTYPE_UNARMED );
            pWeapon->SetAmmoInClip ( 0 );
            pWeapon->SetAmmoTotal ( 0 );
            pWeapon->Remove ();
        }
    }
    for ( int i = 0 ; i < (int)WEAPONSLOT_MAX ; i++ )
    {
        if ( m_WeaponTypes [ i ] == weaponType )
        {
            m_WeaponTypes [ i ] = WEAPONTYPE_UNARMED;
            if ( m_CurrentWeaponSlot == ( eWeaponSlot ) i )
            {
                m_CurrentWeaponSlot = WEAPONSLOT_TYPE_UNARMED;
            }
        }
    }
}


void CClientPed::RemoveAllWeapons ( void )
{
    if ( m_bIsLocalPlayer )
    {
        g_pClientGame->ResetAmmoInClip();
        g_pMultiplayer->SetNightVisionEnabled( false );
        g_pMultiplayer->SetThermalVisionEnabled( false );
    }
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->ClearWeapons ();
    }

    for ( int i = 0 ; i < (int)WEAPONSLOT_MAX ; i++ )
        m_WeaponTypes [ i ] = WEAPONTYPE_UNARMED;
    m_CurrentWeaponSlot = WEAPONSLOT_TYPE_UNARMED;
}


CWeapon* CClientPed::GetWeapon ( eWeaponSlot weaponSlot )
{    
    if ( weaponSlot < WEAPONSLOT_MAX )
    {
        if ( m_pPlayerPed )
        {
            return m_pPlayerPed->GetWeapon ( weaponSlot );
        }
    }
    return NULL;
}


CWeapon* CClientPed::GetWeapon ( eWeaponType weaponType )
{
    if ( weaponType < WEAPONTYPE_LAST_WEAPONTYPE )
    {
        if ( m_pPlayerPed )
        {
            return m_pPlayerPed->GetWeapon ( weaponType );
        }
    }
    return NULL;
}


bool CClientPed::HasWeapon ( eWeaponType weaponType )
{
    if ( m_pPlayerPed )
    {
        CWeapon* pWeapon = GetWeapon ( weaponType );
        if ( pWeapon )
            return true;
    }
    else
    {
        for ( int i = 0 ; i < (int)WEAPONSLOT_MAX ; i++ )
        {
            if ( m_WeaponTypes [ i ] == weaponType )
            {
                return true;
            }
        }
    }

    return false;
}

eMovementState CClientPed::GetMovementState ( void )
{
    // Do we have a player, and are we on foot? (streamed in)
    if ( m_pPlayerPed && !GetRealOccupiedVehicle () )
    {
        CControllerState cs;
        GetControllerState ( cs );

        // Grab his controller state
        bool bWalkKey = false;
        if ( GetType () == CCLIENTPLAYER )
            bWalkKey = CClientPad::GetControlState ( "walk", cs, true );
        else
            m_Pad.GetControlState("walk",bWalkKey);
        
        // Is he standing up?
        if ( !IsDucked() )
        {
            unsigned int iRunState = m_pPlayerPed->GetRunState();

            // Is he moving the contoller at all?
            if ( iRunState == 1 && cs.LeftStickX == 0 && cs.LeftStickY == 0 )
                return MOVEMENTSTATE_STAND;

            //Is he either pressing the walk key, or has run state 1?
            if ( iRunState == 1 || bWalkKey && iRunState == 6 )
                return MOVEMENTSTATE_WALK;
            else if ( iRunState == 4 )
                return MOVEMENTSTATE_POWERWALK;
            else if ( iRunState == 6 )
                return MOVEMENTSTATE_JOG;
            else if ( iRunState == 7 )
                return MOVEMENTSTATE_SPRINT;
        }
        else
        {
            // Is he moving the contoller at all?
            if ( cs.LeftStickX == 0 && cs.LeftStickY == 0 )
                return MOVEMENTSTATE_CROUCH;
        }
    }
    return MOVEMENTSTATE_UNKNOWN;
}

bool CClientPed::GetMovementState ( std::string& strStateName )
{
    eMovementState eCurrentMoveState = GetMovementState();
    if ( eCurrentMoveState == MOVEMENTSTATE_UNKNOWN )
        return false;

    strStateName = m_MovementStateNames[eCurrentMoveState];
    return true;
}

CTask* CClientPed::GetCurrentPrimaryTask ( void )
{
    if ( m_pPlayerPed )
    {
        return m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
    }

    return NULL;
}


bool CClientPed::IsSimplestTask ( int iTaskType )
{
    if ( m_pPlayerPed )
    {
        CTask * pTask = m_pTaskManager->GetSimplestActiveTask ();
        if ( pTask )
        {
            return ( pTask->GetTaskType () == iTaskType );
        }
    }
    return false;
}


bool CClientPed::HasTask ( int iTaskType, int iTaskPriority, bool bPrimary )
{
    if ( m_pPlayerPed )
    {
        int iNumTasks = ( bPrimary ) ? TASK_PRIORITY_MAX : TASK_SECONDARY_MAX;
        CTask * pTask = NULL;
        if ( iTaskPriority >= 0 && iTaskPriority < iNumTasks )
        {
            pTask = ( bPrimary ) ? m_pTaskManager->GetTask ( iTaskPriority ) : m_pTaskManager->GetTaskSecondary ( iTaskPriority );
            if ( pTask && pTask->GetTaskType () == iTaskType )
            {
                return true;
            }
        }
        else
        {
            for ( int i = 0 ; i < TASK_PRIORITY_MAX ; i++ )
            {
                pTask = m_pTaskManager->GetTask ( i );
                if ( pTask && pTask->GetTaskType () == iTaskType )
                {
                    return true;
                }
            }
            for ( int i = 0 ; i < TASK_SECONDARY_MAX ; i++ )
            {
                pTask = m_pTaskManager->GetTaskSecondary ( i );
                if ( pTask && pTask->GetTaskType () == iTaskType )
                {
                    return true;
                }
            }
        }
    }
    return false;
}


bool CClientPed::SetTask ( CTask* pTask, int iTaskPriority )
{
    if ( m_pTaskManager )
    {
        pTask->SetAsPedTask ( m_pPlayerPed, iTaskPriority );
        return true;
    }

    return false;
}


bool CClientPed::SetTaskSecondary ( CTask* pTask, int iTaskPriority )
{
    if ( m_pTaskManager )
    {
        pTask->SetAsSecondaryPedTask ( m_pPlayerPed, iTaskPriority );
        return true;
    }

    return false;
}


bool CClientPed::KillTask ( int iTaskPriority, bool bGracefully )
{
    if ( m_pTaskManager )
    {
        CTask * pTask = m_pTaskManager->GetTask ( iTaskPriority );
        if ( pTask )
        {
            if ( bGracefully )
            {
                pTask->MakeAbortable ( m_pPlayerPed, ABORT_PRIORITY_IMMEDIATE, NULL );
                pTask->Destroy ();                
            }
            m_pTaskManager->RemoveTask ( iTaskPriority );
            return true;
        }
    }
    return false;
}


bool CClientPed::KillTaskSecondary ( int iTaskPriority, bool bGracefully )
{
    if ( m_pTaskManager )
    {
        CTask * pTask = m_pTaskManager->GetTaskSecondary ( iTaskPriority );
        if ( pTask )
        {
            if ( bGracefully )
            {
                pTask->MakeAbortable ( m_pPlayerPed, ABORT_PRIORITY_IMMEDIATE, NULL );
                pTask->Destroy ();
            }
            m_pTaskManager->RemoveTaskSecondary ( iTaskPriority );
            return true;
        }
    }
    return false;
}


CVector CClientPed::GetAim ( void ) const
{
    if ( m_shotSyncData )
        return CVector ( m_shotSyncData->m_fArmDirectionX, m_shotSyncData->m_fArmDirectionY, 0 );
    return CVector ();
}


void CClientPed::SetAim ( float fArmDirectionX, float fArmDirectionY, unsigned char cInVehicleAimAnim )
{
    if ( !m_bIsLocalPlayer )
    {
        m_ulBeginAimTime = 0;
        m_ulTargetAimTime = 0;
        m_shotSyncData->m_fArmDirectionX = fArmDirectionX;
        m_shotSyncData->m_fArmDirectionY = fArmDirectionY;
        m_shotSyncData->m_cInVehicleAimDirection = cInVehicleAimAnim;
    }
}


void CClientPed::SetAimInterpolated ( unsigned long ulDelay, float fArmDirectionX, float fArmDirectionY, bool bAkimboAimUp, unsigned char cInVehicleAimAnim )
{
    if ( !m_bIsLocalPlayer )
    {
        // Force the old akimbo up thing
        m_remoteDataStorage->SetAkimboTargetUp ( m_bTargetAkimboUp );

        // Set the new data
        m_ulBeginAimTime = CClientTime::GetTime ();
        m_ulTargetAimTime = m_ulBeginAimTime + ulDelay;
        m_bTargetAkimboUp = bAkimboAimUp;
        m_fBeginAimX = m_shotSyncData->m_fArmDirectionX;
        m_fBeginAimY = m_shotSyncData->m_fArmDirectionY;
        m_fTargetAimX = fArmDirectionX;
        m_fTargetAimY = fArmDirectionY;
        m_shotSyncData->m_cInVehicleAimDirection = cInVehicleAimAnim;
    }
}


void CClientPed::SetAimingData ( unsigned long ulDelay, const CVector& vecTargetPosition, float fArmDirectionX, float fArmDirectionY, char cInVehicleAimAnim, CVector* pSource, bool bInterpolateAim )
{
    if ( !m_bIsLocalPlayer )
    {
        if ( bInterpolateAim )
        {
            m_ulBeginAimTime = CClientTime::GetTime ();
            m_ulTargetAimTime = m_ulBeginAimTime + ulDelay;
            m_fBeginAimX = m_shotSyncData->m_fArmDirectionX;
            m_fBeginAimY = m_shotSyncData->m_fArmDirectionY;
            m_fTargetAimX = fArmDirectionX;
            m_fTargetAimY = fArmDirectionY;
        }
        else
        {
            m_ulBeginAimTime = 0;
            m_ulTargetAimTime = 0;
            m_shotSyncData->m_fArmDirectionX = fArmDirectionX;
            m_shotSyncData->m_fArmDirectionY = fArmDirectionY;
        }

        m_shotSyncData->m_vecShotTarget = vecTargetPosition;
        m_shotSyncData->m_cInVehicleAimDirection = cInVehicleAimAnim;

        m_shotSyncData->m_bUseOrigin = pSource != NULL;
        if ( pSource )
        {
            m_shotSyncData->m_vecShotOrigin = *pSource;
        }
    }
}


void CClientPed::WorldIgnore ( bool bIgnore )
{
    if ( bIgnore )
    {
        if ( m_pPlayerPed )
        {
            g_pGame->GetWorld ()->IgnoreEntity ( m_pPlayerPed );
        }
    }
    else
    {
        g_pGame->GetWorld ()->IgnoreEntity ( NULL );
    }
    m_bWorldIgnored = bIgnore;
}


void CClientPed::StreamedInPulse ( void )
{
    // Grab some vars here, saves getting them twice
    CClientVehicle* pVehicle = GetOccupiedVehicle ();

    // Do we have a player? (streamed in)
    if ( m_pPlayerPed )
    {
        // Handle waiting for the ground to load
        if ( IsFrozenWaitingForGroundToLoad () )
            HandleWaitingForGroundToLoad ();

        // Bodge to get things loaded quicker on spawn
        if ( m_iLoadAllModelsCounter )
        {
            m_iLoadAllModelsCounter--;
            if ( GetModelInfo () )
                g_pGame->GetStreaming()->LoadAllRequestedModels( false, "CClientPed::StreamedInPulse - m_iLoadAllModelsCounter" );
        }

        if ( m_bPendingRebuildPlayer )
            ProcessRebuildPlayer ();

        CControllerState Current;
        GetControllerState ( Current );

        if ( m_bIsLocalPlayer )
        {
            // Check if the ped got in fire without the script control
            m_bIsOnFire = m_pPlayerPed->IsOnFire();

            // Do our stealth aiming stuff
            SetStealthAiming ( ShouldBeStealthAiming () );

            // Process our scripted control settings
            bool bOnFoot = pVehicle ? false : true;
            CClientPad::ProcessAllToggledControls    ( Current, bOnFoot );
            CClientPad::ProcessSetAnalogControlState ( Current, bOnFoot );
        }

        // Is the player stealth aiming?
        if ( m_bStealthAiming )
        {
            // Grab our current anim
            CAnimBlendAssociation * pAssoc = GetFirstAnimation ();
            if ( pAssoc )
            {
                // Check we're not doing any important animations
                AnimationId animId = pAssoc->GetAnimID ();
                if ( animId == ANIM_ID_WALK_CIVI || animId == ANIM_ID_RUN_CIVI ||
                     animId == ANIM_ID_IDLE_STANCE || animId == ANIM_ID_WEAPON_CROUCH ||
                     animId == ANIM_ID_STEALTH_AIM )
                {
                    // Are our knife anims loaded?
                    CAnimBlock * pBlock = g_pGame->GetAnimManager ()->GetAnimationBlock ( "KNIFE" );
                    if ( pBlock->IsLoaded () )
                    {
                        // Force the animation
                        BlendAnimation ( ANIM_GROUP_STEALTH_KN, ANIM_ID_STEALTH_AIM, 8.0f );
                    }
                }
            }
        }

        // Is the player choking?
        if ( m_bIsChoking )
        {
            // Grab the choking task
            CTask* pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PHYSICAL_RESPONSE );
            if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_CHOKING )
            {
                // Update the task so he keeps on choking until we make him stop
                CTaskSimpleChoking* pTaskChoking = dynamic_cast < CTaskSimpleChoking* > ( pTask );
                pTaskChoking->UpdateChoke ( m_pPlayerPed, NULL, true );
            }
        }
        
        unsigned long ulNow = CClientTime::GetTime (); 
        //MS checks must take into account the gamespeed
        float fSpeedRatio = (1.0f/g_pGame->GetGameSpeed ()); 

        // Remember when we started standing from crouching
        if ( m_bWasDucked && m_bWasDucked != IsDucked() )
        {
            m_ulLastTimeBeganStand = ulNow;
            m_bWasDucked = false;
        }
        
        // Remember when we start aiming if we're aiming.
        CTask* pTask = m_pTaskManager->GetTaskSecondary ( TASK_SECONDARY_ATTACK );
        if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_USE_GUN )
        {
            if ( m_ulLastTimeAimed == 0 )
            {
                m_ulLastTimeAimed = ulNow;
            }
            if ( m_ulLastTimeBeganStand >= ulNow - 200.0f*fSpeedRatio )
            {
                if ( !g_pClientGame->IsGlitchEnabled ( CClientGame::GLITCH_FASTMOVE ) )
                {
                    //Disable movement keys.  This stops an exploit where players can run
                    //with guns shortly after standing
                    Current.LeftStickX = 0;
                    Current.LeftStickY = 0;
                }
            }

            // Fix to disable the quick cutting of the post deagle shooting animation
            // If we're USE_GUN, but aren't pressing the fire or aim keys we must be
            // in a post-fire state where the player is preparing to move back to 
            // a normal stance.  This can normally be cut using the crouch key, so block it
            if ( !g_pClientGame->IsGlitchEnabled ( CClientGame::GLITCH_CROUCHBUG ) )
            {
                if ( Current.RightShoulder1 == 0 && Current.LeftShoulder1 == 0 && Current.ButtonCircle == 0 )
                    Current.ShockButtonL = 0;
            }
        }
        else
        {
            m_ulLastTimeAimed = 0;
            // If we have the aim button pressed but aren't aiming, we're probably sprinting
            // If we're sprinting with an MP5,Deagle,Fire Extinguisher,Spray can, we shouldnt be able to shoot 
            // These weapons are weapons you can run with, but can't run with while aiming
            // This fixes a weapon desync bug involving aiming and sprinting packets arriving simultaneously
            eWeaponType iCurrentWeapon = GetCurrentWeaponType ();
            if ( Current.RightShoulder1 != 0 && 
             ( iCurrentWeapon == 29 || iCurrentWeapon == 24 || iCurrentWeapon == 23 || iCurrentWeapon == 41 || iCurrentWeapon == 42 ) )
            { 
                Current.ButtonCircle = 0;
                Current.LeftShoulder1 = 0;
            }
        }

        // Remember when we start the crouching if we're crouching.
        pTask = m_pTaskManager->GetTaskSecondary ( TASK_SECONDARY_DUCK );
        if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_DUCK )
        {
            m_bWasDucked = true;
            if ( m_ulLastTimeBeganCrouch == 0 )
                m_ulLastTimeBeganCrouch = ulNow;
                // No longer aiming if we're in the process of crouching
                m_ulLastTimeAimed = 0;
        }
        else
        {
            m_bWasDucked = false;
            m_ulLastTimeBeganCrouch = 0;
        }

        // If we started crouching less than some time ago, make sure we can't jump or sprint.
        // This fixes the exploit both locally and remotly that enables players to abort
        // the crouching animation and shoot quickly with slow shooting weapons. Also fixes
        // the exploit making you able to get crouched without being able to move and shoot
        // with infinite ammo for remote players.
        if ( m_ulLastTimeBeganCrouch != 0 )
        {
            if ( m_ulLastTimeBeganCrouch >= ulNow - 600.0f*fSpeedRatio )
            {
                if ( !g_pClientGame->IsGlitchEnabled ( CClientGame::GLITCH_FASTFIRE ) )
                {
                    Current.ButtonSquare = 0;
                    Current.ButtonCross = 0;
                }
                //Disable the fire keys whilst crouching as well
                Current.ButtonCircle = 0;
                Current.LeftShoulder1 = 0;
                if ( m_ulLastTimeBeganCrouch >= ulNow - 400.0f*fSpeedRatio )
                {
                    //Disable double crouching (another anim cut)
                    Current.ShockButtonL = 0;
                }
            }
        }
        // If we just started aiming, make sure they dont try and crouch
        else if ( (m_ulLastTimeAimed != 0 &&
                  m_ulLastTimeAimed >= ulNow - 300.0f*fSpeedRatio) || 
                  (ulNow - m_ulLastTimeFired) <= 300.0f*fSpeedRatio )
        {
            if ( !g_pClientGame->IsGlitchEnabled (  CClientGame::GLITCH_FASTFIRE ) )
            {
                Current.ShockButtonL = 0;
            }
        }

        // Are we working on entering a vehicle?
        pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
        if ( pTask )
        {
            // Entering as driver or passenger?
            int iTaskType = pTask->GetTaskType ();
            if ( iTaskType == TASK_COMPLEX_ENTER_CAR_AS_DRIVER ||
                 iTaskType == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER ||
                 iTaskType == TASK_COMPLEX_ENTER_BOAT_AS_DRIVER )
            {
                // Don't allow the aiming key (RightShoulder1)
                // This fixes bug allowing you to run around in aim mode while
                // entering a vehicle both locally and remotly.
                Current.RightShoulder1 = 0;
            }
        }
        //Fix for reloading aborting
        if ( GetWeapon()->GetState() == WEAPONSTATE_RELOADING )
        {
            //Disable changing weapons
            Current.DPadUp = 0;
            Current.DPadDown = 0;
            //Disable vehicle entry
            Current.ButtonTriangle = 0;
            //Disable jumping
            Current.ButtonSquare = 0;
            //if we are ducked disable movement (otherwise it will abort reloading)
            if ( IsDucked() ) {
                Current.LeftStickX = 0;
                Current.LeftStickY = 0;
            }
        }
        //Fix for crouching the end of animation aborting reload
        CControllerState Previous;
        GetLastControllerState ( Previous );
        if ( IsDucked() && ( Current.LeftStickX == 0 || Current.LeftStickY == 0)) {
            if (Previous.LeftStickY != 0 || Previous.LeftStickX != 0 ) 
                m_ulLastTimeMovedWhileCrouched = ulNow;
        }
        // Is this the local player?
        if ( m_bIsLocalPlayer )
        {
            // * Fix for weapons continuing to fire without any ammo (only needs to be applied locally)
            // Do we have a weapon?
            CWeapon * pWeapon = GetWeapon ();
            if ( pWeapon )
            {
                // Weapon wielding slot?
                eWeaponSlot slot = pWeapon->GetSlot ();
                if ( slot != WEAPONSLOT_TYPE_UNARMED && slot != WEAPONSLOT_TYPE_MELEE )
                {
                    eWeaponType eWeapon = pWeapon->GetType ( );
                    // No Ammo left?
                    float fSkill = GetStat ( g_pGame->GetStats ()->GetSkillStatIndex ( eWeapon ) );
                    CWeaponStat* pWeaponStat = g_pGame->GetWeaponStatManager ( )->GetWeaponStatsFromSkillLevel ( eWeapon, fSkill );
                    if ( ( pWeapon->GetAmmoInClip ( ) == 0 && pWeaponStat->GetMaximumClipAmmo ( ) > 0 ) || pWeapon->GetAmmoTotal () == 0 )
                    {
                        // Make sure our fire key isn't pressed
                        Current.ButtonCircle = 0;
                        Current.LeftShoulder1 = 0;
                    }
                }
            }


            // * Fix for warp glitches when sprinting and blocking simultaneously
            // This is applied locally, and prevents you using the backwards key while sprint-blocking
            CTask * pTask = m_pTaskManager->GetSimplestActiveTask ();
            if ( ( pTask && pTask->GetTaskType () == TASK_SIMPLE_PLAYER_ON_FOOT ) &&
                 ( GetWeapon()->GetSlot() == WEAPONSLOT_TYPE_UNARMED ) &&
                 ( Current.RightShoulder1 != 0 ) &&
                 ( Current.ButtonSquare != 0 ) &&
                 ( Current.ButtonCross != 0 ) )
            {  // We are block jogging
                if ( Current.LeftStickY > 0 )  
                    // We're pressing target+jump+sprint+backwards.  Using the backwards key in this situation is prone to bugs, swap it with forwards
                    Current.LeftStickY = -Current.LeftStickY;
                else if ( Current.LeftStickY == 0 )
                    // We're pressing target+jump+sprint 
                    // This causes some sliding, so let's disable this glitchy animation
                    Current.ButtonCross = 0;
            }

            // * Check for entering a vehicle whilst using a gun
            pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
            if ( pTask )
            {
                int iTaskType = pTask->GetTaskType ();
                if ( iTaskType == TASK_COMPLEX_ENTER_CAR_AS_DRIVER ||
                    iTaskType == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER )
                {
                    if ( IsUsingGun () )
                    {
                        pTask->MakeAbortable ( m_pPlayerPed, ABORT_PRIORITY_URGENT, NULL );
                    }
                }
            }
        }

        // Fix for stuck aim+move when FPS is greater than 45. This hack will raise the limit to 70 FPS
        // Fix works by applying a small input pulse on the other axis, at the start of movement
        pTask = m_pTaskManager->GetTaskSecondary ( TASK_SECONDARY_ATTACK );
        if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_USE_GUN )
        {
            // Make sure not crouching
            pTask = m_pTaskManager->GetTaskSecondary ( TASK_SECONDARY_DUCK );
            if ( !pTask || pTask->GetTaskType () != TASK_SIMPLE_DUCK )
            {
                eWeaponType eWeapon = GetCurrentWeaponType ( );
                float fSkill = GetStat ( g_pGame->GetStats ()->GetSkillStatIndex ( eWeapon ) );
                CWeaponStat * pWeaponStat = g_pGame->GetWeaponStatManager()->GetWeaponStatsFromSkillLevel ( eWeapon, fSkill ) ;
                // Apply fix for aimable weapons only
                if ( pWeaponStat && pWeaponStat->IsFlagSet ( WEAPONTYPE_CANAIMWITHARM ) == false  )
                {
                        // See which way input wants to go
                        const bool bInputRight = Current.LeftStickX >  6;
                        const bool bInputLeft  = Current.LeftStickX < -6;
                        const bool bInputFwd   = Current.LeftStickY < -6;
                        const bool bInputBack  = Current.LeftStickY >  6;

                        // See which way ped is currently going
                        CVector vecVelocity, vecRotationRadians;
                        GetMoveSpeed ( vecVelocity );
                        GetRotationRadians ( vecRotationRadians );
                        RotateVector ( vecVelocity, -vecRotationRadians );

                        // Calc how much to pulse other axis
                        short sFixY = 0;
                        short sFixX = 0;

                        if ( bInputRight && vecVelocity.fX >= 0.f )
                            sFixY = static_cast < short > ( UnlerpClamped ( 0.02f, vecVelocity.fX, 0.f ) * 64 );
                        else
                        if ( bInputLeft && vecVelocity.fX <= 0.f )
                            sFixY = static_cast < short > ( UnlerpClamped ( -0.02f, vecVelocity.fX, 0.f ) * -64 );

                        if ( bInputFwd && vecVelocity.fY >= 0.f )
                            sFixX = static_cast < short > ( UnlerpClamped ( 0.02f, vecVelocity.fY, 0.f ) * 64 );
                        else
                        if ( bInputBack && vecVelocity.fY <= 0.f )
                            sFixX = static_cast < short > ( UnlerpClamped ( -0.02f, vecVelocity.fY, 0.f ) * -64 );

                        // Apply pulse if bigger than existing input value
                        if ( abs ( sFixY ) > abs ( Current.LeftStickY ) )
                             Current.LeftStickY = sFixY;

                        if ( abs ( sFixX ) > abs ( Current.LeftStickX ) )
                             Current.LeftStickX = sFixX;
                }
            }
        }

        // Set the controller state we might've changed something in
        // We can't use SetControllerState as it will update the previous
        // controller state aswell and that will screw up with impulse buttons
        // like weapon switching.
        if ( m_bIsLocalPlayer )
        {
            m_pPad->SetCurrentControllerState ( &Current );
        }
        else
        {
            memcpy ( m_currentControllerState, &Current, sizeof ( CControllerState ) ); 
        }

        // Are we frozen and not in a vehicle
        if ( IsFrozen () && !pVehicle )
        {
            m_pPlayerPed->SetMatrix ( &m_matFrozen );
        }

        // Is our health locked?
        if ( m_bHealthLocked )
        {
            InternalSetHealth ( m_fHealth );
        }

        // Is our armor locked?
        if ( m_bArmorLocked )
        {
            m_pPlayerPed->SetArmor ( m_fArmor );
        }

        // In a vehicle?
        if ( pVehicle )
        {
            // Jax: this stops the game removing weapons in vehicles
            CWeapon *pCurrentWeapon = GetWeapon ();
            if ( pCurrentWeapon )
            {
                pCurrentWeapon->SetAsCurrentWeapon ();
            }

            // Remove any contact entity we have saved (we won't have one in a vehicle)
            if ( m_pCurrentContactEntity )
            {
                m_pCurrentContactEntity->RemoveContact ( this );
                m_pCurrentContactEntity = NULL;
            }
        }
        // Not in a vehicle?
        else
        {
            // Not the local player?
            if ( !m_bIsLocalPlayer )
            {
                // Force the player in/out?
                if ( m_bForceGettingIn )
                {
                    // Are we entering a vehicle and it's a different vehicle from the one we've entered?
                    if ( m_pOccupyingVehicle )
                    {
                        // Are we not already getting in?
                        CTask* pTask = GetCurrentPrimaryTask ();
                        if ( pTask )
                        {
                            int iTaskType = pTask->GetTaskType ();
                            if ( iTaskType != TASK_COMPLEX_ENTER_CAR_AS_DRIVER &&
                                iTaskType != TASK_COMPLEX_ENTER_CAR_AS_PASSENGER )
                            {
                                _GetIntoVehicle ( m_pOccupyingVehicle, m_uiOccupiedVehicleSeat, m_ucEnteringDoor );
                            }
                        }
                        else
                        {
                            _GetIntoVehicle ( m_pOccupyingVehicle, m_uiOccupiedVehicleSeat, m_ucEnteringDoor );
                        }
                    }
                }
                // Force him to get out of the vehicle as this tasks can sometimes cancel. This also
                // applies to the local player and can cause problem #2870.
                if ( m_bForceGettingOut )
                {
                    // Are we out of the car? If not, continue forcing.
                    if ( GetRealOccupiedVehicle () )
                    {
                        // Are we not already getting out?
                        CTask* pTask = GetCurrentPrimaryTask ();
                        if ( !pTask || pTask->GetTaskType () != TASK_COMPLEX_LEAVE_CAR )
                        {
                            GetOutOfVehicle ( m_ucLeavingDoor );
                        }
                    }
                    else
                    {
                        m_bForceGettingOut = false;
                        m_ucLeavingDoor = 0xFF;
                    }
                }

                Interpolate ();
                UpdateKeysync ();
            }

            // Store contact entities for quick-access (relies on playerManager being pulsed before vehicleManager + objectManager)
            CClientEntity * pContactEntity = NULL;
            pContactEntity = GetContactEntity ();
            // Is the current contact-entity different to our stored one?
            if ( pContactEntity != m_pCurrentContactEntity )
            {
                if ( m_pCurrentContactEntity )
                {
                    m_pCurrentContactEntity->RemoveContact ( this );
                }
                if ( pContactEntity )
                {
                    pContactEntity->AddContact ( this );
                }
                m_pCurrentContactEntity = pContactEntity;
            }
        }

        // Are we a CClientPed and not a CClientPlayer
        if ( GetType () == CCLIENTPED )
        {
            // Update our controller state to match our scripted pad
            m_Pad.DoPulse ( this );
        }

        // Are we waiting on an unloaded anim-block?
        if ( m_bRequestedAnimation && m_pAnimationBlock )
        {
            // Is it loaded now?
            if ( m_pAnimationBlock->IsLoaded () )
            {
                m_bRequestedAnimation = false;

                // Copy our name incase it gets deleted
                SString strAnimName = m_strAnimationName;
                // Run our animation
                RunNamedAnimation ( m_pAnimationBlock, strAnimName, m_iTimeAnimation, m_bLoopAnimation, m_bUpdatePositionAnimation, m_bInterruptableAnimation, m_bFreezeLastFrameAnimation );
            }            
        }

        // Update our alpha
        unsigned char ucAlpha = m_ucAlpha;
        // Are we in a different interior to the camera? set our alpha to 0
        if ( m_ucInterior != g_pGame->GetWorld ()->GetCurrentArea () ) ucAlpha = 0;
        RpClump * pClump = m_pPlayerPed->GetRpClump ();
        if ( pClump ) g_pGame->GetVisibilityPlugins ()->SetClumpAlpha ( pClump, ucAlpha );

        // Grab our current position
        CVector vecPosition = *m_pPlayerPed->GetPosition ();
        // Have we moved?
        if ( vecPosition != m_Matrix.vPos )
        {
            // Store our new position
            m_Matrix.vPos = vecPosition;
            m_matFrozen.vPos = vecPosition;

            // Update our streaming position
            UpdateStreamPosition ( vecPosition );
        }
        // Fix for unloading ped models which are currently streamed in (DO NOT REMOVE or players will not reset to the default models!)
        if ( m_ulStoredModel > 0 && m_ulModel == 0 )
        {
            // Make sure the scripter hasn't fixed this himself as well by changing from CJ back. (unlikely but who knows).
            if ( m_ulModel == 0 )
            {
                // Give him back his previous model
                SetModel ( m_ulStoredModel );
            }
            // Reset the stored model
            m_ulStoredModel = 0;
        }
        // Fix for unloading weapon models which are currently streamed in (DO NOT REMOVE or weapons will not reset to the default models!)
        if ( m_eStoredWeaponID > 0 )
        {
            // Give our Weapon back after deleting to reload the model
            CWeapon * pWeapon = GiveWeapon ( m_eStoredWeaponID, m_dwStoredAmmo );

            // Reset our states
            pWeapon->SetAmmoInClip ( m_dwStoredClipAmmo );
            if ( m_bStoredCurrentWeapon )
                pWeapon->SetAsCurrentWeapon ( );

            // Reset for next time.
            m_eStoredWeaponID = (eWeaponType)0;
            m_dwStoredAmmo = 0;
            m_dwStoredClipAmmo = 0;
            m_bStoredCurrentWeapon = false;
        }
    }
}


float CClientPed::GetCurrentRotation ( void )
{
    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetCurrentRotation ();
    }
    return m_fCurrentRotation;
}


void CClientPed::SetCurrentRotation ( float fRotation, bool bIncludeTarget )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetCurrentRotation ( fRotation );
        m_fCurrentRotation = fRotation;
        if ( bIncludeTarget )
        {
            m_pPlayerPed->SetTargetRotation ( fRotation );
            m_fTargetRotation = fRotation;
        }
    }
    else
    {
        // The ped model is still not loaded
        m_fCurrentRotation = fRotation;
        if ( bIncludeTarget )
            m_fTargetRotation = fRotation;
    }
}


void CClientPed::SetTargetRotation ( float fRotation )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetTargetRotation ( fRotation );
    }
    m_fTargetRotation = fRotation;
    SetCurrentRotation ( fRotation );
}

void CClientPed::SetTargetRotation ( unsigned long ulDelay, float fRotation, float fCameraRotation )
{
    m_ulBeginRotationTime = CClientTime::GetTime ();
    m_ulEndRotationTime = m_ulBeginRotationTime + ulDelay;
    m_fBeginRotation = ( m_pPlayerPed ) ? m_pPlayerPed->GetTargetRotation () : m_fCurrentRotation;
    m_fTargetRotationA = fRotation;
    m_fBeginCameraRotation = GetCameraRotation ();
    m_fTargetCameraRotation = fCameraRotation;
}

// Temporary
#include "../mods/deathmatch/logic/CClientGame.h"
extern CClientGame* g_pClientGame;

void CClientPed::Interpolate ( void )
{
    // Grab the current time
    unsigned long ulCurrentTime = CClientTime::GetTime ();

    // Do we have interpolation data for aiming?
    if ( m_ulBeginAimTime != 0 )
    {
        // We're not at the end of the interpolation?
        if ( ulCurrentTime < m_ulTargetAimTime )
        {
            // Interpolate the aiming
            float fDeltaTime = float ( m_ulTargetAimTime - m_ulBeginAimTime );
            float fDeltaX = m_fTargetAimX - m_fBeginAimX;
            float fDeltaY = m_fTargetAimY - m_fBeginAimY;
            float fProgress = float ( ulCurrentTime - m_ulBeginAimTime );
            m_shotSyncData->m_fArmDirectionY = m_fBeginAimY + ( fDeltaY / fDeltaTime * fProgress );

            // Hack for the wrap-around (the edge seems varying for different weapons...)
            if ( fDeltaX > 5.0f || fDeltaX < -5.0f )
            {
                m_shotSyncData->m_fArmDirectionX = m_fTargetAimX;
            }
            else
            {
                m_shotSyncData->m_fArmDirectionX = m_fBeginAimX + ( fDeltaX / fDeltaTime * fProgress );
            }
        }
        else
        {
            // Force the aiming to the final target position
            m_shotSyncData->m_fArmDirectionX = m_fTargetAimX;
            m_shotSyncData->m_fArmDirectionY = m_fTargetAimY;
            m_ulBeginAimTime = 0;

            // Force the hands to the correct "up" position for akimbos
            m_remoteDataStorage->SetAkimboTargetUp ( m_bTargetAkimboUp );
        }
    }

    // Don't interpolate rotation and position if we're working on getting in/out of a vehicle, or are attached to something
    if ( !m_bForceGettingIn && !m_bForceGettingOut && !m_pOccupiedVehicle && !m_pAttachedToEntity )
    {
        // We have interpolation data for rotation?
        if ( m_ulBeginRotationTime != 0 )
        {
            // We're not at the end?
            if ( ulCurrentTime < m_ulEndRotationTime )
            {
                // Interpolate the player rotation
                float fDeltaTime = float ( m_ulEndRotationTime - m_ulBeginRotationTime );
                float fDelta = GetOffsetDegrees ( m_fBeginRotation, m_fTargetRotationA );
                float fCameraDelta = GetOffsetDegrees ( m_fBeginCameraRotation, m_fTargetCameraRotation );
                float fProgress = float ( ulCurrentTime - m_ulBeginRotationTime );
                float fNewRotation = m_fBeginRotation + ( fDelta / fDeltaTime * fProgress );
                float fNewCameraRotation = m_fBeginCameraRotation + ( fCameraDelta / fDeltaTime * fProgress );

                // Hack for the wrap-around (the edge seems to be varying...)
                if ( fDelta < -5.0f || fDelta > 5.0f )
                {
                    SetCurrentRotation ( m_fTargetRotationA );
                    SetCameraRotation ( m_fTargetCameraRotation );
                }
                else
                {
                    SetCurrentRotation ( fNewRotation );
                    SetCameraRotation ( fNewCameraRotation );
                }
            }
            else
            {
                // Set the rotation to the final target
                SetCurrentRotation ( m_fTargetRotationA );
                SetCameraRotation ( m_fTargetCameraRotation );
                m_ulBeginRotationTime = 0;
            }
        }

        UpdateTargetPosition ();
    }


    // Interpolate the source and target vector for aiming
    if ( m_ulBeginTarget != 0 )
    {
        // We're not at the end?
        if ( ulCurrentTime < m_ulEndTarget )
        {
            // Grab the amount of time and how much of it we've progressed
            float fDeltaTime = float ( m_ulEndTarget - m_ulBeginTarget );
            float fProgress = float ( ulCurrentTime - m_ulBeginTarget );
            float fTime = fProgress / fDeltaTime;

            // Grab the delta source vector and interpolate it with the time
            CVector vecDelta = m_vecTargetSource - m_vecBeginSource;
            m_shotSyncData->m_vecShotOrigin = m_vecBeginSource + ( vecDelta * fTime );

            // Grab the radius of the target circle
            float fRadius = DistanceBetweenPoints3D ( m_vecTargetSource, m_vecTargetTarget );

            // Interpolate it with the time
            CVector vecInterpolateAngle = m_vecTargetInterpolateAngle * fTime;

            // Convert the angles back to the interpolated target position
            CVector vecInterpolated;
            vecInterpolated.fX = cos ( m_vecBeginTargetAngle.fX + vecInterpolateAngle.fX ) * fRadius + m_shotSyncData->m_vecShotOrigin.fX;
            vecInterpolated.fY = cos ( m_vecBeginTargetAngle.fY + vecInterpolateAngle.fY ) * fRadius + m_shotSyncData->m_vecShotOrigin.fY;
            vecInterpolated.fZ = cos ( m_vecBeginTargetAngle.fZ + vecInterpolateAngle.fZ ) * fRadius + m_shotSyncData->m_vecShotOrigin.fZ;

            // Set it
            m_shotSyncData->m_vecShotTarget = vecInterpolated;

            // Also set this as the target position for akimbo guns
            m_remoteDataStorage->SetAkimboTarget ( vecInterpolated );
        }
        else
        {
            m_shotSyncData->m_vecShotOrigin = m_vecTargetSource;
            m_shotSyncData->m_vecShotTarget = m_vecTargetTarget;
            m_ulBeginTarget = 0;

            // Also set this as the target position for akimbo guns
            m_remoteDataStorage->SetAkimboTarget ( m_vecTargetTarget );
        }
    }
    // Make sure we're using our origin vector
    m_shotSyncData->m_bUseOrigin = true;
}


void CClientPed::UpdateKeysync ( bool bCleanup )
{
    // TODO: we should ignore any 'old' keysyncs and set only the latest

    // Got any keysyncs to apply?
    if ( m_SyncBuffer.size () > 0 )
    {
        // Time to apply it?
        unsigned long ulCurrentTime = 0;
        if ( !bCleanup )
            ulCurrentTime = CClientTime::GetTime ();

        // Get the sync data at the front
        SDelayedSyncData* pData = m_SyncBuffer.front ();

        // Is the front data valid
        if ( pData )
        {
            // Check the front data's time (if this isn't valid, nothing else will be either so just leave it in the buffer)
            if ( bCleanup || ulCurrentTime >= pData->ulTime )
            {
                // Loop through until one of the conditions are caught
                do
                {
                    // Remove it from the list straight away so we don't end up picking it up again
                    m_SyncBuffer.pop_front ();

                    switch ( pData->ucType )
                    {
                        case DELAYEDSYNC_KEYSYNC:
                        {
                            SetControllerState ( pData->State );
                            Duck ( pData->bDucking );
                            break;
                        }
                        case DELAYEDSYNC_CHANGEWEAPON:
                        {
                            if ( pData->slot > WEAPONSLOT_TYPE_UNARMED )
                            {
                                // Grab the current weapon the player has
                                CWeapon* pPlayerWeapon = GetWeapon ();
                                eWeaponSlot eCurrentSlot = pData->slot;
                                if ( !pPlayerWeapon || pPlayerWeapon->GetSlot () != eCurrentSlot || GetRealOccupiedVehicle () )
                                {
                                    CWeapon* pSlotWeapon = GetWeapon ( eCurrentSlot );
                                    if ( pSlotWeapon )
                                    {
                                        pPlayerWeapon = GiveWeapon ( pSlotWeapon->GetType (), pData->usWeaponAmmo );
                                        if ( pPlayerWeapon )
                                        {
                                            pPlayerWeapon->SetAsCurrentWeapon ();
                                        }
                                    }
                                }

                                // Give it unlimited ammo, set the ammo in clip and weapon state
                                if ( pPlayerWeapon )
                                {
                                    pPlayerWeapon->SetAmmoTotal ( 9999 );
                                   // r1154 - Commented out below as it was causing reload animation desync (Issue #4503). Although it must have been there for a reason...
                                    if ( /*pData->usWeaponAmmo < pPlayerWeapon->GetAmmoInClip () &&*/ pPlayerWeapon->GetState () != WEAPONSTATE_RELOADING )
                                        pPlayerWeapon->SetAmmoInClip ( pData->usWeaponAmmo );
                                }
                            }
                            else
                            {
                                SetCurrentWeaponSlot ( WEAPONSLOT_TYPE_UNARMED );
                            }
                            break;
                        }
                        case DELAYEDSYNC_MOVESPEED:
                        {
                            SetMoveSpeed ( pData->vecTarget );
                            break;
                        }                
                    }

                    // Delete the data
                    delete pData;

                    // Reset the current sync data pointer
                    pData = NULL;

                    // Loop through until we have a new valid sync data, or we don't have any data left to process
                    while ( pData == NULL && m_SyncBuffer.size () > 0)
                    {
                        // Get the next sync data at the front
                        pData = m_SyncBuffer.front ();

                        // Check to see if the data is invalid
                        if ( !pData )
                        {
                            // It is, so remove it from the list
                            m_SyncBuffer.pop_front ();
                        }
                    }
                } while ( pData && ( bCleanup || ulCurrentTime >= pData->ulTime ) );
            }
        }
    }
}


void CClientPed::_CreateModel ( void )
{
    // Replace the loaded model info with the model we're going to load and
    // add a reference to it.
    m_pLoadedModelInfo = m_pModelInfo;
    m_pLoadedModelInfo->ModelAddRef ( BLOCKING, "CClientPed::_CreateModel" );

    // Create the new ped
    m_pPlayerPed = dynamic_cast < CPlayerPed* > ( g_pGame->GetPools ()->AddPed ( static_cast < ePedModel > ( m_ulModel ) ) );
    if ( m_pPlayerPed )
    {
        // Put our pointer in the stored data and update the remote data with the new model pointer
        m_pPlayerPed->SetStoredPointer ( this );

        // Add XRef
        g_pClientGame->GetGameEntityXRefManager ()->AddEntityXRef ( this, m_pPlayerPed );

        g_pMultiplayer->AddRemoteDataStorage ( m_pPlayerPed, m_remoteDataStorage );

        // Grab the task manager
        m_pTaskManager = m_pPlayerPed->GetPedIntelligence ()->GetTaskManager ();

        // Validate
        m_pManager->RestoreEntity ( this );                

        // Jump straight to the target position if we have one
        if ( HasTargetPosition () )
        {
            CVector vecPosition = m_interp.pos.vecTarget;
            if ( m_interp.pTargetOriginSource )
            {
                CVector vecOrigin;
                m_interp.pTargetOriginSource->GetPosition ( vecOrigin );
                vecPosition += vecOrigin;
            }
            m_Matrix.vPos = vecPosition;
        }

        // Restore any settings 
        m_pPlayerPed->SetMatrix ( &m_Matrix );
        m_pPlayerPed->SetCurrentRotation ( m_fCurrentRotation );
        m_pPlayerPed->SetTargetRotation ( m_fTargetRotation );
        m_pPlayerPed->SetMoveSpeed ( &m_vecMoveSpeed );
        m_pPlayerPed->SetTurnSpeed ( &m_vecTurnSpeed );
        Duck ( m_bDucked );
        SetWearingGoggles ( m_bWearingGoggles );
        m_pPlayerPed->SetVisible ( m_bVisible );
        m_pPlayerPed->SetUsesCollision ( m_bUsesCollision );
        m_pPlayerPed->SetHealth ( m_fHealth );
        m_pPlayerPed->SetArmor ( m_fArmor );
        m_pPlayerPed->SetLighting ( m_fLighting );
        WorldIgnore ( m_bWorldIgnored );

        // Set remote players to not fall off bikes locally, let them decide
        if ( m_bIsLocalPlayer )
            SetCanBeKnockedOffBike ( m_bCanBeKnockedOffBike );
        else
            SetCanBeKnockedOffBike ( false );

        // Restore their weapons
        for ( int i = 0 ; i < (int)WEAPONSLOT_MAX ; i++ )
        {
            if ( m_WeaponTypes [ i ] != WEAPONTYPE_UNARMED )
                GiveWeapon ( m_WeaponTypes [ i ], 1 );   // TODO: store ammo for each weapon
        }

        m_pPlayerPed->SetCurrentWeaponSlot ( m_CurrentWeaponSlot );
        m_pPlayerPed->SetFightingStyle ( m_FightingStyle, 6 );  
        m_pPlayerPed->SetMoveAnim ( m_MoveAnim );
        SetHasJetPack ( m_bHasJetPack );                
        SetInterior ( m_ucInterior );
        SetAlpha ( m_ucAlpha );
        SetChoking ( m_bIsChoking );
        SetSunbathing ( m_bSunbathing, false );
        SetHeadless ( m_bHeadless );
        SetOnFire ( m_bIsOnFire );
        SetSpeechEnabled ( m_bSpeechEnabled );

        // Rebuild the player if it's CJ. So we get the clothes.
        RebuildModel ();

        // Reattach to an entity + any entities attached to this
        ReattachEntities ();

        // Warp it into a vehicle, if necessary
        if ( m_pOccupiedVehicle )
            WarpIntoVehicle ( m_pOccupiedVehicle, m_uiOccupiedVehicleSeat );      

        // Are we dead?
        if ( m_fHealth == 0.0f )
        {
            Kill ( WEAPONTYPE_UNARMED, 0, false, true );
        }

        // Are we still playing a looped animation?
        if ( m_bLoopAnimation && m_pAnimationBlock )
        {
            // Copy our anim name incase it gets deleted
            SString strAnimName = m_strAnimationName;
            // Run our animation
            RunNamedAnimation ( m_pAnimationBlock, strAnimName, m_iTimeAnimation, m_bLoopAnimation, m_bUpdatePositionAnimation, m_bInterruptableAnimation, m_bFreezeLastFrameAnimation );
        }

        // Set the voice that corresponds to our model
        short sVoiceType, sVoiceID;
        m_pModelInfo->GetVoice ( &sVoiceType, &sVoiceID );
        SetVoice ( sVoiceType, sVoiceID );

        // Tell the streamer we created the player
        NotifyCreate ();
    }
    else
    {
        // Remove the reference again
        m_pLoadedModelInfo->RemoveRef ();
        m_pLoadedModelInfo = NULL;

        // Tell the streamed we were unable to create
        NotifyUnableToCreate ();
    }
}


void CClientPed::_CreateLocalModel ( void )
{
    // Init the local player and grab the pointers
    g_pGame->InitLocalPlayer ();
    m_pPlayerPed = dynamic_cast < CPlayerPed* > ( g_pGame->GetPools ()->GetPedFromRef ( (DWORD)1 ) );
    
    if ( m_pPlayerPed )
    {
        m_pTaskManager = m_pPlayerPed->GetPedIntelligence ()->GetTaskManager ();

        // Put our pointer in its stored pointer
        m_pPlayerPed->SetStoredPointer ( this );

        // Add XRef
        g_pClientGame->GetGameEntityXRefManager ()->AddEntityXRef ( this, m_pPlayerPed );

        // Add a reference to the model we're using
        m_pLoadedModelInfo = m_pModelInfo;
        m_pLoadedModelInfo->ModelAddRef ( BLOCKING, "CClientPed::_CreateLocalModel" );

        // Make sure we are CJ
        if ( m_pPlayerPed->GetModelIndex () != m_ulModel )
        {
            m_pPlayerPed->SetModelIndex ( m_ulModel );
        }

        // Give him the default fighting style
        m_pPlayerPed->SetFightingStyle ( m_FightingStyle, 6 );
        m_pPlayerPed->SetMoveAnim ( m_MoveAnim );
        SetHasJetPack ( m_bHasJetPack );

        // Rebuild him so he gets his clothes
        RebuildModel ();

        // Validate
        m_pManager->RestoreEntity ( this );

        // Tell the streamer we created the player
        NotifyCreate ();
    }
}


void CClientPed::_DestroyModel ()
{
    // Remove our linked contact entity
    if ( m_pCurrentContactEntity )
    {
        m_pCurrentContactEntity->RemoveContact ( this );
        m_pCurrentContactEntity = NULL;
    }

    // Remember the player position
    m_Matrix.vPos = *m_pPlayerPed->GetPosition ();

    m_fCurrentRotation = m_pPlayerPed->GetCurrentRotation ();
    m_pPlayerPed->GetMoveSpeed ( &m_vecMoveSpeed );
    m_pPlayerPed->GetTurnSpeed ( &m_vecTurnSpeed );
    m_bDucked = IsDucked ();
    m_bWearingGoggles = IsWearingGoggles ();
    m_pPlayerPed->SetOnFire ( false );
    m_fLighting = m_pPlayerPed->GetLighting ();

    /* Eventually remove from vehicle
        MUST use internal-func, to save the the occupied-vehicle (streaming) */
    CClientVehicle* pVehicle = GetRealOccupiedVehicle ();
    if ( !pVehicle )
    {
        pVehicle = GetOccupiedVehicle ();
        if ( !pVehicle )
        {
            pVehicle = m_pOccupyingVehicle;
        }
    }
    if ( pVehicle )
    {
        CVehicle* pGameVehicle = pVehicle->GetGameVehicle ();
        if ( pGameVehicle )
        {
            InternalRemoveFromVehicle ( pGameVehicle );
        }
    }

    g_pMultiplayer->RemoveRemoteDataStorage ( m_pPlayerPed );

    // Invalidate
    m_pManager->InvalidateEntity ( this );

    // Remove XRef
    g_pClientGame->GetGameEntityXRefManager ()->RemoveEntityXRef ( this, m_pPlayerPed );

    // Remove the ped from the world
    g_pGame->GetPools ()->RemovePed ( m_pPlayerPed );
    m_pPlayerPed = NULL;
    m_pTaskManager = NULL;

    // Remove the reference to its model
    m_pLoadedModelInfo->RemoveRef ();
    m_pLoadedModelInfo = NULL;

    // Any pending rebuild will not be required now
    m_bPendingRebuildPlayer = false;

    NotifyDestroy ();
}


void CClientPed::_DestroyLocalModel ()
{
    /* Eventually remove from vehicle
        MUST use internal-func, to save the the occupied-vehicle (streaming) */
    CClientVehicle* pVehicle = GetRealOccupiedVehicle ();
    if ( !pVehicle )
    {
        pVehicle = GetOccupiedVehicle ();
        if ( !pVehicle )
        {
            pVehicle = m_pOccupyingVehicle;
        }
    }
    if ( pVehicle )
    {           
        CVehicle* pGameVehicle = pVehicle->GetGameVehicle ();
        if ( pGameVehicle )
        {
            InternalRemoveFromVehicle ( pGameVehicle );
        }

        pVehicle->RemoveStreamReference ();
    }

    // Invalidate
    m_pManager->InvalidateEntity ( this );

    // Remove XRef
    g_pClientGame->GetGameEntityXRefManager ()->RemoveEntityXRef ( this, m_pPlayerPed );

    // Make sure we are CJ again
    if ( m_pPlayerPed->GetModelIndex () != 0 )
    {
        m_pPlayerPed->SetModelIndex ( 0 );
    }

    // Remove reference to our previous model
    m_pLoadedModelInfo->RemoveRef ();
    m_pLoadedModelInfo = NULL;

    // NULL our pointers, we don't destroy the local player
    m_pPlayerPed = NULL;
    m_pTaskManager = NULL;
}


void CClientPed::_ChangeModel ( void )
{
    // Different model than before?
    if ( m_pPlayerPed->GetModelIndex () != m_ulModel )
    {
        g_pMultiplayer->SetAutomaticVehicleStartupOnPedEnter ( false );

        // We need to reset visual stats when changing from CJ model
        if ( m_pPlayerPed->GetModelIndex () == 0 )
        {
            // Reset visual stats
            SetStat ( 21, 0.0f );
            SetStat ( 23, 0.0f );
        }

        if ( m_bIsLocalPlayer )
        {
            // TODO: Create a simple function to save and restore player states and use it
            //       all the places that to context saving/restoring.

            // Save the vehicle he's in
            CClientVehicle* pVehicle = GetOccupiedVehicle ();
            unsigned int uiSeat = GetOccupiedVehicleSeat ();

            // Are we leaving it? Don't warp him back into anything
            if ( pVehicle )
            {
                // Are we leaving it? Don't warp back into it.
                if ( IsLeavingVehicle () ) pVehicle = NULL;

                // Remove him from the vehicle
                RemoveFromVehicle ();
            }

            m_pPlayerPed->GetFightingStyle ();

            // Takes care of clothes/task issues
            Respawn ( NULL, true, false );

            // Remember the model we had loaded and store the new model we're going to load
            CModelInfo* pLoadedModel = m_pLoadedModelInfo;
            m_pLoadedModelInfo = m_pModelInfo;

            // Add reference to the model
            m_pLoadedModelInfo->ModelAddRef ( BLOCKING, "CClientPed::_ChangeModel" );

            // Set the new player model and restore the interior
            m_pPlayerPed->SetModelIndex ( m_ulModel );

            // Rebuild the player after a skin change
            if ( m_ulModel == 0 )
            {
                // When the local player changes to CJ, the clothes geometry gets an extra ref from somewhere, causing a memory leak.
                // So make sure clothes geometry is built now...
                m_pClothes->AddAllToModel ();
                m_pPlayerPed->RebuildPlayer ();

                // ...and decrement the extra ref
                m_pPlayerPed->RemoveGeometryRef ();
            }
            else
            {
                // When the local player changes to another (non CJ) model, the geometry gets an extra ref from somewhere, causing a memory leak.
                // So decrement the extra ref here
                m_pPlayerPed->RemoveGeometryRef ();
            }


            // Remove reference to the old model we used (Flag extra GTA reference to be removed as well)
            pLoadedModel->RemoveRef ( true );
            pLoadedModel = NULL;

            // Warp into it again
            if ( pVehicle )
            {
                WarpIntoVehicle ( pVehicle, uiSeat );
            }
            m_bDontChangeRadio = false;

            // Are we still playing a looped animation?
            if ( m_bLoopAnimation && m_pAnimationBlock )
            {
                // Copy our anim name incase it gets deleted
                SString strAnimName = m_strAnimationName;
                // Run our animation
                RunNamedAnimation ( m_pAnimationBlock, strAnimName, m_iTimeAnimation, m_bLoopAnimation, m_bUpdatePositionAnimation, m_bInterruptableAnimation, m_bFreezeLastFrameAnimation );
            }

            // Set the voice that corresponds to the new model
            short sVoiceType, sVoiceID;
            m_pModelInfo->GetVoice ( &sVoiceType, &sVoiceID );
            SetVoice ( sVoiceType, sVoiceID );
        }
        else
        {
            // ChrML: Changing the skin in certain cases causes player sliding. So we recreate instead.

            // Kill the old player
            _DestroyModel ();

            // Create the new with the new skin
            _CreateModel ();
        }

        g_pMultiplayer->SetAutomaticVehicleStartupOnPedEnter ( true );
    }
}


void CClientPed::ReCreateModel ( void )
{
    // We can only recreate if we're not the local player and if we have a player model
    if ( !m_bIsLocalPlayer && m_pPlayerPed )
    {
        // Make sure we don't unload then load unneccessarily if the new and the old model were the same
        bool bSameModel = ( m_pLoadedModelInfo == m_pModelInfo );
        if ( bSameModel )
        {
            m_pLoadedModelInfo->ModelAddRef ( BLOCKING, "CClientPed::ReCreateModel" );
        }

        // Destroy the old model
        _DestroyModel ();

        // Create the new model
        _CreateModel ();

        // Remove the reference we temporarily added again
        if ( bSameModel )
        {
            m_pLoadedModelInfo->RemoveRef ();
        }
    }
}


void CClientPed::ModelRequestCallback ( CModelInfo* pModelInfo )
{
    // If we have a player loaded
    if ( m_pPlayerPed )
    {
        // Change its skin
        _ChangeModel ();
    }
    else
    {
        // If we don't have a player loaded, load it
        _CreateModel ();
    }
}


void CClientPed::RebuildModel ( bool bDelayChange )
{
    // We have a player
    if ( m_pPlayerPed )
    {
        // We are CJ?
        if ( m_ulModel == 0 )
        {
            // Adds only the neccesary textures
            m_pClothes->AddAllToModel ();

            m_bPendingRebuildPlayer = true;

            // Apply immediately unless there is a chance more clothes states will change (e.g. via script)
            if ( !bDelayChange )
                ProcessRebuildPlayer ();
        }
    }
}


//
// Process any pending build but avoid rebuilding more than once a frame
//
void CClientPed::ProcessRebuildPlayer ( void )
{
    assert ( m_pPlayerPed );

    if ( m_bPendingRebuildPlayer && m_uiFrameLastRebuildPlayer != g_pClientGame->GetFrameCount () )
    {
        m_bPendingRebuildPlayer = false;
        m_uiFrameLastRebuildPlayer = g_pClientGame->GetFrameCount ();

        if ( m_bIsLocalPlayer )
        {
            m_pPlayerPed->RebuildPlayer ();
        }
        else
        {
            g_pMultiplayer->RebuildMultiplayerPlayer ( m_pPlayerPed );
        }
    }
}


void CClientPed::StreamIn ( bool bInstantly )
{
    if ( m_bIsLocalPlayer )
    {
        NotifyCreate ();
        return;
    }
#if 0
    // We need to create now?
    if ( bInstantly )
    {
        // Request its model blocking
        if ( !m_pPlayerPed && m_pRequester->RequestBlocking ( static_cast < unsigned short > ( m_ulModel ), "CClientVehicle::StreamIn - bInstantly" ) )
        {
            m_pModelInfo->MakeCustomModel ( );
            // If it was loaded, create it immediately.
            _CreateModel ();
        }
        else NotifyUnableToCreate ();
    }
    else
#endif
    {
        // Request it
        if ( !m_pPlayerPed && m_pRequester->Request ( static_cast < unsigned short > ( m_ulModel ), this ) )
        {
            m_pModelInfo->MakeCustomModel ( );
            // If it was loaded, create it immediately.
            _CreateModel ();
        }
        else NotifyUnableToCreate ();
    }
}


void CClientPed::StreamOut ( void )
{
    // Make sure we have a player ped and that we're not
    // the local player
    if ( m_pPlayerPed && !m_bIsLocalPlayer )
    {
        // Destroy us
        _DestroyModel ();

        // Make sure no model loading is pending. This would recreate
        // us very soon.
        m_pRequester->Cancel ( this, true );
    }
}

void CClientPed::StreamOutWeaponForABit ( eWeaponSlot eSlot )
{
    // Get the Weapon
    CWeapon * pWeapon = GetWeapon ( eSlot );
    if ( pWeapon )
    {
        // Store our states i.e. clip Ammo, Ammo, type and if it's the current weapon
        m_dwStoredClipAmmo = pWeapon->GetAmmoInClip ( );
        m_dwStoredAmmo = pWeapon->GetAmmoTotal ( );
        m_eStoredWeaponID = pWeapon->GetType();
        m_bStoredCurrentWeapon = GetCurrentWeaponType () == m_eStoredWeaponID;

        // Remove it
        pWeapon->Remove ();
    }
}

void CClientPed::InternalWarpIntoVehicle ( CVehicle* pGameVehicle )
{
    if ( m_pPlayerPed )
    {
        // Reset whatever task
        m_pTaskManager->RemoveTask ( TASK_PRIORITY_PRIMARY );

        // Create a task to warp the player in and execute it
        CTaskSimpleCarSetPedInAsDriver* pInTask = g_pGame->GetTasks ()->CreateTaskSimpleCarSetPedInAsDriver ( pGameVehicle );
        if ( pInTask )
        {
            pInTask->SetIsWarpingPedIntoCar ();
            pInTask->ProcessPed ( m_pPlayerPed );
            pInTask->Destroy ();
        }        

        // If we're a remote player, make sure we can't fall off
        if ( !m_bIsLocalPlayer )
        {
            SetCanBeKnockedOffBike ( false );
        }

        // Jax: make sure our camera is fixed on the new vehicle
        if ( m_bIsLocalPlayer )
        {
            CClientCamera * pCamera = m_pManager->GetCamera ();
            if ( !pCamera->IsInFixedMode () )
            {
                // Jax: very hacky, clean up if possible (some camera-target pointer)
                * ( unsigned long * ) ( 0xB6F3B8 ) = ( unsigned long ) pGameVehicle->GetVehicleInterface ();
            }
        }
    }
}


void CClientPed::InternalRemoveFromVehicle ( CVehicle* pGameVehicle )
{
    if ( m_pPlayerPed && m_pTaskManager )
    {
        // Reset whatever task
        m_pTaskManager->RemoveTask ( TASK_PRIORITY_PRIMARY );

        // Create a task to warp the player in and execute it
        CTaskSimpleCarSetPedOut* pOutTask = g_pGame->GetTasks ()->CreateTaskSimpleCarSetPedOut ( pGameVehicle, 1, false );
        if ( pOutTask )
        {
            // May seem illogical, but it'll crash without this
            pOutTask->SetKnockedOffBike(); 

            pOutTask->ProcessPed ( m_pPlayerPed );
            pOutTask->SetIsWarpingPedOutOfCar ();
            pOutTask->Destroy ();
        }

        m_Matrix.vPos = *m_pPlayerPed->GetPosition ();

        // Local player?
        if ( m_bIsLocalPlayer )
        {
            // Turn off the radio
            StopRadio ();
        }
    }
}


bool CClientPed::PerformChecks ( void )
{
    // Must be streamed in
    if ( m_pPlayerPed )
    {
        // Is this the local player?
        if ( m_bIsLocalPlayer )
        {
            // Is GTA's health/armor less than or equal to our health/armor?
            // The player should not be able to gain any health/armor without us knowing..
            // meaning all health/armor giving must go through SetHealth/SetArmor.
            if ( ( m_fHealth > 0.0f && m_pPlayerPed->GetHealth () > m_fHealth + FLOAT_EPSILON ) ||
                 ( m_fArmor < 100.0f && m_pPlayerPed->GetArmor () > m_fArmor + FLOAT_EPSILON ) )
            {
                g_pCore->GetConsole ()->Printf ( "healthCheck: %f %f", m_pPlayerPed->GetHealth (), m_fHealth );
                g_pCore->GetConsole ()->Printf ( "armorCheck: %f %f", m_pPlayerPed->GetArmor (), m_fArmor );
                return false;
            }
            //Perform the checks in CGame
            if ( !g_pGame->PerformChecks() ) {
                return false;
            }
        }
    }


    // Player is not a cheater yet
    return true;
}


void CClientPed::StartRadio ( void )
{
    // We use this to avoid radio lags sometimes. Also make sure
    // it's not already on
    if ( !m_bDontChangeRadio && !m_bRadioOn )
    {
        // Turn it on if we're not on channel none
        if ( m_ucRadioChannel != 0 )
            g_pGame->GetAudioEngine ()->StartRadio ( m_ucRadioChannel );

        m_bRadioOn = true;
    }
}


void CClientPed::StopRadio ( void )
{
    // We use this to avoid radio lags sometimes
    if ( !m_bDontChangeRadio )
    {
        // Stop the radio and mark it as off
        g_pGame->GetAudioEngine ()->StopRadio ();
        m_bRadioOn = false;
    }
}


void CClientPed::Duck ( bool bDuck )
{
    if ( m_pPlayerPed )
    {
        if ( bDuck )
        {
            // Check if he's already ducking
            CTask* pTaskDuck = m_pTaskManager->GetTaskSecondary ( TASK_SECONDARY_DUCK );
            if ( !pTaskDuck || pTaskDuck->GetTaskType () != TASK_SIMPLE_DUCK )
            {
                // DUCK_TASK_CONTROLLED means we can move around while ducked, I think
                pTaskDuck = g_pGame->GetTasks ()->CreateTaskSimpleDuck ( DUCK_TASK_CONTROLLED );
                if ( pTaskDuck )
                {
                    pTaskDuck->SetAsSecondaryPedTask ( m_pPlayerPed, TASK_SECONDARY_DUCK );
                }
            }
        }
        else
        {
            //Reset ducking
            m_ulLastTimeBeganCrouch = 0;
            // Jax: lets give this a whirl (it seems to cancel the task automatically)
            m_pPlayerPed->SetDucking ( false );
        }
    }
    m_bDucked = bDuck;
}

bool CClientPed::IsDucked ( void )
{
    if ( m_pPlayerPed )
    {
        CTask * pTaskDuck = m_pTaskManager->GetTaskSecondary ( TASK_SECONDARY_DUCK );
        if ( pTaskDuck )
        {
            return true;
        }
    }
    
    return m_bDucked;
}


void CClientPed::SetChoking ( bool bChoking )
{
    // Remember the choking state
    m_bIsChoking = bChoking;

    // We have a task manager?
    if ( m_pTaskManager && m_pPlayerPed )
    {
        // Grab the physical response task. Is he already choking?
        CTask* pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PHYSICAL_RESPONSE );
        if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_CHOKING )
        {
            // Make him stop choking if that's what we're supposed to do
            if ( !bChoking )
            {
                m_pTaskManager->RemoveTask ( TASK_PRIORITY_PHYSICAL_RESPONSE );
            }
        }
        else
        {
            // His not choking. Make him choke if that's what we're supposed to do.
            if ( bChoking )
            {
                // Create the choking task
                CTaskSimpleChoking* pTask = g_pGame->GetTasks ()->CreateTaskSimpleChoking ( NULL, true );
                if ( pTask )
                {
                    pTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PHYSICAL_RESPONSE );
                }
            }
        }
    }
}


bool CClientPed::IsChoking ( void )
{
    // We have a task manager?
    if ( m_pTaskManager )
    {
        // Return whether we have a physical task and it's choking
        CTask* pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PHYSICAL_RESPONSE );
        return ( pTask && pTask->GetTaskType () == TASK_SIMPLE_CHOKING );
    }
    else
    {
        // Otherwize remember the state we've stored
        return m_bIsChoking;
    }
}


void CClientPed::SetWearingGoggles ( bool bWearing )
{
    if ( m_pPlayerPed )
    {
        if ( bWearing != IsWearingGoggles () )
        {
            // Make him wear goggles
            m_pPlayerPed->SetGogglesState ( bWearing );

            // Are our goggle anims loaded?
            CAnimBlock * pBlock = g_pGame->GetAnimManager ()->GetAnimationBlock ( "GOGGLES" );
            if ( pBlock->IsLoaded () )
            {
                BlendAnimation ( ANIM_GROUP_GOGGLES, ANIM_ID_GOGGLES_ON, 4.0f );
            }
        }
    }
    m_bWearingGoggles = bWearing;
}


bool CClientPed::IsWearingGoggles ( bool bCheckMoving )
{
    if ( m_pPlayerPed )
    {
        if ( bCheckMoving )
        {
            bool bPuttingOn;
            if ( IsMovingGoggles ( bPuttingOn ) ) return bPuttingOn;
        }

        return m_pPlayerPed->IsWearingGoggles ();
    }
    return m_bWearingGoggles;
}


bool CClientPed::IsMovingGoggles ( bool & bPuttingOn )
{
    if ( m_pPlayerPed )
    {
        CTask * pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
        if ( pTask && pTask->GetTaskType () == TASK_COMPLEX_USE_GOGGLES )
        {
            pTask = pTask->GetSubTask ();
            if ( pTask )
            {
                if ( pTask->GetTaskType () == TASK_SIMPLE_GOGGLES_ON )
                {
                    bPuttingOn = true;
                    return true;
                }
                else if ( pTask->GetTaskType () == TASK_SIMPLE_GOGGLES_OFF )
                {
                    bPuttingOn = false;
                    return true;
                }
            }
        }
    }
    return false;
}


void CClientPed::_GetIntoVehicle ( CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor )
{
    assert ( m_pOccupiedVehicle == NULL );
    assert ( m_pOccupyingVehicle == NULL || m_pOccupyingVehicle == pVehicle );

    //Check for swimming task and warp to door.
    CTask* pTask = 0;
    if ( m_pTaskManager ) pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
    unsigned short usVehicleModel = pVehicle->GetModel();
    if ( ((pTask && pTask->GetTaskType () == TASK_COMPLEX_IN_WATER) || pVehicle->IsOnWater()) && ( usVehicleModel == VT_SKIMMER ||
                                                                                                   usVehicleModel == VT_SEASPAR ||
                                                                                                   usVehicleModel == VT_LEVIATHN ||
                                                                                                   usVehicleModel == VT_VORTEX ) )
    {
        CVector vecDoorPos;
        unsigned int uiDoor;
        GetClosestDoor( pVehicle, uiSeat == 0, uiSeat != 0, uiDoor, &vecDoorPos );
        Teleport(vecDoorPos);
    }
    // Driverseat
    if ( uiSeat == 0 )
    {
        if ( m_bIsLocalPlayer )
        {
            pVehicle->SetSwingingDoorsAllowed ( true );
        }

        if ( m_pPlayerPed )
        {
            // Grab the game vehicle. If it exists, begin walking the player into it
            CVehicle* pGameVehicle = pVehicle->m_pVehicle;
            if ( pGameVehicle )
            {
                // Create and set the get-in task                
                CTaskComplexEnterCarAsDriver* pInTask = g_pGame->GetTasks ()->CreateTaskComplexEnterCarAsDriver ( pGameVehicle );
                if ( pInTask )
                {
                    pInTask->SetTargetDoor ( ucDoor );
                    pInTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PRIMARY, true );
                }
            }
        }

        // Tell the vehicle that we're occupying it
        CClientVehicle::SetPedOccupyingVehicle ( this, pVehicle, uiSeat, ucDoor );
    }
    else
    {
        // HACK: Grabs the closest passenger-door for bikes
        eClientVehicleType vehicleType = CClientVehicleManager::GetVehicleType ( pVehicle->m_usModel );
        if ( vehicleType == CLIENTVEHICLE_BIKE || vehicleType == CLIENTVEHICLE_QUADBIKE )
        {
            unsigned int uiTemp;
            if ( GetClosestDoor ( pVehicle, false, true, uiTemp ) )
            {
                uiSeat = uiTemp;
            }
        }

        unsigned char ucSeat = CClientVehicleManager::ConvertIndexToGameSeat ( pVehicle->m_usModel, uiSeat );
        if ( ucSeat != 0 && ucSeat != 0xFF )
        {
            if ( m_pPlayerPed )
            {
                // Grab the game vehicle. If it exists, begin walking the player into it
                CVehicle* pGameVehicle = pVehicle->m_pVehicle;
                if ( pGameVehicle )
                {
                    // Create the task for walking him in
                    CTaskComplexEnterCarAsPassenger* pInTask = g_pGame->GetTasks ()->CreateTaskComplexEnterCarAsPassenger ( pGameVehicle, ucSeat, false );
                    if ( pInTask )
                    {
                        pInTask->SetTargetDoor ( ucDoor );
                        pInTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PRIMARY, true );
                    }
                }
            }

            // Tell the vehicle we're occupying it
            CClientVehicle::SetPedOccupyingVehicle ( this, pVehicle, uiSeat, ucDoor );
        }
    }
}


bool CClientPed::SetHasJetPack ( bool bHasJetPack )
{
    if ( m_pPlayerPed )
    {
        if ( bHasJetPack )
        {
            if ( !IsInVehicle() && !HasJetPack() )
            {
                // jumping task
                CTask * pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
                if ( pTask )
                {
                    pTask->MakeAbortable ( m_pPlayerPed, ABORT_PRIORITY_IMMEDIATE, NULL );
                    pTask->Destroy ();
                    m_pTaskManager->RemoveTask ( TASK_PRIORITY_PRIMARY );
                    
                }
                // falling task    
                pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_TEMP );
                if ( pTask )
                {
                    pTask->MakeAbortable ( m_pPlayerPed, ABORT_PRIORITY_IMMEDIATE, NULL );
                    pTask->Destroy ();
                    m_pTaskManager->RemoveTask ( TASK_PRIORITY_EVENT_RESPONSE_TEMP );                    
                }
                // swimming task
                pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
                if ( pTask )
                {
                    pTask->MakeAbortable ( m_pPlayerPed, ABORT_PRIORITY_IMMEDIATE, NULL );
                    pTask->Destroy ();
                    m_pTaskManager->RemoveTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );                    
                }

                CTaskSimpleJetPack * pJetPackTask = g_pGame->GetTasks ()->CreateTaskSimpleJetpack ();
                if ( pJetPackTask )
                {
                    pJetPackTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PRIMARY, true );
                    m_bHasJetPack = true;
                    return true;
                }
            }
            return false;
        }
        else
        {
            CTask * pPrimaryTask = m_pTaskManager->GetSimplestActiveTask ( );
            if ( pPrimaryTask && pPrimaryTask->GetTaskType() == TASK_SIMPLE_JETPACK )
            {
                pPrimaryTask->MakeAbortable ( m_pPlayerPed, ABORT_PRIORITY_URGENT, NULL );
            }

            m_bHasJetPack = false;
            return true;
        }
    }
    m_bHasJetPack = bHasJetPack;
    return true;
}


bool CClientPed::HasJetPack ( void )
{
    if ( m_pPlayerPed )
    {
        CTask * pPrimaryTask = m_pTaskManager->GetSimplestActiveTask ( );
        if ( pPrimaryTask && pPrimaryTask->GetTaskType() == TASK_SIMPLE_JETPACK )
        {
            return true;
        }
        return false;
    }
    return m_bHasJetPack;
}


bool CClientPed::IsInWater ( void )
{
    if ( m_pPlayerPed )
    {
        CTask * pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
        if ( pTask )
        {
            if ( pTask->GetTaskType () == TASK_COMPLEX_IN_WATER )
            {
                return true;
            }
        }
    }
    return false;
}


float CClientPed::GetDistanceFromGround ( void )
{
    CVector vecPosition;
    GetPosition ( vecPosition );
    float fGroundLevel = static_cast < float > (
        g_pGame->GetWorld ()->FindGroundZFor3DPosition ( &vecPosition ) );

    return ( vecPosition.fZ - fGroundLevel );
}


bool CClientPed::IsOnGround ( void )
{
    CVector vecPosition;
    GetPosition ( vecPosition );
    float fGroundLevel = static_cast < float > (
        g_pGame->GetWorld ()->FindGroundZFor3DPosition ( &vecPosition ) );
    return ( vecPosition.fZ > fGroundLevel && ( vecPosition.fZ - fGroundLevel ) <= 1.0f );
}


bool CClientPed::IsClimbing ( void )
{
    if ( m_pPlayerPed )
    {
        CTask * pPrimaryTask = m_pTaskManager->GetSimplestActiveTask ();
        if ( pPrimaryTask && pPrimaryTask->GetTaskType() == TASK_SIMPLE_CLIMB )
        {
            return true;
        }
    }
    return false;
}


void CClientPed::NextRadioChannel ( void )
{
    // Is our radio on?
    if ( m_bRadioOn )
    {
        SetCurrentRadioChannel ( ( m_ucRadioChannel + 1 ) % 13 );
    }
}


void CClientPed::PreviousRadioChannel ( void )
{
    // Is our radio on?
    if ( m_bRadioOn )
    {
        if ( m_ucRadioChannel == 0 )
        {
            m_ucRadioChannel = 13;
        }

        SetCurrentRadioChannel ( m_ucRadioChannel - 1 );
    }
}


bool CClientPed::SetCurrentRadioChannel ( unsigned char ucChannel )
{
    // Local player?
    if ( m_bIsLocalPlayer && ucChannel >= 0 && ucChannel <= 12 )
    {
        if ( m_ucRadioChannel != ucChannel )
        {
            CLuaArguments Arguments;
            Arguments.PushNumber ( ucChannel );
            if ( !CallEvent ( "onClientPlayerRadioSwitch", Arguments, true ) )
            {
                // if we cancel the radio channel setting at 12 then when they go through previous it will get to 0, then the next time it is used set to 13 in preperation to set to 12 but if it is cancelled it stays at 13.
                // Issue 6113 - Caz
                if ( m_ucRadioChannel == 13 )
                    m_ucRadioChannel = 0;

                return false;
            }
        }

        m_ucRadioChannel = ucChannel;

        g_pGame->GetAudioEngine ()->StartRadio ( m_ucRadioChannel );
        if ( m_ucRadioChannel == 0 )
            g_pGame->GetAudioEngine ()->StopRadio ();

        return true;
    }
    return false;
}


bool CClientPed::GetShotData ( CVector * pvecOrigin, CVector * pvecTarget, CVector * pvecGunMuzzle, CVector * pvecFireOffset, float* fAimX, float* fAimY )
{
    CWeapon* pWeapon = GetWeapon ( GetCurrentWeaponSlot () );
    if ( !pWeapon )
        return false;

    unsigned char ucWeaponType = pWeapon->GetType ();
    CClientVehicle* pVehicle = GetRealOccupiedVehicle ();
    CControllerState Controller;
    GetControllerState ( Controller );
    float fRotation = GetCurrentRotation ();

    // Grab the target range of the current weapon
    float fSkill = GetStat ( g_pGame->GetStats ()->GetSkillStatIndex ( pWeapon->GetType () ) );
    CWeaponStat* pCurrentWeaponInfo = g_pGame->GetWeaponStatManager ( )->GetWeaponStatsFromSkillLevel ( pWeapon->GetType (), fSkill );
    float fRange = pCurrentWeaponInfo->GetWeaponRange ();

    // Grab the gun muzzle position
    CVector vecFireOffset =*pCurrentWeaponInfo->GetFireOffset ();    
    CVector vecGunMuzzle = vecFireOffset;
    GetTransformedBonePosition ( BONE_RIGHTWRIST, vecGunMuzzle );    

    CVector vecOrigin, vecTarget;
    if ( m_bIsLocalPlayer )
    {
        if ( pCurrentWeaponInfo->IsFlagSet ( WEAPONTYPE_FIRSTPERSON ) )
        {
            // Grab the active cam
            CCamera* pCamera = g_pGame->GetCamera ();
            CCam* pActive = pCamera->GetCam ( pCamera->GetActiveCam () );

            // Find the target position
            CVector vecFront = *pActive->GetFront ();
            vecFront.Normalize ();
            vecOrigin = *pActive->GetSource ();
            // Jax: advance along the line 2 units (seems to decrease the chance of corrupt bullet vectors)
            vecOrigin += ( vecFront * 2.0f );
            vecTarget = vecOrigin + ( vecFront * fRange );

            // Apply shoot through walls fix
            vecOrigin = AdjustShotOriginForWalls ( vecOrigin, vecTarget, 2.5f );
        }
        else
        { 
            // Always use the gun muzzle as origin
            vecOrigin = vecGunMuzzle;

            if ( false && HasAkimboPointingUpwards () )             // Upwards pointing akimbo's
            {
                // Disabled temporarily until we actually get working akimbos
                vecTarget = vecOrigin;
                vecTarget.fZ += fRange;
            }
            else if ( Controller.RightShoulder1 == 255 )    // First-person weapons, crosshair active: sync the crosshair
            {
                g_pGame->GetCamera ()->Find3rdPersonCamTargetVector ( fRange, &vecGunMuzzle, &vecOrigin, &vecTarget );
                // Apply shoot through walls fix
                vecOrigin = AdjustShotOriginForWalls ( vecOrigin, vecTarget, 0.5f );
            }
            else if ( pVehicle )                            // Drive-by/vehicle weapons: camera origin as origin, performing collision tests
            {
                CColPoint* pCollision;
                CMatrix mat;
                bool bCollision;

                g_pGame->GetCamera ()->GetMatrix ( &mat );

                CVector vecCameraOrigin = mat.vPos;
                CVector vecTemp = vecCameraOrigin;
                g_pGame->GetCamera ()->Find3rdPersonCamTargetVector ( fRange, &vecCameraOrigin, &vecTemp, &vecTarget );

                bCollision = g_pGame->GetWorld ()->ProcessLineOfSight ( &mat.vPos, &vecTarget, &pCollision, NULL );
                if ( pCollision )
                {
                    if ( bCollision )
                    {
                        CVector vecBullet = *pCollision->GetPosition() - vecOrigin;
                        vecBullet.Normalize();
                        vecTarget = vecOrigin + (vecBullet * fRange);
                    }
                    pCollision->Destroy();
                }
            }
            else
            {
                // For shooting without the crosshair showing (just holding the fire button)
                vecOrigin = vecGunMuzzle;

                float fTemp = 6.283152f - fRotation;
                CVector vecTemp = CVector ( sin ( fTemp ),
                                            cos ( fTemp ),
                                            0.0f );
                vecTarget = CVector ( vecOrigin.fX + ( vecTemp.fX * fRange ),
                                      vecOrigin.fY + ( vecTemp.fY * fRange ),
                                      vecOrigin.fZ );
            }
        }
    }
    else // Always use only the last reported shot data for remote players?
    {
        vecOrigin = m_shotSyncData->m_vecShotOrigin;
        vecTarget = m_shotSyncData->m_vecShotTarget;
    }

    if ( pvecOrigin ) *pvecOrigin = vecOrigin;
    if ( pvecTarget ) *pvecTarget = vecTarget;
    if ( pvecFireOffset ) *pvecFireOffset = vecFireOffset;
    if ( pvecGunMuzzle ) *pvecGunMuzzle = vecGunMuzzle;    
    if ( fAimX ) *fAimX = m_shotSyncData->m_fArmDirectionX;
    if ( fAimY ) *fAimY = m_shotSyncData->m_fArmDirectionY;
    return true;
}


//
// Fix firing through walls by pulling back shot origin when next to a wall
//
CVector CClientPed::AdjustShotOriginForWalls ( const CVector& vecOrigin, const CVector& vecTarget, float fMaxPullBack )
{
    CVector vecResultOrigin = vecOrigin;

    // Do a short line of sight check from the max pullback position
    CVector vecFront = ( vecTarget - vecOrigin );
    vecFront.Normalize ();
    CVector vecTempOrigin = vecOrigin - vecFront * fMaxPullBack;
    CVector vecTempTarget = vecOrigin + vecFront * 1;

    g_pGame->GetWorld ()->IgnoreEntity ( m_pPlayerPed );
    CColPoint* pCollision;   
    bool bCollision = g_pGame->GetWorld ()->ProcessLineOfSight ( &vecTempOrigin, &vecTempTarget, &pCollision, NULL );
    g_pGame->GetWorld ()->IgnoreEntity ( NULL );

    if ( pCollision )
    {
        if ( bCollision )
        {
            float fDist = ( *pCollision->GetPosition() - vecTempOrigin ).Length ();

            if ( fDist < fMaxPullBack )
            {
                // If wall is hit, move origin back to the wall
                float fFrontMul = fDist - fMaxPullBack;
                vecResultOrigin = vecOrigin + ( vecFront * fFrontMul );
            }
        }
        pCollision->Destroy();
    }

    return vecResultOrigin;
}


eFightingStyle CClientPed::GetFightingStyle ( void )
{
    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetFightingStyle ();
    }
    return m_FightingStyle;
}


void CClientPed::SetFightingStyle ( eFightingStyle style )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetFightingStyle ( style, 6 );
    }
    m_FightingStyle = style;
}


eMoveAnim CClientPed::GetMoveAnim ( void )
{
    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetMoveAnim ();
    }
    return m_MoveAnim;
}


void CClientPed::SetMoveAnim ( eMoveAnim iAnim )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetMoveAnim ( iAnim );
    }
    m_MoveAnim = iAnim;
}

unsigned int CClientPed::CountProjectiles ( eWeaponType weaponType )
{
    if ( weaponType == WEAPONTYPE_UNARMED )
        return static_cast < unsigned int > ( m_Projectiles.size () );

    unsigned int uiCount = 0;
    list < CClientProjectile* > ::iterator iter = m_Projectiles.begin ();
    for ( ; iter != m_Projectiles.end () ; iter++ )
    {
        if ( (*iter)->GetWeaponType () == weaponType )
        {
            uiCount++;
        }
    }
    return uiCount;
}


void CClientPed::RemoveAllProjectiles ( void )
{
    CClientProjectile * pProjectile = NULL;
    list < CClientProjectile* > ::iterator iter = m_Projectiles.begin ();
    for ( ; iter != m_Projectiles.end () ; iter++ )
    {
        pProjectile = *iter;
        pProjectile->m_pCreator = NULL;
        g_pClientGame->GetElementDeleter ()->Delete ( pProjectile );        
    }
    m_Projectiles.clear ();
}


void CClientPed::DestroySatchelCharges ( bool bBlow, bool bDestroy )
{
    // Don't allow any recurrance
    if ( m_bDestroyingSatchels ) return;
    m_bDestroyingSatchels = true;

    CClientProjectile * pProjectile = NULL;
    CVector vecPosition;
    list < CClientProjectile* > ::iterator iter = m_Projectiles.begin ();
    while ( iter != m_Projectiles.end () )
    {
        pProjectile = *iter;
        if ( pProjectile->GetWeaponType () == WEAPONTYPE_REMOTE_SATCHEL_CHARGE )
        {            
            if ( bBlow )
            {                
                pProjectile->GetPosition ( vecPosition );

                CLuaArguments Arguments;
                Arguments.PushNumber ( vecPosition.fX );
                Arguments.PushNumber ( vecPosition.fY );
                Arguments.PushNumber ( vecPosition.fZ );
                Arguments.PushNumber ( EXP_TYPE_GRENADE );
                bool bCancelExplosion = !CallEvent ( "onClientExplosion", Arguments, true );
                
                if ( !bCancelExplosion )
                    m_pManager->GetExplosionManager ()->Create ( EXP_TYPE_GRENADE, vecPosition, this, true, -1.0f, false, WEAPONTYPE_REMOTE_SATCHEL_CHARGE );
            }
            if ( bDestroy )
            {
                // Destroy the projectile
                pProjectile->Destroy ();
                iter = m_Projectiles.erase ( iter );
                continue;
            } 
        }
        iter++;
    }
    m_bDestroyingSatchels = false;
}


bool CClientPed::IsEnteringVehicle ( void )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = GetCurrentPrimaryTask ();
        if ( pTask )
        {
            switch ( pTask->GetTaskType () )
            {
                case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
                case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
                {
                    return true;
                    break;
                }
                default: break;
            }
        }
    }
    return false;
}


bool CClientPed::IsLeavingVehicle ( void )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = GetCurrentPrimaryTask ();
        if ( pTask )
        {
            switch ( pTask->GetTaskType () )
            {
                case TASK_COMPLEX_LEAVE_CAR: // We only use this task
                case TASK_COMPLEX_LEAVE_CAR_AND_DIE:
                case TASK_COMPLEX_LEAVE_CAR_AND_FLEE:
                case TASK_COMPLEX_LEAVE_CAR_AND_WANDER:
                case TASK_COMPLEX_SCREAM_IN_CAR_THEN_LEAVE:
                {
                    return true;
                    break;
                }
                default: break;
            }
        }
    }

    return false;
}


bool CClientPed::IsGettingIntoVehicle ( void )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = GetCurrentPrimaryTask ();
        if ( pTask )
        {
            if ( pTask->GetTaskType () == TASK_COMPLEX_ENTER_CAR_AS_DRIVER ||
                 pTask->GetTaskType () == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER )
            {
                CTask * pSubTask = pTask->GetSubTask ();
                if ( pSubTask )
                {
                    switch ( pSubTask->GetTaskType () )
                    {
                        case TASK_SIMPLE_CAR_GET_IN:
                        case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE:
                        case TASK_SIMPLE_CAR_SHUFFLE:
                        case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
                        {
                            return true;
                            break;
                        }
                        default: break;
                    }
                }
            }
        }
    }
    return false;
}


bool CClientPed::IsGettingOutOfVehicle ( void )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = GetCurrentPrimaryTask ();
        if ( pTask )
        {
            if ( pTask->GetTaskType () == TASK_COMPLEX_LEAVE_CAR )
            {
                CTask * pSubTask = pTask->GetSubTask ();
                if ( pSubTask )
                {
                    switch ( pSubTask->GetTaskType () )
                    {
                        case TASK_SIMPLE_CAR_GET_OUT:
                        case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE:
                        {
                            return true;
                            break;
                        }
                        default: break;
                    }
                }
            }
        }
    }
    return false;
}


bool CClientPed::IsGettingJacked ( void )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = GetCurrentPrimaryTask ();
        if ( pTask )
        {
            switch ( pTask->GetTaskType () )
            {
                case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP:
                case TASK_SIMPLE_BIKE_JACKED:
                {
                    return true;
                    break;
                }
                default: break;
            }
        }
    }
    return false;
}


CClientEntity* CClientPed::GetContactEntity ( void )
{
    CClientEntity* pReturn = NULL;
    if ( m_pPlayerPed )
    {
        CEntity* pEntity = m_pPlayerPed->GetContactEntity ();
        if ( pEntity )
        {
            switch ( pEntity->GetEntityType () )
            {
                case ENTITY_TYPE_VEHICLE:
                    pReturn = m_pManager->GetVehicleManager ()->Get ( dynamic_cast < CVehicle * > ( pEntity ), false );
                    break;
                case ENTITY_TYPE_OBJECT:
                    pReturn = m_pManager->GetObjectManager ()->Get ( dynamic_cast < CObject * > ( pEntity ), false );
                    break;
                default:
                    break;
            }
        }
    }
    return pReturn;
}


bool CClientPed::HasAkimboPointingUpwards ( void )
{
    if ( m_bIsLocalPlayer )
    {
        if ( !GetRealOccupiedVehicle () )
        {
            CControllerState csController;
            GetControllerState ( csController );
            if ( csController.RightShoulder1 )
            {
                CWeapon* pWeapon = GetWeapon ( GetCurrentWeaponSlot () );
                if ( pWeapon )
                {
                    unsigned char ucWeaponType = pWeapon->GetType ();
                    if ( ucWeaponType == 22 || ucWeaponType == 26 ||
                        ucWeaponType == 28 || ucWeaponType == 32 )
                    {
                        if ( !IsDucked () && pWeapon->GetState() != WEAPONSTATE_RELOADING )
                        {
                            CTask* pTask = m_pTaskManager->GetTaskSecondary ( TASK_SECONDARY_IK );
                            if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_IK_MANAGER )
                            {
                                return false;
                            }
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}


float CClientPed::GetDistanceFromCentreOfMassToBaseOfModel ( void )
{
    if ( m_pPlayerPed )
    {
        return m_pPlayerPed->GetDistanceFromCentreOfMassToBaseOfModel ();
    }
   return 0.0f;;
}


void CClientPed::SetAlpha ( unsigned char ucAlpha )
{
    /* Handled in ::StreamedInPulse
    if ( m_pPlayerPed )
    {               
        RpClump * pClump = m_pPlayerPed->GetRpClump ();
        if ( pClump ) g_pGame->GetVisibilityPlugins ()->SetClumpAlpha ( pClump, ucAlpha );
    }
    */
    m_ucAlpha = ucAlpha;
}


void CClientPed::Respawn ( CVector * pvecPosition, bool bRestoreState, bool bCameraCut )
{
    // We must not call CPed::Respawn for remote players
    if ( m_bIsLocalPlayer )
    {
        SetFrozenWaitingForGroundToLoad ( true );
        if ( m_pPlayerPed )
        {
            // Detach our attached entities
            if ( !m_AttachedEntities.empty () )
            {
                list < CClientEntity* > ::iterator iter = m_AttachedEntities.begin ();
                for ( ; iter != m_AttachedEntities.end () ; iter++ )
                {
                    CClientEntity* pEntity = *iter;
                    pEntity->InternalAttachTo ( NULL );
                }
            }
            CVector vecPosition;
            if ( !pvecPosition )
            {
                GetPosition ( vecPosition );
                pvecPosition = &vecPosition;
            }

            // Jax: save some info incase we want to restore the old state
            CVector vecMoveSpeed;
            GetMoveSpeed ( vecMoveSpeed );
            float fHealth = GetHealth ();
            float fArmor = GetArmor ();
            eWeaponSlot weaponSlot = GetCurrentWeaponSlot ();
            float fCurrentRotation = GetCurrentRotation ();
            float fTargetRotation = m_pPlayerPed->GetTargetRotation ();
            unsigned char ucInterior = GetInterior ();
            unsigned char ucCameraInterior = static_cast < unsigned char > ( g_pGame->GetWorld ()->GetCurrentArea () );

            // Don't allow any camera movement if we're in fixed mode
            if ( m_pManager->GetCamera ()->IsInFixedMode () ) bCameraCut = false;

            m_pPlayerPed->Respawn ( pvecPosition, bCameraCut );
            SetPosition ( *pvecPosition );

            if ( bRestoreState )
            {
                // Jax: restore all the things we saved
                SetHealth ( fHealth );
                SetArmor ( fArmor );
                SetCurrentWeaponSlot ( weaponSlot );
                SetCurrentRotation ( fCurrentRotation );
                m_pPlayerPed->SetTargetRotation ( fTargetRotation );
                SetMoveSpeed ( vecMoveSpeed );
                SetHasJetPack ( m_bHasJetPack );
                SetInterior ( ucInterior );
            }        
            // Restore the camera's interior whether we're restoring player states or not
            g_pGame->GetWorld ()->SetCurrentArea ( ucCameraInterior );

            // Reattach our attached entities
            if ( !m_AttachedEntities.empty () )
            {
                list < CClientEntity* > ::iterator iter = m_AttachedEntities.begin ();
                for ( ; iter != m_AttachedEntities.end () ; iter++ )
                {
                    CClientEntity* pEntity = *iter;
                    pEntity->InternalAttachTo ( this );
                }
            }
        }
    }
}


const char* CClientPed::GetBodyPartName ( unsigned char ucID )
{
    if ( ucID <= 10 )
    {
        return BodyPartNames [ucID].szName;
    }

    return "Unknown";
}


void CClientPed::GetTargetPosition ( CVector & vecPosition )
{
    vecPosition = m_interp.pos.vecTarget;
    if ( m_interp.pTargetOriginSource )
    {
        CVector vecTemp;
        m_interp.pTargetOriginSource->GetPosition ( vecTemp );
        vecPosition += vecTemp;
    }
}


void CClientPed::SetTargetPosition ( const CVector& vecPosition, unsigned long ulDelay, CClientEntity* pTargetOriginSource )
{

    UpdateTargetPosition ();

    // Get the origin of the position if we are in contact with anything
    CVector vecOrigin;
    if ( pTargetOriginSource )
        pTargetOriginSource->GetPosition ( vecOrigin );

    // Update the references to the contact entity
    if ( pTargetOriginSource != m_interp.pTargetOriginSource )
    {
        if ( m_interp.pTargetOriginSource )
            m_interp.pTargetOriginSource->RemoveOriginSourceUser ( this );
        if ( pTargetOriginSource )
            pTargetOriginSource->AddOriginSourceUser ( this );
        m_interp.pTargetOriginSource = pTargetOriginSource;
    }

    if ( m_pPlayerPed )
    {
        // The ped is streamed in
        CVector vecCurrentPosition;
        GetPosition ( vecCurrentPosition );
        vecCurrentPosition -= vecOrigin;     

        m_interp.pos.vecTarget = vecPosition;
        m_interp.vecOriginSourceLastPosition = vecOrigin;
        m_interp.bHadOriginSource = false;

        // Calculate the relative error
        m_interp.pos.vecError = vecPosition - vecCurrentPosition;

        // Get the interpolation interval
        unsigned long ulTime = CClientTime::GetTime ();
        m_interp.pos.ulStartTime = ulTime;
        m_interp.pos.ulFinishTime = ulTime + ulDelay;

        // Initialize the interpolation
        m_interp.pos.fLastAlpha = 0.0f;
    }
    else
    {
        // Set the position straight
        SetPosition ( vecPosition + vecOrigin );
    }
}


void CClientPed::RemoveTargetPosition ( void )
{
    m_interp.pos.ulFinishTime = 0;
    if ( m_interp.pTargetOriginSource )
    {
        m_interp.pTargetOriginSource->RemoveOriginSourceUser ( this );
        m_interp.pTargetOriginSource = NULL;
    }
}


void CClientPed::UpdateTargetPosition ( void )
{
    if ( HasTargetPosition () )
    {
        unsigned long ulCurrentTime = CClientTime::GetTime ();

        // Get the origin position if there is any contact
        CVector vecOrigin;
        if ( m_interp.pTargetOriginSource )
            m_interp.pTargetOriginSource->GetPosition ( vecOrigin );
        else if ( m_interp.bHadOriginSource )
            vecOrigin = m_interp.vecOriginSourceLastPosition;

        // Grab our currrent position
        CVector vecCurrentPosition;
        GetPosition ( vecCurrentPosition );   
        vecCurrentPosition -= vecOrigin;

        // Get the factor of time spent from the interpolation start
        // to the current time.
        float fAlpha = SharedUtil::Unlerp ( m_interp.pos.ulStartTime,
                                            ulCurrentTime,
                                            m_interp.pos.ulFinishTime );

        // Don't let it overcompensate the error
        fAlpha = SharedUtil::Clamp ( 0.0f, fAlpha, 1.0f );

        // Get the current error portion to compensate
        float fCurrentAlpha = fAlpha - m_interp.pos.fLastAlpha;
        m_interp.pos.fLastAlpha = fAlpha;

        // Apply the error compensation
        CVector vecCompensation = SharedUtil::Lerp ( CVector (), fCurrentAlpha, m_interp.pos.vecError );

        // If we finished compensating the error, finish it for the next pulse
        if ( fAlpha == 1.0f )
        {
            m_interp.pos.ulFinishTime = 0;
        }

        CVector vecNewPosition = vecCurrentPosition + vecCompensation;

        // Check if the distance to interpolate is too far.
        CVector vecVelocity;
        GetMoveSpeed ( vecVelocity );
        float fThreshold = ( PED_INTERPOLATION_WARP_THRESHOLD + PED_INTERPOLATION_WARP_THRESHOLD_FOR_SPEED * vecVelocity.Length () ) * g_pGame->GetGameSpeed () * TICK_RATE / 100;

        // There is a reason to have this condition this way: To prevent NaNs generating new NaNs after interpolating (Comparing with NaNs always results to false).
        if ( ! ( ( vecCurrentPosition - m_interp.pos.vecTarget ).Length () <= fThreshold ) )
        {
            // Abort all interpolation
            m_interp.pos.ulFinishTime = 0;
            vecNewPosition = m_interp.pos.vecTarget;
        }

        SetPosition ( vecNewPosition + vecOrigin, false );
    }
}


CClientEntity* CClientPed::GetTargetedEntity ( void )
{
    CClientEntity* pReturn = NULL;
    if ( m_pPlayerPed )
    {
        CEntity* pEntity = m_pPlayerPed->GetTargetedEntity ();
        if ( pEntity )
        {
            switch ( pEntity->GetEntityType () )
            {
                case ENTITY_TYPE_PED:
                    pReturn = m_pManager->GetPedManager ()->Get ( dynamic_cast < CPlayerPed * > ( pEntity ), true, false );
                    break;
                case ENTITY_TYPE_VEHICLE:
                    pReturn = m_pManager->GetVehicleManager ()->Get ( dynamic_cast < CVehicle * > ( pEntity ), false );
                    break;
                case ENTITY_TYPE_OBJECT:
                    pReturn = m_pManager->GetObjectManager ()->Get ( dynamic_cast < CObject * > ( pEntity ), false );
                    break;
                default:
                    break;
            }
        }
    }
    return pReturn;
}


CClientPed * CClientPed::GetTargetedPed ( void )
{
    CClientEntity * pTargetEntity = GetTargetedEntity ();
    if ( pTargetEntity && IS_PED ( pTargetEntity ) )
    {
        return static_cast < CClientPed * > ( pTargetEntity );
    }
    return NULL;
}


void CClientPed::NotifyCreate ( void )
{
    m_pManager->GetPedManager ()->OnCreation ( this );
    CClientStreamElement::NotifyCreate ();
}


void CClientPed::NotifyDestroy ( void )
{
    m_pManager->GetPedManager ()->OnDestruction ( this );
    UpdateKeysync ( true );
}


bool CClientPed::IsSunbathing ( void )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
        if ( pTask && pTask->GetTaskType () == TASK_COMPLEX_SUNBATHE )
        {
            return true;
        }
    }
    return m_bSunbathing;
}


void CClientPed::SetSunbathing ( bool bSunbathing, bool bStartStanding )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
        if ( pTask && pTask->GetTaskType () == TASK_COMPLEX_SUNBATHE )
        {
            if ( !bSunbathing )
            {
                CTaskComplexSunbathe * pSunbatheTask = dynamic_cast < CTaskComplexSunbathe * > ( pTask );
                CTask * pNewTask = pSunbatheTask->CreateNextSubTask ( m_pPlayerPed );
                if ( pNewTask )
                {
                    pSunbatheTask->SetSubTask ( pNewTask );
                }
            }
        }
        else
        {
            if ( bSunbathing )
            {
                CTaskComplexSunbathe* pTask = g_pGame->GetTasks ()->CreateTaskComplexSunbathe ( NULL, bStartStanding );
                if ( pTask )
                {
                    pTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PRIMARY );
                }
            }
        }
    }
    m_bSunbathing = bSunbathing;
}


bool CClientPed::LookAt ( CVector vecOffset, int iTime, int iBlend, CClientEntity * pEntity )
{   
    if ( m_pPlayerPed )
    {          
        CEntity * pGameEntity = NULL;
        if ( pEntity ) pGameEntity = pEntity->GetGameEntity ();
        CTaskSimpleTriggerLookAt * pTask = g_pGame->GetTasks ()->CreateTaskSimpleTriggerLookAt ( pGameEntity, iTime, 0, vecOffset, false, 0.250000, iBlend );
        if ( pTask )
        {
            pTask->SetAsSecondaryPedTask ( m_pPlayerPed, TASK_SECONDARY_PARTIAL_ANIM );

            return true;
        }
    }
    return false;
}


bool CClientPed::UseGun ( CVector vecTarget, CClientEntity * pEntity )
{   
    if ( m_pPlayerPed )
    {          
        CEntity * pGameEntity = NULL;
        if ( pEntity ) pGameEntity = pEntity->GetGameEntity ();
        CTaskSimpleUseGun * pTask = g_pGame->GetTasks ()->CreateTaskSimpleUseGun ( pGameEntity, vecTarget, 0, 1, false );
        if ( pTask )
        {
            pTask->SetAsSecondaryPedTask ( m_pPlayerPed, TASK_SECONDARY_PARTIAL_ANIM );

            return true;
        }
    }
    return false;
}


bool CClientPed::IsAttachToable ( void )
{
    // We're not attachable if we're inside a vehicle (that would get messy)
    if ( !GetOccupiedVehicle () )
    {
        return true;
    }
    return false;
}


bool CClientPed::IsDoingGangDriveby ( void )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
        if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_GANG_DRIVEBY )
        {
            return true;
        }
    }
    return m_bDoingGangDriveby;
}


void CClientPed::SetDoingGangDriveby ( bool bDriveby )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
        if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_GANG_DRIVEBY )
        {
            if ( !bDriveby )
            {
                pTask->MakeAbortable ( m_pPlayerPed, ABORT_PRIORITY_URGENT, NULL );
            }
        }
        else if ( bDriveby )
        {
            CTask * pTask = g_pGame->GetTasks ()->CreateTaskSimpleGangDriveBy ( NULL, NULL, 0.0f, 0, 0, false );
            if ( pTask )
            {
                pTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PRIMARY );
            }
        }
    }
    m_bDoingGangDriveby = bDriveby;
}


bool CClientPed::IsRunningAnimation ( void )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
        if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_NAMED_ANIM )
        {
            return true;
        }
        return false;
    }
    return ( m_bLoopAnimation && m_pAnimationBlock );
}


void CClientPed::RunAnimation ( AssocGroupId animGroup, AnimationId animID )
{
    KillAnimation ();

    if ( m_pPlayerPed )
    {
        CTask * pTask = g_pGame->GetTasks ()->CreateTaskSimpleRunAnim ( animGroup, animID, 4.0f, TASK_SIMPLE_ANIM, "TASK_SIMPLE_ANIM" );
        if ( pTask )
        {
            pTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PRIMARY );
        }
    }
}


void CClientPed::RunNamedAnimation ( CAnimBlock * pBlock, const char * szAnimName, int iTime, bool bLoop, bool bUpdatePosition, bool bInterruptable, bool bFreezeLastFrame, bool bRunInSequence, bool bOffsetPed, bool bHoldLastFrame )
{
    /* lil_Toady: this seems to break things
    // Kill any current animation that might be running
    KillAnimation ();
    */

    // Are we streamed in?
    if ( m_pPlayerPed )
    {  
        if( !pBlock->IsLoaded() )
        {
            pBlock->Request ( BLOCKING, true );
        }

        if ( pBlock->IsLoaded() )
        {
            int flags = 0x10; // // Stops jaw fucking up, some speaking flag maybe   
            if ( bLoop ) flags |= 0x2; // flag that triggers the loop (Maccer)
            if ( bUpdatePosition ) 
            {
                // 0x40 enables position updating on Y-coord, 0x80 on X. (Maccer)
                flags |= 0x40; 
                flags |= 0x80;
            }
            
            // Kill any higher priority tasks if we dont want this anim interuptable
            if ( !bInterruptable )
            {
                KillTask ( TASK_PRIORITY_PHYSICAL_RESPONSE );
                KillTask ( TASK_PRIORITY_EVENT_RESPONSE_TEMP );
                KillTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
            }
            
            if ( !bFreezeLastFrame ) flags |= 0x08; // flag determines whether to freeze player when anim ends. Really annoying (Maccer)
            CTask * pTask = g_pGame->GetTasks ()->CreateTaskSimpleRunNamedAnim ( szAnimName, pBlock->GetName (), flags, 4.0f, iTime, !bInterruptable, bRunInSequence, bOffsetPed, bHoldLastFrame );
            if ( pTask )
            {
                pTask->SetAsPedTask ( m_pPlayerPed, TASK_PRIORITY_PRIMARY );
            }                
        }
        else
        {
            SString strMessage ( "%s %d (%s)", pBlock->GetName (), pBlock->GetIndex (), szAnimName ); 
            g_pCore->LogEvent ( 543, "Blocking anim load fail", "", strMessage );
            AddReportLog ( 5431, SString ( "Failed to load animation %s", *strMessage ) );           
/*
            // TODO: unload unreferenced blocks later on
            g_pGame->GetStreaming ()->RequestAnimations ( pBlock->GetIndex (), 8 );
            m_bRequestedAnimation = true;
*/
        }
    }
    m_pAnimationBlock = pBlock;
    m_strAnimationName = szAnimName; 
    m_iTimeAnimation = iTime;
    m_bLoopAnimation = bLoop;
    m_bUpdatePositionAnimation = bUpdatePosition;
    m_bInterruptableAnimation = bInterruptable;
    m_bFreezeLastFrameAnimation = bFreezeLastFrame;
}


void CClientPed::KillAnimation ( void )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = m_pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
        if ( pTask )
        {
            int iTaskType = pTask->GetTaskType ();
            if ( iTaskType == TASK_SIMPLE_NAMED_ANIM || iTaskType == TASK_SIMPLE_ANIM )
            {                
                pTask->MakeAbortable ( m_pPlayerPed, ABORT_PRIORITY_IMMEDIATE, NULL );
                pTask->Destroy ();
                m_pTaskManager->RemoveTask ( TASK_PRIORITY_PRIMARY );
            }
        }
    }
    m_pAnimationBlock = NULL;
    m_strAnimationName = "";
    m_bRequestedAnimation = false;
}

void CClientPed::PostWeaponFire ( void )
{
    m_ulLastTimeFired = CClientTime::GetTime ();
}

void CClientPed::SetBulletImpactData ( CClientEntity* pEntity, const CVector& vecHitPosition )
{
    m_bBulletImpactData = true;
    m_pBulletImpactEntity = pEntity;
    m_vecBulletImpactHit = vecHitPosition;
}

bool CClientPed::GetBulletImpactData ( CClientEntity** ppEntity, CVector* pvecHitPosition )
{
    if ( m_bBulletImpactData )
    {
        if ( ppEntity )
            *ppEntity = m_pBulletImpactEntity;
        if ( pvecHitPosition )
            *pvecHitPosition = m_vecBulletImpactHit;
        return true;
    }
    else
        return false;
}

bool CClientPed::IsUsingGun ( void )
{
    if ( m_pPlayerPed )
    {
        CTask* pTask = m_pTaskManager->GetTaskSecondary ( TASK_SECONDARY_ATTACK );
        if ( pTask && pTask->GetTaskType () == TASK_SIMPLE_USE_GUN )
        {
            return true;
        }
    }
    return false;
}


void CClientPed::SetHeadless ( bool bHeadless )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->RemoveBodyPart ( ( bHeadless ) ? 2 : 1, 0 );
    }
    m_bHeadless = bHeadless;
}


void CClientPed::SetFootBloodEnabled ( bool bHasFootBlood )
{
    if ( m_pPlayerPed )
    {
        if ( bHasFootBlood )
        {
            m_pPlayerPed->SetFootBlood( -1 );
        }
        else
        {
            m_pPlayerPed->SetFootBlood( 0 );
        }
    }
}

bool CClientPed::IsFootBloodEnabled ( void )
{
    if ( m_pPlayerPed )
    {
        return ( m_pPlayerPed->GetFootBlood() > 0 );
    }
    return false;
}


void CClientPed::SetOnFire ( bool bIsOnFire )
{
    if ( m_pPlayerPed )
    {
        m_pPlayerPed->SetOnFire ( bIsOnFire );
    }
    m_bIsOnFire = bIsOnFire;
}

void CClientPed::GetVoice ( short* psVoiceType, short* psVoiceID )
{
    if ( m_pPlayerPed )
        m_pPlayerPed->GetVoice ( psVoiceType, psVoiceID );
}

void CClientPed::GetVoice ( const char** pszVoiceType, const char** pszVoice )
{
    if ( m_pPlayerPed )
        m_pPlayerPed->GetVoice ( pszVoiceType, pszVoice );
}

void CClientPed::SetVoice ( short sVoiceType, short sVoiceID )
{
    if ( m_pPlayerPed )
        m_pPlayerPed->SetVoice ( sVoiceType, sVoiceID );
}

void CClientPed::SetVoice ( const char* szVoiceType, const char* szVoice )
{
    if ( m_pPlayerPed )
        m_pPlayerPed->SetVoice ( szVoiceType, szVoice );
}


bool CClientPed::IsSpeechEnabled ( void )
{
    if ( m_pPlayerPed )
    {
        return !m_pPlayerPed->GetPedSound ()->IsSpeechDisabled ();
    }
    return m_bSpeechEnabled;
}


void CClientPed::SetSpeechEnabled ( bool bEnabled )
{
    if ( m_pPlayerPed )
    {
        if ( bEnabled ) m_pPlayerPed->GetPedSound ()->EnablePedSpeech ();
        else m_pPlayerPed->GetPedSound ()->DisablePedSpeech ( true );
    }
    m_bSpeechEnabled = bEnabled;
}


bool CClientPed::CanReloadWeapon ( void )
{
    unsigned long ulNow = CClientTime::GetTime ();
    CControllerState Current;
    GetControllerState ( Current );
    int iWeaponType = GetWeapon()->GetType();
    //Hes not Aiming, ducked or if he is ducked he is not currently moving and he hasn't moved while crouching in the last 300ms (sometimes the crouching move anim runs over and kills the reload animation)
    if ( Current.RightShoulder1 == false && 
        ( !IsDucked() || ( Current.LeftStickX == 0 && Current.LeftStickY == 0 ) )
        && ulNow - m_ulLastTimeMovedWhileCrouched > 300) 
    {
        //Ignore certain weapons (anything without clip ammo)
        if ( iWeaponType >= WEAPONTYPE_PISTOL && iWeaponType <= WEAPONTYPE_TEC9 && iWeaponType != WEAPONTYPE_SHOTGUN ) {
            return true;
        }
    }
    return false;
}


bool CClientPed::ReloadWeapon ( void )
{
    if ( m_pTaskManager )
    {
        CWeapon * pWeapon = GetWeapon ();
        CTask * pTask = m_pTaskManager->GetTaskSecondary ( TASK_SECONDARY_ATTACK );
        
        // Check his control states for anything that can cancel the anim instantly and make sure he is not firing
        if ( CanReloadWeapon () && 
           ( !pTask || ( pTask && pTask->GetTaskType() != TASK_SIMPLE_USE_GUN ) ) )
        {
            // Play anim + reload
            pWeapon->SetState ( WEAPONSTATE_RELOADING );

            return true;
        }
    }
    return false;
}


bool CClientPed::ShouldBeStealthAiming ( void )
{
    if ( m_pPlayerPed )
    {
        // Do we have a knife?
        if ( GetCurrentWeaponType () == WEAPONTYPE_KNIFE )
        {
            // Do we have the aim key pressed?
            CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds();
            if ( pKeyBinds )
            {
                SBindableGTAControl* pAimControl = pKeyBinds->GetBindableFromControl ( "aim_weapon" );
                if ( pAimControl && pAimControl->bState )
                {
                    //We need to be either crouched, walking or standing
                    SBindableGTAControl* pWalkControl = pKeyBinds->GetBindableFromControl ( "walk" );
                    if ( m_pPlayerPed->GetRunState() == 1 || m_pPlayerPed->GetRunState() == 4 || pWalkControl && pWalkControl->bState )
                    {
                        // Do we have a target ped?
                        CClientPed * pTargetPed = GetTargetedPed ();
                        if ( pTargetPed && pTargetPed->GetGamePlayer () )
                        {
                            // Are we close enough to the target?
                            CVector vecPos, vecPos_2;
                            GetPosition ( vecPos );
                            pTargetPed->GetPosition ( vecPos_2 );
                            if ( DistanceBetweenPoints3D ( vecPos, vecPos_2 ) <= STEALTH_KILL_RANGE )
                            {
                                // Grab our current anim
                                CAnimBlendAssociation * pAssoc = GetFirstAnimation ();
                                if ( pAssoc )
                                {
                                    // Our game checks for stealth killing
                                    if ( m_pPlayerPed->GetPedIntelligence ()->TestForStealthKill ( pTargetPed->GetGamePlayer (), false ) )
                                    {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}


void CClientPed::SetStealthAiming ( bool bAiming )
{
    if ( bAiming != m_bStealthAiming )
    {
        // Stop aiming?
        if ( !bAiming )
        {
            // Do we have the aiming animation?
            CAnimBlendAssociation * pAssoc = GetAnimation ( ANIM_ID_STEALTH_AIM );
            if ( pAssoc )
            {
                // Stop our animation
                pAssoc->SetBlendAmount ( -2.0f );
            }
        }
        m_bStealthAiming = bAiming;
    }
}


CAnimBlendAssociation * CClientPed::AddAnimation ( AssocGroupId group, AnimationId id )
{
    if ( m_pPlayerPed )
    {
        return g_pGame->GetAnimManager ()->AddAnimation ( m_pPlayerPed->GetRpClump (), group, id );
    }
    return NULL;
}


CAnimBlendAssociation * CClientPed::BlendAnimation ( AssocGroupId group, AnimationId id, float fBlendDelta )
{
    if ( m_pPlayerPed )
    {
        return g_pGame->GetAnimManager ()->BlendAnimation ( m_pPlayerPed->GetRpClump (), group, id, fBlendDelta );
    }
    return NULL;
}


CAnimBlendAssociation * CClientPed::GetAnimation ( AnimationId id )
{
    if ( m_pPlayerPed )
    {
        return g_pGame->GetAnimManager ()->RpAnimBlendClumpGetAssociation ( m_pPlayerPed->GetRpClump (), id );
    }
    return NULL;
}


CAnimBlendAssociation * CClientPed::GetFirstAnimation ( void )
{
    if ( m_pPlayerPed )
    {
        return g_pGame->GetAnimManager ()->RpAnimBlendClumpGetFirstAssociation ( m_pPlayerPed->GetRpClump () );
    }
    return NULL;
}


CSphere CClientPed::GetWorldBoundingSphere ( void )
{
    CSphere sphere;
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( GetModel () );
    if ( pModelInfo )
    {
        CBoundingBox* pBoundingBox = pModelInfo->GetBoundingBox ();
        if ( pBoundingBox )
        {
            sphere.vecPosition = pBoundingBox->vecBoundOffset;
            sphere.fRadius = pBoundingBox->fRadius;
        }
    }
    sphere.vecPosition += GetStreamPosition ();
    return sphere;
}


// Currently, this should only be called for the local player
void CClientPed::HandleWaitingForGroundToLoad ( void )
{
    // Check if near any MTA objects
    bool bNearObject = false;
    CVector vecPosition;
    GetPosition ( vecPosition );
    CClientEntityResult result;
    GetClientSpatialDatabase ()->SphereQuery ( result, CSphere ( vecPosition + CVector ( 0, 0, -3 ), 5 ) );
    for ( CClientEntityResult::const_iterator it = result.begin () ; it != result.end (); ++it )
    {
        if  ( (*it)->GetType () == CCLIENTOBJECT )
        {
            bNearObject = true;
            break;
        }
    }

    if ( !bNearObject )
    {
        // If not near any MTA objects, then don't bother waiting
        SetFrozenWaitingForGroundToLoad ( false );
        #ifdef ASYNC_LOADING_DEBUG_OUTPUTA
            OutputDebugLine ( "[AsyncLoading]   FreezeUntilCollisionLoaded - Early stop" );
        #endif 
        return;
    }

    // Reset position
    SetPosition ( m_matFrozen.vPos );
    SetMatrix ( m_matFrozen );
    SetMoveSpeed ( CVector () );

    // Load load load
    if ( GetModelInfo () )
        g_pGame->GetStreaming()->LoadAllRequestedModels ( false, "CClientPed::HandleWaitingForGroundToLoad" );

    // Start out with a fairly big radius to check, and shrink it down over time
    float fUseRadius = 50.0f * ( 1.f - Max ( 0.f, m_fObjectsAroundTolerance ) );

    // Gather up some flags
    CClientObjectManager* pObjectManager = g_pClientGame->GetObjectManager ();
    bool bASync         = g_pGame->IsASyncLoadingEnabled ();
    bool bMTAObjLimit   = pObjectManager->IsObjectLimitReached ();
    bool bHasModel      = GetModelInfo () != NULL;
    #ifndef ASYNC_LOADING_DEBUG_OUTPUTA
        bool bMTALoaded = pObjectManager->ObjectsAroundPointLoaded ( vecPosition, fUseRadius, m_usDimension );
    #else
        SString strAround;
        bool bMTALoaded = pObjectManager->ObjectsAroundPointLoaded ( vecPosition, fUseRadius, m_usDimension, &strAround );
    #endif

    #ifdef ASYNC_LOADING_DEBUG_OUTPUTA
        SString status = SString ( "%2.2f,%2.2f,%2.2f  bASync:%d   bHasModel:%d   bMTALoaded:%d   bMTAObjLimit:%d   m_fGroundCheckTolerance:%2.2f   m_fObjectsAroundTolerance:%2.2f  fUseRadius:%2.1f"
                                       ,vecPosition.fX, vecPosition.fY, vecPosition.fZ
                                       ,bASync, bHasModel, bMTALoaded, bMTAObjLimit, m_fGroundCheckTolerance, m_fObjectsAroundTolerance, fUseRadius );
    #endif

    // See if ground is ready
    if ( ( !bHasModel || !bMTALoaded ) && m_fObjectsAroundTolerance < 1.f )
    {
        m_fGroundCheckTolerance = 0.f;
        m_fObjectsAroundTolerance = Min ( 1.f, m_fObjectsAroundTolerance + 0.01f );
        #ifdef ASYNC_LOADING_DEBUG_OUTPUTA
            status += ( "  FreezeUntilCollisionLoaded - wait" );
        #endif
    }
    else
    {
        // Models should be loaded, but sometimes the collision is still not ready
        // Do a ground distance check to make sure.
        // Make the check tolerance larger with each passing frame
        m_fGroundCheckTolerance = Min ( 1.f, m_fGroundCheckTolerance + 0.01f );
        float fDist = GetDistanceFromGround ();
        float fUseDist = fDist * ( 1.f - m_fGroundCheckTolerance );
        if ( fUseDist > -0.2f && fUseDist < 1.5f )
            SetFrozenWaitingForGroundToLoad ( false );

        #ifdef ASYNC_LOADING_DEBUG_OUTPUTA
            status += ( SString ( "  GetDistanceFromGround:  fDist:%2.2f   fUseDist:%2.2f", fDist, fUseDist ) );
        #endif

        // Stop waiting after 3 frames, if the object limit has not been reached. (bASync should always be false here) 
        if ( m_fGroundCheckTolerance > 0.03f && !bMTAObjLimit && !bASync )
            SetFrozenWaitingForGroundToLoad ( false );
    }

    #ifdef ASYNC_LOADING_DEBUG_OUTPUTA
        OutputDebugLine ( SStringX ( "[AsyncLoading] " ) ++ status );
        g_pCore->GetGraphics ()->DrawText ( 10, 220, -1, 1, status );

        std::vector < SString > lineList;
        strAround.Split ( "\n", lineList );
        for ( unsigned int i = 0 ; i < lineList.size () ; i++ )
            g_pCore->GetGraphics ()->DrawText ( 10, 230 + i * 10, -1, 1, lineList[i] );
    #endif
}