/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientVehicle.cpp
*  PURPOSE:     Vehicle entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Oliver Brown <>
*               Kent Simon <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

extern CClientGame* g_pClientGame;


// To hide the ugly "pointer truncation from DWORD* to unsigned long warning
#pragma warning(disable:4311)

// Maximum distance between current position and target position (for interpolation)
// before we disable interpolation and warp to the position instead
#define VEHICLE_INTERPOLATION_WARP_THRESHOLD            15
#define VEHICLE_INTERPOLATION_WARP_THRESHOLD_FOR_SPEED  1.8f

CClientVehicle::CClientVehicle ( CClientManager* pManager, ElementID ID, unsigned short usModel, unsigned char ucVariation, unsigned char ucVariation2 ) : ClassInit ( this ), CClientStreamElement ( pManager->GetVehicleStreamer (), ID )
{
    CClientEntityRefManager::AddEntityRefs ( ENTITY_REF_DEBUG ( this, "CClientVehicle" ), &m_pDriver, &m_pOccupyingDriver, &m_pPreviousLink, &m_pNextLink, &m_pTowedVehicle, &m_pTowedByVehicle, &m_pPickedUpWinchEntity, NULL );

    // Initialize members
    m_pManager = pManager;
    m_pObjectManager = m_pManager->GetObjectManager ();
    m_pVehicleManager = pManager->GetVehicleManager ();
    m_pModelRequester = pManager->GetModelRequestManager ();
    m_usModel = usModel;
    m_eVehicleType = CClientVehicleManager::GetVehicleType ( usModel );
    m_bHasDamageModel = CClientVehicleManager::HasDamageModel ( m_eVehicleType );
    m_pVehicle = NULL;
    m_pUpgrades = new CVehicleUpgrades ( this );
    m_pClump = NULL;
    m_pOriginalHandlingEntry = g_pGame->GetHandlingManager ()->GetOriginalHandlingData ( static_cast < eVehicleTypes > ( usModel ) );
    m_pHandlingEntry = g_pGame->GetHandlingManager ()->CreateHandlingData ();
    m_pHandlingEntry->Assign ( m_pOriginalHandlingEntry );

    SetTypeName ( "vehicle" );

    // Grab the model info and the bounding box
    m_pModelInfo = g_pGame->GetModelInfo ( usModel );
    m_ucMaxPassengers = CClientVehicleManager::GetMaxPassengerCount ( usModel );

    // Set our default properties
    m_pDriver = NULL;
    m_pOccupyingDriver = NULL;
    memset ( &m_pPassengers[0], 0, sizeof ( m_pPassengers ) );
    memset ( &m_pOccupyingPassengers[0], 0, sizeof ( m_pOccupyingPassengers ) );
    m_pPreviousLink = NULL;
    m_pNextLink = NULL;
    m_Matrix.vFront.fY = 1.0f;
    m_Matrix.vUp.fZ = 1.0f;
    m_Matrix.vRight.fX = 1.0f;
    m_MatrixLast = m_Matrix;
    m_dLastRotationTime = 0;
    m_fHealth = DEFAULT_VEHICLE_HEALTH;
    m_fTurretHorizontal = 0.0f;
    m_fTurretVertical = 0.0f;
    m_fGasPedal = 0.0f;
    m_bVisible = true;
    m_bIsCollisionEnabled = true;
    m_bEngineOn = false;
    m_bEngineBroken = false;
    m_bSireneOrAlarmActive = false;
    m_bLandingGearDown = true;
    m_usAdjustablePropertyValue = 0;
    for ( unsigned int i = 0; i < 6; ++i )
    {
        m_bAllowDoorRatioSetting [ i ] = true;
        m_fDoorOpenRatio [ i ] = 0.0f;
        m_doorInterp.fStart [ i ] = 0.0f;
        m_doorInterp.fTarget [ i ] = 0.0f;
        m_doorInterp.ulStartTime [ i ] = 0UL;
        m_doorInterp.ulTargetTime [ i ] = 0UL;
    }
    m_bSwingingDoorsAllowed = false;
    m_bDoorsLocked = false;
    m_bDoorsUndamageable = false;
    m_bCanShootPetrolTank = true;
    m_bCanBeTargettedByHeatSeekingMissiles = true;
    m_bColorSaved = false;
    m_bIsFrozen = false;
    m_bScriptFrozen = false;
    m_bFrozenWaitingForGroundToLoad = false;
    m_fGroundCheckTolerance = 0.f;
    m_fObjectsAroundTolerance = 0.f;
    GetInitialDoorStates ( m_ucDoorStates );
    memset ( &m_ucWheelStates[0], 0, sizeof ( m_ucWheelStates ) );
    memset ( &m_ucPanelStates[0], 0, sizeof ( m_ucPanelStates ) );
    memset ( &m_ucLightStates[0], 0, sizeof ( m_ucLightStates ) );
    m_bCanBeDamaged = true;
    m_bSyncUnoccupiedDamage = false;
    m_bScriptCanBeDamaged = true;
    m_bTyresCanBurst = true;
    m_ucOverrideLights = 0;
    m_pTowedVehicle = NULL;
    m_pTowedByVehicle = NULL;
    m_eWinchType = WINCH_NONE;
    m_pPickedUpWinchEntity = NULL;
    m_ucPaintjob = 3;
    m_fDirtLevel = 0.0f;
    m_bSmokeTrail = false;
    m_bJustBlewUp = false;
    m_ucAlpha = 255;
    m_bAlphaChanged = false;
    m_bBlowNextFrame = false;
    m_bIsOnGround = false;
    m_ulIllegalTowBreakTime = 0;
    m_bBlown = false;
    m_LastSyncedData = new SLastSyncedVehData;
    m_bIsDerailed = false;
    m_bIsDerailable = true;
    m_bTrainDirection = false;
    m_fTrainSpeed = 0.0f;
    m_bTaxiLightOn = false;
    m_vecGravity = CVector ( 0.0f, 0.0f, -1.0f );
    m_HeadLightColor = SColorRGBA ( 255, 255, 255, 255 );
    m_bHeliSearchLightVisible = false;
    m_fHeliRotorSpeed = 0.0f;
    m_bHasCustomHandling = false;
    m_ucVariation = ucVariation;
    m_ucVariation2 = ucVariation2;

#ifdef MTA_DEBUG
    m_pLastSyncer = NULL;
    m_ulLastSyncTime = 0;
    m_szLastSyncType = "none";
#endif

    m_interp.rot.ulFinishTime = 0;
    m_interp.pos.ulFinishTime = 0;
    ResetInterpolation ();

    // Check if we have landing gears
    m_bHasLandingGear = DoCheckHasLandingGear ();
    m_bHasAdjustableProperty = CClientVehicleManager::HasAdjustableProperty ( m_usModel );

    // Add this vehicle to the vehicle list
    m_pVehicleManager->AddToList ( this );
    m_tSirenBeaconInfo.m_bSirenSilent = false;
}


CClientVehicle::~CClientVehicle ( void )
{
    // Unreference us
    m_pManager->UnreferenceEntity ( this );    

    // Unlink any towing attachments
    if ( m_pTowedVehicle )
        m_pTowedVehicle->m_pTowedByVehicle = NULL;
    if ( m_pTowedByVehicle )
        m_pTowedByVehicle->m_pTowedVehicle = NULL;

    AttachTo ( NULL );

    // Remove all our projectiles
    RemoveAllProjectiles ();

    // Destroy the vehicle
    Destroy ();

    // Make sure we haven't requested any model that will make us crash
    // when it's done loading.
    m_pModelRequester->Cancel ( this, false );

    // Unreference us from the driving player model (if any)
    if ( m_pDriver )
    {
        m_pDriver->SetVehicleInOutState ( VEHICLE_INOUT_NONE );
        UnpairPedAndVehicle( m_pDriver, this );
    }

    // And the occupying ones eventually
    if ( m_pOccupyingDriver )
    {
        m_pOccupyingDriver->SetVehicleInOutState ( VEHICLE_INOUT_NONE );
        UnpairPedAndVehicle( m_pOccupyingDriver, this );
    }

    // And the passenger models
    int i;
    for ( i = 0; i < (sizeof(m_pPassengers)/sizeof(CClientPed*)); i++ )
    {
        if ( m_pPassengers [i] )
        {
            m_pPassengers [i]->m_uiOccupiedVehicleSeat = 0;
            m_pPassengers [i]->SetVehicleInOutState ( VEHICLE_INOUT_NONE );
            UnpairPedAndVehicle( m_pPassengers [i], this );
        }
    }

    // Occupying passenger models
    for ( i = 0; i < (sizeof(m_pOccupyingPassengers)/sizeof(CClientPed*)); i++ )
    {
        if ( m_pOccupyingPassengers [i] )
        {
            m_pOccupyingPassengers [i]->m_uiOccupiedVehicleSeat = 0;
            m_pOccupyingPassengers [i]->SetVehicleInOutState ( VEHICLE_INOUT_NONE );
            UnpairPedAndVehicle( m_pOccupyingPassengers [i], this );
        }
    }

    // Remove us from the vehicle list
    Unlink ();

    delete m_pUpgrades;
    delete m_pHandlingEntry;
    CClientEntityRefManager::RemoveEntityRefs ( 0, &m_pDriver, &m_pOccupyingDriver, &m_pPreviousLink, &m_pNextLink, &m_pTowedVehicle, &m_pTowedByVehicle, &m_pPickedUpWinchEntity, NULL );
}


void CClientVehicle::Unlink ( void )
{
    m_pVehicleManager->RemoveFromList ( this );
    m_pVehicleManager->m_Attached.remove ( this );
    ListRemove( m_pVehicleManager->m_StreamedIn, this );
}


void CClientVehicle::GetPosition ( CVector& vecPosition ) const
{
    if ( m_bIsFrozen )
    {
        vecPosition = m_matFrozen.vPos;
    }
    // Is this a trailer being towed?
    else if ( m_pTowedByVehicle )
    {
        // Is it streamed out or not attached properly?
        if ( !m_pVehicle || !m_pVehicle->GetTowedByVehicle () )
        {
            // Grab the position behind the vehicle (should take X/Y rotation into acount)
            m_pTowedByVehicle->GetPosition ( vecPosition );

            CVector vecRotation;
            m_pTowedByVehicle->GetRotationRadians ( vecRotation );
            vecPosition.fX -= ( 5.0f * sin ( vecRotation.fZ ) );
            vecPosition.fY -= ( 5.0f * cos ( vecRotation.fZ ) );
        }
        else
        {
            vecPosition = *m_pVehicle->GetPosition ( );
        }
    }
    // Streamed in?
    else if ( m_pVehicle )
    {
        vecPosition = *m_pVehicle->GetPosition ();
    }
    // Attached to something?
    else if ( m_pAttachedToEntity )
    {
        m_pAttachedToEntity->GetPosition ( vecPosition );
        vecPosition += m_vecAttachedPosition;
    }
    else
    {
        vecPosition = m_Matrix.vPos;
    }
}


void CClientVehicle::SetPosition ( const CVector& vecPosition, bool bResetInterpolation )
{
    // Is the local player in the vehicle
    if ( g_pClientGame->GetLocalPlayer ()->GetOccupiedVehicle () == this )
    {
        // If move is big enough, do ground checks
        float DistanceMoved = ( m_Matrix.vPos - vecPosition ).Length ();
        if ( DistanceMoved > 50 && !IsFrozen () )
            SetFrozenWaitingForGroundToLoad ( true );
    }

    if ( m_pVehicle )
    {
        m_pVehicle->SetPosition ( const_cast < CVector* > ( &vecPosition ) );
        
        // Jax: can someone find a cleaner alternative for this, it fixes vehicles not being affected by gravity (supposed to be a flag used only on creation, but isnt)
        if ( !m_pDriver )
        {
            CVector vecMoveSpeed;
            m_pVehicle->GetMoveSpeed ( &vecMoveSpeed );
            if ( vecMoveSpeed.fX == 0.0f && vecMoveSpeed.fY == 0.0f && vecMoveSpeed.fZ == 0.0f )
            {
                vecMoveSpeed.fZ -= 0.01f;
                m_pVehicle->SetMoveSpeed ( &vecMoveSpeed );
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

    // If we have any occupants, update their positions
    for ( int i = 0; i <= NUMELMS ( m_pPassengers ) ; i++ )
        if ( CClientPed* pOccupant = GetOccupant ( i ) )
            pOccupant->SetPosition ( vecPosition );

    // Reset interpolation
    if ( bResetInterpolation )
        RemoveTargetPosition ();
}


void CClientVehicle::GetRotationDegrees ( CVector& vecRotation ) const
{
    // Grab our rotations in radians
    GetRotationRadians ( vecRotation );
    ConvertRadiansToDegrees ( vecRotation );
}


void CClientVehicle::GetRotationRadians ( CVector& vecRotation ) const
{
    // Grab the rotation in radians from the matrix
    CMatrix matTemp;
    GetMatrix ( matTemp );
    g_pMultiplayer->ConvertMatrixToEulerAngles ( matTemp, vecRotation.fX, vecRotation.fY, vecRotation.fZ );

    // ChrML: We flip the actual rotation direction so that the rotation is consistent with
    //        objects and players.
    vecRotation.fX = ( 2 * PI ) - vecRotation.fX;
    vecRotation.fY = ( 2 * PI ) - vecRotation.fY;
    vecRotation.fZ = ( 2 * PI ) - vecRotation.fZ;
}


void CClientVehicle::SetRotationDegrees ( const CVector& vecRotation, bool bResetInterpolation )
{
    // Convert from degrees to radians
    CVector vecTemp;
    vecTemp.fX = vecRotation.fX * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fY = vecRotation.fY * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fZ = vecRotation.fZ * 3.1415926535897932384626433832795f / 180.0f;

    // Set the rotation as radians
    SetRotationRadians ( vecTemp, bResetInterpolation );
}


void CClientVehicle::SetRotationRadians ( const CVector& vecRotation, bool bResetInterpolation )
{
    // Grab the matrix, apply the rotation to it and set it again
    // ChrML: We flip the actual rotation direction so that the rotation is consistent with
    //        objects and players.
    CMatrix matTemp;
    GetMatrix ( matTemp );
    g_pMultiplayer->ConvertEulerAnglesToMatrix ( matTemp, ( 2 * PI ) - vecRotation.fX, ( 2 * PI ) - vecRotation.fY, ( 2 * PI ) - vecRotation.fZ );
    SetMatrix ( matTemp );

    // Reset target rotatin
    if ( bResetInterpolation )
        RemoveTargetRotation ();
}


void CClientVehicle::ReportMissionAudioEvent ( unsigned short usSound )
{
    if ( m_pVehicle )
    {
//      g_pGame->GetAudio ()->ReportMissionAudioEvent ( m_pVehicle, usSound );
    }
}

bool CClientVehicle::SetTaxiLightOn ( bool bLightOn )
{
    m_bTaxiLightOn = bLightOn;
    if ( m_pVehicle )
    {
        m_pVehicle->SetTaxiLightOn ( bLightOn );
        return true;
    }
    return false;
}

float CClientVehicle::GetDistanceFromCentreOfMassToBaseOfModel ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetDistanceFromCentreOfMassToBaseOfModel ();
    }
    return 0.0f;
}

bool CClientVehicle::GetMatrix ( CMatrix& Matrix ) const
{
    if ( m_bIsFrozen )
    {
        Matrix = m_matFrozen;
    }
    else
    {
        if ( m_pVehicle )
        {
            m_pVehicle->GetMatrix ( &Matrix );
        }
        else
        {
            Matrix = m_Matrix;
        }
    }

    return true;
}


bool CClientVehicle::SetMatrix ( const CMatrix& Matrix )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetMatrix ( const_cast < CMatrix* > ( &Matrix ) );

        // If it is a boat, we need to call FixBoatOrientation or it won't move/rotate properly
        if ( m_eVehicleType == CLIENTVEHICLE_BOAT )
        {
            m_pVehicle->FixBoatOrientation ();
        }
    }

    // Have we moved to a different position?
    if ( m_Matrix.vPos != Matrix.vPos )
    {
        // Update our streaming position
        UpdateStreamPosition ( Matrix.vPos );
    }

    m_Matrix = Matrix;
    m_matFrozen = Matrix;
    m_MatrixPure = Matrix;

    // If we have any occupants, update their positions
    if ( m_pDriver ) m_pDriver->SetPosition ( m_Matrix.vPos );
    for ( int i = 0; i < ( sizeof ( m_pPassengers ) / sizeof ( CClientPed * ) ) ; i++ )
    {
        if ( m_pPassengers [ i ] )
        {
            m_pPassengers [ i ]->SetPosition ( m_Matrix.vPos );
        }
    }

    return true;
}


void CClientVehicle::GetMoveSpeed ( CVector& vecMoveSpeed ) const
{
    if ( m_bIsFrozen )
    {
        vecMoveSpeed = CVector ( 0, 0, 0 );
    }
    else
    {
        if ( m_pVehicle )
        {
            m_pVehicle->GetMoveSpeed ( &vecMoveSpeed );
        }
        else
        {
            vecMoveSpeed = m_vecMoveSpeed;
        }
    }
}


void CClientVehicle::GetMoveSpeedMeters ( CVector& vecMoveSpeed ) const
{
    if ( m_bIsFrozen )
    {
        vecMoveSpeed = CVector ( 0, 0, 0 );     
    }
    else
    {
        vecMoveSpeed = m_vecMoveSpeedMeters;
    }
}


void CClientVehicle::SetMoveSpeed ( const CVector& vecMoveSpeed )
{
    if ( !m_bIsFrozen )
    {
        if ( m_pVehicle )
        {
            m_pVehicle->SetMoveSpeed ( const_cast < CVector* > ( &vecMoveSpeed ) );
        }
        m_vecMoveSpeed = vecMoveSpeed;
    }
}


void CClientVehicle::GetTurnSpeed ( CVector& vecTurnSpeed ) const
{
    if ( m_bIsFrozen )
    {
        vecTurnSpeed = CVector ( 0, 0, 0 );
    }
    if ( m_pVehicle )
    {
        m_pVehicle->GetTurnSpeed ( &vecTurnSpeed );
    }
    else
    {
        vecTurnSpeed = m_vecTurnSpeed;
    }
}


void CClientVehicle::SetTurnSpeed ( const CVector& vecTurnSpeed )
{
    if ( !m_bIsFrozen )
    {
        if ( m_pVehicle )
        {
            m_pVehicle->SetTurnSpeed ( const_cast < CVector* > ( &vecTurnSpeed ) );
        }
        m_vecTurnSpeed = vecTurnSpeed;
    }
}


bool CClientVehicle::IsVisible ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->IsVisible ();
    }
    
    return m_bVisible;
}


void CClientVehicle::SetVisible ( bool bVisible )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetVisible ( bVisible );
    }
    m_bVisible = bVisible;
}

void CClientVehicle::SetDoorOpenRatioInterpolated ( unsigned char ucDoor, float fRatio, unsigned long ulDelay )
{
    unsigned long ulTime = CClientTime::GetTime ();
    m_doorInterp.fStart [ ucDoor ] = m_fDoorOpenRatio [ ucDoor ];
    m_doorInterp.fTarget [ ucDoor ] = fRatio;
    m_doorInterp.ulStartTime [ ucDoor ] = ulTime;
    m_doorInterp.ulTargetTime [ ucDoor ] = ulTime + ulDelay;
}

void CClientVehicle::ResetDoorInterpolation ( )
{
    for ( unsigned char i = 0; i < 6; ++i )
    {
        if ( m_doorInterp.ulTargetTime [ i ] != 0 )
            SetDoorOpenRatio ( i, m_doorInterp.fTarget [ i ], 0, true );
        m_doorInterp.ulTargetTime [ i ] = 0;
    }
}

void CClientVehicle::CancelDoorInterpolation ( unsigned char ucDoor )
{
    m_doorInterp.ulTargetTime [ ucDoor ] = 0;
}


void CClientVehicle::ProcessDoorInterpolation ()
{
    unsigned long ulTime = CClientTime::GetTime ();

    for ( unsigned char i = 0; i < 6; ++i )
    {
        if ( m_doorInterp.ulTargetTime [ i ] != 0 )
        {
            if ( m_doorInterp.ulTargetTime [ i ] <= ulTime )
            {
                // Interpolation finished.
                SetDoorOpenRatio ( i, m_doorInterp.fTarget [ i ], 0, true );
                m_doorInterp.ulTargetTime [ i ] = 0;
            }
            else
            {
                unsigned long ulElapsedTime = ulTime - m_doorInterp.ulStartTime [ i ];
                unsigned long ulDelay = m_doorInterp.ulTargetTime [ i ] - m_doorInterp.ulStartTime [ i ];
                float fStep = ulElapsedTime / (float)ulDelay;
                float fRatio = SharedUtil::Lerp ( m_doorInterp.fStart [ i ], fStep, m_doorInterp.fTarget [ i ] );
                SetDoorOpenRatio ( i, fRatio, 0, true );
            }
        }
    }
}

void CClientVehicle::SetDoorOpenRatio ( unsigned char ucDoor, float fRatio, unsigned long ulDelay, bool bForced )
{
    unsigned char ucSeat;

    if ( ucDoor <= 5 )
    {
        bool bAllow = m_bAllowDoorRatioSetting [ ucDoor ];

        // Prevent setting the door angle ratio while a ped is entering/leaving the vehicle.
        if ( bAllow && bForced == false )
        {
            switch ( ucDoor )
            {
                case 2:
                    bAllow = m_pOccupyingDriver == 0;
                    break;
                case 3:
                case 4:
                case 5:
                    ucSeat = ucDoor - 2;
                    bAllow = m_pOccupyingPassengers [ ucSeat ] == 0;
                    break;
            }
        }

        if ( bAllow )
        {
            if ( ulDelay == 0UL )
            {
                if ( m_pVehicle )
                {
                    m_pVehicle->OpenDoor ( ucDoor, fRatio, false );
                }
                m_fDoorOpenRatio [ ucDoor ] = fRatio;
            }
            else
            {
                SetDoorOpenRatioInterpolated ( ucDoor, fRatio, ulDelay );
            }
        }
    }
}

float CClientVehicle::GetDoorOpenRatio ( unsigned char ucDoor )
{
    if ( ucDoor <= 5 )
    {
        if ( m_pVehicle )
        {
            return m_pVehicle->GetDoor ( ucDoor )->GetAngleOpenRatio ();
        }
        return m_fDoorOpenRatio [ ucDoor ];
    }
    return 0.0f;
}

void CClientVehicle::SetSwingingDoorsAllowed ( bool bAllowed )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetSwingingDoorsAllowed ( bAllowed );
    }
    m_bSwingingDoorsAllowed = bAllowed;
}

bool CClientVehicle::AreSwingingDoorsAllowed () const
{
    if ( m_pVehicle )
    {
        return m_pVehicle->AreSwingingDoorsAllowed ();
    }
    return m_bSwingingDoorsAllowed;
}

void CClientVehicle::AllowDoorRatioSetting ( unsigned char ucDoor, bool bAllow )
{
    if ( ucDoor < NUMELMS(m_bAllowDoorRatioSetting) )
    {
        m_bAllowDoorRatioSetting [ucDoor] = bAllow;
        CancelDoorInterpolation ( ucDoor );
    }
}

bool CClientVehicle::AreDoorsLocked ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->AreDoorsLocked ();
    }
    return m_bDoorsLocked;
}


void CClientVehicle::SetDoorsLocked ( bool bLocked )
{
    if ( m_pVehicle )
    {
        m_pVehicle->LockDoors ( bLocked );
    }
    m_bDoorsLocked = bLocked;
}


bool CClientVehicle::AreDoorsUndamageable ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->AreDoorsUndamageable ();
    }
    return m_bDoorsUndamageable;
}


void CClientVehicle::SetDoorsUndamageable ( bool bUndamageable )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetDoorsUndamageable ( bUndamageable );
    }
    m_bDoorsUndamageable = bUndamageable;
}


float CClientVehicle::GetHealth ( void ) const
{
    // If we're blown, return 0
    if ( m_bBlown ) return 0.0f;

    if ( m_pVehicle )
    {      
        return m_pVehicle->GetHealth ();
    }

    return m_fHealth;
}


void CClientVehicle::SetHealth ( float fHealth )
{
    if ( m_pVehicle )
    {
        // Is the car is dead and we want to un-die it?
        if ( fHealth > 0.0f && GetHealth () <= 0.0f )
        {
            Destroy ();
            m_fHealth = fHealth;    // NEEDS to be here!
            Create ();
        }
        else
        {
            m_pVehicle->SetHealth ( fHealth );
        }
    }
    m_fHealth = fHealth;
}

void CClientVehicle::Fix ( void )
{
    m_bBlown = false;
    m_bBlowNextFrame = false;
    if ( m_pVehicle )
    {
        m_pVehicle->Fix ();
        // Make sure its visible, if its supposed to be
        m_pVehicle->SetVisible ( m_bVisible );
    } 

    SetHealth ( DEFAULT_VEHICLE_HEALTH );

    SFixedArray < unsigned char, MAX_DOORS >   ucDoorStates;
    GetInitialDoorStates ( ucDoorStates );
    for ( int i = 0 ; i < MAX_DOORS ; i++ ) SetDoorStatus ( i, ucDoorStates [ i ] );
    for ( int i = 0 ; i < MAX_PANELS ; i++ ) SetPanelStatus ( i, 0 );
    for ( int i = 0 ; i < MAX_LIGHTS ; i++ ) SetLightStatus ( i, 0 );
    for ( int i = 0 ; i < MAX_WHEELS ; i++ ) SetWheelStatus ( i, 0 );    
}


void CClientVehicle::Blow ( bool bAllowMovement )
{
    if ( m_pVehicle )
    {
        // Make sure it can be damaged
        m_pVehicle->SetCanBeDamaged ( true );       

        // Grab our current speeds
        CVector vecMoveSpeed, vecTurnSpeed;
        GetMoveSpeed ( vecMoveSpeed );
        GetTurnSpeed ( vecTurnSpeed );

        // Set the health to 0
        m_pVehicle->SetHealth ( 0.0f );     

        // "Fuck" the car completely, so we don't have weird client-side jumpyness because of differently synced wheel states on clients
        FuckCarCompletely ( true );     

        m_pVehicle->BlowUp ( NULL, 0 );

        // And force the wheel states to "burst"
        SetWheelStatus ( FRONT_LEFT_WHEEL, DT_WHEEL_BURST );
        SetWheelStatus ( FRONT_RIGHT_WHEEL, DT_WHEEL_BURST );
        SetWheelStatus ( REAR_LEFT_WHEEL, DT_WHEEL_BURST );
        SetWheelStatus ( REAR_RIGHT_WHEEL, DT_WHEEL_BURST );

        if ( !bAllowMovement )
        {
            // Make sure it doesn't change speeds (slightly cleaner for syncing)
            SetMoveSpeed ( vecMoveSpeed );
            SetTurnSpeed ( vecTurnSpeed );
        }

        // Restore the old can be damaged state
        CalcAndUpdateCanBeDamagedFlag ();
    }
    m_fHealth = 0.0f;
    m_bBlown = true;
}


CVehicleColor& CClientVehicle::GetColor ( void )
{
    if ( m_pVehicle )
    {
        SColor colors[4];
        m_pVehicle->GetColor ( &colors[0], &colors[1], &colors[2], &colors[3], 0 );
        m_Color.SetRGBColors ( colors[0], colors[1], colors[2], colors[3] );
    }
    return m_Color;
}

void CClientVehicle::SetColor ( const CVehicleColor& color )
{
    m_Color = color;
    if ( m_pVehicle )
    {
        m_pVehicle->SetColor ( m_Color.GetRGBColor ( 0 ), m_Color.GetRGBColor ( 1 ), m_Color.GetRGBColor ( 2 ), m_Color.GetRGBColor ( 3 ), 0 );
    }
    m_bColorSaved = true;
}


void CClientVehicle::GetTurretRotation ( float& fHorizontal, float& fVertical )
{
    if ( m_pVehicle )
    {
        // Car, plane or quad?
        if ( m_eVehicleType == CLIENTVEHICLE_CAR ||
            m_eVehicleType == CLIENTVEHICLE_PLANE ||
            m_eVehicleType == CLIENTVEHICLE_QUADBIKE )
        {
            m_pVehicle->GetTurretRotation ( &fHorizontal, &fVertical );
        }
        else
        {
            fHorizontal = 0;
            fVertical = 0;
        }
    }
    else
    {
        fHorizontal = m_fTurretHorizontal;
        fVertical = m_fTurretVertical;
    }
}


void CClientVehicle::SetTurretRotation ( float fHorizontal, float fVertical )
{
    if ( m_pVehicle )
    {
        // Car, plane or quad?
        if ( m_eVehicleType == CLIENTVEHICLE_CAR ||
            m_eVehicleType == CLIENTVEHICLE_PLANE ||
            m_eVehicleType == CLIENTVEHICLE_QUADBIKE )
        {
            m_pVehicle->SetTurretRotation ( fHorizontal, fVertical );
        }
    }
    m_fTurretHorizontal = fHorizontal;
    m_fTurretVertical = fVertical;
}


void CClientVehicle::SetModelBlocking ( unsigned short usModel, bool bLoadImmediately )
{
    // Different vehicle ID than we have now?
    if ( m_usModel != usModel )
    {
        // Destroy the old vehicle if we have one
        if ( m_pVehicle )
        {
            Destroy ();
        }

        // Get rid of our upgrades, they might be incompatible
        if ( m_pUpgrades )
            m_pUpgrades->RemoveAll ( false );

        // Are we swapping from a vortex or skimmer?
        bool bResetWheelAndDoorStates = ( m_usModel == VT_VORTEX || m_usModel == VT_SKIMMER || ( m_eVehicleType == CLIENTVEHICLE_PLANE && m_eVehicleType != CClientVehicleManager::GetVehicleType ( usModel ) ) );

        // Set the new vehicle id and type
        m_usModel = usModel;
        m_eVehicleType = CClientVehicleManager::GetVehicleType ( usModel );
        m_bHasDamageModel = CClientVehicleManager::HasDamageModel ( m_eVehicleType );

        if ( bResetWheelAndDoorStates )
        {
            GetInitialDoorStates ( m_ucDoorStates );
            memset ( &m_ucWheelStates[0], 0, sizeof ( m_ucWheelStates ) );
        }

        // Check if we have landing gears and adjustable properties
        m_bHasLandingGear = DoCheckHasLandingGear ();
        m_bHasAdjustableProperty = CClientVehicleManager::HasAdjustableProperty ( m_usModel );
        m_usAdjustablePropertyValue = 0;

        // Grab the model info and the bounding box
        m_pModelInfo = g_pGame->GetModelInfo ( usModel );
        m_ucMaxPassengers = CClientVehicleManager::GetMaxPassengerCount ( usModel );

        // Reset handling to fit the vehicle
        m_pOriginalHandlingEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData ( (eVehicleTypes)usModel );
        m_pHandlingEntry->Assign ( m_pOriginalHandlingEntry );
        ApplyHandling ();
        
        SetSirenOrAlarmActive ( false );
        // Create the vehicle if we're streamed in
        if ( IsStreamedIn () )
        {
            // Preload the model
            if( !m_pModelInfo->IsLoaded () )
            {
                m_pModelInfo->Request ( BLOCKING, "CClientVehicle::SetModelBlocking" );
                m_pModelInfo->MakeCustomModel ();
            }

            // Create the vehicle now. Don't prerequest the model ID for this func.
            Create ();
        }
    }
}

void CClientVehicle::SetVariant ( unsigned char ucVariant, unsigned char ucVariant2 )
{
    m_ucVariation = ucVariant;
    m_ucVariation2 = ucVariant2;

    ReCreate ( );
}

bool CClientVehicle::IsEngineBroken ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->IsEngineBroken ();
    }

    return m_bEngineBroken;
}


void CClientVehicle::SetEngineBroken ( bool bEngineBroken )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetEngineBroken ( bEngineBroken );
        m_pVehicle->SetEngineOn ( !bEngineBroken );

        // We need to recreate the vehicle if we're going from broken to unbroken
        if ( !bEngineBroken && m_pVehicle->IsEngineBroken () )
        {
            ReCreate ();
        }
    }
    m_bEngineBroken = bEngineBroken;
}


bool CClientVehicle::IsEngineOn ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->IsEngineOn ();
    }
    
    return m_bEngineOn;
}


void CClientVehicle::SetEngineOn ( bool bEngineOn )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetEngineOn ( bEngineOn );
    }

    m_bEngineOn = bEngineOn;
}


bool CClientVehicle::CanBeDamaged ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetCanBeDamaged ();
    }

    return m_bCanBeDamaged;
}


// This can be called frequently to ensure the correct setting gets to the SA vehicle
void CClientVehicle::CalcAndUpdateCanBeDamagedFlag ( void )
{
     bool bCanBeDamaged = false;

    // CanBeDamaged if local driver or syncing unoccupiedVehicle
    if ( m_pDriver && m_pDriver->IsLocalPlayer () )
        bCanBeDamaged = true;

    if ( m_bSyncUnoccupiedDamage )
        bCanBeDamaged = true;

    // Script override
    if ( !m_bScriptCanBeDamaged )
        bCanBeDamaged = false;

    if ( m_pVehicle )
    {
        m_pVehicle->SetCanBeDamaged ( bCanBeDamaged );
    }

    m_bCanBeDamaged = bCanBeDamaged;
}


void CClientVehicle::SetScriptCanBeDamaged ( bool bCanBeDamaged )
{
    // Needed so script doesn't interfere with syncing unoccupied vehicles
    m_bScriptCanBeDamaged = bCanBeDamaged;
    CalcAndUpdateCanBeDamagedFlag ();
    CalcAndUpdateTyresCanBurstFlag ();
}


void CClientVehicle::SetSyncUnoccupiedDamage ( bool bCanBeDamaged )
{
    m_bSyncUnoccupiedDamage = bCanBeDamaged;
    CalcAndUpdateCanBeDamagedFlag ();
    CalcAndUpdateTyresCanBurstFlag ();
}


bool CClientVehicle::GetTyresCanBurst ( void )
{
    if ( m_pVehicle )
    {
        return !m_pVehicle->GetTyresDontBurst ();
    }

    return m_bTyresCanBurst;
}


// This can be called frequently to ensure the correct setting gets to the SA vehicle
void CClientVehicle::CalcAndUpdateTyresCanBurstFlag ( void )
{
     bool bTyresCanBurst = false;

    // TyresCanBurst if local driver or syncing unoccupiedVehicle
    if ( m_pDriver && m_pDriver->IsLocalPlayer () )
        bTyresCanBurst = true;

    if ( m_bSyncUnoccupiedDamage )
        bTyresCanBurst = true;

    // Script override
    if ( !m_bScriptCanBeDamaged )
        bTyresCanBurst = false;

    if ( m_pVehicle )
    {
        m_pVehicle->SetTyresDontBurst ( !bTyresCanBurst );
    }

    m_bTyresCanBurst = bTyresCanBurst;
}


float CClientVehicle::GetGasPedal ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetGasPedal ();
    }

    return m_fGasPedal;
}


bool CClientVehicle::IsBelowWater ( void ) const
{
    CVector vecPosition;
    GetPosition ( vecPosition );
    float fWaterLevel = 0.0f;

    if ( g_pGame->GetWaterManager ()->GetWaterLevel ( vecPosition, &fWaterLevel, true, NULL ) )
    {
        if ( vecPosition.fZ < fWaterLevel - 0.7 )
        {
            return true; 
        }
    }

    return false;
}


bool CClientVehicle::IsDrowning ( void ) const
{
    if ( m_pVehicle )
    {
        return m_pVehicle->IsDrowning ();
    }

    return false;
}


bool CClientVehicle::IsDriven ( void ) const
{
    if ( m_pVehicle )
    {
        return m_pVehicle->IsBeingDriven () ? true:false;
    }
    else
    {
        return GetOccupant ( 0 ) != NULL;
    }
}


bool CClientVehicle::IsUpsideDown ( void ) const
{
    if ( m_pVehicle )
    {
        return m_pVehicle->IsUpsideDown ();
    }

    // TODO: Figure out this using matrix?
    return false;
}


bool CClientVehicle::IsBlown ( void ) const
{
    // Game layer functions aren't reliable
    return m_bBlown;
}


bool CClientVehicle::IsSirenOrAlarmActive ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->IsSirenOrAlarmActive () ? true:false;
    }

    return m_bSireneOrAlarmActive;
}


void CClientVehicle::SetSirenOrAlarmActive ( bool bActive )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetSirenOrAlarmActive ( bActive );
    }
    m_bSireneOrAlarmActive = bActive;
}


float CClientVehicle::GetLandingGearPosition ( void )
{
    if ( m_bHasLandingGear )
    {
        if ( m_pVehicle )
        {
            return m_pVehicle->GetLandingGearPosition ();
        }
    }

    return 0.0f;
}


void CClientVehicle::SetLandingGearPosition ( float fPosition )
{
    if ( m_bHasLandingGear )
    {
        if ( m_pVehicle )
        {
            m_pVehicle->SetLandingGearPosition ( fPosition );
        }
    }
}


bool CClientVehicle::IsLandingGearDown ( void )
{
    if ( m_bHasLandingGear )
    {
        if ( m_pVehicle )
        {
            return m_pVehicle->IsLandingGearDown ();
        }

        return m_bLandingGearDown;
    }

    return true;
}


void CClientVehicle::SetLandingGearDown ( bool bLandingGearDown )
{
    if ( m_bHasLandingGear )
    {
        if ( m_pVehicle )
        {
            m_pVehicle->SetLandingGearDown ( bLandingGearDown );
        }
        m_bLandingGearDown = bLandingGearDown;
    }
}


unsigned short CClientVehicle::GetAdjustablePropertyValue ( void )
{
    unsigned short usPropertyValue;
    if ( m_pVehicle )
    {
        usPropertyValue = m_pVehicle->GetAdjustablePropertyValue ();
        // If it's a Hydra invert it with 5000 (as 0 is "forward"), so we can maintain a standard of 0 being "normal"
        if ( m_usModel == VT_HYDRA ) usPropertyValue = 5000 - usPropertyValue;
    }
    else
    {
        usPropertyValue = m_usAdjustablePropertyValue;
    }

    // Return it
    return usPropertyValue;
}


void CClientVehicle::SetAdjustablePropertyValue ( unsigned short usValue )
{
    if ( m_usModel == VT_HYDRA ) usValue = 5000 - usValue;

    _SetAdjustablePropertyValue ( usValue );
}


void CClientVehicle::_SetAdjustablePropertyValue ( unsigned short usValue )
{
    // Set it
    if ( m_pVehicle )
    {
        if ( m_bHasAdjustableProperty )
        {
            m_pVehicle->SetAdjustablePropertyValue ( usValue );
            
            // Update our collision for this adjustable?
            if ( HasMovingCollision () )
            {
                float fAngle = ( float ) usValue / 2499.0f;
                m_pVehicle->UpdateMovingCollision ( fAngle );
            }
        }
    }
    m_usAdjustablePropertyValue = usValue;
}


bool CClientVehicle::HasMovingCollision ( void )
{
    return ( m_usModel == VT_FORKLIFT || m_usModel == VT_FIRELA || m_usModel == VT_ANDROM ||
             m_usModel == VT_DUMPER || m_usModel == VT_DOZER || m_usModel == VT_PACKER );
}


unsigned char CClientVehicle::GetDoorStatus ( unsigned char ucDoor )
{
    if ( ucDoor < MAX_DOORS )
    {
        if ( m_pVehicle && HasDamageModel () )
        {
            return m_pVehicle->GetDamageManager ()->GetDoorStatus ( static_cast < eDoors > ( ucDoor ) );
        }

        return m_ucDoorStates [ucDoor];
    }

    return 0;
}


unsigned char CClientVehicle::GetWheelStatus ( unsigned char ucWheel )
{
    if ( ucWheel < MAX_WHEELS )
    {
        // Return our custom state?
        if ( m_ucWheelStates [ucWheel] == DT_WHEEL_INTACT_COLLISIONLESS ) return DT_WHEEL_INTACT_COLLISIONLESS;

        if ( m_pVehicle )
        {
            if ( HasDamageModel () )
                return m_pVehicle->GetDamageManager ()->GetWheelStatus ( static_cast < eWheels > ( ucWheel ) );
            if ( m_eVehicleType == CLIENTVEHICLE_BIKE && ucWheel < 2 )
                return m_pVehicle->GetBikeWheelStatus ( ucWheel );
        }

        return m_ucWheelStates [ucWheel];
    }

    return 0;
}


unsigned char CClientVehicle::GetPanelStatus ( unsigned char ucPanel )
{
    if ( ucPanel < MAX_PANELS )
    {
        if ( m_pVehicle && HasDamageModel () )
        {
            return m_pVehicle->GetDamageManager ()->GetPanelStatus ( ucPanel );
        }

        return m_ucPanelStates [ ucPanel ];
    }

    return 0;
}


unsigned char CClientVehicle::GetLightStatus ( unsigned char ucLight )
{
    if ( ucLight < MAX_LIGHTS )
    {
        if ( m_pVehicle && HasDamageModel () )
        {
            return m_pVehicle->GetDamageManager ()->GetLightStatus ( ucLight );
        }
        return m_ucLightStates [ ucLight ];
    }

    return 0;
}


void CClientVehicle::SetDoorStatus ( unsigned char ucDoor, unsigned char ucStatus )
{
    if ( ucDoor < MAX_DOORS )
    {
        if ( m_pVehicle && HasDamageModel () )
        {
            m_pVehicle->GetDamageManager ()->SetDoorStatus ( static_cast < eDoors > ( ucDoor ), ucStatus );
        }
        m_ucDoorStates [ucDoor] = ucStatus;
    }
}


void CClientVehicle::SetWheelStatus ( unsigned char ucWheel, unsigned char ucStatus, bool bSilent )
{
    if ( ucWheel < MAX_WHEELS )
    {
        if ( m_pVehicle )
        {
            // Is our new status a burst tyre? and do we need to call BurstTyre?
            if ( ucStatus == DT_WHEEL_BURST && !bSilent ) m_pVehicle->BurstTyre ( ucWheel );  

            // Are we using our custom state?
            unsigned char ucGTAStatus = ucStatus;
            if ( ucStatus == DT_WHEEL_INTACT_COLLISIONLESS ) ucGTAStatus = DT_WHEEL_MISSING;

            // Do we have a damage model?
            if ( HasDamageModel () )
            {
                m_pVehicle->GetDamageManager ()->SetWheelStatus ( ( eWheels ) ( ucWheel ), ucGTAStatus );
                
                // Update the wheel's visibility
                m_pVehicle->SetWheelVisibility ( ( eWheels ) ucWheel, ( ucStatus != DT_WHEEL_MISSING ) );
            }
            else if ( m_eVehicleType == CLIENTVEHICLE_BIKE && ucWheel < 2 )
                m_pVehicle->SetBikeWheelStatus ( ucWheel, ucGTAStatus );
        }
        m_ucWheelStates [ucWheel] = ucStatus;
    }
}


void CClientVehicle::SetPanelStatus ( unsigned char ucPanel, unsigned char ucStatus )
{
    if ( ucPanel < MAX_PANELS )
    {
        if ( m_pVehicle && HasDamageModel () )
        {
            m_pVehicle->GetDamageManager ()->SetPanelStatus ( static_cast < ePanels > ( ucPanel ), ucStatus );
        }
        m_ucPanelStates [ ucPanel ] = ucStatus;
    }
}


void CClientVehicle::SetLightStatus ( unsigned char ucLight, unsigned char ucStatus )
{
    if ( ucLight < MAX_LIGHTS )
    {
        if ( m_pVehicle && HasDamageModel () )
        {
            m_pVehicle->GetDamageManager ()->SetLightStatus ( static_cast < eLights > ( ucLight ), ucStatus );
        }
        m_ucLightStates [ ucLight ] = ucStatus;
    }
}


float CClientVehicle::GetHeliRotorSpeed ( void )
{
    if ( m_pVehicle && m_eVehicleType == CLIENTVEHICLE_HELI )
    {
        return m_pVehicle->GetHeliRotorSpeed ();
    }

    return m_fHeliRotorSpeed;
}


void CClientVehicle::SetHeliRotorSpeed ( float fSpeed )
{
    if ( m_pVehicle && m_eVehicleType == CLIENTVEHICLE_HELI )
    {
        m_pVehicle->SetHeliRotorSpeed ( fSpeed );
    }
    m_fHeliRotorSpeed = fSpeed;
}


bool CClientVehicle::IsHeliSearchLightVisible ( void )
{
    if ( m_pVehicle && m_eVehicleType == CLIENTVEHICLE_HELI )
    {
        return m_pVehicle->IsHeliSearchLightVisible ();
    }
    return m_bHeliSearchLightVisible;
}


void CClientVehicle::SetHeliSearchLightVisible ( bool bVisible )
{
    if ( m_pVehicle && m_eVehicleType == CLIENTVEHICLE_HELI )
    {
        m_pVehicle->SetHeliSearchLightVisible ( bVisible );
    }
    m_bHeliSearchLightVisible = bVisible;
}


void CClientVehicle::SetCollisionEnabled ( bool bCollisionEnabled )
{
    if ( m_pVehicle )
    {
        if ( m_bHasAdjustableProperty )
        {
            m_pVehicle->SetUsesCollision ( bCollisionEnabled );
        }
    }

    m_bIsCollisionEnabled = bCollisionEnabled;
}


bool CClientVehicle::GetCanShootPetrolTank ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetCanShootPetrolTank ();
    }

    return m_bCanShootPetrolTank;
}


void CClientVehicle::SetCanShootPetrolTank ( bool bCanShoot )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetCanShootPetrolTank ( bCanShoot );
    }
    m_bCanShootPetrolTank = bCanShoot;
}


bool CClientVehicle::GetCanBeTargettedByHeatSeekingMissiles ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetCanBeTargettedByHeatSeekingMissiles ();
    }

    return m_bCanBeTargettedByHeatSeekingMissiles;
}


void CClientVehicle::SetCanBeTargettedByHeatSeekingMissiles ( bool bEnabled )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetCanBeTargettedByHeatSeekingMissiles ( bEnabled );
    }
    m_bCanBeTargettedByHeatSeekingMissiles = bEnabled;
}


void CClientVehicle::SetAlpha ( unsigned char ucAlpha )
{
    if ( ucAlpha != m_ucAlpha )
    {
        if ( m_pVehicle )
        {
            m_pVehicle->SetAlpha ( ucAlpha );
        }
        m_ucAlpha = ucAlpha;
        m_bAlphaChanged = true;
    }
}


CClientPed* CClientVehicle::GetOccupant ( int iSeat ) const
{
    // Return the driver if the seat is 0
    if ( iSeat == 0 )
    {
        return m_pDriver;
    }
    else if ( iSeat <= (sizeof(m_pPassengers)/sizeof(CClientPed*)) )
    {
        return m_pPassengers [iSeat - 1];
    }

    return NULL;
}


CClientPed* CClientVehicle::GetControllingPlayer ( void )
{
    CClientPed* pControllingPlayer = m_pDriver;

    if ( pControllingPlayer == NULL )
    {
        CClientVehicle* pCurrentVehicle = this;
        CClientVehicle* pTowedByVehicle = m_pTowedByVehicle;
        pControllingPlayer = pCurrentVehicle->GetOccupant ( 0 );
        while ( pTowedByVehicle )
        {
            pCurrentVehicle = pTowedByVehicle;
            pTowedByVehicle = pCurrentVehicle->GetTowedByVehicle ();
            CClientPed* pCurrentDriver = pCurrentVehicle->GetOccupant ( 0 );
            if ( pCurrentDriver )
                pControllingPlayer = pCurrentDriver;
        }
    }

    return pControllingPlayer;
}


void CClientVehicle::ClearForOccupants ( void )
{
    // TODO: Also check passenger seats for players
    // If there are people in the vehicle, remove them from the vehicle
    CClientPed* pPed = GetOccupant ( 0 );
    if ( pPed )
    {
        pPed->RemoveFromVehicle ();
    }
}


void CClientVehicle::PlaceProperlyOnGround ( void )
{
    if ( m_pVehicle )
    {
        // Place it properly at the ground depending on what kind of vehicle it is
        if ( m_eVehicleType == CLIENTVEHICLE_BMX || m_eVehicleType == CLIENTVEHICLE_BIKE )
        {
            m_pVehicle->PlaceBikeOnRoadProperly ();
        }
        else if ( m_eVehicleType != CLIENTVEHICLE_BOAT )
        {
            m_pVehicle->PlaceAutomobileOnRoadProperly ();
        }
    }
}


void CClientVehicle::FuckCarCompletely ( bool bKeepWheels )
{
    if ( m_pVehicle )
    {
        if ( HasDamageModel () )
        {
            m_pVehicle->GetDamageManager()->FuckCarCompletely ( bKeepWheels );
        }
    }
}


unsigned long CClientVehicle::GetMemoryValue ( unsigned long ulOffset )
{
    if ( m_pVehicle )
    {
        return *m_pVehicle->GetMemoryValue ( ulOffset );
    }

    return 0;
}


unsigned long CClientVehicle::GetGameBaseAddress ( void )
{
    if ( m_pVehicle )
    {
        return reinterpret_cast < unsigned long > ( m_pVehicle->GetMemoryValue ( 0 ) );
    }

    return 0;
}


void CClientVehicle::WorldIgnore ( bool bWorldIgnore )
{
    if ( bWorldIgnore )
    {
        if ( m_pVehicle )
        {
            g_pGame->GetWorld ()->IgnoreEntity ( m_pVehicle );
        }
    }
    else
    {
        g_pGame->GetWorld ()->IgnoreEntity ( NULL );
    }
}


void CClientVehicle::FadeOut ( bool bFadeOut )
{
    if ( m_pVehicle )
    {
        m_pVehicle->FadeOut ( bFadeOut );
    }
}


bool CClientVehicle::IsFadingOut ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->IsFadingOut ();
    }

    return false;
}


void CClientVehicle::SetFrozen ( bool bFrozen )
{
    if ( m_bScriptFrozen && bFrozen )
    {
        m_bIsFrozen = bFrozen;
        CVector vecTemp;
        if ( m_pVehicle )
        {
            m_pVehicle->GetMatrix ( &m_matFrozen );
            m_pVehicle->SetMoveSpeed ( &vecTemp );
            m_pVehicle->SetTurnSpeed ( &vecTemp );
        }
        else
        {
            m_matFrozen = m_Matrix;
            m_vecMoveSpeed = vecTemp;
            m_vecTurnSpeed = vecTemp;
        }
    }
    else if ( !m_bScriptFrozen )
    {
        m_bIsFrozen = bFrozen;

        if ( bFrozen )
        {
            CVector vecTemp;
            if ( m_pVehicle )
            {
                m_pVehicle->GetMatrix ( &m_matFrozen );
                m_pVehicle->SetMoveSpeed ( &vecTemp );
                m_pVehicle->SetTurnSpeed ( &vecTemp );
            }
            else
            {
                m_matFrozen = m_Matrix;
                m_vecMoveSpeed = vecTemp;
                m_vecTurnSpeed = vecTemp;
            }
        }
    }
}


bool CClientVehicle::IsFrozenWaitingForGroundToLoad ( void ) const
{
    return m_bFrozenWaitingForGroundToLoad;
}


void CClientVehicle::SetFrozenWaitingForGroundToLoad ( bool bFrozen )
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

            CVector vecTemp;
            if ( m_pVehicle )
            {
                m_pVehicle->GetMatrix ( &m_matFrozen );
                m_pVehicle->SetMoveSpeed ( &vecTemp );
                m_pVehicle->SetTurnSpeed ( &vecTemp );
            }
            else
            {
                m_matFrozen = m_Matrix;
                m_vecMoveSpeed = vecTemp;
                m_vecTurnSpeed = vecTemp;
            }
        }
        else
        {
            g_pGame->SuspendASyncLoading ( false );
        }
    }
}


CClientVehicle* CClientVehicle::GetPreviousTrainCarriage ( void )
{
    return NULL;
}


CClientVehicle* CClientVehicle::GetNextTrainCarriage ( void )
{
    return NULL;
}


void CClientVehicle::SetPreviousTrainCarriage ( CClientVehicle* pPrevious )
{
     // Tell the given vehicle we're the previous link and save the given vehicle as the next link
    m_pPreviousLink = pPrevious;
    if ( pPrevious )
    {
        pPrevious->m_pNextLink = this;
    }

    // If both vehicles are streamed in, do the link
    if ( m_pVehicle && pPrevious->m_pVehicle )
    {
        m_pVehicle->SetPreviousTrainCarriage ( pPrevious->m_pVehicle );
    }
}


void CClientVehicle::SetNextTrainCarriage ( CClientVehicle* pNext )
{
    // Tell the given vehicle we're the previous link and save the given vehicle as the next link
    m_pNextLink = pNext;
    if ( pNext )
    {
        pNext->m_pPreviousLink = this;
    }

    // If both vehicles are streamed in, do the link
    if ( m_pVehicle && pNext->m_pVehicle )
    {
        m_pVehicle->SetNextTrainCarriage ( pNext->m_pVehicle );
    }
}


bool CClientVehicle::IsDerailed ( void )
{
    if ( GetVehicleType() == CLIENTVEHICLE_TRAIN )
    {
        if ( m_pVehicle )
        {
            return m_pVehicle->IsDerailed ();
        }
        return m_bIsDerailed;
    }
    else
        return false;
}


void CClientVehicle::SetDerailed ( bool bDerailed )
{
    if ( GetVehicleType() == CLIENTVEHICLE_TRAIN )
    {
        if ( m_pVehicle && bDerailed != IsDerailed () )
        {
            m_pVehicle->SetDerailed ( bDerailed );
        }
        m_bIsDerailed = bDerailed;
    }
}

bool CClientVehicle::IsDerailable ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->IsDerailable ();
    }
    return m_bIsDerailable;
}

void CClientVehicle::SetDerailable ( bool bDerailable )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetDerailable ( bDerailable );
    }
    m_bIsDerailable = bDerailable;
}


bool CClientVehicle::GetTrainDirection ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetTrainDirection ();
    }
    return m_bTrainDirection;
}


void CClientVehicle::SetTrainDirection ( bool bDirection )
{
    if ( m_pVehicle && GetVehicleType() == CLIENTVEHICLE_TRAIN  )
    {
        m_pVehicle->SetTrainDirection ( bDirection );
    }
    m_bTrainDirection = bDirection;
}


float CClientVehicle::GetTrainSpeed ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetTrainSpeed ();
    }
    return m_fTrainSpeed;
}


void CClientVehicle::SetTrainSpeed ( float fSpeed )
{
    if ( m_pVehicle && GetVehicleType() == CLIENTVEHICLE_TRAIN  )
    {
        m_pVehicle->SetTrainSpeed ( fSpeed );
    }
    m_fTrainSpeed = fSpeed;
}


void CClientVehicle::SetOverrideLights ( unsigned char ucOverrideLights )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetOverrideLights ( static_cast < unsigned int > ( ucOverrideLights ) );
    }
    m_ucOverrideLights = ucOverrideLights;
}


void CClientVehicle::StreamedInPulse ( void )
{
    // Make sure the vehicle doesn't go too far down
    if ( m_pVehicle )
    {
        if ( m_bBlowNextFrame )
        {
            Blow ( false );
            m_bBlowNextFrame = false;
        }
 
        // Are we an unmanned, invisible, blown-up plane?
        if ( !GetOccupant () && m_eVehicleType == CLIENTVEHICLE_PLANE && m_bBlown && !m_pVehicle->IsVisible () )
        {
            // Disable our collisions
            m_pVehicle->SetUsesCollision ( false );
        }
        else
        {
            // Vehicles have a way of getting their cols back, so we have to force this each frame (not much overhead anyway)
            m_pVehicle->SetUsesCollision ( m_bIsCollisionEnabled );
        }

        // Handle waiting for the ground to load
        if ( IsFrozenWaitingForGroundToLoad () )
            HandleWaitingForGroundToLoad ();

        // If we are frozen, make sure we freeze our matrix and keep move/turn speed at 0,0,0
        if ( m_bIsFrozen )
        {
            CVector vecTemp;
            m_pVehicle->SetMatrix ( &m_matFrozen );
            m_pVehicle->SetMoveSpeed ( &vecTemp );
            m_pVehicle->SetTurnSpeed ( &vecTemp );
        }
        else
        {
            // Cols been loaded for where the vehicle is? Only check this if it has no drivers.
            if ( m_pDriver ||
                 ( g_pGame->GetWorld ()->HasCollisionBeenLoaded ( &m_matFrozen.vPos ) /*&&
                   m_pObjectManager->ObjectsAroundPointLoaded ( m_matFrozen.vPos, 200.0f, m_usDimension )*/ ) )
            {
                // Remember the matrix
                m_pVehicle->GetMatrix ( &m_matFrozen );
            }
            else
            {
                // Force the position to the last remembered matrix (..and make sure gravity doesn't pull it down)
                m_pVehicle->SetMatrix ( &m_matFrozen );
                CVector vec(0.0f, 0.0f, 0.0f);
                m_pVehicle->SetMoveSpeed ( &vec );

                // Added by ChrML 27. Nov: Shouldn't cause any problems
                m_pVehicle->SetUsesCollision ( false );
            }
        }

        // Calculate the velocity
        CMatrix MatrixCurrent;
        m_pVehicle->GetMatrix ( &MatrixCurrent );
        m_vecMoveSpeedMeters = ( MatrixCurrent.vPos - m_MatrixLast.vPos ) * g_pGame->GetFPS ();
        // Store the current matrix
        m_MatrixLast = MatrixCurrent;        

        // We dont interpolate attached trailers
        if ( m_pTowedByVehicle )
        {
            RemoveTargetPosition ();
            RemoveTargetRotation ();
        }

        /*
        // Are we blown?
        if ( m_bBlown )
        {
            // Has our engine status been reset to on_fire somewhere?
            CDamageManager* pDamageManager = m_pVehicle->GetDamageManager ();
            if ( pDamageManager->GetEngineStatus () == DT_ENGINE_ON_FIRE )
            {
                // Change it back to fucked
                pDamageManager->SetEngineStatus ( DT_ENGINE_ENGINE_PIPES_BURST );
            }
        }
        */

        // Limit burnout turn speed to ensure smoothness
        if ( m_pDriver )
        {
            CControllerState cs;
            m_pDriver->GetControllerState ( cs );
            bool bAcclerate = cs.ButtonCross > 128;
            bool bBrake = cs.ButtonSquare > 128;

            // Is doing burnout ?
            if ( bAcclerate && bBrake )
            {
                CVector vecMoveSpeed;
                m_pVehicle->GetMoveSpeed ( &vecMoveSpeed );
                if ( fabsf ( vecMoveSpeed.fX ) < 0.06f * 2 && fabsf ( vecMoveSpeed.fY ) < 0.06f * 2 && fabsf ( vecMoveSpeed.fZ ) < 0.01f * 2 )
                {
                    CVector vecTurnSpeed;
                    m_pVehicle->GetTurnSpeed ( &vecTurnSpeed );
                    if ( fabsf ( vecTurnSpeed.fX ) < 0.006f * 2 && fabsf ( vecTurnSpeed.fY ) < 0.006f * 2 && fabsf ( vecTurnSpeed.fZ ) < 0.04f * 2 )
                    {
                        // Apply turn speed limit
                        float fLength = vecTurnSpeed.Normalize ();
                        fLength = Min ( fLength, 0.02f );
                        vecTurnSpeed *= fLength;

                        m_pVehicle->SetTurnSpeed ( &vecTurnSpeed );
                    }
                }
            }
        }

        Interpolate ();
        ProcessDoorInterpolation ();

        // Grab our current position
        CVector vecPosition = *m_pVehicle->GetPosition ();

        if ( m_pAttachedToEntity )
        {
            m_pAttachedToEntity->GetPosition ( vecPosition );
            vecPosition += m_vecAttachedPosition;
        }

        // Have we moved?
        if ( vecPosition != m_Matrix.vPos )
        {
            // If we're setting the position, check whether we're under-water.
            // If so, we need to set the Underwater flag so the render draw order is changed.
            m_pVehicle->SetUnderwater ( IsBelowWater () );

            // Store our new position
            m_Matrix.vPos = vecPosition;
            m_matFrozen.vPos = vecPosition;

            // Update our streaming position
            UpdateStreamPosition ( vecPosition );
        }

        // Update doors
        if ( CClientVehicleManager::HasDoors ( GetModel() ) )
        {
            for ( unsigned char i = 0; i < 6; ++i )
            {
                CDoor* pDoor = m_pVehicle->GetDoor ( i );
                if ( pDoor )
                    m_fDoorOpenRatio [ i ] = pDoor->GetAngleOpenRatio ();
            }
        }
    }
}


void CClientVehicle::StreamIn ( bool bInstantly )
{
    // We need to create now?
    if ( bInstantly )
    {
        // Request its model blocking
        if ( m_pModelRequester->RequestBlocking ( m_usModel, "CClientVehicle::StreamIn - bInstantly" ) )
        {
            // Create us
            Create ();
        }
        else NotifyUnableToCreate ();
    }
    else
    {
        // Request its model.
        if ( m_pModelRequester->Request ( m_usModel, this ) )
        {
            // If it got loaded immediately, create the vehicle now.
            // Otherwise we create it when the model loaded callback calls us
            Create ();
        }
        else NotifyUnableToCreate ();
    }
}


void CClientVehicle::StreamOut ( void )
{
    // Destroy the vehicle. 
    Destroy ();

    // Make sure we don't have any model requests
    // pending in the model request manager. If we
    // had and don't do this it could create us when
    // we're not streamed in.
    m_pModelRequester->Cancel ( this, true );
}


void CClientVehicle::AttachTo ( CClientEntity* pEntity )
{    
    // Add/remove us to/from our managers attached list
    if ( m_pAttachedToEntity && !pEntity ) m_pVehicleManager->m_Attached.remove ( this );
    else if ( !m_pAttachedToEntity && pEntity ) m_pVehicleManager->m_Attached.push_back ( this );

    CClientEntity::AttachTo ( pEntity );
}


bool CClientVehicle::DoCheckHasLandingGear ( void )
{
    return ( m_usModel == VT_ANDROM ||
             m_usModel == VT_AT400 ||
             m_usModel == VT_NEVADA ||
             m_usModel == VT_RUSTLER ||
             m_usModel == VT_SHAMAL ||
             m_usModel == VT_HYDRA ||
             m_usModel == VT_STUNT );
}


void CClientVehicle::Create ( void )
{
    // If the vehicle doesn't exist
    if ( !m_pVehicle )
    {
        #ifdef MTA_DEBUG
            g_pCore->GetConsole ()->Printf ( "CClientVehicle::Create %d", GetModel() );
        #endif

        // Check again that the limit isn't reached. We are required to do so because
        // we load async. The streamer isn't always aware of our limits.
        if ( CClientVehicleManager::IsVehicleLimitReached () )
        {
            // Tell the streamer we could not create it
            NotifyUnableToCreate ();
            return;
        }

        // Add a reference to the vehicle model we're creating.
        m_pModelInfo->ModelAddRef ( BLOCKING, "CClientVehicle::Create" );

        // Might want to make this settable by users? Could just leave it like this, don't mind.
        // Doesn't appear to work with trucks - only cars - stored string is up to 8 chars, will be reset after
        // each vehicle spawned of this model type (i.e. after AddVehicle below)
        if ( !m_strRegPlate.empty () )
            m_pModelInfo->SetCustomCarPlateText ( m_strRegPlate.c_str () );

        // Create the vehicle
        if ( CClientVehicleManager::IsTrainModel ( m_usModel ) )
        {
            DWORD dwModels [1];
            dwModels [0] = m_usModel;
            m_pVehicle = g_pGame->GetPools ()->AddTrain ( &m_Matrix.vPos, dwModels, 1, m_bTrainDirection );
        }
        else
        {            
            m_pVehicle = g_pGame->GetPools ()->AddVehicle ( static_cast < eVehicleTypes > ( m_usModel ), m_ucVariation, m_ucVariation2 );
        }

        // Failed. Remove our reference to the vehicle model and return
        if ( !m_pVehicle )
        {
            // Tell the streamer we could not create it
            NotifyUnableToCreate ();

            m_pModelInfo->RemoveRef ();
            return;
        }

        // Put our pointer in its custom data
        m_pVehicle->SetStoredPointer ( this );

        // Add XRef
        g_pClientGame->GetGameEntityXRefManager ()->AddEntityXRef ( this, m_pVehicle );
        
        // Jump straight to the target position if we have one
        if ( HasTargetPosition () )
        {
            GetTargetPosition ( m_Matrix.vPos );
        }

        // Jump straight to the target rotation if we have one
        if ( HasTargetRotation () )
        {
            CVector vecTemp = m_interp.rot.vecTarget;
            ConvertDegreesToRadians ( vecTemp );
            g_pMultiplayer->ConvertEulerAnglesToMatrix ( m_Matrix, ( 2 * PI ) - vecTemp.fX, ( 2 * PI ) - vecTemp.fY, ( 2 * PI ) - vecTemp.fZ );
        }

        // Got any settings to restore?
        m_pVehicle->SetMatrix ( &m_Matrix );
        m_matFrozen = m_Matrix;
        m_pVehicle->SetMoveSpeed ( &m_vecMoveSpeed );
        m_pVehicle->SetTurnSpeed ( &m_vecTurnSpeed );
        m_pVehicle->SetVisible ( m_bVisible );
        m_pVehicle->SetUsesCollision ( m_bIsCollisionEnabled );
        m_pVehicle->SetEngineBroken ( m_bEngineBroken );

        if ( m_tSirenBeaconInfo.m_bOverrideSirens )
        {
            GiveVehicleSirens( m_tSirenBeaconInfo.m_ucSirenType, m_tSirenBeaconInfo.m_ucSirenCount );
            for ( unsigned char i = 0; i < m_tSirenBeaconInfo.m_ucSirenCount; i++ )
            {
                m_pVehicle->SetVehicleSirenPosition( i, m_tSirenBeaconInfo.m_tSirenInfo[i].m_vecSirenPositions );
                m_pVehicle->SetVehicleSirenMinimumAlpha( i, m_tSirenBeaconInfo.m_tSirenInfo[i].m_dwMinSirenAlpha );
                m_pVehicle->SetVehicleSirenColour( i, m_tSirenBeaconInfo.m_tSirenInfo[i].m_RGBBeaconColour );
            }
            SetVehicleFlags ( m_tSirenBeaconInfo.m_b360Flag, m_tSirenBeaconInfo.m_bUseRandomiser, m_tSirenBeaconInfo.m_bDoLOSCheck, m_tSirenBeaconInfo.m_bSirenSilent );
        }
        m_pVehicle->SetSirenOrAlarmActive ( m_bSireneOrAlarmActive );
        SetLandingGearDown ( m_bLandingGearDown );
        _SetAdjustablePropertyValue ( m_usAdjustablePropertyValue );
        m_pVehicle->SetSwingingDoorsAllowed ( m_bSwingingDoorsAllowed );
        m_pVehicle->LockDoors ( m_bDoorsLocked );
        m_pVehicle->SetDoorsUndamageable ( m_bDoorsUndamageable );
        m_pVehicle->SetCanShootPetrolTank ( m_bCanShootPetrolTank );
        m_pVehicle->SetTaxiLightOn ( m_bTaxiLightOn );
        m_pVehicle->SetCanBeTargettedByHeatSeekingMissiles ( m_bCanBeTargettedByHeatSeekingMissiles );
        CalcAndUpdateTyresCanBurstFlag ();
        if ( GetVehicleType () == CLIENTVEHICLE_TRAIN )
        {
            m_pVehicle->SetDerailed ( m_bIsDerailed );
            m_pVehicle->SetDerailable ( m_bIsDerailable );
            m_pVehicle->SetTrainDirection ( m_bTrainDirection );
            m_pVehicle->SetTrainSpeed ( m_fTrainSpeed );
        }

        m_pVehicle->SetOverrideLights ( m_ucOverrideLights );
        m_pVehicle->SetRemap ( static_cast < unsigned int > ( m_ucPaintjob ) );
        m_pVehicle->SetBodyDirtLevel ( m_fDirtLevel );
        m_pVehicle->SetEngineOn ( m_bEngineOn );
        m_pVehicle->SetAreaCode ( m_ucInterior );
        m_pVehicle->SetSmokeTrailEnabled ( m_bSmokeTrail );
        m_pVehicle->SetGravity ( &m_vecGravity );
        m_pVehicle->SetHeadLightColor ( m_HeadLightColor );

        if ( m_eVehicleType == CLIENTVEHICLE_HELI )
        {
            m_pVehicle->SetHeliRotorSpeed ( m_fHeliRotorSpeed );
            m_pVehicle->SetHeliSearchLightVisible ( m_bHeliSearchLightVisible );
        }

        m_pVehicle->SetUnderwater ( IsBelowWater () );

        // HACK: temp fix until windows are fixed using setAlpha
        if ( m_bAlphaChanged )
            m_pVehicle->SetAlpha ( m_ucAlpha );

        m_pVehicle->SetHealth ( m_fHealth );

        if ( m_bBlown || m_fHealth == 0.0f ) m_bBlowNextFrame = true;

        CalcAndUpdateCanBeDamagedFlag ();

        // Restore the color
        if ( m_bColorSaved )
        {
            m_pVehicle->SetColor ( m_Color.GetRGBColor ( 0 ), m_Color.GetRGBColor ( 1 ), m_Color.GetRGBColor ( 2 ), m_Color.GetRGBColor ( 3 ), 0 );
        }

        // Link us with stored next and previous vehicles
        if ( m_pPreviousLink && m_pPreviousLink->m_pVehicle )
        {
            m_pVehicle->SetPreviousTrainCarriage ( m_pPreviousLink->m_pVehicle );
        }

        if ( m_pNextLink && m_pNextLink->m_pVehicle )
        {
            m_pVehicle->SetNextTrainCarriage ( m_pNextLink->m_pVehicle );
        }

        // Restore turret rotation
        if ( m_eVehicleType == CLIENTVEHICLE_CAR ||
            m_eVehicleType == CLIENTVEHICLE_PLANE ||
            m_eVehicleType == CLIENTVEHICLE_QUADBIKE )
        {
            m_pVehicle->SetTurretRotation ( m_fTurretHorizontal, m_fTurretVertical );
        }

        // Does this car have a damage model?
        if ( HasDamageModel () )
        {
            // Set the damage model doors
            CDamageManager* pDamageManager = m_pVehicle->GetDamageManager ();

            for ( int i = 0; i < MAX_DOORS; i++ )
                pDamageManager->SetDoorStatus ( static_cast < eDoors > ( i ), m_ucDoorStates [i] );            
            for ( int i = 0; i < MAX_PANELS; i++ )
                pDamageManager->SetPanelStatus ( static_cast < ePanels > ( i ), m_ucPanelStates [i] );
            for ( int i = 0; i < MAX_LIGHTS; i++ )
                pDamageManager->SetLightStatus ( static_cast < eLights > ( i ), m_ucLightStates [i] );
        }
        for ( int i = 0; i < MAX_WHEELS; i++ )
            SetWheelStatus ( i, m_ucWheelStates [i], true );

        // Eventually warp driver back in
        if ( m_pDriver ) m_pDriver->WarpIntoVehicle ( this, 0 );

        // Warp the passengers back in
        for ( unsigned int i = 0; i < 8; i++ )
        {
            if ( m_pPassengers [i] )
            {
                m_pPassengers [i]->WarpIntoVehicle ( this, i + 1 );
                if ( m_pPassengers [i] )
                    m_pPassengers [i]->StreamIn ( true );
            }
        }

        // Reattach a towed vehicle?
        if ( m_pTowedVehicle )
        {
            CVehicle* pGameVehicle = m_pTowedVehicle->GetGameVehicle ();
            if ( pGameVehicle )
                pGameVehicle->SetTowLink ( m_pVehicle );
        }

        // Reattach if we're being towed
        if ( m_pTowedByVehicle )
        {
            CVector vecTowedByPos;
            m_pTowedByVehicle->GetPosition ( vecTowedByPos );
            SetPosition ( vecTowedByPos );

            CVehicle* pGameVehicle = m_pTowedByVehicle->GetGameVehicle ();
            if ( pGameVehicle )
            {
                m_pVehicle->SetTowLink ( pGameVehicle );
            }
        }

        // Reattach to an entity + any entities attached to this
        ReattachEntities ();

        // Give it a tap so it doesn't stick in the air if movespeed is standing still
        if ( m_vecMoveSpeed.fX < 0.01f && m_vecMoveSpeed.fX > -0.01f &&
             m_vecMoveSpeed.fY < 0.01f && m_vecMoveSpeed.fY > -0.01f &&
             m_vecMoveSpeed.fZ < 0.01f && m_vecMoveSpeed.fZ > -0.01f )
        {
            m_vecMoveSpeed = CVector ( 0.0f, 0.0f, 0.01f );
            m_pVehicle->SetMoveSpeed ( &m_vecMoveSpeed );
        }

        // Validate
        m_pManager->RestoreEntity ( this );

        // Set the frozen matrix to our position
        m_matFrozen = m_Matrix;

        // Reset the interpolation
        ResetInterpolation ();
        ResetDoorInterpolation ();

        for ( unsigned char i = 0; i < 6; ++i )
            SetDoorOpenRatio ( i, m_fDoorOpenRatio [ i ], 0, true );


        // Re-apply handling entry
        if ( m_pHandlingEntry )
        {
            m_pVehicle->SetHandlingData ( m_pHandlingEntry );
            
            if ( m_bHasCustomHandling )
                ApplyHandling ();
        }
        // Re-add all the upgrades - Has to be applied after handling *shrugs*
        if ( m_pUpgrades )
            m_pUpgrades->ReAddAll ();

        // Tell the streamer we've created this object
        NotifyCreate ();
    }
}


void CClientVehicle::Destroy ( void )
{
    // If the vehicle exists
    if ( m_pVehicle )
    {
        #ifdef MTA_DEBUG
            g_pCore->GetConsole ()->Printf ( "CClientVehicle::Destroy %d", GetModel() );
        #endif

        // Invalidate
        m_pManager->InvalidateEntity ( this );

        // Store anything we allow GTA to change
        m_pVehicle->GetMatrix ( &m_Matrix );
        m_pVehicle->GetMoveSpeed ( &m_vecMoveSpeed );
        m_pVehicle->GetTurnSpeed ( &m_vecTurnSpeed );
        m_fHealth = GetHealth ();
        m_bSireneOrAlarmActive = m_pVehicle->IsSirenOrAlarmActive () ? true:false;
        m_bLandingGearDown = IsLandingGearDown ();
        m_usAdjustablePropertyValue = m_pVehicle->GetAdjustablePropertyValue ();
        m_bEngineOn = m_pVehicle->IsEngineOn ();
        m_bIsOnGround = IsOnGround ();
        m_bIsDerailed = IsDerailed ();
        m_fHeliRotorSpeed = GetHeliRotorSpeed ();
        m_bHeliSearchLightVisible = IsHeliSearchLightVisible ();
        m_pHandlingEntry = m_pVehicle->GetHandlingData();
        if ( m_eVehicleType == CLIENTVEHICLE_CAR ||
            m_eVehicleType == CLIENTVEHICLE_PLANE ||
            m_eVehicleType == CLIENTVEHICLE_QUADBIKE )
        {
            m_pVehicle->GetTurretRotation ( &m_fTurretHorizontal, &m_fTurretVertical );
        }

        // This vehicle has a damage model?
        if ( HasDamageModel () )
        {
            // Grab the damage model
            CDamageManager* pDamageManager = m_pVehicle->GetDamageManager ();

            for ( int i = 0; i < MAX_DOORS; i++ )
                m_ucDoorStates [i] = pDamageManager->GetDoorStatus ( static_cast < eDoors > ( i ) );            
            for ( int i = 0; i < MAX_PANELS; i++ )
                m_ucPanelStates [i] = pDamageManager->GetPanelStatus ( static_cast < ePanels > ( i ) );
            for ( int i = 0; i < MAX_LIGHTS; i++ )
                m_ucLightStates [i] = pDamageManager->GetLightStatus ( static_cast < eLights > ( i ) );
        }
        for ( int i = 0; i < MAX_WHEELS; i++ )
            m_ucWheelStates [i] = GetWheelStatus ( i );

        // Remove the driver from the vehicle
        CClientPed* pPed = GetOccupant ( 0 );
        if ( pPed )
        {
            // Only remove him physically. Don't let the ped update us
            pPed->InternalRemoveFromVehicle ( m_pVehicle );
        }

        // Remove all the passengers physically
        for ( unsigned int i = 0; i < 8; i++ )
        {
            if ( m_pPassengers [i] )
            {
                m_pPassengers [i]->InternalRemoveFromVehicle ( m_pVehicle );
            }
        }

        // Do we have any occupying players? (that could be working on entering this vehicle)
        if ( m_pOccupyingDriver && m_pOccupyingDriver->m_pOccupyingVehicle == this )
        {
            if ( m_pOccupyingDriver->IsGettingIntoVehicle () || m_pOccupyingDriver->IsGettingOutOfVehicle () )
            {
                m_pOccupyingDriver->RemoveFromVehicle ();
            }
        }
        for ( unsigned int i = 0; i < 8; i++ )
        {
            if ( m_pOccupyingPassengers [i] && m_pOccupyingPassengers [i]->m_pOccupyingVehicle == this )
            {
                if ( m_pOccupyingPassengers [i]->IsGettingIntoVehicle () || m_pOccupyingPassengers [i]->IsGettingOutOfVehicle () )
                {
                    m_pOccupyingPassengers [i]->RemoveFromVehicle ();
                }
            }
        }

        // Remove XRef
        g_pClientGame->GetGameEntityXRefManager ()->RemoveEntityXRef ( this, m_pVehicle );

        // Destroy the vehicle
        g_pGame->GetPools ()->RemoveVehicle ( m_pVehicle );
        m_pVehicle = NULL;

        // Remove reference to its model
        m_pModelInfo->RemoveRef ();

        NotifyDestroy ();
    }
}


void CClientVehicle::ReCreate ( void )
{
    // Recreate the vehicle if it exists
    if ( m_pVehicle )
    {
        Destroy ();
        Create ();
    }
}


void CClientVehicle::ModelRequestCallback ( CModelInfo* pModelInfo )
{
    // Create the vehicle. The model is now loaded.
    Create ();
}


void CClientVehicle::NotifyCreate ( void )
{
    m_pVehicleManager->OnCreation ( this );
    CClientStreamElement::NotifyCreate ();
}


void CClientVehicle::NotifyDestroy ( void )
{
    m_pVehicleManager->OnDestruction ( this );
}


CClientVehicle* CClientVehicle::GetTowedVehicle ( void )
{
    if ( m_pVehicle )
    {
        CVehicle* pGameVehicle = m_pVehicle->GetTowedVehicle ();
        if ( pGameVehicle ) return m_pVehicleManager->Get ( pGameVehicle, false );
    }
    
    return m_pTowedVehicle;
}


CClientVehicle* CClientVehicle::GetRealTowedVehicle ( void )
{
    if ( m_pVehicle )
    {
        CVehicle* pGameVehicle = m_pVehicle->GetTowedVehicle ();
        if ( pGameVehicle ) return m_pVehicleManager->Get ( pGameVehicle, false );

        // This is the only difference from ::GetTowedVehicle
        return NULL;
    }
    
    return m_pTowedVehicle;
}


bool CClientVehicle::SetTowedVehicle ( CClientVehicle* pVehicle )
{
    if ( pVehicle == m_pTowedVehicle )
        return true;

    // Do we already have a towed vehicle?
    if ( m_pTowedVehicle && pVehicle != m_pTowedVehicle )
    {
        // Remove it
        CVehicle * pGameVehicle = m_pTowedVehicle->GetGameVehicle ();
        if ( pGameVehicle && m_pVehicle )
            pGameVehicle->BreakTowLink ();
        m_pTowedVehicle->m_pTowedByVehicle = NULL;
        m_pTowedVehicle = NULL;
    }

    // Do we have a new one to set?
    if ( pVehicle )
    {
        // Are we trying to establish a circular loop? (this would freeze everything up)
        CClientVehicle* pCircTestVehicle = pVehicle;
        while ( pCircTestVehicle )
        {
            if ( pCircTestVehicle == this )
                return false;
            pCircTestVehicle = pCircTestVehicle->m_pTowedVehicle;
        }

        pVehicle->m_pTowedByVehicle = this;

        // Add it
        if ( m_pVehicle )
        {
            CVehicle * pGameVehicle = pVehicle->GetGameVehicle ();

            if ( pGameVehicle )
            {
                // Both vehicles are streamed in
                if ( m_pVehicle->GetTowedVehicle () != pGameVehicle )
                    pGameVehicle->SetTowLink ( m_pVehicle );
            }
            else
            {
                // If only the towing vehicle is streamed in, force the towed vehicle to stream in
                pVehicle->StreamIn ( true );
            }
        }
        else
        {
            // If the towing vehicle is not streamed in, the towed vehicle can't be streamed in,
            // so we move it to the towed position.
            CVector vecPosition;
            pVehicle->GetPosition ( vecPosition );
            pVehicle->UpdateStreamPosition ( vecPosition );
        }
    }
    else
        m_ulIllegalTowBreakTime = 0;

    m_pTowedVehicle = pVehicle;
    return true;
}


bool CClientVehicle::SetWinchType ( eWinchType winchType )
{
    if ( GetModel () == 417 ) // Leviathan
    {
        if ( m_pVehicle )
        {
            if ( m_eWinchType == WINCH_NONE )
            {
                m_pVehicle->SetWinchType ( winchType );
                m_eWinchType = winchType;
            }
            return true;
        }
        else
        {
            m_eWinchType = winchType;
            return true;
        }
    }

    return false;
}


bool CClientVehicle::PickupEntityWithWinch ( CClientEntity* pEntity )
{
    if ( m_pVehicle )
    {
        if ( m_eWinchType != WINCH_NONE )
        {
            CEntity* pGameEntity = NULL;
            eClientEntityType entityType = pEntity->GetType ();
            switch ( entityType )
            {
                case CCLIENTOBJECT:
                {
                    CClientObject* pObject = static_cast < CClientObject* > ( pEntity );
                    pGameEntity = pObject->GetGameObject ();
                    break;
                }
                case CCLIENTPED:
                case CCLIENTPLAYER:
                {
                    CClientPed* pModel = static_cast < CClientPed* > ( pEntity );
                    pGameEntity = pModel->GetGameEntity ();
                    break;
                }
                case CCLIENTVEHICLE:
                {
                    CClientVehicle* pVehicle = static_cast < CClientVehicle* > ( pEntity );
                    pGameEntity = pVehicle->GetGameVehicle ();
                    break;
                }
            }

            if ( pGameEntity )
            {
                m_pVehicle->PickupEntityWithWinch ( pGameEntity );
                m_pPickedUpWinchEntity = pEntity;

                return true;
            }
        }
    }

    return false;
}


bool CClientVehicle::ReleasePickedUpEntityWithWinch ( void )
{
    if ( m_pVehicle )
    {
        if ( m_pPickedUpWinchEntity )
        {
            m_pVehicle->ReleasePickedUpEntityWithWinch ();
            m_pPickedUpWinchEntity = NULL;
        
            return true;
        }
    }

    return false;
}


void CClientVehicle::SetRopeHeightForHeli ( float fRopeHeight )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetRopeHeightForHeli ( fRopeHeight );
    }
}


CClientEntity* CClientVehicle::GetPickedUpEntityWithWinch ( void )
{
    CClientEntity* pEntity = m_pPickedUpWinchEntity;
    if ( m_pVehicle )
    {
        CPhysical* pPhysical = m_pVehicle->QueryPickedUpEntityWithWinch ();
        if ( pPhysical )
        {
            eEntityType entityType = pPhysical->GetEntityType ();
            switch ( entityType )
            {
                case ENTITY_TYPE_VEHICLE:
                {
                    CVehicle* pGameVehicle = dynamic_cast < CVehicle* > ( pPhysical );
                    CClientVehicle* pVehicle = m_pVehicleManager->Get ( pGameVehicle, false );
                    if ( pVehicle )
                        pEntity = static_cast < CClientEntity* > ( pVehicle );
                    break;
                }
                case ENTITY_TYPE_PED:
                {
                    CPlayerPed* pPlayerPed = dynamic_cast < CPlayerPed* > ( pPhysical );
                    CClientPed* pModel = m_pManager->GetPedManager ()->Get ( pPlayerPed, false, false );
                    if ( pModel )
                        pEntity = static_cast < CClientEntity* > ( pModel );
                    break;
                }
                case ENTITY_TYPE_OBJECT:
                {
                    CObject* pGameObject = dynamic_cast < CObject* > ( pPhysical );
                    CClientObject* pObject = m_pManager->GetObjectManager ()->Get ( pGameObject, false );
                    if ( pObject )
                        pEntity = static_cast < CClientEntity* > ( pObject );
                    break;
                }
            }
        }
    }

    return pEntity;
}


void CClientVehicle::SetRegPlate ( const char* szPlate )
{
    if ( szPlate )
    {
        m_strRegPlate = szPlate;
    }
}


unsigned char CClientVehicle::GetPaintjob ( void )
{
    if ( m_pVehicle )
    {
        int iRemap = m_pVehicle->GetRemapIndex ();
        return (iRemap == -1) ? 3 : iRemap;
    }
    
    return m_ucPaintjob;
}


void CClientVehicle::SetPaintjob ( unsigned char ucPaintjob )
{
    if ( ucPaintjob != m_ucPaintjob && ucPaintjob <= 4 )
    {
        if ( m_pVehicle )
        {
            m_pVehicle->SetRemap ( static_cast < unsigned int > ( ucPaintjob ) );
        }
        m_ucPaintjob = ucPaintjob;
        m_bColorSaved = false;
    }
}


float CClientVehicle::GetDirtLevel ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetBodyDirtLevel ();
    }

    return m_fDirtLevel;
}


void CClientVehicle::SetDirtLevel ( float fDirtLevel )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetBodyDirtLevel ( fDirtLevel );
    }
    m_fDirtLevel = fDirtLevel;
}
bool CClientVehicle::IsOnWater ( void )
{
    if ( m_pVehicle )
    {
        float fWaterLevel;
        CVector vecPosition, vecTemp;
        GetPosition ( vecPosition );
        float fDistToBaseOfModel = vecPosition.fZ - m_pVehicle->GetDistanceFromCentreOfMassToBaseOfModel();
        if ( g_pGame->GetWaterManager ()->GetWaterLevel ( vecPosition, &fWaterLevel, true, &vecTemp ) )
        {
            if (fDistToBaseOfModel <= fWaterLevel) {
                return true;
            }
        }
    }
    return false;
}

bool CClientVehicle::IsInWater ( void )
{
    if ( m_pModelInfo )
    {
        CBoundingBox* pBoundingBox = m_pModelInfo->GetBoundingBox ();
        if ( pBoundingBox )
        {
            CVector vecMin = pBoundingBox->vecBoundMin;
            CVector vecPosition, vecTemp;
            GetPosition ( vecPosition );
            vecMin += vecPosition;
            float fWaterLevel;
            if ( g_pGame->GetWaterManager ()->GetWaterLevel ( vecPosition, &fWaterLevel, true, &vecTemp ) )
            {
                if ( vecPosition.fZ <= fWaterLevel )
                {
                    return true;
                }
            }
        }
    }
    return false;
}


float CClientVehicle::GetDistanceFromGround ( void )
{
    CVector vecPosition;
    GetPosition ( vecPosition );
    float fGroundLevel = static_cast < float > ( g_pGame->GetWorld ()->FindGroundZFor3DPosition ( &vecPosition ) );

    CBoundingBox* pBoundingBox = m_pModelInfo->GetBoundingBox ();
    if ( pBoundingBox )
        fGroundLevel -= pBoundingBox->vecBoundMin.fZ + pBoundingBox->vecBoundOffset.fZ;

    return ( vecPosition.fZ - fGroundLevel );
}


bool CClientVehicle::IsOnGround ( void )
{
    if ( m_pModelInfo )
    {
        CBoundingBox* pBoundingBox = m_pModelInfo->GetBoundingBox ();
        if ( pBoundingBox )
        {
            CVector vecMin = pBoundingBox->vecBoundMin;
            CVector vecPosition;
            GetPosition ( vecPosition );
            vecMin += vecPosition;
            float fGroundLevel = static_cast < float > (
                g_pGame->GetWorld ()->FindGroundZFor3DPosition ( &vecPosition ) );

            /* Is the lowest point of the bounding box lower than 0.5 above the floor,
            or is the lowest point of the bounding box higher than 0.3 below the floor */
            return ( ( fGroundLevel > vecMin.fZ && ( fGroundLevel - vecMin.fZ ) < 0.5f ) ||
                ( vecMin.fZ > fGroundLevel && ( vecMin.fZ - fGroundLevel ) < 0.3f ) );
        }
    }
    return m_bIsOnGround;
}


void CClientVehicle::LockSteering ( bool bLock )
{
    // STATUS_TRAIN_MOVING or STATUS_PLAYER_DISABLED will do. STATUS_TRAIN_NOT_MOVING is neater but will screw up planes (turns off the engine).

    eEntityStatus Status = m_pVehicle->GetEntityStatus ();
    
    if ( bLock && Status != STATUS_TRAIN_MOVING ) {
        m_NormalStatus = Status;
        m_pVehicle->SetEntityStatus ( STATUS_TRAIN_MOVING );
    } else if ( !bLock && Status == STATUS_TRAIN_MOVING ) {
        m_pVehicle->SetEntityStatus ( m_NormalStatus );
    }

    return;
}


bool CClientVehicle::IsSmokeTrailEnabled ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->IsSmokeTrailEnabled ();
    }
    return m_bSmokeTrail;
}


void CClientVehicle::SetSmokeTrailEnabled ( bool bEnabled )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetSmokeTrailEnabled ( bEnabled );
    }
    m_bSmokeTrail = bEnabled;
}


void CClientVehicle::ResetInterpolation ( void )
{
    if ( HasTargetPosition () )
        SetPosition ( m_interp.pos.vecTarget );
    if ( HasTargetRotation () )
        SetRotationDegrees ( m_interp.rot.vecTarget );
    m_interp.pos.ulFinishTime = 0;
    m_interp.rot.ulFinishTime = 0;
}


void CClientVehicle::AddMatrix ( CMatrix& Matrix, double dTime, unsigned short usTickRate )
{
    /*
    // See if we need an update yet (since the rotation could have a lower tick rate)
    if ( ( ( dTime - m_dLastRotationTime ) * 1000 ) >= usTickRate ) {
        m_fLERP = LERP_FACTOR;

        // Set the source quaternion to whatever we have right now
        m_QuatA = m_QuatLERP;

        // Set the destination quaternion to whatever we just received
        m_QuatB = CQuat ( &Matrix );

        // Store the matrix into the pure matrix
        m_MatrixPure = Matrix;

        // Debug stuff
        #ifdef MTA_DEBUG_INTERPOLATION
        g_pCore->GetGraphics()->DrawTextTTF(232,200,300,216,0xDDDDDDDD, "RENEW", 1.0f, 0);
        #endif

        // Store the time for this rotation
        m_dLastRotationTime = dTime;
    }
    */
}


void CClientVehicle::AddVelocity ( CVector& vecVelocity )
{
    m_vecMoveSpeedInterpolate = vecVelocity;
}


void CClientVehicle::Interpolate ( void )
{
    // Interpolate it if: It has a driver and it's not local and we're not syncing it or
    //                    It has no driver and we're not syncing it.
    if ( ( m_pDriver && !m_pDriver->IsLocalPlayer () && !static_cast < CDeathmatchVehicle* > ( this ) ->IsSyncing () ) ||
         ( !m_pDriver && !static_cast < CDeathmatchVehicle* > ( this ) ->IsSyncing () ) )
    {
        UpdateTargetPosition ();
        UpdateTargetRotation ();
    }
    else
    {
        // Otherwize make sure we have no interpolation stuff stored
        RemoveTargetPosition ();
        RemoveTargetRotation ();
    }
}


void CClientVehicle::GetInitialDoorStates ( SFixedArray < unsigned char, MAX_DOORS >& ucOutDoorStates )
{
    switch ( m_usModel )
    {
        case VT_BAGGAGE:
        case VT_BANDITO:
        case VT_BFINJECT:
        case VT_CADDY:
        case VT_DOZER:
        case VT_FORKLIFT:
        case VT_KART:
        case VT_MOWER:
        case VT_QUAD:
        case VT_RCBANDIT:
        case VT_RCCAM:
        case VT_RCGOBLIN:
        case VT_RCRAIDER:
        case VT_RCTIGER:
        case VT_TRACTOR:
        case VT_VORTEX:
            memset ( &ucOutDoorStates[0], DT_DOOR_MISSING, MAX_DOORS );

            // Keep the bonet and boot intact
            ucOutDoorStates [ 0 ] = ucOutDoorStates [ 1 ] = DT_DOOR_INTACT;
            break;        
        default:
            memset ( &ucOutDoorStates[0], DT_DOOR_INTACT, MAX_DOORS );
    }
}


void CClientVehicle::SetTargetPosition ( CVector& vecPosition, unsigned long ulDelay, bool bValidVelocityZ, float fVelocityZ )
{   
    // Are we streamed in?
    if ( m_pVehicle )
    {
        UpdateTargetPosition ();

        unsigned long ulTime = CClientTime::GetTime ();
        CVector vecLocalPosition;
        GetPosition ( vecLocalPosition );

        // Cars under road fix hack
        if ( bValidVelocityZ && m_eVehicleType != CLIENTVEHICLE_HELI && m_eVehicleType != CLIENTVEHICLE_PLANE )
        {
            // If remote z higher by too much and remote not doing any z movement, warp local z coord
            float fDeltaZ = vecPosition.fZ - vecLocalPosition.fZ;
            if ( fDeltaZ > 0.4f && fDeltaZ < 10.0f )
            {
                if ( fabsf ( fVelocityZ ) < 0.01f )
                {
                    vecLocalPosition.fZ = vecPosition.fZ;
                    SetPosition ( vecLocalPosition );
                }
            }
        }

#ifdef MTA_DEBUG
        m_interp.pos.vecStart = vecLocalPosition;
#endif
        m_interp.pos.vecTarget = vecPosition;
        // Calculate the relative error
        m_interp.pos.vecError = vecPosition - vecLocalPosition;

        // Apply the error over 400ms (i.e. 1/4 per 100ms )
        m_interp.pos.vecError *= Lerp < const float > ( 0.25f, UnlerpClamped( 100, ulDelay, 400 ), 1.0f );

        // Get the interpolation interval
        m_interp.pos.ulStartTime = ulTime;
        m_interp.pos.ulFinishTime = ulTime + ulDelay;

        // Initialize the interpolation
        m_interp.pos.fLastAlpha = 0.0f;
    }
    else
    {
        // Update our position now
        SetPosition ( vecPosition );
    }
}


void CClientVehicle::RemoveTargetPosition ( void )
{
    m_interp.pos.ulFinishTime = 0;
}


void CClientVehicle::SetTargetRotation ( CVector& vecRotation, unsigned long ulDelay )
{
    // Are we streamed in?
    if ( m_pVehicle )
    {
        UpdateTargetRotation ();

        unsigned long ulTime = CClientTime::GetTime ();
        CVector vecLocalRotation;
        GetRotationDegrees ( vecLocalRotation );

#ifdef MTA_DEBUG
        m_interp.rot.vecStart = vecLocalRotation;
#endif
        m_interp.rot.vecTarget = vecRotation;
        // Get the error
        m_interp.rot.vecError.fX = GetOffsetDegrees ( vecLocalRotation.fX, vecRotation.fX );
        m_interp.rot.vecError.fY = GetOffsetDegrees ( vecLocalRotation.fY, vecRotation.fY );
        m_interp.rot.vecError.fZ = GetOffsetDegrees ( vecLocalRotation.fZ, vecRotation.fZ );
    
        // Apply the error over 250ms (i.e. 2/5 per 100ms )
        m_interp.rot.vecError *= Lerp < const float > ( 0.40f, UnlerpClamped( 100, ulDelay, 400 ), 1.0f );
    
        // Get the interpolation interval
        m_interp.rot.ulStartTime = ulTime;
        m_interp.rot.ulFinishTime = ulTime + ulDelay;

        // Initialize the interpolation
        m_interp.rot.fLastAlpha = 0.0f;
    }
    else
    {
        // Update our rotation now
        SetRotationDegrees ( vecRotation );
    }
}


void CClientVehicle::RemoveTargetRotation ( void )
{
    m_interp.rot.ulFinishTime = 0;
}

void CClientVehicle::UpdateTargetPosition ( void )
{
    if ( HasTargetPosition () )
    {
        // Grab the current game position
        CVector vecCurrentPosition;
        GetPosition ( vecCurrentPosition );

        // Get the factor of time spent from the interpolation start
        // to the current time.
        unsigned long ulCurrentTime = CClientTime::GetTime ();
        float fAlpha = SharedUtil::Unlerp ( m_interp.pos.ulStartTime,
                                            ulCurrentTime,
                                            m_interp.pos.ulFinishTime );

        // Don't let it overcompensate the error too much
        fAlpha = SharedUtil::Clamp ( 0.0f, fAlpha, 1.5f );

        // Get the current error portion to compensate
        float fCurrentAlpha = fAlpha - m_interp.pos.fLastAlpha;
        m_interp.pos.fLastAlpha = fAlpha;

        // Apply the error compensation
        CVector vecCompensation = SharedUtil::Lerp ( CVector (), fCurrentAlpha, m_interp.pos.vecError );

        // If we finished compensating the error, finish it for the next pulse
        if ( fAlpha == 1.5f )
        {
            m_interp.pos.ulFinishTime = 0;
        }

        CVector vecNewPosition = vecCurrentPosition + vecCompensation;

        // Check if the distance to interpolate is too far.
        CVector vecVelocity;
        GetMoveSpeed ( vecVelocity );
        float fThreshold = ( VEHICLE_INTERPOLATION_WARP_THRESHOLD + VEHICLE_INTERPOLATION_WARP_THRESHOLD_FOR_SPEED * vecVelocity.Length () ) * g_pGame->GetGameSpeed () * TICK_RATE / 100;

        // There is a reason to have this condition this way: To prevent NaNs generating new NaNs after interpolating (Comparing with NaNs always results to false).
        if ( ! ( ( vecCurrentPosition - m_interp.pos.vecTarget ).Length () <= fThreshold ) )
        {
            // Abort all interpolation
            m_interp.pos.ulFinishTime = 0;
            vecNewPosition = m_interp.pos.vecTarget;

            if ( HasTargetRotation () )
                SetRotationDegrees ( m_interp.rot.vecTarget );
            m_interp.rot.ulFinishTime = 0;
        }

        SetPosition ( vecNewPosition, false );

        if ( !m_bIsCollisionEnabled )
        {
            if ( m_eVehicleType != CLIENTVEHICLE_HELI && m_eVehicleType != CLIENTVEHICLE_BOAT )
            {
                // Ghostmode upwards movement compensation
                CVector MoveSpeed;
                m_pVehicle->GetMoveSpeed ( &MoveSpeed );
                float SpeedXY = CVector( MoveSpeed.fX, MoveSpeed.fY, 0 ).Length ();
                if ( MoveSpeed.fZ > 0.00 && MoveSpeed.fZ < 0.02 && MoveSpeed.fZ > SpeedXY )
                    MoveSpeed.fZ = SpeedXY;
                m_pVehicle->SetMoveSpeed ( &MoveSpeed );
            }
        }

#ifdef MTA_DEBUG
        if ( g_pClientGame->IsShowingInterpolation () &&
             g_pClientGame->GetLocalPlayer ()->GetOccupiedVehicle () == this )
        {
            // DEBUG
            SString strBuffer ( "-== Vehicle interpolation ==-\n"
                                "vecStart: %f %f %f\n"
                                "vecTarget: %f %f %f\n"
                                "Position: %f %f %f\n"
                                "Error: %f %f %f\n"
                                "Alpha: %f\n"
                                "Interpolating: %s\n",
                                m_interp.pos.vecStart.fX, m_interp.pos.vecStart.fY, m_interp.pos.vecStart.fZ,
                                m_interp.pos.vecTarget.fX, m_interp.pos.vecTarget.fY, m_interp.pos.vecTarget.fZ,
                                vecNewPosition.fX, vecNewPosition.fY, vecNewPosition.fZ,
                                m_interp.pos.vecError.fX, m_interp.pos.vecError.fY, m_interp.pos.vecError.fZ,
                                fAlpha, ( m_interp.pos.ulFinishTime == 0 ? "no" : "yes" ) );
            g_pClientGame->GetManager ()->GetDisplayManager ()->DrawText2D ( strBuffer, CVector ( 0.45f, 0.05f, 0 ), 1.0f, 0xFFFFFFFF );
        }
#endif

        // Update our contact players
        CVector vecPlayerPosition;
        CVector vecOffset;
        list < CClientPed * > ::iterator iter = m_Contacts.begin ();
        for ( ; iter != m_Contacts.end () ; iter++ )
        {
            CClientPed * pModel = *iter;
            pModel->GetPosition ( vecPlayerPosition );                
            vecOffset = vecPlayerPosition - vecCurrentPosition;
            vecPlayerPosition = vecNewPosition + vecOffset;
            pModel->SetPosition ( vecPlayerPosition );
        }
    }
}


void CClientVehicle::UpdateTargetRotation ( void )
{
    // Do we have a rotation to move towards? and are we streamed in?
    if ( HasTargetRotation () )
    {
        // Grab the current game rotation
        CVector vecCurrentRotation;
        GetRotationDegrees ( vecCurrentRotation );

        // Get the factor of time spent from the interpolation start
        // to the current time.
        unsigned long ulCurrentTime = CClientTime::GetTime ();
        float fAlpha = SharedUtil::Unlerp ( m_interp.rot.ulStartTime,
                                            ulCurrentTime,
                                            m_interp.rot.ulFinishTime );

        // Don't let it to overcompensate the error
        fAlpha = SharedUtil::Clamp ( 0.0f, fAlpha, 1.0f );

        // Get the current error portion to compensate
        float fCurrentAlpha = fAlpha - m_interp.rot.fLastAlpha;
        m_interp.rot.fLastAlpha = fAlpha;

        CVector vecCompensation = SharedUtil::Lerp ( CVector (), fCurrentAlpha, m_interp.rot.vecError );

        // If we finished compensating the error, finish it for the next pulse
        if ( fAlpha == 1.0f )
        {
            m_interp.rot.ulFinishTime = 0;
        }

        SetRotationDegrees ( vecCurrentRotation + vecCompensation, false );
    }
}


bool CClientVehicle::IsEnterable ( void )
{
    if ( m_pVehicle )
    {
        // Server vehicle?
        if ( !IsLocalEntity () )
        {
            if ( GetHealth () > 0.0f )
            {
                if ( !IsInWater() || (GetVehicleType() == CLIENTVEHICLE_BOAT || 
                    m_usModel == 447 /* sea sparrow */
                    || m_usModel == 417 /* levithan */
                    || m_usModel == 460 /* skimmer */ ) )
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool CClientVehicle::HasRadio ( void )
{
    if ( m_eVehicleType != CLIENTVEHICLE_BMX ) return true;
    return false;
}


bool CClientVehicle::HasPoliceRadio ( void )
{
    switch ( m_usModel )
    {
        case VT_COPCARLA:
        case VT_COPCARSF:
        case VT_COPCARVG:
        case VT_COPCARRU:
        case VT_POLMAV:
        case VT_COPBIKE:
        case VT_SWATVAN:
            return true;
            break;
        default:
            break;
    }
    return false;
}

void CClientVehicle::RemoveAllProjectiles ( void )
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

void CClientVehicle::SetGravity ( const CVector& vecGravity )
{
    if ( m_pVehicle )
        m_pVehicle->SetGravity ( &vecGravity );

    m_vecGravity = vecGravity;
}


SColor CClientVehicle::GetHeadLightColor ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetHeadLightColor ();
    }
    return m_HeadLightColor;
}


int CClientVehicle::GetCurrentGear ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetCurrentGear ();
    }
    return 0;
}


void CClientVehicle::SetHeadLightColor ( const SColor color )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetHeadLightColor ( color );
    }
    m_HeadLightColor = color;
}


//
// Below here is basically awful.
// But there you go.
//

#if OCCUPY_DEBUG_INFO
    #define INFO(x)    g_pCore->GetConsole ()->Printf x
    #define WARN(x)    g_pCore->GetConsole ()->Printf x
#else
    #define INFO(x)    {}
    #define WARN(x)    {}
#endif

std::string GetPlayerName ( CClientPed* pClientPed )
{
    if ( !pClientPed )
        return "null";
    if ( IS_PLAYER ( pClientPed ) )
    {
        CClientPlayer* pPlayer = static_cast < CClientPlayer * > ( pClientPed );
        return pPlayer->GetNick ();
    }
    return "ped";
}

//
// Make a ped become an occupied driver/passenger
// Static function
//
void CClientVehicle::SetPedOccupiedVehicle ( CClientPed* pClientPed, CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor )
{
    INFO (( "SetPedOccupiedVehicle:%s in vehicle:0x%08x  seat:%d  door:%u", GetPlayerName( pClientPed ).c_str (), pVehicle, uiSeat, ucDoor ));

    if ( !pClientPed || !pVehicle )
        return;

    // Clear ped from any current occupied seat in this vehicle
    if ( pClientPed->m_pOccupiedVehicle == pVehicle )
    {
        if ( pVehicle->m_pDriver == pClientPed )
            pVehicle->m_pDriver = NULL;

        for ( int i = 0 ; i < NUMELMS ( pVehicle->m_pPassengers ) ; i++ )
            if ( pVehicle->m_pPassengers[i] == pClientPed )
                pVehicle->m_pPassengers[i] = NULL;
    }

    // Vehicle vars
    if ( uiSeat == 0 )
    {
        if ( pVehicle->m_pDriver && pVehicle->m_pDriver != pClientPed )
        {
            WARN (( "Emergency occupied driver eject by %s on %s\n", GetPlayerName( pClientPed ).c_str (), GetPlayerName( pVehicle->m_pDriver ).c_str () ));
            UnpairPedAndVehicle ( pVehicle->m_pDriver, pVehicle );
        }
        pVehicle->m_pDriver = pClientPed;
    }
    else
    {
        assert ( uiSeat <= NUMELMS(pVehicle->m_pPassengers) );
        if ( pVehicle->m_pPassengers [uiSeat-1] && pVehicle->m_pPassengers [uiSeat-1] != pClientPed )
        {
            WARN (( "Emergency occupied passenger eject by %s on %s\n", GetPlayerName( pClientPed ).c_str (), GetPlayerName( pVehicle->m_pPassengers [uiSeat-1] ).c_str () ));
            UnpairPedAndVehicle ( pVehicle->m_pPassengers [uiSeat-1], pVehicle );
        }
        pVehicle->m_pPassengers [uiSeat-1] = pClientPed;
    }

    // Ped vars
    pClientPed->m_pOccupiedVehicle = pVehicle;
    pClientPed->m_uiOccupiedVehicleSeat = uiSeat;

    if ( ucDoor != 0xFF )
        pVehicle->AllowDoorRatioSetting ( ucDoor, true );

    // Checks
    ValidatePedAndVehiclePair ( pClientPed, pVehicle );
}


//
// Make a ped become an occupying driver/passenger
// Static function
//
void CClientVehicle::SetPedOccupyingVehicle ( CClientPed* pClientPed, CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor )
{
    INFO (( "SetPedOccupyingVehicle:%s in vehicle:0x%08x  seat:%d  door:%u", GetPlayerName( pClientPed ).c_str (), pVehicle, uiSeat, ucDoor ));

    if ( !pClientPed || !pVehicle )
        return;

    // Clear ped from any current occupying seat in this vehicle
    if ( pClientPed->m_pOccupyingVehicle == pVehicle )
    {
        if ( pVehicle->m_pOccupyingDriver == pClientPed )
            pVehicle->m_pOccupyingDriver = NULL;

        for ( int i = 0 ; i < NUMELMS ( pVehicle->m_pOccupyingPassengers ) ; i++ )
            if ( pVehicle->m_pOccupyingPassengers[i] == pClientPed )
                pVehicle->m_pOccupyingPassengers[i] = NULL;
    }

    // Vehicle vars
    if ( uiSeat == 0 )
    {
        if ( pVehicle->m_pOccupyingDriver && pVehicle->m_pOccupyingDriver != pClientPed )
        {
            WARN (( "Emergency occupying driver eject by %s on %s\n", GetPlayerName( pClientPed ).c_str (), GetPlayerName( pVehicle->m_pOccupyingDriver ).c_str () ));
            UnpairPedAndVehicle ( pVehicle->m_pOccupyingDriver, pVehicle );
        }
        pVehicle->m_pOccupyingDriver = pClientPed;
    }
    else
    {
        assert ( uiSeat <= NUMELMS(pVehicle->m_pOccupyingPassengers) );
        if ( pVehicle->m_pOccupyingPassengers [uiSeat-1] && pVehicle->m_pOccupyingPassengers [uiSeat-1] != pClientPed )
        {
            WARN (( "Emergency occupying passenger eject by %s on %s\n", GetPlayerName( pClientPed ).c_str (), GetPlayerName( pVehicle->m_pOccupyingPassengers [uiSeat-1] ).c_str () ));
            UnpairPedAndVehicle ( pVehicle->m_pOccupyingPassengers [uiSeat-1], pVehicle );
        }
       pVehicle->m_pOccupyingPassengers [uiSeat-1] = pClientPed;
    }

    // Ped vars
    pClientPed->m_pOccupyingVehicle = pVehicle;
//  if ( uiSeat >= 0 && uiSeat < 8 )
//      pClientPed->m_uiOccupyingSeat = uiSeat;

    if ( ucDoor != 0xFF )
        pVehicle->AllowDoorRatioSetting ( ucDoor, false );

    // Checks
    ValidatePedAndVehiclePair ( pClientPed, pVehicle );
}


//
// Check ped <> vehicle pointers
// Static function
//
void CClientVehicle::ValidatePedAndVehiclePair( CClientPed* pClientPed, CClientVehicle* pVehicle )
{
#if MTA_DEBUG
    // Occupied    
    // Vehicle vars
    if ( pVehicle->m_pDriver )
        assert ( pVehicle->m_pDriver->m_pOccupiedVehicle == pVehicle );

    for ( int i = 0 ; i < NUMELMS ( pVehicle->m_pPassengers ) ; i++ )
        if ( pVehicle->m_pPassengers[i] )
            assert ( pVehicle->m_pPassengers[i]->m_pOccupiedVehicle == pVehicle );

    // Ped vars
    if ( pClientPed->m_pOccupiedVehicle )
    {
        // Make sure refed once by vehicle
        int iCount = 0;
        if ( pClientPed->m_pOccupiedVehicle->m_pDriver == pClientPed )
            iCount++;

        for ( int i = 0 ; i < NUMELMS ( pClientPed->m_pOccupiedVehicle->m_pPassengers ) ; i++ )
            if ( pClientPed->m_pOccupiedVehicle->m_pPassengers[i] == pClientPed )
                iCount++;

        assert ( iCount == 1 );
    }

    // Occupying
    // Vehicle vars
    if ( pVehicle->m_pOccupyingDriver )
        assert ( pVehicle->m_pOccupyingDriver->m_pOccupyingVehicle == pVehicle );

    for ( int i = 0 ; i < NUMELMS ( pVehicle->m_pOccupyingPassengers ) ; i++ )
        if ( pVehicle->m_pOccupyingPassengers[i] )
            assert ( pVehicle->m_pOccupyingPassengers[i]->m_pOccupyingVehicle == pVehicle );

    // Ped vars
    if ( pClientPed->m_pOccupyingVehicle )
    {
         // Make sure refed once by vehicle
        int iCount = 0;
        if ( pClientPed->m_pOccupyingVehicle->m_pOccupyingDriver == pClientPed )
            iCount++;

        for ( int i = 0 ; i < NUMELMS ( pClientPed->m_pOccupyingVehicle->m_pOccupyingPassengers ) ; i++ )
            if ( pClientPed->m_pOccupyingVehicle->m_pOccupyingPassengers[i] == pClientPed )
                iCount++;

       assert ( iCount == 1 );
    }
#endif
}


//
// Make sure there is no association between a ped and a vehicle
// Static function
//
void CClientVehicle::UnpairPedAndVehicle( CClientPed* pClientPed, CClientVehicle* pVehicle )
{
    if ( !pClientPed || !pVehicle )
        return;

    // Checks
    ValidatePedAndVehiclePair ( pClientPed, pVehicle );

    // Occupied
    // Vehicle vars
    if ( pVehicle->m_pDriver == pClientPed )
    {
        INFO (( "UnpairPedAndVehicle: m_pDriver:%s from vehicle:0x%08x", GetPlayerName( pClientPed ).c_str (), pVehicle ));
        pVehicle->m_pDriver = NULL;
    }


    for ( int i = 0 ; i < NUMELMS ( pVehicle->m_pPassengers ) ; i++ )
        if ( pVehicle->m_pPassengers[i] == pClientPed )
        {
            INFO (( "UnpairPedAndVehicle: m_pPassenger:%s seat:%d from vehicle:0x%08x", GetPlayerName( pClientPed ).c_str (), i + 1, pVehicle ));
            pVehicle->m_pPassengers[i] = NULL;
        }

    // Ped vars
    if ( pClientPed->m_pOccupiedVehicle == pVehicle )
    {
        INFO (( "UnpairPedAndVehicle: pClientPed:%s from m_pOccupiedVehicle:0x%08x", GetPlayerName( pClientPed ).c_str (), pVehicle ));
        if ( pClientPed->m_ucLeavingDoor != 0xFF )
        {
            pVehicle->AllowDoorRatioSetting ( pClientPed->m_ucLeavingDoor, true );
            pClientPed->m_ucLeavingDoor = 0xFF;
        }
        pClientPed->m_pOccupiedVehicle = NULL;
        pClientPed->m_uiOccupiedVehicleSeat = 0xFF;
    }

    // Occupying
    // Vehicle vars
    if ( pVehicle->m_pOccupyingDriver == pClientPed )
    {
        INFO (( "UnpairPedAndVehicle: m_pOccupyingDriver:%s from vehicle:0x%08x", GetPlayerName( pClientPed ).c_str (), pVehicle ));
        pVehicle->m_pOccupyingDriver = NULL;
    }

    for ( int i = 0 ; i < NUMELMS ( pVehicle->m_pOccupyingPassengers ) ; i++ )
        if ( pVehicle->m_pOccupyingPassengers[i] == pClientPed )
        {
            INFO (( "UnpairPedAndVehicle: m_pOccupyingPassenger:%s seat:%d from vehicle:0x%08x", GetPlayerName( pClientPed ).c_str (), i + 1, pVehicle ));
            pVehicle->m_pOccupyingPassengers[i] = NULL;
        }

    // Ped vars
    if ( pClientPed->m_pOccupyingVehicle == pVehicle )
    {
        INFO (( "UnpairPedAndVehicle: pClientPed:%s from m_pOccupyingVehicle:0x%08x", GetPlayerName( pClientPed ).c_str (), pVehicle ));
        pClientPed->m_pOccupyingVehicle = NULL;
        //pClientPed->m_uiOccupyingSeat = 0xFF;
    }
}


//
// Make sure there is no association between a ped and its vehicle
// Static function
//
void CClientVehicle::UnpairPedAndVehicle( CClientPed* pClientPed )
{
    UnpairPedAndVehicle ( pClientPed, pClientPed->GetOccupiedVehicle () );
    UnpairPedAndVehicle ( pClientPed, pClientPed->m_pOccupyingVehicle );
    UnpairPedAndVehicle ( pClientPed, pClientPed->GetRealOccupiedVehicle () );

    if ( pClientPed->m_pOccupiedVehicle )
    {
        WARN (( "*** Unexpected m_pOccupiedVehicle:0x%08x for %s\n", pClientPed->m_pOccupiedVehicle, GetPlayerName( pClientPed ).c_str () ));
        pClientPed->m_pOccupiedVehicle = NULL;
    }

    if ( pClientPed->m_pOccupyingVehicle )
    {
        WARN (( "*** Unexpected m_pOccupyingVehicle:0x%08x for %s\n", pClientPed->m_pOccupyingVehicle, GetPlayerName( pClientPed ).c_str () ));
        pClientPed->m_pOccupyingVehicle = NULL;
    }
}

void CClientVehicle::ApplyHandling ( void )
{
    if ( m_pVehicle )
        m_pVehicle->RecalculateHandling ();

    m_bHasCustomHandling = true;
}


CHandlingEntry* CClientVehicle::GetHandlingData ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetHandlingData ();
    }
    else if ( m_pHandlingEntry )
    {
        return m_pHandlingEntry;
    }
    return NULL;
}


CSphere CClientVehicle::GetWorldBoundingSphere ( void )
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


// Currently, this should only be called if the local player is, or was just in the vehicle
void CClientVehicle::HandleWaitingForGroundToLoad ( void )
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
    CVector vecTemp;
    m_pVehicle->SetMatrix ( &m_matFrozen );
    m_pVehicle->SetMoveSpeed ( &vecTemp );
    m_pVehicle->SetTurnSpeed ( &vecTemp );
    m_vecMoveSpeedMeters = vecTemp;
    m_vecMoveSpeed = vecTemp;
    m_vecTurnSpeed = vecTemp;

    // Load load load
    if ( GetModelInfo () )
        g_pGame->GetStreaming()->LoadAllRequestedModels ( false, "CClientVehicle::HandleWaitingForGroundToLoad" );

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

bool CClientVehicle::GiveVehicleSirens ( unsigned char ucSirenType, unsigned char ucSirenCount )
{
    m_tSirenBeaconInfo.m_bOverrideSirens = true;
    m_tSirenBeaconInfo.m_ucSirenType = ucSirenType;
    m_tSirenBeaconInfo.m_ucSirenCount = ucSirenCount;
    if ( m_pVehicle )
    {
        m_pVehicle->GiveVehicleSirens ( ucSirenType, ucSirenCount );
    }
    return true;
}
void CClientVehicle::SetVehicleSirenPosition ( unsigned char ucSirenID, CVector vecPos )
{
    m_tSirenBeaconInfo.m_tSirenInfo[ucSirenID].m_vecSirenPositions = vecPos;
    if ( m_pVehicle )
    {
        m_pVehicle->SetVehicleSirenPosition ( ucSirenID, vecPos );
    }
}

void CClientVehicle::SetVehicleSirenMinimumAlpha( unsigned char ucSirenID, DWORD dwPercentage )
{
    m_tSirenBeaconInfo.m_tSirenInfo[ucSirenID].m_dwMinSirenAlpha = dwPercentage;
    if ( m_pVehicle )
    {
        m_pVehicle->SetVehicleSirenMinimumAlpha ( ucSirenID, dwPercentage );
    }
}

void CClientVehicle::SetVehicleSirenColour ( unsigned char ucSirenID, SColor tVehicleSirenColour )
{
    m_tSirenBeaconInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour = tVehicleSirenColour;
    if ( m_pVehicle )
    {
        m_pVehicle->SetVehicleSirenColour ( ucSirenID, tVehicleSirenColour );
    }
}

void CClientVehicle::SetVehicleFlags ( bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent )
{
     m_tSirenBeaconInfo.m_b360Flag = bEnable360; 
     m_tSirenBeaconInfo.m_bDoLOSCheck = bEnableLOSCheck; 
     m_tSirenBeaconInfo.m_bUseRandomiser = bEnableRandomiser;
     m_tSirenBeaconInfo.m_bSirenSilent = bEnableSilent;
     if ( m_pVehicle )
     {
        m_pVehicle->SetVehicleFlags ( bEnable360, bEnableRandomiser, bEnableLOSCheck, bEnableSilent );
     }
}

void CClientVehicle::RemoveVehicleSirens ( void )
{
    if ( m_pVehicle )
    {
        m_pVehicle->RemoveVehicleSirens ( );
    }
    m_tSirenBeaconInfo.m_bOverrideSirens = false;
    SetSirenOrAlarmActive ( false );
    for ( unsigned char i = 0; i < 7; i++ )
    {
        SetVehicleSirenPosition( i, CVector ( 0, 0, 0 ) );
        SetVehicleSirenMinimumAlpha( i, 0.0f );
        SetVehicleSirenColour( i, SColor ( ) );
    }
}