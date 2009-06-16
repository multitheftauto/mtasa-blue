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
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

extern CClientGame* g_pClientGame;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Enables debug view which shows information about interpolation
//#define MTA_DEBUG_INTERPOLATION

// Calculation for the LERP factor
#define LERP_FACTOR		(1.0f / ( ( m_Extrapolator.EstimateUpdateTime() * ( TICK_RATE_ROTATION / TICK_RATE ) ) / ( 1.0f / g_pGame->GetFPS() )))

// Unused LERP factor indicating an interpolation reset
#define LERP_UNUSED		-1.0f

// Maximum time between packets before interpolation will be reset
#define LERP_TIMEOUT	3.0f

// To hide the ugly "pointer truncation from DWORD* to unsigned long warning
#pragma warning(disable:4311)

// Maximum distance between current position and target position (for interpolation)
// before we disable interpolation and warp to the position instead
#define INTERPOLATION_WARP_THRESHOLD    20

CClientVehicle::CClientVehicle ( CClientManager* pManager, ElementID ID, unsigned short usModel ) : CClientStreamElement ( pManager->GetVehicleStreamer (), ID )
{
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

    SetTypeName ( "vehicle" );

    // Grab the model info and the bounding box
    m_pModelInfo = g_pGame->GetModelInfo ( usModel );
    m_ucMaxPassengers = CClientVehicleManager::GetMaxPassengerCount ( usModel );

    // Set our default properties
    m_pDriver = NULL;
    m_pOccupyingDriver = NULL;
    memset ( m_pPassengers, 0, sizeof ( m_pPassengers ) );
    memset ( m_pOccupyingPassengers, 0, sizeof ( m_pOccupyingPassengers ) );
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
    m_bDoorsLocked = false;
    m_bDoorsUndamageable = false;
    m_bCanShootPetrolTank = true;
    m_bCanBeTargettedByHeatSeekingMissiles = true;
    m_bColorSaved = false;
    m_bIsFrozen = false;
    m_bScriptFrozen = false;
    GetInitialDoorStates ( m_ucDoorStates );
    memset ( m_ucWheelStates, 0, sizeof ( m_ucWheelStates ) );
    memset ( m_ucPanelStates, 0, sizeof ( m_ucPanelStates ) );
    memset ( m_ucLightStates, 0, sizeof ( m_ucLightStates ) );
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
    m_bHasTargetPosition = false;
    m_bHasTargetRotation = false;
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
    m_ucHeadLightR = 255, m_ucHeadLightG = 255, m_ucHeadLightB = 255;

#ifdef MTA_DEBUG
    m_pLastSyncer = NULL;
    m_ulLastSyncTime = 0;
    m_szLastSyncType = "none";
#endif

    m_bInterpolationEnabled = false;
    m_dResetInterpolationTime = 0;

	ResetInterpolation ();

    // Check if we have landing gears
    m_bHasLandingGear = DoCheckHasLandingGear ();
    m_bHasAdjustableProperty = CClientVehicleManager::HasAdjustableProperty ( m_usModel );

    // Add this vehicle to the vehicle list
    m_pVehicleManager->AddToList ( this );
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
    m_pModelRequester->Cancel ( this );

    // Unreference us from the driving player model (if any)
    if ( m_pDriver )
    {
        m_pDriver->m_pOccupiedVehicle = NULL;
        m_pDriver->m_pOccupyingVehicle = NULL;

        m_pDriver->SetVehicleInOutState ( VEHICLE_INOUT_NONE );
    }

    // And the occupying ones eventually
    if ( m_pOccupyingDriver )
    {
        m_pOccupyingDriver->m_pOccupiedVehicle = NULL;
        m_pOccupyingDriver->m_pOccupyingVehicle = NULL;

        m_pOccupyingDriver->SetVehicleInOutState ( VEHICLE_INOUT_NONE );
    }

    // And the passenger models
    int i;
    for ( i = 0; i < (sizeof(m_pPassengers)/sizeof(CClientPed*)); i++ )
    {
        if ( m_pPassengers [i] )
        {
            m_pPassengers [i]->m_pOccupiedVehicle = NULL;
            m_pPassengers [i]->m_pOccupyingVehicle = NULL;
            m_pPassengers [i]->m_uiOccupiedVehicleSeat = 0;

            m_pPassengers [i]->SetVehicleInOutState ( VEHICLE_INOUT_NONE );
        }
    }

    // Occupying passenger models
    for ( i = 0; i < (sizeof(m_pOccupyingPassengers)/sizeof(CClientPed*)); i++ )
    {
        if ( m_pOccupyingPassengers [i] )
        {
            m_pOccupyingPassengers [i]->m_pOccupiedVehicle = NULL;
            m_pOccupyingPassengers [i]->m_pOccupyingVehicle = NULL;
            m_pOccupyingPassengers [i]->m_uiOccupiedVehicleSeat = 0;

            m_pOccupyingPassengers [i]->SetVehicleInOutState ( VEHICLE_INOUT_NONE );
        }
    }

    // Remove us from the vehicle list
    Unlink ();

	delete m_pUpgrades;
}


void CClientVehicle::Unlink ( void )
{
    m_pVehicleManager->RemoveFromList ( this );
    m_pVehicleManager->m_Attached.remove ( this );
    m_pVehicleManager->m_StreamedIn.remove ( this );
}


RpClump * CClientVehicle::GetClump ( void )
{
    if ( m_pVehicle )
    {
        return m_pVehicle->GetRpClump ();
    }
    return NULL;
}


void CClientVehicle::GetName ( char* szBuf )
{
    // Get the name
    const char* szName = m_pModelInfo->GetNameIfVehicle ();
    if ( szName )
    {
        strcpy ( szBuf, szName );
    }
    else
    {
        // Shouldn't happen, copy over an empty string
        szBuf[0] = '\0';
    }
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


void CClientVehicle::SetRoll ( const CVector &vecRoll )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetRoll ( const_cast < CVector* > ( &vecRoll ) );
    }
    m_Matrix.vRight = vecRoll;
    m_matFrozen.vRight = vecRoll;
}


void CClientVehicle::SetDirection ( const CVector &vecDir )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetDirection ( const_cast < CVector* > ( &vecDir ) );
    }
    m_Matrix.vFront = vecDir;
    m_matFrozen.vFront = vecDir;
}

void CClientVehicle::SetWas ( const CVector &vecWas )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetWas ( const_cast < CVector* > ( &vecWas ) );
    }
    m_Matrix.vUp = vecWas;
    m_matFrozen.vUp = vecWas;
}


void CClientVehicle::SetPosition ( const CVector& vecPosition )
{
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
    if ( m_pDriver ) m_pDriver->SetPosition ( vecPosition );
    for ( int i = 0; i < ( sizeof ( m_pPassengers ) / sizeof ( CClientPed * ) ) ; i++ )
    {
        if ( m_pPassengers [ i ] )
        {
            m_pPassengers [ i ]->SetPosition ( vecPosition );
        }
    }

    // Reset interpolation
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


void CClientVehicle::SetRotationDegrees ( const CVector& vecRotation )
{
    // Convert from degrees to radians
    CVector vecTemp;
    vecTemp.fX = vecRotation.fX * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fY = vecRotation.fY * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fZ = vecRotation.fZ * 3.1415926535897932384626433832795f / 180.0f;

    // Set the rotation as radians
    SetRotationRadians ( vecTemp );
}


void CClientVehicle::SetRotationRadians ( const CVector& vecRotation )
{
    // Grab the matrix, apply the rotation to it and set it again
    // ChrML: We flip the actual rotation direction so that the rotation is consistent with
    //        objects and players.
    CMatrix matTemp;
    GetMatrix ( matTemp );
    g_pMultiplayer->ConvertEulerAnglesToMatrix ( matTemp, ( 2 * PI ) - vecRotation.fX, ( 2 * PI ) - vecRotation.fY, ( 2 * PI ) - vecRotation.fZ );
    SetMatrix ( matTemp );

    // Reset target rotatin
    RemoveTargetRotation ();
}


void CClientVehicle::ReportMissionAudioEvent ( unsigned short usSound )
{
	if ( m_pVehicle )
    {
//		g_pGame->GetAudio ()->ReportMissionAudioEvent ( m_pVehicle, usSound );
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

    unsigned char ucDoorStates [ MAX_DOORS ];
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


void CClientVehicle::GetColor ( unsigned char& ucColor1, unsigned char& ucColor2, unsigned char& ucColor3, unsigned char& ucColor4 )
{
    if ( m_pVehicle )
    {
        m_pVehicle->GetColor ( &ucColor1, &ucColor2, &ucColor3, &ucColor4 );
    }
    else
    {
        ucColor1 = m_ucColor1;
        ucColor2 = m_ucColor2;
        ucColor3 = m_ucColor3;
        ucColor4 = m_ucColor4;
    }
}


void CClientVehicle::SetColor ( unsigned char ucColor1, unsigned char ucColor2, unsigned char ucColor3, unsigned char ucColor4 )
{
    if ( m_pVehicle )
    {
	    m_pVehicle->SetColor ( ucColor1, ucColor2, ucColor3, ucColor4 );
    }
    m_ucColor1 = ucColor1;
    m_ucColor2 = ucColor2;
    m_ucColor3 = ucColor3;
    m_ucColor4 = ucColor4;
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

        // Set the new vehicle id and type
        m_usModel = usModel;
        m_eVehicleType = CClientVehicleManager::GetVehicleType ( usModel );
        m_bHasDamageModel = CClientVehicleManager::HasDamageModel ( m_eVehicleType );

        // Check if we have landing gears and adjustable properties
        m_bHasLandingGear = DoCheckHasLandingGear ();
        m_bHasAdjustableProperty = CClientVehicleManager::HasAdjustableProperty ( m_usModel );
        m_usAdjustablePropertyValue = 0;

        // Grab the model info and the bounding box
        m_pModelInfo = g_pGame->GetModelInfo ( usModel );
        m_ucMaxPassengers = CClientVehicleManager::GetMaxPassengerCount ( usModel );

        // Create the vehicle if we're streamed in
        if ( IsStreamedIn () )
        {
            // Create the vehicle now. Don't prerequest the model ID for this func.
            Create ();
        }
    }
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
//    if ( !m_bScriptCanBeDamaged )
//        bTyresCanBurst = false;

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
        if ( m_usModel == VT_HYDRA )
        {
            usPropertyValue = 5000 - usPropertyValue;
        }
    }
    else
    {
        usPropertyValue = m_usAdjustablePropertyValue;
    }

    // Return it
    return usPropertyValue;
}


void CClientVehicle::SetAdjustablePropertyValue ( unsigned short usAdjustableProperty )
{
    if ( m_usModel == VT_HYDRA )
    {
        usAdjustableProperty = 5000 - usAdjustableProperty;
    }

    // Set it
    if ( m_pVehicle )
    {
        if ( m_bHasAdjustableProperty )
        {
            m_pVehicle->SetAdjustablePropertyValue ( usAdjustableProperty );
        }
    }
    m_usAdjustablePropertyValue = usAdjustableProperty;
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
            // Make sure our tyres can burst
            m_pVehicle->SetTyresDontBurst ( false );

            // Is our new status a burst tyre? and do we need to call BurstTyre?
            if ( ucStatus == DT_WHEEL_BURST && !bSilent ) m_pVehicle->BurstTyre ( ucWheel );

            // Do we have a damage model?
            if ( HasDamageModel () )
                m_pVehicle->GetDamageManager ()->SetWheelStatus ( static_cast < eWheels > ( ucWheel ), ucStatus );
            else if ( m_eVehicleType == CLIENTVEHICLE_BIKE && ucWheel < 2 )
                m_pVehicle->SetBikeWheelStatus ( ucWheel, ucStatus );

            // Restore our tyre-burst flag
            CalcAndUpdateTyresCanBurstFlag ();
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


float CClientVehicle::GetHelicopterRotorSpeed ( void )
{
    if ( m_pVehicle && m_eVehicleType == CLIENTVEHICLE_HELI )
    {
        return m_pVehicle->GetHelicopterRotorSpeed ();
    }

    return 0;
}


void CClientVehicle::SetHelicopterRotorSpeed ( float fSpeed )
{
    if ( m_pVehicle && m_eVehicleType == CLIENTVEHICLE_HELI )
    {
        m_pVehicle->SetHelicopterRotorSpeed ( fSpeed );
    }
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
        if ( m_eVehicleType == CLIENTVEHICLE_CAR || 
             m_eVehicleType == CLIENTVEHICLE_HELI ||
             m_eVehicleType == CLIENTVEHICLE_PLANE ||
             m_eVehicleType == CLIENTVEHICLE_MONSTERTRUCK ||
             m_eVehicleType == CLIENTVEHICLE_QUADBIKE ||
             m_eVehicleType == CLIENTVEHICLE_TRAILER )
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

        // Vehicles have a way of getting their cols back, so we have to force this each frame (not much overhead anyway)
        m_pVehicle->SetUsesCollision ( m_bIsCollisionEnabled );

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
                 ( g_pGame->GetWorld ()->HasCollisionBeenLoaded ( &m_matFrozen.vPos ) &&
                   m_pObjectManager->ObjectsAroundPointLoaded ( m_matFrozen.vPos, 200.0f, m_usDimension ) ) )
            {
                // Remember the matrix
                m_pVehicle->GetMatrix ( &m_matFrozen );
            }
            else
            {
                // Force the position to the last remembered matrix (..and make sure gravity doesn't pull it down)
                m_pVehicle->SetMatrix ( &m_matFrozen );
                m_pVehicle->SetMoveSpeed ( &CVector ( 0.0f, 0.0f, 0.0f ) );

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

        Interpolate ();

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
    }
}


void CClientVehicle::StreamIn ( bool bInstantly )
{
    // We need to create now?
    if ( bInstantly )
    {
        // Request its model blocking
        if ( m_pModelRequester->RequestBlocking ( m_usModel ) )
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
    m_pModelRequester->Cancel ( this );
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
        m_pModelInfo->AddRef ( true );

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
            m_pVehicle = g_pGame->GetPools ()->AddVehicle ( static_cast < eVehicleTypes > ( m_usModel ) );
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
        
        // Jump straight to the target position if we have one
        if ( m_bHasTargetPosition )
        {
            m_Matrix.vPos = m_vecTargetPosition;
        }

        // Jump straight to the target rotation if we have one
        if ( m_bHasTargetRotation )
        {
            CVector vecTemp = m_vecTargetRotation;
            ConvertDegreesToRadians ( vecTemp );
            g_pMultiplayer->ConvertEulerAnglesToMatrix ( m_Matrix, ( 2 * PI ) - vecTemp.fX, ( 2 * PI ) - vecTemp.fY, ( 2 * PI ) - vecTemp.fZ );
        }

        // Got any settings to restore?
        m_pVehicle->SetMatrix ( &m_Matrix );
        m_pVehicle->SetMoveSpeed ( &m_vecMoveSpeed );
        m_pVehicle->SetTurnSpeed ( &m_vecTurnSpeed );
        m_pVehicle->SetVisible ( m_bVisible );
        m_pVehicle->SetUsesCollision ( m_bIsCollisionEnabled );
        m_pVehicle->SetEngineBroken ( m_bEngineBroken );
        m_pVehicle->SetSirenOrAlarmActive ( m_bSireneOrAlarmActive );
        SetLandingGearDown ( m_bLandingGearDown );
        m_pVehicle->SetAdjustablePropertyValue ( m_usAdjustablePropertyValue );
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

        // Re-add all the upgrades
        if ( m_pUpgrades )
            m_pUpgrades->ReAddAll ();

        m_pVehicle->SetOverrideLights ( m_ucOverrideLights );
        m_pVehicle->SetRemap ( static_cast < unsigned int > ( m_ucPaintjob ) );
        m_pVehicle->SetBodyDirtLevel ( m_fDirtLevel );
        m_pVehicle->SetEngineOn ( m_bEngineOn );
        m_pVehicle->SetAreaCode ( m_ucInterior );
        m_pVehicle->SetSmokeTrailEnabled ( m_bSmokeTrail );
        m_pVehicle->SetGravity ( &m_vecGravity );
        m_pVehicle->SetHeadLightColor ( m_ucHeadLightR, m_ucHeadLightG, m_ucHeadLightB );

        // Check the paintjob hasn't reset our colors
        if ( m_bColorSaved )
        {
            unsigned char ucColor1, ucColor2, ucColor3, ucColor4;
            m_pVehicle->GetColor ( &ucColor1, &ucColor2, &ucColor3, &ucColor4 );
            if ( ucColor1 != m_ucColor1 || ucColor2 != m_ucColor2 ||
                 ucColor3 != m_ucColor3 || ucColor4 != m_ucColor4 )
            {
                m_pVehicle->SetColor ( m_ucColor1, m_ucColor2, m_ucColor3, m_ucColor4 );
            }
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
            m_pVehicle->SetColor ( m_ucColor1, m_ucColor2, m_ucColor3, m_ucColor4 );
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
	float f[3];
	double dCurrentTime = CClientTime::GetTimeNano ();

	// Reset the extrapolator with the pure matrix
	f[0] = m_MatrixPure.vPos.fX;
	f[1] = m_MatrixPure.vPos.fY;
	f[2] = m_MatrixPure.vPos.fZ;
//	m_Extrapolator.Reset ( dCurrentTime, dCurrentTime, f );

	// Set LERP factor to 1
	m_fLERP = LERP_UNUSED;

	// Reset the source and destination quaternions
	m_QuatA = m_QuatB = CQuat ( &m_MatrixPure );

    // Turn off interpolation for the first little bit
    m_bInterpolationEnabled = false;

    // Store the last reset time, so we know when to turn interpolation back on
    m_dResetInterpolationTime = dCurrentTime;

#ifdef MTA_DEBUG_INTERPOLATION
	g_pCore->GetGraphics()->DrawTextTTF(300,200,332,216,0xDDDDDDDD, "RESET", 1.0f, 0);
#endif
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


void CClientVehicle::GetInitialDoorStates ( unsigned char * pucDoorStates )
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
        case VT_TIGER:
        case VT_TRACTOR:
        case VT_VORTEX:
            memset ( pucDoorStates, DT_DOOR_MISSING, 6 );

            // Keep the bonet and boot intact
            pucDoorStates [ 0 ] = pucDoorStates [ 1 ] = DT_DOOR_INTACT;
            break;        
        default:
            memset ( pucDoorStates, DT_DOOR_INTACT, 6 );
    }
}


void CClientVehicle::SetTargetPosition ( CVector& vecPosition )
{   
    // Are we streamed in?
    if ( m_pVehicle )
    {
        CVector vecTemp;
        GetPosition ( vecTemp );
        m_bTargetPositionDirections [ 0 ] = ( vecTemp.fX < vecPosition.fX );
        m_bTargetPositionDirections [ 1 ] = ( vecTemp.fY < vecPosition.fY );
        m_bTargetPositionDirections [ 2 ] = ( vecTemp.fZ < vecPosition.fZ );
        m_vecTargetPosition = vecPosition;
        m_bHasTargetPosition = true;
    }
    else
    {
        // Update our position now and remove any previous target we had
        SetPosition ( vecPosition );
        m_bHasTargetPosition = false;
    }
}


void CClientVehicle::RemoveTargetPosition ( void )
{
    m_bHasTargetPosition = false;
}


void CClientVehicle::SetTargetRotation ( CVector& vecRotation )
{
    // Are we streamed in?
    if ( m_pVehicle )
    {
        // Set our target rotation
        m_vecTargetRotation = vecRotation;
        m_bHasTargetRotation = true;
    }
    else
    {
        // Update our rotation now and remove any previous target we had
        SetRotationDegrees ( vecRotation );
        m_bHasTargetRotation = false;
    }
}


void CClientVehicle::RemoveTargetRotation ( void )
{
    m_bHasTargetRotation = false;
}

float fInterpolationStrengthXY = 12;
float fInterpolationStrengthZ = 12;
float fInterpolationStrengthR = 8;

void CClientVehicle::UpdateTargetPosition ( void )
{
    // Do we have a position to move towards? and are we streamed in?
    if ( m_bHasTargetPosition && m_pVehicle )
    {
        // Grab the vehicle's current position
        CVector vecPosition, vecPreviousPosition;
        GetPosition ( vecPosition );
        vecPreviousPosition = vecPosition;

        // Grab the x, y and z distance between target and the real position
        CVector vecOffset = m_vecTargetPosition - vecPosition;

        // Grab the distance to between current point and real point
        float fDistance = DistanceBetweenPoints3D ( m_vecTargetPosition, vecPosition );

        /* Incredibly slow code
        // Is there anything blocking our path to the target position?
        CColPoint* pColPoint = NULL;
        CEntity* pEntity = NULL;
        bool bCollision = g_pGame->GetWorld ()->ProcessLineOfSight ( &vecPosition,
                                                                     &m_vecTargetPosition,
                                                                     &pColPoint,
                                                                     &pEntity,
                                                                     true, false, false, true,
                                                                     false, false, false, false );

        // Destroy the colpoint or we get a leak
        if ( pColPoint ) pColPoint->Destroy ();
        */

        // If the distance is above our warping threshold
        if ( fDistance > INTERPOLATION_WARP_THRESHOLD )
        {
            // Warp to the target
            vecPosition = m_vecTargetPosition;
            if ( m_bHasTargetRotation )
                SetRotationDegrees ( m_vecTargetRotation );
        }
        else
        {
            // Calculate how much to interpolate and add it as long as this is the direction we're interpolating
            vecOffset /= CVector ( fInterpolationStrengthXY, fInterpolationStrengthXY, fInterpolationStrengthZ );
            if ( ( vecOffset.fX > 0.0f ) == m_bTargetPositionDirections [ 0 ] )
                vecPosition.fX += vecOffset.fX;
            if ( ( vecOffset.fY > 0.0f ) == m_bTargetPositionDirections [ 1 ] )
                vecPosition.fY += vecOffset.fY;
            if ( ( vecOffset.fZ > 0.0f ) == m_bTargetPositionDirections [ 2 ] )
                vecPosition.fZ += vecOffset.fZ;
        }

        // Set the new position
        m_pVehicle->SetPosition ( const_cast < CVector* > ( &vecPosition ) );

        // Update our contact players
        CVector vecPlayerPosition;
        list < CClientPed * > ::iterator iter = m_Contacts.begin ();
        for ( ; iter != m_Contacts.end () ; iter++ )
        {
            CClientPed * pModel = *iter;
            pModel->GetPosition ( vecPlayerPosition );                
            vecOffset = vecPlayerPosition - vecPreviousPosition;
            vecPlayerPosition = vecPosition + vecOffset;
            pModel->SetPosition ( vecPlayerPosition );
        }
    }
}


void CClientVehicle::UpdateTargetRotation ( void )
{
    // Do we have a rotation to move towards? and are we streamed in?
    if ( m_bHasTargetRotation && m_pVehicle )
    {
        CVector vecRotation;
        GetRotationDegrees ( vecRotation );

        CVector vecOffset;
        vecOffset.fX = GetOffsetDegrees ( vecRotation.fX, m_vecTargetRotation.fX );
        vecOffset.fY = GetOffsetDegrees ( vecRotation.fY, m_vecTargetRotation.fY );
        vecOffset.fZ = GetOffsetDegrees ( vecRotation.fZ, m_vecTargetRotation.fZ );

        vecOffset /= CVector ( fInterpolationStrengthR, fInterpolationStrengthR, fInterpolationStrengthR );
        vecRotation += vecOffset;

        SetRotationDegrees ( vecRotation );
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


void CClientVehicle::GetHeadLightColor ( unsigned char & ucR, unsigned char & ucG, unsigned char & ucB )
{
    if ( m_pVehicle )
    {
        m_pVehicle->GetHeadLightColor ( ucR, ucG, ucB );
    }
    else ucR = m_ucHeadLightR, ucG = m_ucHeadLightG, ucB = m_ucHeadLightB;
}


void CClientVehicle::SetHeadLightColor ( unsigned char & ucR, unsigned char & ucG, unsigned char & ucB )
{
    if ( m_pVehicle )
    {
        m_pVehicle->SetHeadLightColor ( ucR, ucG, ucB );
    }
    m_ucHeadLightR = ucR, m_ucHeadLightG = ucG, m_ucHeadLightB = ucB;
}