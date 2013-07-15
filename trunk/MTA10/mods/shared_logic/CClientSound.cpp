/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSound.cpp
*  PURPOSE:     Sound entity class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*               arc_
*               Florian Busse <flobu@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>
#include "CBassAudio.h"

CClientSound::CClientSound ( CClientManager* pManager, ElementID ID ) : ClassInit ( this ), CClientEntity ( ID )
{
    m_pSoundManager = pManager->GetSoundManager();
    m_pAudio = NULL;

    SetTypeName ( "sound" );

    m_pSoundManager->AddToList ( this );

    m_fVolume = 1.0f;
    m_fMinDistance = 5.0f;
    m_fMaxDistance = 20.0f;
    m_fPlaybackSpeed = 1.0f;
    m_bPan = true;
    m_fPan = 0.0f;

    m_pBuffer = NULL;
}

CClientSound::~CClientSound ( void )
{
    Destroy ();
    m_pSoundManager->RemoveFromList ( this );

    delete m_pBuffer;
    m_pBuffer = NULL;
}


////////////////////////////////////////////////////////////
//
// CClientSound::GetWorldBoundingSphere
//
// For spatial database
//
////////////////////////////////////////////////////////////
CSphere CClientSound::GetWorldBoundingSphere ( void )
{
    return CSphere ( m_vecPosition, m_fMaxDistance );
}


////////////////////////////////////////////////////////////
//
// CClientSound::DistanceStreamIn
//
// Sound is now close enough to be heard, so must be activated
//
////////////////////////////////////////////////////////////
void CClientSound::DistanceStreamIn ( void )
{
    if ( !m_pAudio )
    {
        Create ();
        m_pSoundManager->OnDistanceStreamIn ( this );

        // Call Stream In event
        CLuaArguments Arguments;
        CallEvent ( "onClientElementStreamIn", Arguments, true );
    }
}


////////////////////////////////////////////////////////////
//
// CClientSound::DistanceStreamOut
//
// Sound is now far enough away to not be heard, so can be deactivated
//
////////////////////////////////////////////////////////////
void CClientSound::DistanceStreamOut ( void )
{
    if ( m_pAudio )
    {
        m_pSoundManager->OnDistanceStreamOut ( this );
        Destroy ();

        // Call Stream Out event
        CLuaArguments Arguments;
        CallEvent ( "onClientElementStreamOut", Arguments, true );
    }
}


////////////////////////////////////////////////////////////
//
// CClientSound::Create
//
// Create underlying audio
//
////////////////////////////////////////////////////////////
bool CClientSound::Create ( void )
{
    if ( m_pAudio )
        return false;

    // Initial state
    if ( !m_pBuffer )
        m_pAudio = new CBassAudio ( m_bStream, m_strPath, m_bLoop, m_b3D );
    else
        m_pAudio = new CBassAudio ( m_pBuffer, m_uiBufferLength, m_bLoop, m_b3D );
        
    m_bDoneCreate = true;

    // Load file/start connect
    if ( !m_pAudio->BeginLoadingMedia (  ) )
        return false;

    // Get and save length
    m_dLength = m_pAudio->GetLength ();

    // Save the length in the resource incase we need it another time
#if 0
    // TODO
    //   Storing the length of previouly loaded files in the resource will help the client avoid
    //   loading far away sounds, just to get the length.
    //   (The length is required to simulate the finish time of streamed out sounds)
    this->GetResource ()->SetAudioFileLength ( m_strPath, m_dLength );
#endif

    // Transfer dynamic state
    m_pAudio->SetVolume ( m_fVolume );
    m_pAudio->SetPlaybackSpeed ( m_fPlaybackSpeed );
    m_pAudio->SetPosition ( m_vecPosition );
    m_pAudio->SetVelocity ( m_vecVelocity );
    m_pAudio->SetMinDistance ( m_fMinDistance );
    m_pAudio->SetMaxDistance ( m_fMaxDistance );
    m_pAudio->SetFxEffects ( &m_EnabledEffects[0], NUMELMS( m_EnabledEffects ) );
    m_pAudio->SetTempoValues ( m_fSampleRate, m_fTempo, m_fPitch, m_bReversed );
    m_pAudio->SetPanEnabled ( m_bPan );
    m_pAudio->SetPan ( m_fPan );

    // Transfer play position if it was being simulated
    EndSimulationOfPlayPositionAndApply ();

    //
    // Note:
    //   m_pAudio does not actually start until the next call to m_pAudio->DoPulse.
    //   This is to allow for settings to be changed before playback, avoiding sound pops etc.
    //
    return true;
}


////////////////////////////////////////////////////////////
//
// CClientSound::Destroy
//
// Destroy underlying audio
//
////////////////////////////////////////////////////////////
void CClientSound::Destroy ( void )
{
    if ( !m_pAudio )
        return;

    BeginSimulationOfPlayPosition ();

    m_pAudio->Destroy ();
    m_pAudio = NULL;
}


////////////////////////////////////////////////////////////
//
// CClientSound::BeginSimulationOfPlayPosition
//
//
//
////////////////////////////////////////////////////////////
void CClientSound::BeginSimulationOfPlayPosition ( void )
{
    // Only 3d sounds will be distance streamed in and out. Also streams can't be seeked.
    // So only non-streamed 3D sounds need the play position simulated.
    if ( m_b3D && !m_bStream )
    {
        m_SimulatedPlayPosition.SetLooped ( m_bLoop );
        m_SimulatedPlayPosition.SetLength ( m_dLength );
        m_SimulatedPlayPosition.SetPaused ( m_bPaused );
        m_SimulatedPlayPosition.SetPlaybackSpeed( GetPlaybackSpeed () );
        m_SimulatedPlayPosition.SetPlayPositionNow ( GetPlayPosition () );
        m_SimulatedPlayPosition.SetValid ( true );
    }
}


////////////////////////////////////////////////////////////
//
// CClientSound::EndSimulationOfPlayPositionAndApply
//
//
//
////////////////////////////////////////////////////////////
void CClientSound::EndSimulationOfPlayPositionAndApply ( void )
{
    if ( m_SimulatedPlayPosition.IsValid () )
    {
        m_SimulatedPlayPosition.SetLength ( m_dLength );
        m_pAudio->SetPlayPosition ( m_SimulatedPlayPosition.GetPlayPositionNow () );
        m_SimulatedPlayPosition.SetValid ( false );
    }
}


////////////////////////////////////////////////////////////
//
// CClientSound::Play and pals
//
//
//
////////////////////////////////////////////////////////////
bool CClientSound::Play ( const SString& strPath, bool bLoop )
{
    assert ( m_strPath.empty () );

    m_bStream = false;
    m_b3D = false;
    m_strPath = strPath;
    m_bLoop = bLoop;
    m_bPan = false;

    // Instant distance-stream in
    return Create ();
}


bool CClientSound::Play ( void* pMemory, unsigned int uiLength, bool bLoop )
{
    assert ( pMemory );

    m_bStream = false;
    m_b3D = false;
    m_pBuffer = pMemory;
    m_uiBufferLength = uiLength;
    m_bLoop = bLoop;
    m_bPan = false;

    // Instant distance-stream in
    return Create ();
}


bool CClientSound::Play3D ( const SString& strPath, bool bLoop )
{
    assert ( m_strPath.empty () );

    m_bStream = false;
    m_b3D = true;
    m_strPath = strPath;
    m_bLoop = bLoop;

    BeginSimulationOfPlayPosition ();

    return true;
}


bool CClientSound::Play3D ( void* pMemory, unsigned int uiLength, bool bLoop )
{
    m_bStream = false;
    m_b3D = true;
    m_pBuffer = pMemory;
    m_uiBufferLength = uiLength;
    m_bLoop = bLoop;

    BeginSimulationOfPlayPosition ();

    return true;
}


void CClientSound::PlayStream ( const SString& strURL, bool bLoop, bool b3D )
{
    assert ( m_strPath.empty () );

    m_bStream = true;
    m_b3D = b3D;
    m_strPath = strURL;
    m_bLoop = bLoop;

    // Instant distance-stream in if not 3D
    if ( !m_b3D )
        Create ();
}


////////////////////////////////////////////////////////////
//
// CClientSound:: Sea of sets 'n' gets
//
//
//
////////////////////////////////////////////////////////////
void CClientSound::SetPlayPosition ( double dPosition )
{
    if ( m_pAudio )
    {
        // Use actual audio if active
        m_pAudio->SetPlayPosition ( dPosition );
    }
    else
    {
        // Use simulation if not active
        m_SimulatedPlayPosition.SetPlayPositionNow ( dPosition );
    }
}

double CClientSound::GetPlayPosition ( void )
{
    if ( m_pAudio )
    {
        // Use actual audio if active
        return m_pAudio->GetPlayPosition ();
    }
    else
    if ( m_SimulatedPlayPosition.IsValid () )
    {
        // Use simulation if not active
        return m_SimulatedPlayPosition.GetPlayPositionNow ();
    }
    return 0;
}

//
// Set bAvoidLoad to true to try to avoid any initial load that may be required
//
double CClientSound::GetLength ( bool bAvoidLoad )
{
    if ( m_dLength == 0 && !m_bDoneCreate && !m_bStream )
    {
        // Not loaded by this entity yet

#if 0       // TODO
        if ( bAvoidLoad )
        {
            // Caller wants to avoid loading the file to find out the length,
            // so see if resouce has already loaded the file
            m_dLength == this->GetResource ()->GetAudioFileLength ( m_strPath );
        }
#endif
        // If needed, do a create and destroy to get the length
        if ( m_dLength == 0 )
        {
            Create ();
            Destroy ();
        }
    }

    if ( m_dLength == 0 && m_bStream && m_pAudio )
    {
        // Try to get the length now (otherwise 0 would be returned)
        m_dLength = m_pAudio->GetLength ( );
    }

    return m_dLength;
}

float CClientSound::GetVolume ( void )
{
    return m_fVolume;
}

void CClientSound::SetVolume ( float fVolume, bool bStore )
{
   m_fVolume = fVolume;

    if ( m_pAudio )
        m_pAudio->SetVolume ( m_fVolume );
}

float CClientSound::GetPlaybackSpeed ( void )
{
    return m_fPlaybackSpeed;
}

void CClientSound::SetPlaybackSpeed ( float fSpeed )
{
    m_fPlaybackSpeed = fSpeed;
    m_SimulatedPlayPosition.SetPlaybackSpeed( fSpeed );

    if ( m_pAudio )
        m_pAudio->SetPlaybackSpeed ( m_fPlaybackSpeed );
}

void CClientSound::SetPosition ( const CVector& vecPosition )
{
    m_vecPosition = vecPosition;
    UpdateSpatialData ();
    if ( m_pAudio )
        m_pAudio->SetPosition ( m_vecPosition );
}

void CClientSound::GetPosition ( CVector& vecPosition ) const
{
    if ( m_pAttachedToEntity )
    {
        m_pAttachedToEntity->GetPosition( vecPosition );
        vecPosition += m_vecAttachedPosition;
    }
    else
        vecPosition = m_vecPosition;
}

void CClientSound::SetVelocity ( const CVector& vecVelocity )
{
    m_vecVelocity = vecVelocity;
    if ( m_pAudio )
        m_pAudio->SetVelocity ( m_vecVelocity );
}

void CClientSound::GetVelocity ( CVector& vecVelocity )
{
    vecVelocity = m_vecVelocity;
}

void CClientSound::SetPaused ( bool bPaused )
{
    if ( m_bPaused != bPaused )
    {
        if ( bPaused )
        {
            // call onClientSoundStopped
            CLuaArguments Arguments;
            Arguments.PushString ( "paused" );     // Reason
            this->CallEvent ( "onClientSoundStopped", Arguments, false );
        }
        else
        {
            // call onClientSoundStarted
            CLuaArguments Arguments;
            Arguments.PushString ( "resumed" );     // Reason
            this->CallEvent ( "onClientSoundStarted", Arguments, false );
        }
    }

    m_bPaused = bPaused;

    m_SimulatedPlayPosition.SetPaused ( bPaused );

    if ( m_pAudio )
        m_pAudio->SetPaused ( m_bPaused );
}

bool CClientSound::IsPaused ( void )
{
    return m_bPaused;
}

void CClientSound::SetMinDistance ( float fDistance )
{
    m_fMinDistance = fDistance;
    if ( m_pAudio )
        m_pAudio->SetMinDistance ( m_fMinDistance );
}

float CClientSound::GetMinDistance ( void )
{
    return m_fMinDistance;
}

void CClientSound::SetMaxDistance ( float fDistance )
{
    bool bChanged = m_fMaxDistance != fDistance;

    m_fMaxDistance = fDistance;
    if ( m_pAudio )
        m_pAudio->SetMaxDistance ( m_fMaxDistance );

    if ( bChanged )
        UpdateSpatialData ();
}

float CClientSound::GetMaxDistance ( void )
{
    return m_fMaxDistance;
}

void CClientSound::ApplyFXModifications ( float fSampleRate, float fTempo, float fPitch, bool bReversed )
{
    m_bReversed = bReversed;
    m_fSampleRate = fSampleRate;
    m_fTempo = fTempo;
    m_fPitch = fPitch;
    if ( m_pAudio )
        m_pAudio->SetTempoValues ( fSampleRate, fTempo, fPitch, bReversed );
}

void CClientSound::GetFXModifications ( float &fSampleRate, float &fTempo, float &fPitch, bool &bReversed )
{
    if ( m_pAudio )
    {
        m_pAudio->GetTempoValues ( fSampleRate, fTempo, fPitch, bReversed );
    }
}

float* CClientSound::GetFFTData ( int iLength )
{
    if ( m_pAudio )
    {
        return m_pAudio->GetFFTData ( iLength );
    }
    return NULL;
}


float* CClientSound::GetWaveData ( int iLength )
{
    if ( m_pAudio )
    {
        return m_pAudio->GetWaveData ( iLength );
    }
    return NULL;
}
bool CClientSound::SetPanEnabled ( bool bPan )
{
    if ( m_pAudio && m_b3D )
    {
        m_pAudio->SetPanEnabled ( bPan );
        m_bPan = bPan;
        return true;
    }
    return false;
}

bool CClientSound::IsPanEnabled ( void )
{
    if ( m_pAudio )
    {
        return m_pAudio->GetPanEnabled ( );
    }
    return m_bPan;
}


DWORD CClientSound::GetLevelData ( void )
{
    if ( m_pAudio )
    {
        return m_pAudio->GetLevelData ( );
    }
    return 0;
}

float CClientSound::GetSoundBPM ( void )
{
    if ( m_pAudio )
    {
        return m_pAudio->GetSoundBPM ( );
    }
    return 0.0f;
}

////////////////////////////////////////////////////////////
//
// CClientSound::GetMetaTags
//
// If the stream is not active, this may not work correctly
//
////////////////////////////////////////////////////////////
SString CClientSound::GetMetaTags( const SString& strFormat )
{
    SString strMetaTags = "";
    if ( m_pAudio )
    {
        strMetaTags = m_pAudio->GetMetaTags ( strFormat );
        m_SavedTags[ strFormat ] = strMetaTags;
    }
    else
    {
        // Search previously found tags for this stream when it is not active
        // This may not be such a good idea btw
        if ( SString* pstrMetaTags = MapFind ( m_SavedTags, strFormat ) )
            strMetaTags = *pstrMetaTags;
    }

    return strMetaTags;
}


////////////////////////////////////////////////////////////
//
// CClientSound::SetFxEffect
//
//
//
////////////////////////////////////////////////////////////
bool CClientSound::SetFxEffect ( uint uiFxEffect, bool bEnable )
{
    if ( uiFxEffect >= NUMELMS( m_EnabledEffects ) )
        return false;

    m_EnabledEffects[uiFxEffect] = bEnable;

    if ( m_pAudio )
        m_pAudio->SetFxEffects ( &m_EnabledEffects[0], NUMELMS( m_EnabledEffects ) );

    return true;
}

bool CClientSound::IsFxEffectEnabled ( uint uiFxEffect )
{
    if ( uiFxEffect >= NUMELMS( m_EnabledEffects ) )
        return false;
    return m_EnabledEffects[uiFxEffect] ? true : false;
}


////////////////////////////////////////////////////////////
//
// CClientSound::Process3D
//
// Update position and velocity and pass on the BASS for processing.
// m_pAudio->DoPulse needs to be called for non-3D sounds also.
//
////////////////////////////////////////////////////////////
void CClientSound::Process3D ( const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt )
{
    // Update 3D things if required
    if ( m_b3D )
    {
        // Update our position and velocity if we're attached
        CClientEntity* pAttachedToEntity = GetAttachedTo ();
        if ( pAttachedToEntity )
        {
            GetPosition( m_vecPosition );
            DoAttaching ();
            CVector vecVelocity;
            if ( CStaticFunctionDefinitions::GetElementVelocity ( *pAttachedToEntity, vecVelocity ) )
                SetVelocity ( vecVelocity );
            // Update our spatial data position
            UpdateSpatialData ();
        }
    }
    // If the sound isn't active, we don't need to process it
    // Moved after 3D updating as the streamer didn't know the position changed if a sound isn't streamed in when attached.
    if ( !m_pAudio )
        return;

    m_pAudio->DoPulse ( vecPlayerPosition, vecCameraPosition, vecLookAt );


    // Trigger script events for things
    SSoundEventInfo eventInfo;
    while ( m_pAudio->GetQueuedEvent ( eventInfo ) )
    {
        if ( eventInfo.type == SOUND_EVENT_FINISHED_DOWNLOAD )
        {
            CLuaArguments Arguments;
            Arguments.PushNumber ( eventInfo.dNumber );
            CallEvent ( "onClientSoundFinishedDownload", Arguments, true );
            OutputDebugLine ( SString ( "[ClientSound] onClientSoundFinishedDownload %f", eventInfo.dNumber ) );
        }
        else
        if ( eventInfo.type == SOUND_EVENT_CHANGED_META )
        {
            CLuaArguments Arguments;
            Arguments.PushString ( eventInfo.strString );
            CallEvent ( "onClientSoundChangedMeta", Arguments, true );
            OutputDebugLine ( SString ( "[ClientSound] onClientSoundChangedMeta %s", *eventInfo.strString ) );
        }
        else
        if ( eventInfo.type == SOUND_EVENT_STREAM_RESULT )
        {
            // Call onClientSoundStream LUA event
            CLuaArguments Arguments;
            Arguments.PushBoolean ( eventInfo.bBool );
            Arguments.PushNumber ( eventInfo.dNumber );
            if ( !eventInfo.strString.empty () )
                Arguments.PushString ( eventInfo.strString );
            CallEvent ( "onClientSoundStream", Arguments, true );
            OutputDebugLine ( SString ( "[ClientSound] onClientSoundStream %d %f %s", eventInfo.bBool, eventInfo.dNumber, *eventInfo.strString ) );
        }
        else
        if ( eventInfo.type == SOUND_EVENT_BEAT )
        {
            CLuaArguments Arguments;
            Arguments.PushNumber ( eventInfo.dNumber );
            CallEvent ( "onClientSoundBeat", Arguments, true );
        }
    }
}


////////////////////////////////////////////////////////////
//
// CClientSound::IsFinished
//
//
////////////////////////////////////////////////////////////
bool CClientSound::IsFinished ( void )
{
    if ( m_pAudio )
        return m_pAudio->IsFinished ();

    // For 3D non-streamed non-looped sounds, check if simulated position has reached the end
    if ( m_b3D && !m_bStream && !m_bLoop )
    {
        // SimulatedPlayPosition needs the correct length. Try to get the length without loading the file
        if ( m_dLength == 0 )
            GetLength ( true );

        m_SimulatedPlayPosition.SetLength ( m_dLength );
        return m_SimulatedPlayPosition.IsFinished ();
    }

    return false;
}


bool CClientSound::GetPan ( float& fPan )
{
    if ( m_pAudio && !m_b3D )
    {
        fPan = m_pAudio->GetPan();
        return true;
    }
    
    return false;
}


bool CClientSound::SetPan ( float fPan )
{
    if ( m_pAudio && !m_b3D )
    {
        m_pAudio->SetPan ( fPan );
        m_fPan = fPan;

        return true;
    }

    return false;
}