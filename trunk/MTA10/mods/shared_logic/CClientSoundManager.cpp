/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSoundManager.cpp
*  PURPOSE:     Sound manager class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*               Marcus Bauer <mabako@gmail.com>
*               Florian Busse <flobu@gmx.net>
*
*****************************************************************************/

#include "StdInc.h"

using SharedUtil::CalcMTASAPath;
using std::list;

extern CCoreInterface* g_pCore;

CClientSoundManager::CClientSoundManager ( CClientManager* pClientManager )
{
    m_pClientManager = pClientManager;

    // Initialize BASS audio library
    if (!BASS_Init ( -1,44100,NULL,NULL,NULL ))
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in Init", BASS_ErrorGetCode() );
    
    // Load the Plugins
    if (!BASS_PluginLoad ( "basswma.dll", 0 ) && BASS_ErrorGetCode () != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PluginLoad WMA", BASS_ErrorGetCode() );
    if (!BASS_PluginLoad ( "bassflac.dll", 0 ) && BASS_ErrorGetCode () != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PluginLoad FLAC", BASS_ErrorGetCode() );
    if (!BASS_PluginLoad ( "bassmidi.dll", 0 ) && BASS_ErrorGetCode () != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PluginLoad MIDI", BASS_ErrorGetCode() );
    if (!BASS_PluginLoad ( "bass_aac.dll", 0 ) && BASS_ErrorGetCode () != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PluginLoad AAC", BASS_ErrorGetCode() );
    if (!BASS_PluginLoad ( "bass_ac3.dll", 0 ) && BASS_ErrorGetCode () != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PluginLoad AC3", BASS_ErrorGetCode() );

    BASS_SetConfig ( BASS_CONFIG_NET_PREBUF, 0 );
    BASS_SetConfig ( BASS_CONFIG_NET_PLAYLIST, 1 ); // Allow playlists
    
    UpdateVolume ();

    m_FxEffectNames["chorus"] =         BASS_FX_DX8_CHORUS;
    m_FxEffectNames["compressor"] =     BASS_FX_DX8_COMPRESSOR;
    m_FxEffectNames["distortion"] =     BASS_FX_DX8_DISTORTION;
    m_FxEffectNames["echo"] =           BASS_FX_DX8_ECHO;
    m_FxEffectNames["flanger"] =        BASS_FX_DX8_FLANGER;
    m_FxEffectNames["gargle"] =         BASS_FX_DX8_GARGLE;
    m_FxEffectNames["i3dl2reverb"] =    BASS_FX_DX8_I3DL2REVERB;
    m_FxEffectNames["parameq"] =        BASS_FX_DX8_PARAMEQ;
    m_FxEffectNames["reverb"] =         BASS_FX_DX8_REVERB;
}

CClientSoundManager::~CClientSoundManager ( void )
{
    BASS_Stop();
    BASS_Free();
}

void CClientSoundManager::DoPulse ( void )
{
    UpdateVolume ();

    CClientCamera* pCamera = m_pClientManager->GetCamera();

    CVector vecPosition, vecLookAt, vecFront, vecVelocity;
    pCamera->GetPosition ( vecPosition );
    pCamera->GetTarget ( vecLookAt );
    vecFront = vecLookAt - vecPosition;

    CClientPlayer* p_LocalPlayer = m_pClientManager->GetPlayerManager()->GetLocalPlayer();
    if ( p_LocalPlayer )
        p_LocalPlayer->GetMoveSpeed( vecVelocity );

    BASS_3DVECTOR pos ( vecPosition.fX, vecPosition.fY, vecPosition.fZ );
    BASS_3DVECTOR vel ( vecVelocity.fX, vecVelocity.fY, vecVelocity.fZ );
    BASS_3DVECTOR front ( vecFront.fX, vecFront.fY, vecFront.fZ );
    BASS_3DVECTOR top ( 0, 0, -1 );

    // Update the listener position
    BASS_Set3DPosition ( &pos, &vel, &front, &top );

    // Update volume position and velocity from all sounds
    list < CClientSound* > ::iterator iter = m_Sounds.begin ();
    for ( ; iter != m_Sounds.end () ; ++iter )
    {
        (*iter)->Process3D ( vecPosition, vecLookAt );
    }

    // Apply the 3D changes
    BASS_Apply3D ();
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

CClientSound* CClientSoundManager::PlaySound2D ( const SString& strSound, bool bIsURL, bool bLoop )
{
    CClientSound* pSound = new CClientSound ( m_pClientManager, INVALID_ELEMENT_ID );
    if ( bIsURL )
    {
        pSound->PlayStream ( strSound, bLoop );
        return pSound;
    }
    else
        if ( pSound->Play ( strSound, bLoop ) )
            return pSound;

    delete pSound;
    return NULL;
}

CClientSound* CClientSoundManager::PlaySound3D ( const SString& strSound, bool bIsURL, const CVector& vecPosition, bool bLoop )
{
    CClientSound* pSound = new CClientSound ( m_pClientManager, INVALID_ELEMENT_ID );

    if ( bIsURL )
    {
        pSound->PlayStream ( strSound, bLoop, true, vecPosition );
        return pSound;
    }
    else
        if ( pSound->Play3D ( strSound, vecPosition, bLoop ) )
            return pSound;

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

CClientSound* CClientSoundManager::Get ( DWORD pSound )
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

int CClientSoundManager::GetFxEffectFromName ( const std::string& strEffectName )
{
    std::map < std::string, int >::iterator it;
    it = m_FxEffectNames.find ( strEffectName );

    if ( it != m_FxEffectNames.end () )
    {
        return it->second;
    }
    return -1;
}

void CClientSoundManager::UpdateVolume ()
{
    // set our master sound volume if the cvar changed
    float fValue = 0.0f;
    if( g_pCore->GetCVars ()->Get ( "mtavolume", fValue ) )
    {
        if ( fValue*10000 == BASS_GetConfig ( BASS_CONFIG_GVOL_STREAM ) )
            return;

        fValue = max( 0.0f, min( 1.0f, fValue ) );
    }
    else
    {
        fValue = 1.0f;
    }

    BASS_SetConfig( BASS_CONFIG_GVOL_STREAM, static_cast <DWORD> ( fValue * 10000 ) );
    BASS_SetConfig( BASS_CONFIG_GVOL_MUSIC, static_cast <DWORD> ( fValue * 10000 ) );
}
