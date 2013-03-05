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

    CVector vecCameraPosition, vecPosition, vecLookAt, vecFront, vecVelocity;
    pCamera->GetPosition ( vecCameraPosition );
    pCamera->GetTarget ( vecLookAt );
    vecFront = vecLookAt - vecCameraPosition;

    CClientPlayer* p_LocalPlayer = m_pClientManager->GetPlayerManager()->GetLocalPlayer();
    if ( p_LocalPlayer )
    {
        p_LocalPlayer->GetMoveSpeed( vecVelocity );

        if ( pCamera->IsInFixedMode() )
            vecPosition = vecCameraPosition;
        else
            p_LocalPlayer->GetPosition( vecPosition );

    }
    else
    {
        // Make sure the players position at least has something (I'd be surprised if we get here though)
        vecPosition = vecCameraPosition;
    }

    // Update volume position and velocity from all sounds
    list < CClientSound* > ::iterator iter = m_Sounds.begin ();
    for ( ; iter != m_Sounds.end () ; ++iter )
    {
        CClientSound* pSound = *iter;
        pSound->Process3D ( vecPosition, vecCameraPosition, vecLookAt );

        // Delete sound if finished
        if ( pSound->IsFinished () )
        {
            // call onClientSoundStopped
            CLuaArguments Arguments;
            Arguments.PushString ( "finished" );     // Reason
            pSound->CallEvent ( "onClientSoundStopped", Arguments, false );
            g_pClientGame->GetElementDeleter()->Delete ( pSound );
        }
    }
    UpdateDistanceStreaming ( vecPosition );
}

void CClientSoundManager::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
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
        pSound->PlayStream ( strSound, bLoop, true );
        pSound->SetPosition ( vecPosition );
        return pSound;
    }
    else
        if ( pSound->Play3D ( strSound, bLoop ) )
        {
            pSound->SetPosition ( vecPosition );
            return pSound;
        }

    delete pSound;
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

        fValue = Max( 0.0f, Min( 1.0f, fValue ) );
    }
    else
    {
        fValue = 1.0f;
    }

    BASS_SetConfig( BASS_CONFIG_GVOL_STREAM, static_cast <DWORD> ( fValue * 10000 ) );
    BASS_SetConfig( BASS_CONFIG_GVOL_MUSIC, static_cast <DWORD> ( fValue * 10000 ) );
}


//
// Lists
//
void CClientSoundManager::AddToList ( CClientSound* pSound )
{
    m_Sounds.push_back ( pSound );
}

void CClientSoundManager::RemoveFromList ( CClientSound* pSound )
{
    m_Sounds.remove ( pSound );
    MapRemove ( m_DistanceStreamedInMap, pSound );
}


//
// Distance streaming
//
void CClientSoundManager::OnDistanceStreamIn ( CClientSound* pSound )
{
    assert ( !MapContains ( m_DistanceStreamedInMap, pSound ) );
    m_DistanceStreamedInMap.insert( pSound );
}

void CClientSoundManager::OnDistanceStreamOut ( CClientSound* pSound )
{
    assert ( MapContains ( m_DistanceStreamedInMap, pSound ) );
    MapRemove ( m_DistanceStreamedInMap, pSound );
}


//
// Distance streaming like what is done with visible objects
//
void CClientSoundManager::UpdateDistanceStreaming ( const CVector& vecListenerPosition )
{
    //
    // Make a copy of the current list of sounds that are active
    //
    std::set < CClientSound* > considerMap = m_DistanceStreamedInMap;

    //
    // Mix in all sounds near enough to the listener to be heard
    //
    {
        // Find all entities overlapping the listener position
        CClientEntityResult result;
        GetClientSpatialDatabase()->SphereQuery ( result, CSphere( vecListenerPosition, 0 ) );

        // Extract relevant types
        for ( CClientEntityResult::const_iterator iter = result.begin () ; iter != result.end (); ++iter )
        {
            if ( CClientSound* pSound = DynamicCast < CClientSound > ( *iter ) )
            {
                if ( pSound->IsSound3D() )
                {
                    // Add to consider map
                    considerMap.insert ( pSound );
                }
            }
        }
    }

    //
    // Step through each sound
    //  If the sound is more than 40 units away (or in another dimension), make sure it is deactivated
    //  If the sound is less than 20 units away, make sure it is activated
    //
    for ( std::set < CClientSound* >::iterator iter = considerMap.begin () ; iter != considerMap.end () ; ++iter )
    {
        CClientSound* pSound = *iter;

        // Calculate distance to the edge of the sphere
        CSphere sphere = pSound->GetWorldBoundingSphere ();
        float fDistance = ( vecListenerPosition - sphere.vecPosition ).Length () - sphere.fRadius;

        if ( fDistance > 40 || m_usDimension != pSound->GetDimension () )
            pSound->DistanceStreamOut ();
        else
        if ( fDistance < 20 )
            pSound->DistanceStreamIn ();
    }
}
