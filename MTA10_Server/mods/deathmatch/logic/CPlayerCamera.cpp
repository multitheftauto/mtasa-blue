/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerCamera.cpp
*  PURPOSE:     Player camera entity class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerCamera::CPlayerCamera ( CPlayer * pPlayer )
{
    m_pPlayer = pPlayer;    
    m_fRotation = 0.0f;
    m_ucInterior = 0;
    m_pTarget = pPlayer;

    // We start off at chilliad
    m_Mode = CAMERAMODE_FIXED;
    m_vecPosition = CVector ( -2377, -1636, 700 );
}


void CPlayerCamera::GetMode ( eCameraMode Mode, char* szBuffer, size_t sizeBuffer )
{
    switch ( Mode )
    {
        case CAMERAMODE_PLAYER:
        {
            strncpy ( szBuffer, "player", sizeBuffer );
	        szBuffer [sizeBuffer - 1] = 0;
            break;
        }

        case CAMERAMODE_FIXED:
        {
            strncpy ( szBuffer, "fixed", sizeBuffer );
			szBuffer [sizeBuffer - 1] = 0;
            break;
        }

        default:
        {
            strncpy ( szBuffer, "invalid", sizeBuffer );
			szBuffer [sizeBuffer - 1] = 0;
            break;
        }
    }
}


eCameraMode CPlayerCamera::GetMode ( const char * szMode )
{
    if ( !stricmp ( szMode, "player" ) )
        return CAMERAMODE_PLAYER;
    if ( !stricmp ( szMode, "fixed" ) )
        return CAMERAMODE_FIXED;

    return CAMERAMODE_INVALID;
}


void CPlayerCamera::GetPosition ( CVector & vecPosition )
{
    if ( m_Mode == CAMERAMODE_FIXED )
    {
        vecPosition = m_vecPosition;
    }
    else if ( m_pTarget )
    {
        vecPosition = m_pTarget->GetPosition ();
    }
}


void CPlayerCamera::SetPosition ( CVector & vecPosition )
{
    if ( m_Mode == CAMERAMODE_FIXED )
    {
        m_vecPosition = vecPosition;
    }
}


void CPlayerCamera::GetLookAt ( CVector & vecLookAt )
{
    if ( m_Mode == CAMERAMODE_FIXED )
    {
        vecLookAt = m_vecLookAt;
    }
    else
    {
        vecLookAt = CVector ();
    }
}


void CPlayerCamera::SetLookAt ( CVector & vecLookAt )
{
    if ( m_Mode == CAMERAMODE_FIXED )
    {
        m_vecLookAt = vecLookAt;
    }
}


void CPlayerCamera::SetMatrix ( CVector & vecPosition, CVector & vecLookAt )
{
    if ( m_Mode == CAMERAMODE_FIXED )
    {
        m_vecPosition = vecPosition;
        m_vecLookAt = vecLookAt;
    }
}


void CPlayerCamera::SetTarget ( CElement * pElement )
{
    // We should always have a target here
    assert ( pElement );

    if ( m_pTarget ) m_pTarget->m_FollowingCameras.remove ( this );
    if ( pElement ) pElement->m_FollowingCameras.push_back ( this );
    m_pTarget = pElement;
}


void CPlayerCamera::SetRotation ( CVector & vecRotation )
{
    // Rotate a 1000,0,0 vector with the given rotation vector
    CVector vecRotationCopy = vecRotation;
    vecRotationCopy.fX = 0.0f;
    CVector vecNormal = CVector ( 1000.0f, 0.0f, 0.0f );
    RotateVector ( vecNormal, vecRotationCopy );

    // Add it with our current position
    vecNormal += m_vecPosition;

    // Set the calculated vector as the target
    m_vecLookAt = vecNormal;
}

