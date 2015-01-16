/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCamera.cpp
*  PURPOSE:     Camera entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#include <StdInc.h>

#define PI_2 6.283185307179586476925286766559f

CClientCamera::CClientCamera ( CClientManager* pManager ) : ClassInit ( this ), CClientEntity ( INVALID_ELEMENT_ID )
{
    CClientEntityRefManager::AddEntityRefs ( ENTITY_REF_DEBUG ( this, "CClientCamera" ), &m_pFocusedPlayer, &m_pFocusedEntity, NULL );

    // Init
    m_pManager = pManager;
    m_pPlayerManager = m_pManager->GetPlayerManager ();

    m_pFocusedPlayer = NULL;
    m_pFocusedEntity = NULL;
    m_pFocusedGameEntity = NULL;
    m_bInvalidated = false;
    m_bFixed = false;
    m_fRoll = 0.0f;
    m_fFOV = 70.0f;
    SetTypeName( "camera" );

    m_pCamera = g_pGame->GetCamera ();

    // Hook handler for the fixed camera
    g_pMultiplayer->SetProcessCamHandler ( CClientCamera::StaticProcessFixedCamera );

    m_FixedCameraMode = EFixedCameraMode::ROTATION;
}


CClientCamera::~CClientCamera ( void )
{
    // We need to be ingame
    if ( g_pGame->GetSystemState () == 9 )
    {
        // Restore the camera to the local player
        SetFocusToLocalPlayerImpl ();
    }
    CClientEntityRefManager::RemoveEntityRefs ( 0, &m_pFocusedPlayer, &m_pFocusedEntity, NULL );
}


void CClientCamera::DoPulse ( void )
{   
    // If we're fixed, force the target vector
    if ( m_bFixed )
    {
        // Make sure the world center/rotation is where the camera is
        CVector vecRotation;
        CMatrix matTemp;
        GetMatrix ( matTemp );
        g_pMultiplayer->ConvertMatrixToEulerAngles ( matTemp, vecRotation.fX, vecRotation.fY, vecRotation.fZ );    
        g_pMultiplayer->SetCenterOfWorld ( NULL, &m_matFixedMatrix.vPos, 3.1415926535897932384626433832795f - vecRotation.fZ );
    }
    else
    {
        // If we aren't invalidated
        if ( !m_bInvalidated )
        {
            // If we got the camera behind a player but no focused entity
            if ( m_pFocusedPlayer )
            {
                if ( m_pFocusedEntity )
                {
                    // Is the focused entity a vehicle, but the player doesn't have any occupied?
                    CClientVehicle* pVehicle = m_pFocusedPlayer->GetOccupiedVehicle ();
                    if ( m_pFocusedEntity->GetType () == CCLIENTVEHICLE )
                    {
                        if ( !pVehicle )
                        {
                            SetFocus ( m_pFocusedPlayer, MODE_BEHINDCAR );
                        }
                    }
                    else if ( pVehicle )
                    {
                        SetFocus ( m_pFocusedPlayer, MODE_BEHINDCAR );
                    }
                }
                else
                {
                    SetFocus ( m_pFocusedPlayer, MODE_BEHINDCAR );
                }
            }

            // Make sure the world center is where the camera is
            if ( m_pFocusedGameEntity )
            {
                // Make sure we have the proper rotation for what we're spectating
                float fRotation = 0;
                if ( m_pFocusedEntity )
                {
                    eClientEntityType eType = m_pFocusedEntity->GetType ();
                    if ( eType == CCLIENTVEHICLE )
                    {
                        CVector vecVehicleRotation;
                        static_cast < CClientVehicle* > ( (CClientEntity*)m_pFocusedEntity )->GetRotationRadians ( vecVehicleRotation );
                        fRotation = vecVehicleRotation.fZ * 3.14159f / 180;
                    }
                    else if ( eType == CCLIENTPED || eType == CCLIENTPLAYER )
                    {
                        fRotation = static_cast < CClientPed* > ( (CClientEntity*)m_pFocusedEntity )->GetCurrentRotation ();
                    }
                }

                // Set the new world center/rotation
                g_pMultiplayer->SetCenterOfWorld ( NULL, m_pFocusedGameEntity->GetPosition (), fRotation );
            }
        }

        // Save this so position or rotation is preserved when changing to fixed mode
        m_matFixedMatrix = GetGtaMatrix();
    }
}


bool CClientCamera::GetMatrix ( CMatrix& Matrix ) const
{
    if ( m_bFixed )
        Matrix = m_matFixedMatrix;
    else
        Matrix = GetGtaMatrix();
    return true;
}


bool CClientCamera::SetMatrix ( const CMatrix& Matrix )
{
    // Switch to fixed mode if required
    if ( !IsInFixedMode () )        
        ToggleCameraFixedMode ( true );

    m_matFixedMatrix = Matrix;
    m_matFixedMatrix.OrthoNormalize( CMatrix::AXIS_FRONT, CMatrix::AXIS_UP );
    m_FixedCameraMode = EFixedCameraMode::MATRIX;
    SetPosition( m_matFixedMatrix.vPos );   // To update center of world
    return true;
}


void CClientCamera::GetPosition ( CVector& vecPosition ) const
{
    CMatrix matTemp;
    GetMatrix( matTemp );
    vecPosition = matTemp.vPos;
}


void CClientCamera::SetPosition ( const CVector& vecPosition )
{
    // Switch to fixed mode if required
    if ( !IsInFixedMode () )        
        ToggleCameraFixedMode ( true );

    // Make sure that's where the world center is
    CVector vecRotation;
    CMatrix matTemp;
    GetMatrix ( matTemp );
    g_pMultiplayer->ConvertMatrixToEulerAngles ( matTemp, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    g_pMultiplayer->SetCenterOfWorld ( NULL, (CVector*)&vecPosition, 3.1415926535897932384626433832795f - vecRotation.fZ );

    // Store the position so it can be updated from our hook
    m_matFixedMatrix.vPos = vecPosition;
}


void CClientCamera::GetRotationDegrees ( CVector& vecRotation ) const
{
    CMatrix matrix;
    GetMatrix( matrix );

    matrix.OrthoNormalize( CMatrix::AXIS_FRONT, CMatrix::AXIS_UP );
    g_pMultiplayer->ConvertMatrixToEulerAngles ( matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    vecRotation = CVector ( 2*PI, 2*PI, 2*PI ) - vecRotation;
    ConvertRadiansToDegrees ( vecRotation );
    // srsly, f knows, just pretend you never saw this line
    // vecRotation.fY = 360.0f - vecRotation.fY;    // Removed as caused problems with: Camera.rotation = Camera.rotation
}


void CClientCamera::SetRotationRadians ( const CVector& vecRotation )
{
    // Switch to fixed mode if required
    if ( !IsInFixedMode () )        
        ToggleCameraFixedMode ( true );

    CVector vecUseRotation = CVector ( 2*PI, 2*PI, 2*PI ) - vecRotation;
    m_FixedCameraMode = EFixedCameraMode::ROTATION;

    // Set rotational part of fixed matrix
    CMatrix newMatrix;
    g_pMultiplayer->ConvertEulerAnglesToMatrix( newMatrix, vecUseRotation.fX, vecUseRotation.fY, vecUseRotation.fZ );
    m_matFixedMatrix.vUp = newMatrix.vUp;
    m_matFixedMatrix.vFront = newMatrix.vFront;
    m_matFixedMatrix.vRight = newMatrix.vRight;
    m_matFixedMatrix.OrthoNormalize( CMatrix::AXIS_FRONT, CMatrix::AXIS_UP );
}


void CClientCamera::GetFixedTarget ( CVector & vecTarget, float* pfRoll ) const
{
    if ( m_bFixed && m_FixedCameraMode == EFixedCameraMode::TARGET )
    {
        // Use supplied target vector and roll if was set
        if ( pfRoll )
            *pfRoll = m_fRoll;
        vecTarget = m_vecFixedTarget;
    }
    else
    {
        if ( pfRoll )
            *pfRoll = 0;
        CMatrix matTemp;
        GetMatrix( matTemp );
        vecTarget = matTemp.vPos + matTemp.vFront;
    }
}


void CClientCamera::SetFixedTarget ( const CVector& vecPosition, float fRoll )
{
    // Switch to fixed mode if required
    if ( !IsInFixedMode () )        
        ToggleCameraFixedMode ( true );

    // Store the target so it can be updated from our hook
    m_vecFixedTarget = vecPosition;
    m_fRoll = fRoll;
    m_FixedCameraMode = EFixedCameraMode::TARGET;

    // Update fixed matrix
    // Calculate the front vector, target - position. If its length is 0 we'll get problems
    // (i.e. if position and target are the same), so make a new vector then looking horizontally
    CVector vecFront = m_vecFixedTarget - m_matFixedMatrix.vPos;
    if ( vecFront.Length () < FLOAT_EPSILON )
        vecFront = CVector ( 0.0, 1.0f, 0.0f );
    else
        vecFront.Normalize ();

    // Grab the right vector
    CVector vecRight = CVector ( vecFront.fY, -vecFront.fX, 0 );
    if ( vecRight.Length () < FLOAT_EPSILON )
        vecRight = CVector ( 1.0f, 0.0f, 0.0f );
    else
        vecRight.Normalize ();

    // Calculate the up vector from this
    CVector vecUp = vecRight;
    vecUp.CrossProduct ( &vecFront );
    vecUp.Normalize ();

    // Apply roll if needed
    if ( m_fRoll != 0.0f )
    {
        float fRoll = ConvertDegreesToRadiansNoWrap ( m_fRoll );
        vecUp = vecUp*cos(fRoll) - vecRight*sin(fRoll);
    }

    // Set rotational part of fixed matrix
    m_matFixedMatrix.vFront = vecFront;
    m_matFixedMatrix.vUp = vecUp;
    m_matFixedMatrix.OrthoNormalize( CMatrix::AXIS_FRONT, CMatrix::AXIS_UP );
}


//
// Make player 'orbit camera' rotate to face this point
//
void CClientCamera::SetOrbitTarget ( const CVector& vecPosition )
{
    if ( m_pCamera )
    {
        CVector vecPlayerPosition;
        g_pClientGame->GetLocalPlayer ()->GetPosition ( vecPlayerPosition );

        float fDistance = (vecPosition - vecPlayerPosition).Length ();
        float fAngleHorz = -atan2 ( vecPosition.fX - vecPlayerPosition.fX, vecPosition.fY - vecPlayerPosition.fY ) - PI/2;
        float fAngleVert = atan2 ( vecPosition.fZ - vecPlayerPosition.fZ, fDistance );

        CCam* pCam = m_pCamera->GetCam ( m_pCamera->GetActiveCam () );
        pCam->SetDirection ( fAngleHorz, fAngleVert );
    }
}


void CClientCamera::AttachTo ( CElement* pElement )
{
    if ( pElement )
    {
        // Switch to fixed mode if required
        if ( !IsInFixedMode () )        
            ToggleCameraFixedMode ( true );
    }

    CClientEntity::AttachTo( pElement );
}


void CClientCamera::FadeIn ( float fTime )
{
    m_pCamera->Fade ( fTime, FADE_IN );
}


void CClientCamera::FadeOut ( float fTime, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    m_pCamera->SetFadeColor ( ucRed, ucGreen, ucBlue );
    m_pCamera->Fade ( fTime, FADE_OUT );
}


void CClientCamera::SetFocus ( CClientEntity* pEntity, eCamMode eMode, bool bSmoothTransition )
{
    // Should we attach to an entity, or set the camera to free?
    if ( pEntity )
    {
        eClientEntityType eType = pEntity->GetType ();

        // Remove stream reference from the previous target
        if ( m_pFocusedEntity && m_pFocusedEntity->IsStreamingCompatibleClass () )
            static_cast < CClientStreamElement* > ( (CClientEntity*)m_pFocusedEntity )->RemoveStreamReference ();

        // Add stream reference for our new target
        if ( pEntity && pEntity->IsStreamingCompatibleClass () )
            static_cast < CClientStreamElement* > ( pEntity)->AddStreamReference ();

        CEntity* pGameEntity = pEntity->GetGameEntity (); 
        if ( pGameEntity )
        {
            // Set the switch style
            int iSwitchStyle = 2;
            if ( bSmoothTransition )
            {
                iSwitchStyle = 1;
            }

            // Hacky, used to follow peds
            //if ( eMode == MODE_CAM_ON_A_STRING && eType == CCLIENTPLAYERMODEL )
            if ( eMode == MODE_BEHINDCAR && ( eType == CCLIENTPED || eType == CCLIENTPLAYER ) )
                eMode = MODE_FOLLOWPED;

            // Do it
            m_pCamera->TakeControl ( pGameEntity, eMode, iSwitchStyle );            

            // Store
            m_pFocusedEntity = pEntity;
            m_pFocusedGameEntity = pGameEntity;
            m_bFixed = false;
        }
    }
    else
    {
        // ## TODO ## : Don't attach to anything (free camera controlled by SetPosition and SetRotation)
        // Restore the camera to the local player at the moment
        SetFocusToLocalPlayer ();
    }

    m_fRoll = 0.0f;
    m_fFOV = 70.0f;
}


void CClientCamera::SetFocus ( CClientPlayer* pPlayer, eCamMode eMode, bool bSmoothTransition )
{
    // Should we attach to a player, or set the camera to free?
    if ( pPlayer )
    {
        // Attach the camera to the player vehicle if in a vehicle. To the player model otherwize
        CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            SetFocus ( pVehicle, eMode, bSmoothTransition );
        }
        else
        {
            // Cast it so we don't call ourselves...
            SetFocus ( static_cast < CClientEntity* > ( pPlayer ), eMode, bSmoothTransition );
        }
    }
    else
    {
        SetFocus ( reinterpret_cast < CClientEntity* > ( NULL ), MODE_NONE, bSmoothTransition );
    }

    // Store the player we focused
    m_pFocusedPlayer = pPlayer;
    m_fRoll = 0.0f;
    m_fFOV = 70.0f;
    m_bFixed = false;
}


void CClientCamera::SetFocus ( CVector * vecTarget, bool bSmoothTransition )
{
    // Grab the camera
    if ( m_pCamera )
    {
        // Set the switch style
        int iSwitchStyle = 2;
        if ( bSmoothTransition )
        {
            iSwitchStyle = 1;
        }
        // Do it
        m_pCamera->TakeControl ( vecTarget, iSwitchStyle );
        m_bFixed = true;
    }
}


void CClientCamera::SetFocusToLocalPlayer ( void )
{
    // Restore the camera
    SetFocusToLocalPlayerImpl ();

    // Remove stream reference from the previous target
    if ( m_pFocusedEntity && m_pFocusedEntity->IsStreamingCompatibleClass () )
        static_cast < CClientStreamElement* > ( (CClientEntity*)m_pFocusedEntity )->RemoveStreamReference ();

    // Reset
    m_pFocusedPlayer = NULL;
    m_pFocusedEntity = NULL;
    m_pFocusedGameEntity = NULL;
    m_bFixed = false;
    m_fRoll = 0.0f;
    m_fFOV = 70.0f;
}


void CClientCamera::SetFocusToLocalPlayerImpl ( void )
{
    // Restore the camera
    m_pCamera->RestoreWithJumpCut ();
    g_pMultiplayer->SetCenterOfWorld ( NULL, NULL, NULL );
}


void CClientCamera::UnreferenceEntity ( CClientEntity* pEntity )
{
    if ( m_pFocusedEntity == pEntity )
    {
        SetFocusToLocalPlayerImpl ();

        // Remove stream reference from the previous target
        if ( m_pFocusedEntity && m_pFocusedEntity->IsStreamingCompatibleClass () )
            static_cast < CClientStreamElement* > ( (CClientEntity*)m_pFocusedEntity )->RemoveStreamReference ();

        m_pFocusedEntity = NULL;
        m_pFocusedGameEntity = NULL;
        m_bInvalidated = false;
    }
}


void CClientCamera::UnreferencePlayer ( CClientPlayer* pPlayer )
{
    if ( m_pFocusedPlayer == pPlayer )
    {
        SetFocusToLocalPlayerImpl ();
        m_pFocusedPlayer = NULL;
    }
}


void CClientCamera::InvalidateEntity ( CClientEntity* pEntity )
{
    if ( !m_bInvalidated )
    {
        if ( m_pFocusedEntity && m_pFocusedEntity == pEntity )
        {
            SetFocusToLocalPlayerImpl ();
            m_bInvalidated = true;
        }
    }
}


void CClientCamera::RestoreEntity ( CClientEntity* pEntity )
{
    if ( m_bInvalidated )
    {
        if ( m_pFocusedEntity && m_pFocusedEntity == pEntity )
        {
            SetFocus ( pEntity, MODE_BEHINDCAR );
            m_bInvalidated = false;
        }
    }
}


void CClientCamera::SetCameraViewMode ( eVehicleCamMode eMode )
{
    m_pCamera->SetCameraViewMode ( eMode );
}

eVehicleCamMode CClientCamera::GetCameraViewMode ()
{
    return (eVehicleCamMode)m_pCamera->GetCameraViewMode();
}


void CClientCamera::SetCameraClip ( bool bObjects, bool bVehicles )
{
    m_pCamera->SetCameraClip ( bObjects, bVehicles );
}

void CClientCamera::GetCameraClip( bool &bObjects, bool &bVehicles )
{
    m_pCamera->GetCameraClip ( bObjects, bVehicles );
}

void CClientCamera::ToggleCameraFixedMode ( bool bEnabled )
{    
    if ( bEnabled )
    {
        CClientPlayer* pLocalPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer ();
        CClientVehicle* pLocalVehicle = NULL;

        // Get the local vehicle, if any
        if ( pLocalPlayer )
            pLocalVehicle = pLocalPlayer->GetOccupiedVehicle ();

        // Use the local vehicle, otherwise use the local player
        if ( pLocalVehicle )
            SetFocus ( pLocalVehicle, MODE_FIXED, false );
        else
            SetFocus ( pLocalPlayer, MODE_FIXED, false );

        // Set the target position
        SetFocus ( &m_matFixedMatrix.vPos, false );
    }
    else
    {
        g_pMultiplayer->SetCenterOfWorld ( NULL, NULL, NULL );
        SetFocusToLocalPlayer();

        m_fRoll = 0.0f;
        m_fFOV = 70.0f;
    }
}


CClientEntity * CClientCamera::GetTargetEntity ( void )
{
    CClientEntity* pReturn = NULL;
    if ( m_pCamera )
    {
        CCam* pCam = m_pCamera->GetCam ( m_pCamera->GetActiveCam () );
        CEntity * pEntity = pCam->GetTargetEntity ();
        if ( pEntity )
        {
            pReturn = m_pManager->FindEntitySafe ( pEntity );            
        }
    }
    return pReturn;
}


bool CClientCamera::StaticProcessFixedCamera ( CCam* pCam )
{
    return g_pClientGame->GetManager ()->GetCamera ()->ProcessFixedCamera( pCam );
}

bool CClientCamera::ProcessFixedCamera ( CCam* pCam )
{
    assert ( pCam );
    // The purpose of this handler function is changing the Source, Front and Up vectors in CCam
    // when called by GTA. This is called when we are in fixed camera mode.
    // Make sure we actually want to apply our custom camera position/lookat
    // (this handler could also be called from cinematic mode)
    if ( !m_bFixed )
        return false;

    // Update our position/rotation if we're attached
    DoAttaching ();

    SetGtaMatrix( m_matFixedMatrix, pCam );

    // Set the zoom
    pCam->SetFOV ( m_fFOV );

    return true;
}

//
// Return matrix being used by GTA right now
//
CMatrix CClientCamera::GetGtaMatrix ( void ) const
{
    CCam* pCam = m_pCamera->GetCam ( m_pCamera->GetActiveCam () );

    CMatrix matResult;
    m_pCamera->GetMatrix ( &matResult );
    matResult.vFront = *pCam->GetFront ();
    matResult.vUp = *pCam->GetUp ();
    matResult.vPos = *pCam->GetSource ();
    matResult.vRight = -matResult.vRight;   // Camera has this the other way
    matResult.OrthoNormalize( CMatrix::AXIS_FRONT, CMatrix::AXIS_UP );
    return matResult;
}

//
// Set matrix to be used by GTA right now
//
void CClientCamera::SetGtaMatrix ( const CMatrix& matInNew, CCam* pCam ) const
{
    if ( !pCam )
        pCam = m_pCamera->GetCam ( m_pCamera->GetActiveCam () );

    CMatrix matNew = matInNew;
    matNew.OrthoNormalize( CMatrix::AXIS_FRONT, CMatrix::AXIS_UP );
    matNew.vRight = -matNew.vRight;   // Camera has this the other way
    m_pCamera->SetMatrix( &matNew );
    *pCam->GetUp() = matNew.vUp;
    *pCam->GetFront() = matNew.vFront;
    *pCam->GetSource() = matNew.vPos;
}

