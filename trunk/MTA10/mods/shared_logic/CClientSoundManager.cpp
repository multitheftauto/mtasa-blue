/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSound.h
*  PURPOSE:     Sound entity class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*               Marcus Bauer <mabako@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using SharedUtil::CalcMTASAPath;
using std::list;

extern CCoreInterface* g_pCore;

CClientSoundManager::CClientSoundManager ( CClientManager* pClientManager )
{
    m_pClientManager = pClientManager;

    m_bUse3DBuffers = false;

    if ( m_bUse3DBuffers )
    {
        m_pSoundEngine = createIrrKlangDevice ();
    }
    else
    {
        // ESEO_USE_3D_BUFFERS in the default options set fucks up gta sounds
        // on some PCs, so we set the options ourselves.
        // ESEO_MULTI_THREADED is not used as OnSoundStopped() is not thread safe
        m_pSoundEngine = createIrrKlangDevice ( ESOD_AUTO_DETECT, 0 );
    }

    // Load plugins (mp3 in our case)
    m_pSoundEngine->loadPlugins ( CalcMTASAPath("\\MTA\\") );

    UpdateVolume ();
}

CClientSoundManager::~CClientSoundManager ( void )
{
    list < CClientSound* > ::iterator iter = m_Sounds.begin ();
    for ( ; iter != m_Sounds.end () ; iter++ )
    {
        (*iter)->GetSound()->setSoundStopEventReceiver ( NULL );
    }
    m_Sounds.clear();
    m_pSoundEngine->drop ();
}

void CClientSoundManager::DoPulse ( void )
{
    // Update needs to be called in single threaded mode
    m_pSoundEngine->update();

    UpdateVolume ();

    CClientCamera* pCamera = m_pClientManager->GetCamera();

    CVector vecPosition, vecLookAt;
    pCamera->GetPosition ( vecPosition );
    pCamera->GetTarget ( vecLookAt );

    if ( m_bUse3DBuffers )
    {
        m_pSoundEngine->setListenerPosition ( vec3df ( vecPosition.fX, vecPosition.fY, vecPosition.fZ ),
                                              vec3df ( vecLookAt.fX, vecLookAt.fY, vecLookAt.fZ ),
                                              vec3df ( 0, 0, 0 ),
                                              vec3df ( 0, 0, 1 ) );
    }
    else
    {
        list < CClientSound* > ::iterator iter = m_Sounds.begin ();
        for ( ; iter != m_Sounds.end () ; iter++ )
        {
            (*iter)->Process3D ( vecPosition, vecLookAt );
        }
    }
}

void CClientSoundManager::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
    list < CClientSound* > ::iterator iter = m_Sounds.begin ();
    for ( ; iter != m_Sounds.end () ; iter++ )
    {
        (*iter)->RelateDimension ( usDimension );
    }
}

CClientSound* CClientSoundManager::PlaySound2D ( const char* szFile, bool bLoop )
{
    CClientSound* pSound = new CClientSound ( m_pClientManager, INVALID_ELEMENT_ID );
    if ( pSound->Play ( szFile, bLoop ) )
    {
        return pSound;
    }
    delete pSound;
    return NULL;
}

CClientSound* CClientSoundManager::PlaySound3D ( const char* szFile, CVector vecPosition, bool bLoop )
{
    CClientSound* pSound = new CClientSound ( m_pClientManager, INVALID_ELEMENT_ID );
    if ( m_bUse3DBuffers )
    {
        if ( pSound->Play3D ( szFile, vecPosition, bLoop ) )
        {
            return pSound;
        }
    }
    else
    {
        if ( pSound->Play ( szFile, bLoop ) )
        {
            pSound->SetPosition ( vecPosition );
            pSound->Set3D ( true );
            return pSound;
        }
    }

    delete pSound;
    return NULL;
}

bool CClientSoundManager::Exists ( CClientSound* pSound )
{
    list < CClientSound* > ::iterator iter = m_Sounds.begin ();
    for ( ; iter != m_Sounds.end () ; iter++ )
    {
        if ( *iter == pSound )
        {
            return true;
        }
    }
    return false;
}

CClientSound* CClientSoundManager::Get ( ISound* pSound )
{
    list < CClientSound* > ::iterator iter = m_Sounds.begin ();
    for ( ; iter != m_Sounds.end () ; iter++ )
    {
        if ( (*iter)->GetSound() == pSound )
        {
            return *iter;
        }
    }
    return NULL;
}

void CClientSoundManager::OnSoundStopped ( ISound* sound, E_STOP_EVENT_CAUSE reason, void* pObj )
{
    CClientSound* pSound = Get ( sound );
    if ( pSound )
    {
        g_pClientGame->GetElementDeleter()->Delete ( pSound );
        RemoveFromList ( pSound );
    }
}

void CClientSoundManager::UpdateVolume ( )
{
    // set our master sound volume if the cvar changed
    float fValue = 0.0f;
    if( g_pCore->GetCVars ()->Get ( "mtavolume", fValue ) )
    {
        if ( fValue == m_pSoundEngine->getSoundVolume () )
            return;

        fValue = max( 0.0f, min( 1.0f, fValue ) );
    }
    else
    {
        fValue = 1.0f;
    }

    m_pSoundEngine->setSoundVolume( fValue );
}