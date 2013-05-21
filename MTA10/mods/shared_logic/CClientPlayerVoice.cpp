/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVoiceRecorder.cpp
*  PURPOSE:     Remote player voice chat playback
*  DEVELOPERS:  Philip Farquharson <philip@philipfarquharson.co.uk>
*               Talidan <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CClientPlayerVoice.h"
#include "CBassAudio.h"
#include <process.h>
#include <tags.h>
#include <bassmix.h>
#include <basswma.h>
#include <bass_fx.h>

void CALLBACK BPMCallback ( int handle, float bpm, void* user );
void CALLBACK BeatCallback ( DWORD chan, double beatpos, void *user );

#define INVALID_FX_HANDLE (-1)  // Hope that BASS doesn't use this as a valid Fx handle

CClientPlayerVoice::CClientPlayerVoice( CClientPlayer* pPlayer, CVoiceRecorder* pVoiceRecorder )
{
    m_pPlayer = pPlayer;
    m_pVoiceRecorder = pVoiceRecorder;
    m_bVoiceActive = false;
    m_SampleRate = SAMPLERATE_WIDEBAND;
    m_pSpeexDecoderState = NULL;
    m_iSpeexIncomingFrameSampleCount = 0;

    // Get initial voice volume
    m_fVolume = 1.0f;
    g_pCore->GetCVars ()->Get ( "voicevolume", m_fVolumeScale );

    m_fVolume = m_fVolume * m_fVolumeScale;

    if ( pPlayer->IsLocalPlayer ( ) == true )
    {
        m_fVolume = 0.0f;
    }
    m_fPlaybackSpeed = 1.0f;
    Init ();
}
CClientPlayerVoice::~CClientPlayerVoice( void )
{
    DeInit();
}

void CALLBACK BASS_VoiceStateChange ( HSYNC handle, DWORD channel, DWORD data, void* user )
{
    if ( data == 0 ) 
    {
        CClientPlayerVoice* pVoice = static_cast < CClientPlayerVoice* > ( user );
        pVoice->m_CS.Lock ();

        if ( pVoice->m_bVoiceActive )
        {
            pVoice->m_EventQueue.push_back ( "onClientPlayerVoiceStop" );
            pVoice->m_bVoiceActive = false;
        }

        pVoice->m_CS.Unlock ();
    }
}


void CClientPlayerVoice::Init( void )
{
    m_CS.Lock ();

    // Grab our sample rate and quality
    m_SampleRate = m_pVoiceRecorder->GetSampleRate();
    unsigned char ucQuality = m_pVoiceRecorder->GetSampleQuality();

    // Setup our BASS playback device
    m_pBassPlaybackStream = BASS_StreamCreate ( m_SampleRate/VOICE_SAMPLE_SIZE, 2, BASS_STREAM_AUTOFREE, STREAMPROC_PUSH, NULL );
    BASS_ChannelSetSync ( m_pBassPlaybackStream, BASS_SYNC_STALL, 0, &BASS_VoiceStateChange, this );

    BASS_ChannelPlay( m_pBassPlaybackStream, false );
    BASS_ChannelSetAttribute( m_pBassPlaybackStream, BASS_ATTRIB_VOL, m_fVolume * m_fVolumeScale );

    // Get the relevant speex mode for the servers sample rate
    const SpeexMode* speexMode = m_pVoiceRecorder->getSpeexModeFromSampleRate();
    m_pSpeexDecoderState = speex_decoder_init(speexMode);

    // Initialize our speex decoder
    speex_decoder_ctl(m_pSpeexDecoderState, SPEEX_GET_FRAME_SIZE, &m_iSpeexIncomingFrameSampleCount);
    speex_decoder_ctl(m_pSpeexDecoderState, SPEEX_SET_QUALITY, &ucQuality );

    m_CS.Unlock ();
}

void CClientPlayerVoice::DeInit( void )
{
    BASS_ChannelStop( m_pBassPlaybackStream );
    BASS_StreamFree( m_pBassPlaybackStream );

    m_pBassPlaybackStream = NULL;

    speex_decoder_destroy(m_pSpeexDecoderState);
    m_pSpeexDecoderState = NULL;

    m_SampleRate = SAMPLERATE_WIDEBAND;

}


void CClientPlayerVoice::DoPulse( void )
{
    // Dispatch queued events
    ServiceEventQueue ();

    m_CS.Lock ();
    float fPreviousVolume = 0.0f;
    g_pCore->GetCVars ()->Get ( "voicevolume", fNewVolume );
    m_CS.Unlock ();

    if ( fPreviousVolume != m_fVolumeScale && m_pPlayer->IsLocalPlayer() == false )
    {
        m_fVolumeScale = fPreviousVolume;
        float fScaledVolume = m_fVolume * m_fVolumeScale;
        BASS_ChannelSetAttribute( m_pBassPlaybackStream, BASS_ATTRIB_VOL, fScaledVolume );
    }
}


void CClientPlayerVoice::DecodeAndBuffer(char* pBuffer, unsigned int bytesWritten )
{
    m_CS.Lock ();
    CLuaArguments Arguments;
    if ( !m_bVoiceActive )
    {
        m_CS.Unlock ();
        ServiceEventQueue ();
        if ( !m_pPlayer->CallEvent ( "onClientPlayerVoiceStart", Arguments, true ) )
            return;

        m_CS.Lock ();
        m_bVoiceActive = true;
    }

    char pTempBuffer[2048];
    SpeexBits speexBits;
    speex_bits_init(&speexBits);

    speex_bits_read_from(&speexBits, (char*)(pBuffer), bytesWritten);
    speex_decode_int(m_pSpeexDecoderState, &speexBits, (spx_int16_t*)pTempBuffer);

    speex_bits_destroy(&speexBits);

    unsigned int uiSpeexBlockSize = m_iSpeexIncomingFrameSampleCount * VOICE_SAMPLE_SIZE;

    BASS_StreamPutData ( m_pBassPlaybackStream, (void*)pTempBuffer, uiSpeexBlockSize );

    m_CS.Unlock ();
}


void CClientPlayerVoice::ServiceEventQueue ( void )
{
    m_CS.Lock ();
    while ( !m_EventQueue.empty () )
    {
        SString strEvent = m_EventQueue.front ();
        m_EventQueue.pop_front ();

        m_CS.Unlock ();

        CLuaArguments Arguments;
        m_pPlayer->CallEvent ( strEvent, Arguments, true );

        m_CS.Lock ();
    }
    m_CS.Unlock ();
}

////////////////////////////////////////////////////////////
//
// CClientPlayerVoice:: Sea of sets 'n' gets
//
//
//
////////////////////////////////////////////////////////////
void CClientPlayerVoice::SetPlayPosition ( double dPosition )
{
    // Only relevant for non-streams, which are always ready if valid
    if ( m_pBassPlaybackStream )
    {
        // Make sure position is in range
        QWORD bytePosition = BASS_ChannelSeconds2Bytes( m_pBassPlaybackStream, dPosition );
        QWORD byteLength = BASS_ChannelGetLength( m_pBassPlaybackStream, BASS_POS_BYTE );
        BASS_ChannelSetPosition( m_pBassPlaybackStream, Clamp < QWORD > ( 0, bytePosition, byteLength - 1 ), BASS_POS_BYTE );
    }
}

double CClientPlayerVoice::GetPlayPosition ( void )
{
    if ( m_pBassPlaybackStream )
    {
        QWORD pos = BASS_ChannelGetPosition( m_pBassPlaybackStream, BASS_POS_BYTE );
        if ( pos != -1 )
            return BASS_ChannelBytes2Seconds( m_pBassPlaybackStream, pos );
    }
    return 0.0;
}

double CClientPlayerVoice::GetLength ( bool bAvoidLoad )
{
    if ( m_pBassPlaybackStream )
    {
        QWORD length = BASS_ChannelGetLength( m_pBassPlaybackStream, BASS_POS_BYTE );
        if ( length != -1 )
            return BASS_ChannelBytes2Seconds( m_pBassPlaybackStream, length );
    }
    return 0;
}

float CClientPlayerVoice::GetVolume ( void )
{
    return m_fVolume;
}

void CClientPlayerVoice::SetVolume ( float fVolume, bool bStore )
{
    m_fVolume = fVolume;

    if ( m_pBassPlaybackStream && m_pPlayer->IsLocalPlayer ( ) == false )
    {
        float fScaledVolume = m_fVolume * m_fVolumeScale;
        BASS_ChannelSetAttribute( m_pBassPlaybackStream, BASS_ATTRIB_VOL, fScaledVolume );
    }
}

float CClientPlayerVoice::GetPlaybackSpeed ( void )
{
    return m_fPlaybackSpeed;
}

void CClientPlayerVoice::SetPlaybackSpeed ( float fSpeed )
{
    m_fPlaybackSpeed = fSpeed;

    if ( m_pBassPlaybackStream )
        BASS_ChannelSetAttribute ( m_pBassPlaybackStream, BASS_ATTRIB_FREQ, fSpeed * m_fDefaultFrequency );
}

void CClientPlayerVoice::ApplyFXModifications ( float fSampleRate, float fTempo, float fPitch, bool bReversed )
{
    m_fSampleRate = fSampleRate;
    m_fTempo = fTempo;
    m_fPitch = fPitch;
    if ( m_pBassPlaybackStream )
    {
        if ( fTempo != m_fTempo )
        {
            m_fTempo = fTempo;
        }
        if ( fPitch != m_fPitch )
        {
            m_fPitch = fPitch;
        }
        if ( fSampleRate != m_fSampleRate )
        {
            m_fSampleRate = fSampleRate;
        }

        // Update our attributes
        BASS_ChannelSetAttribute ( m_pBassPlaybackStream, BASS_ATTRIB_TEMPO, m_fTempo );
        BASS_ChannelSetAttribute ( m_pBassPlaybackStream, BASS_ATTRIB_TEMPO_PITCH, m_fPitch );
        BASS_ChannelSetAttribute ( m_pBassPlaybackStream, BASS_ATTRIB_TEMPO_FREQ, m_fSampleRate );
    }
}

void CClientPlayerVoice::GetFXModifications ( float &fSampleRate, float &fTempo, float &fPitch, bool &bReversed )
{
    if ( m_pBassPlaybackStream )
    {
        GetTempoValues ( fSampleRate, fTempo, fPitch, bReversed );
    }
}

float* CClientPlayerVoice::GetFFTData ( int iLength )
{
    if ( m_pBassPlaybackStream )
    {
        long lFlags = BASS_DATA_FFT256;
        if ( iLength == 256 )
            lFlags = BASS_DATA_FFT256;
        else if ( iLength == 512 )
            lFlags = BASS_DATA_FFT512;
        else if ( iLength == 1024 )
            lFlags = BASS_DATA_FFT1024;
        else if ( iLength == 2048 )
            lFlags = BASS_DATA_FFT2048;
        else if ( iLength == 4096 )
            lFlags = BASS_DATA_FFT4096;
        else if ( iLength == 8192 )
            lFlags = BASS_DATA_FFT8192;
        else if ( iLength == 16384 )
            lFlags = BASS_DATA_FFT16384;
        else 
            return NULL;

        float* pData = new float[ iLength ];
        if ( BASS_ChannelGetData ( m_pBassPlaybackStream, pData, lFlags ) != -1 )
            return pData;
        else
        {
            delete [] pData;
            return NULL;
        }
    }
    return NULL;
}


float* CClientPlayerVoice::GetWaveData ( int iLength )
{
    if ( m_pBassPlaybackStream )
    {
        long lFlags = 0;
        if ( iLength == 128 || iLength == 256 || iLength == 512 || iLength == 1024 || iLength == 2048 || iLength == 4096 || iLength == 8192 || iLength == 16384 )
        {
            lFlags = 4*iLength|BASS_DATA_FLOAT;
        }
        else 
            return NULL;

        float* pData = new float [ iLength ];
        if ( BASS_ChannelGetData ( m_pBassPlaybackStream, pData, lFlags ) != -1 )
            return pData;
        else
        {
            delete [] pData;
            return NULL;
        }
    }
    return NULL;
}

DWORD CClientPlayerVoice::GetLevelData ( void )
{
    if ( m_pBassPlaybackStream )
    {
        DWORD dwData = BASS_ChannelGetLevel ( m_pBassPlaybackStream );
        if ( dwData != 0 )
            return dwData;
    }
    return 0;
}

////////////////////////////////////////////////////////////
//
// CClientSound::SetFxEffect
//
//
//
////////////////////////////////////////////////////////////
bool CClientPlayerVoice::SetFxEffect ( uint uiFxEffect, bool bEnable )
{
    if ( uiFxEffect >= NUMELMS( m_EnabledEffects ) )
        return false;

    m_EnabledEffects[uiFxEffect] = bEnable;

    // Apply if active
    if ( m_pBassPlaybackStream )
        ApplyFxEffects ();

    return true;
}

//
// Copy state stored in m_EnabledEffects to actual BASS sound
//
void CClientPlayerVoice::ApplyFxEffects ( void )
{
    for ( uint i = 0 ; i < NUMELMS(m_FxEffects) && NUMELMS(m_EnabledEffects) ; i++ )
    {
        if ( m_EnabledEffects[i] && !m_FxEffects[i] )
        {
            // Switch on
            m_FxEffects[i] = BASS_ChannelSetFX ( m_pBassPlaybackStream, i, 0 );
            if ( !m_FxEffects[i] )
                m_FxEffects[i] = INVALID_FX_HANDLE;
        }
        else
        {
            if ( !m_EnabledEffects[i] && m_FxEffects[i] )
            {
                // Switch off
                if ( m_FxEffects[i] != INVALID_FX_HANDLE )
                    BASS_ChannelRemoveFX ( m_pBassPlaybackStream, m_FxEffects[i] );
                m_FxEffects[i] = 0;
            }
        }
    }
}

bool CClientPlayerVoice::IsFxEffectEnabled ( uint uiFxEffect )
{
    if ( uiFxEffect >= NUMELMS( m_EnabledEffects ) )
        return false;

    return m_EnabledEffects[uiFxEffect] ? true : false;
}

bool CClientPlayerVoice::GetPan ( float& fPan )
{
    fPan = 0.0f;
    if ( m_pBassPlaybackStream )
    {
        BASS_ChannelGetAttribute( m_pBassPlaybackStream, BASS_ATTRIB_PAN, &fPan );
        return true;
    }
    return false;
}


bool CClientPlayerVoice::SetPan ( float fPan )
{
    if ( m_pBassPlaybackStream )
    {
        BASS_ChannelSetAttribute( m_pBassPlaybackStream, BASS_ATTRIB_PAN, fPan );

        return true;
    }

    return false;
}

void CClientPlayerVoice::SetPaused ( bool bPaused )
{
    if ( m_bPaused != bPaused )
    {
        if ( bPaused )
        {
            // call onClientPlayerVoicePause
            CLuaArguments Arguments;
            Arguments.PushString ( "paused" );     // Reason
            m_pPlayer->CallEvent ( "onClientPlayerVoicePause", Arguments, false );
        }
        else
        {
            // call onClientPlayerVoiceResumed
            CLuaArguments Arguments;
            Arguments.PushString ( "resumed" );     // Reason
            m_pPlayer->CallEvent ( "onClientPlayerVoiceResumed", Arguments, false );
        }
    }

    m_bPaused = bPaused;


    if ( m_pBassPlaybackStream )
    {
        if ( bPaused )
            BASS_ChannelPause ( m_pBassPlaybackStream );
        else
            BASS_ChannelPlay ( m_pBassPlaybackStream, false );
    }
}

bool CClientPlayerVoice::IsPaused ( void )
{
    return m_bPaused;
}
