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
    g_pCore->GetCVars ()->Get ( "voicevolume", m_fVolume );

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

        if ( !pVoice->m_bVoiceActive )
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
    BASS_ChannelSetAttribute( m_pBassPlaybackStream, BASS_ATTRIB_VOL, m_fVolume );

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

    float fPreviousVolume = m_fVolume;
    g_pCore->GetCVars ()->Get ( "voicevolume", m_fVolume );

    m_CS.Lock ();
    if ( fPreviousVolume != m_fVolume )
        BASS_ChannelSetAttribute( m_pBassPlaybackStream, BASS_ATTRIB_VOL, m_fVolume );
    m_CS.Unlock ();
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
