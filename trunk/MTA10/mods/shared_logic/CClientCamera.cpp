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

// Very hacky
//#include "../deathmatch/logic/CClientGame.h"

#define PI_2 6.283185307179586476925286766559f
#ifndef PI
#define PI 3.1415926535897932384626433832795f
#endif

CClientCamera::CClientCamera ( CClientManager* pManager ) : CClientEntity ( INVALID_ELEMENT_ID )
{
    // Init
    m_pManager = pManager;
    m_pPlayerManager = m_pManager->GetPlayerManager ();

    m_pFocusedPlayer = NULL;
    m_pFocusedEntity = NULL;
    m_pFocusedGameEntity = NULL;
    m_bInvalidated = false;
    m_bFixed = false;

    m_pCamera = g_pGame->GetCamera ();

    // Hook handler for the fixed camera
    g_pMultiplayer->SetProcessCamHandler ( CClientCamera::ProcessFixedCamera );
}


CClientCamera::~CClientCamera ( void )
{
    // We need to be ingame
    if ( g_pGame->GetSystemState () == 9 )
    {
        // Restore the camera to the local player
        SetFocusToLocalPlayerImpl ();
    }
}


void CClientCamera::DoPulse ( void )
{
    // If we aren't invalidated
    if ( !m_bInvalidated )
    {
        // If we aren't in fixed mode
        if ( !m_bFixed )
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
                    static_cast < CClientVehicle* > ( m_pFocusedEntity )->GetRotationRadians ( vecVehicleRotation );
                    fRotation = vecVehicleRotation.fZ * 3.14159f / 180;
                }
                else if ( eType == CCLIENTPED || eType == CCLIENTPLAYER )
                {
                    fRotation = static_cast < CClientPed* > ( m_pFocusedEntity )->GetCurrentRotation ();
                }
            }

            // Set the new world center/rotation
            g_pMultiplayer->SetCenterOfWorld ( NULL, m_pFocusedGameEntity->GetPosition (), fRotation );
        }
    }

    // If we're fixed, force the target vector
    if ( m_bFixed )
    {
        // Make sure the world center/rotation is where the camera is
        CVector vecRotation;
        CMatrix matTemp;
        GetMatrix ( matTemp );
        g_pMultiplayer->ConvertMatrixToEulerAngles ( matTemp, vecRotation.fX, vecRotation.fY, vecRotation.fZ );    
        g_pMultiplayer->SetCenterOfWorld ( NULL, &m_vecFixedPosition, 3.1415926535897932384626433832795f - vecRotation.fZ );
    }
}


bool CClientCamera::GetMatrix ( CMatrix& Matrix ) const
{
    m_pCamera->GetMatrix ( &Matrix );
    return true;
}


void CClientCamera::GetPosition ( CVector& vecPosition ) const
{
    if ( m_bFixed )
    {
        vecPosition = m_vecFixedPosition;
    }
    else
    {
        CMatrix matTemp;
        m_pCamera->GetMatrix ( &matTemp );
        vecPosition = matTemp.vPos;
    }
}


void CClientCamera::SetPosition ( const CVector& vecPosition )
{
    // Make sure that's where the world center is
    CVector vecRotation;
    CMatrix matTemp;
    GetMatrix ( matTemp );
    g_pMultiplayer->ConvertMatrixToEulerAngles ( matTemp, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
	CVector * v = & ( const_cast < CVector& > ( vecPosition ) );
    g_pMultiplayer->SetCenterOfWorld ( NULL, v, 3.1415926535897932384626433832795f - vecRotation.fZ );

    // Store the position so it can be updated from our hook
    m_vecFixedPosition = vecPosition;
}


void CClientCamera::GetRotation ( CVector& vecRotation ) const
{
    CCam* pCam = m_pCamera->GetCam ( m_pCamera->GetActiveCam () );
    CMatrix matrix;
    m_pCamera->GetMatrix ( &matrix );
    g_pMultiplayer->ConvertMatrixToEulerAngles ( matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    ConvertRadiansToDegrees ( vecRotation );
    vecRotation += CVector ( 180.0f, 180.0f, 180.0f );
    if ( vecRotation.fX > 360.0f ) vecRotation.fX -= 360.0f;
    if ( vecRotation.fY > 360.0f ) vecRotation.fY -= 360.0f;
    if ( vecRotation.fZ > 360.0f ) vecRotation.fZ -= 360.0f;
}


void CClientCamera::SetRotation ( const CVector& vecRotation )
{
    // Rotate a 1000,0,0 vector with the given rotation vector
    CVector vecRotationCopy = vecRotation;
    vecRotationCopy.fX = 0.0f;
    CVector vecNormal = CVector ( 1000.0f, 0.0f, 0.0f );
    RotateVector ( vecNormal, vecRotationCopy );

    // Add it with our current position
    CVector vecPosition;
    GetPosition ( vecPosition );
    vecPosition += vecNormal;

    // Set the calculated vector as the target
    SetTarget ( vecPosition );
}


void CClientCamera::GetTarget ( CVector & vecTarget ) const
{
    if ( m_bFixed ) vecTarget = m_vecFixedTarget;
    else
    {
        CMatrix matTemp;
        m_pCamera->GetMatrix ( &matTemp );

        CCam* pCam = m_pCamera->GetCam ( m_pCamera->GetActiveCam () );
        vecTarget = matTemp.vPos + *pCam->GetFront ();
    }
}


void CClientCamera::SetTarget ( const CVector& vecPosition )
{
    // Store the target so it can be updated from our hook
    m_vecFixedTarget = vecPosition;
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
            static_cast < CClientStreamElement* > ( m_pFocusedEntity )->RemoveStreamReference ();

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
        static_cast < CClientStreamElement* > ( m_pFocusedEntity )->RemoveStreamReference ();

    // Reset
    m_pFocusedPlayer = NULL;
    m_pFocusedEntity = NULL;
    m_pFocusedGameEntity = NULL;
    m_bFixed = false;
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
            static_cast < CClientStreamElement* > ( m_pFocusedEntity )->RemoveStreamReference ();

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


bool CClientCamera::SetCameraMode ( eCamMode eMode )
{
    return m_pCamera->TryToStartNewCamMode ( eMode );
}


inline float vecsize3d ( float fX, float fY, float fZ )
{
 	return sqrt ( (fX*fX) + (fY*fY) + (fZ*fZ) );
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
        SetFocus ( &m_vecFixedPosition, false );
    }
    else
    {
        g_pMultiplayer->SetCenterOfWorld ( NULL, NULL, NULL );
        SetFocusToLocalPlayer();
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


void CClientCamera::ProcessFixedCamera ( CCam* pCam )
{
    // The purpose of this handler function is changing the Source, Front and Up vectors in CCam
    // when called by GTA. This is called when we are in fixed camera mode.

    // Grab this camera interface and the position it's supposed to be at and what
    // it should look at.
    CClientCamera* pThis = g_pClientGame->GetManager ()->GetCamera ();
    const CVector& vecPosition = pThis->m_vecFixedPosition;
    const CVector& vecTarget = pThis->m_vecFixedTarget;

    // Set the position in the CCam interface
    *pCam->GetSource () = vecPosition;

    // Calculate the front vector, target - position. If it's length is 0 we'll get problems
    // ie if position and target are the same, so make a new vector then looking horizontally
    CVector vecFront = vecTarget - vecPosition;
    if ( vecFront.Length () == 0 )
        vecFront = CVector ( 1.0, 0.0f, 0.0f );

    // Grab the right vector and normalize it. We assume the camera can't roll.
    CVector vecRight = CVector ( -vecFront.fY, vecFront.fX, 0 );
    vecRight.Normalize ();

    // Normalize the front vector and set it
    vecFront.Normalize ();
    *pCam->GetFront () = vecFront;

    // Grab the up vector in the CCam
    CVector* pCamUp = pCam->GetUp ();

    // Is the right vector valid length?
    CVector vecUp;
    if ( vecRight.Length () != 0 )
    {
        // Cross multiply it with front.
        // TODO: Implement .cross in CVector
        vecUp.fX = vecFront.fY * vecRight.fZ - vecFront.fZ * vecRight.fY;
        vecUp.fY = vecFront.fZ * vecRight.fX - vecFront.fX * vecRight.fZ ;
        vecUp.fZ = vecFront.fX * vecRight.fY - vecFront.fY * vecRight.fX;
    }
    else
    {
        // Use the existing X, Y parts of the up vector to avoid whitescreen if
        // right is invalid (looking straight up or down)
        vecUp = CVector ( pCamUp->fX, pCamUp->fY, 0 );
    }

    // Normalize it
    vecUp.Normalize ();

    // Is it valid? It will become invalid if we look horizontally then look straight
    // down, so we do a check here and fake it if it's invalid
    if ( vecUp.Length () == 0 )
        vecUp = CVector ( 1, 0, 0 );

    // Set the up vector in CCam
    *pCamUp = vecUp;
}
