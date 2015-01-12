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
    m_ucSyncTimeContext = 0;

    // We start off at chilliad
    m_Mode = CAMERAMODE_FIXED;
    m_vecPosition = CVector ( -2377, -1636, 700 );

    m_fRoll = 0.0f;
    m_fFOV = 70.0f;

    GetCameraSpatialDatabase ()->UpdateItem ( this );
}


CPlayerCamera::~CPlayerCamera ( void )
{
    GetCameraSpatialDatabase ()->RemoveItem ( this );
}


void CPlayerCamera::SetMode ( eCameraMode Mode )
{
    if ( m_Mode != Mode )
    {
        m_Mode = Mode;
        if ( m_Mode == CAMERAMODE_FIXED )
            GetCameraSpatialDatabase ()->UpdateItem ( this );
        else
            GetCameraSpatialDatabase ()->RemoveItem ( this );
    }
}

const CVector& CPlayerCamera::GetPosition ( void ) const
{
    if ( m_Mode == CAMERAMODE_PLAYER && m_pTarget )
    {
        return m_pTarget->GetPosition ();
    }
    return m_vecPosition;
}

void CPlayerCamera::GetPosition ( CVector & vecPosition ) const
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


void CPlayerCamera::SetPosition ( const CVector& vecPosition )
{
    if ( m_Mode == CAMERAMODE_FIXED )
    {
        m_vecPosition = vecPosition;
        GetCameraSpatialDatabase ()->UpdateItem ( this );
    }
}


void CPlayerCamera::GetLookAt ( CVector& vecLookAt ) const
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


void CPlayerCamera::SetLookAt ( const CVector& vecLookAt )
{
    if ( m_Mode == CAMERAMODE_FIXED )
    {
        m_vecLookAt = vecLookAt;
    }
}


void CPlayerCamera::SetMatrix ( const CVector& vecPosition, const CVector& vecLookAt )
{
    if ( m_Mode == CAMERAMODE_FIXED )
    {
        m_vecPosition = vecPosition;
        m_vecLookAt = vecLookAt;
        GetCameraSpatialDatabase ()->UpdateItem ( this );
    }
}


void CPlayerCamera::SetTarget ( CElement* pElement )
{
    if ( !pElement )
        pElement = m_pPlayer;

    if ( m_pTarget != pElement )
    {
        if ( m_pTarget ) m_pTarget->m_FollowingCameras.remove ( this );
        if ( pElement ) pElement->m_FollowingCameras.push_back ( this );
        m_pTarget = pElement;
    }
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


uchar CPlayerCamera::GenerateSyncTimeContext( void )
{
    // Increment the sync time index (skipping 0)
    m_ucSyncTimeContext = Max < uchar > ( 1, m_ucSyncTimeContext + 1 );
    return m_ucSyncTimeContext;
}


bool CPlayerCamera::CanUpdateSync( uchar ucRemote )
{
    // We can update this camera's sync only if the sync time
    // matches or either of them are 0 (ignore).
    return ( m_ucSyncTimeContext == ucRemote ||
             ucRemote == 0 ||
             m_ucSyncTimeContext == 0 );
}
