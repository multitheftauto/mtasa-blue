/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSound.h
*  PURPOSE:     Sound entity class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*
*****************************************************************************/

#include <StdInc.h>

extern CClientGame* g_pClientGame;

CClientSound::CClientSound ( CClientManager* pManager, ElementID ID ) : CClientEntity ( ID )
{
    m_pManager = pManager;
    m_pSoundManager = pManager->GetSoundManager();
    m_pSound = NULL;

    SetTypeName ( "sound" );

    m_pSoundManager->AddToList ( this );

    RelateDimension ( pManager->GetSoundManager ()->GetDimension () );

    m_fVolume = 1.0f;
    m_fDistance = 1.0f;
    m_fMinDistance = 1.0f;
    m_usDimension = 0;
    m_b3D = false;
}

CClientSound::~CClientSound ( void )
{
    m_pSoundManager->RemoveFromList ( this );
    if ( m_pSound )
    {
        m_pSound->setSoundStopEventReceiver ( NULL );
        m_pSound->stop ();
        m_pSound->drop ();
    }
}

bool CClientSound::Play ( const char* szPath, bool bLoop )
{
    m_pSound = m_pSoundManager->GetEngine()->play2D ( szPath, bLoop, false, true );
    if ( m_pSound )
    {
        m_b3D = false;
        m_pSound->setSoundStopEventReceiver ( m_pSoundManager );
        return true;
    }
    return false;
}

bool CClientSound::Play3D ( const char* szPath, CVector vecPosition, bool bLoop )
{
    m_vecPosition = vecPosition;
    vec3df pos ( vecPosition.fX, vecPosition.fY, vecPosition.fZ );
	m_pSound = m_pSoundManager->GetEngine()->play3D ( szPath, pos, bLoop, false, true );
    if ( m_pSound )
    {
        m_b3D = true;
        m_pSound->setSoundStopEventReceiver ( m_pSoundManager );
        return true;
    }
    return false;
}

void CClientSound::Stop ( void )
{
    if ( m_pSound )
    {
        m_pSound->stop ();
    }
    g_pClientGame->GetElementDeleter()->Delete ( this );
    m_pSoundManager->RemoveFromList ( this );
}

void CClientSound::SetPaused ( bool bPaused )
{
    if ( m_pSound )
    {
        return m_pSound->setIsPaused ( bPaused );
    }
}

bool CClientSound::IsPaused ( void )
{
    if ( m_pSound )
    {
        return m_pSound->getIsPaused ();
    }
    return false;
}

bool CClientSound::IsFinished ( void )
{
    if ( m_pSound )
    {
        return m_pSound->isFinished ();
    }
    return false;
}

void CClientSound::SetPlayPosition ( unsigned int uiPosition )
{
    if ( m_pSound )
    {
        m_pSound->setPlayPosition ( uiPosition );
    }
}

unsigned int CClientSound::GetPlayPosition ( void )
{
    if ( m_pSound )
    {
        return m_pSound->getPlayPosition ();
    }
    return 0;
}

unsigned int CClientSound::GetLength ( void )
{
    if ( m_pSound )
    {
        return m_pSound->getPlayLength ();
    }
    return 0;
}

void CClientSound::SetVolume ( float fVolume )
{
    m_fVolume = fVolume;
    if ( m_pSound && m_usDimension == m_pManager->GetSoundManager ()->GetDimension () )
    {
        m_pSound->setVolume ( fVolume * m_fDistance );
    }
}

float CClientSound::GetVolume ( void )
{
    return m_fVolume;
}

void CClientSound::SetPosition ( const CVector& vecPosition )
{
    m_vecPosition = vecPosition;
    if ( m_pSound )
    {
        m_pSound->setPosition ( vec3df ( vecPosition.fX, vecPosition.fY, vecPosition.fZ ) );
    }
}

void CClientSound::GetPosition ( CVector& vecPosition ) const
{
    vecPosition = m_vecPosition;
}

void CClientSound::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
    RelateDimension ( m_pManager->GetSoundManager ()->GetDimension () );
}

void CClientSound::RelateDimension ( unsigned short usDimension )
{
    if ( usDimension == m_usDimension )
    {
        SetVolume ( m_fVolume );
    }
    else
    {
        m_fVolume = GetVolume ();
        SetVolume ( 0.0f );
    }
}

void CClientSound::SetMinDistance ( float fDistance )
{
    m_fMinDistance = fDistance;
    if ( m_pSound )
    {
        m_pSound->setMinDistance ( fDistance );
    }
}

float CClientSound::GetMinDistance ( void )
{
    return m_fMinDistance;
}

void CClientSound::SetMaxDistance ( float fDistance )
{
    if ( m_pSound )
    {
        m_pSound->setMaxDistance ( fDistance );
    }
}

float CClientSound::GetMaxDistance ( void )
{
    if ( m_pSound )
    {
        return m_pSound->getMaxDistance ();
    }
    return 0.0f;
}

void CClientSound::Process3D ( CVector vecPosition, CVector vecLookAt )
{
    if ( !m_b3D )
        return;

    if ( m_pSound )
    {
        // Pan
        float fLRot = 360.0f - ConvertRadiansToDegrees ( atan2 ( vecLookAt.fY - vecPosition.fY, vecLookAt.fX - vecPosition.fX ) );
        float fSRot = 360.0f - ConvertRadiansToDegrees ( atan2 ( m_vecPosition.fY - vecPosition.fY, m_vecPosition.fX - vecPosition.fX ) );

        float fDeg = fSRot - fLRot;
        if ( fDeg > 90.0f ) fDeg = 180.0f - fDeg;
        else if ( fDeg < -90.0f ) fDeg = -180.0f - fDeg;

        m_pSound->setPan ( -fDeg / 90.0f );

        // Volume

        float fDistance = DistanceBetweenPoints2D ( vecPosition, m_vecPosition );
        float fVolume = 1.0;

        if ( fDistance <= m_fMinDistance )
        {
            fVolume = 1.0f;
        }
        else if ( fDistance >= MAX_SOUND_DISTANCE + m_fMinDistance )
        {
            fVolume = 0.0f;
        }
        else
        {
            fVolume = 1.0f / ( ( fDistance - m_fMinDistance ) / 3.0f );

            if ( fVolume > 1.0f ) fDistance = 1.0f;
            else if ( fVolume < 0.0f ) fDistance = 0.0f;
        }

        if ( m_fDistance != fVolume )
        {
            m_fDistance = fVolume;
            SetVolume ( GetVolume () );
        }
    }
}