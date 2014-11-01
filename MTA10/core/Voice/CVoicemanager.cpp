/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVoiceManager.cpp
*  PURPOSE:     core based voice manager class - Provides an API for Grabbing Audio data from Port Audio in Speex format
*  DEVELOPERS:  Cazomino05 <>
*
*****************************************************************************/

#include "StdInc.h"
#include "Voice/CVoiceManager.h"

CVoiceManager::CVoiceManager ( void )
{
    
}

int CVoiceManager::PACallback( const void *inputBuffer, void *outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
    // This assumes that PACallback will only be called when userData is a valid CVoiceRecorder pointer
    CVoiceManager* pVoiceManager = g_pCore->GetVoiceManager ( );

    // if we are in test mode
    if ( pVoiceManager->IsInTestMode ( ) )
    {
        // handle test mode
        pVoiceManager->HandleTestMode ( inputBuffer, frameCount );
    }
    else
    {
        // trigger the callback in deathmatch
        pVoiceManager->TriggerCallback ( inputBuffer, outputBuffer, frameCount, timeInfo, statusFlags, userData );
    }
    
    return 0;
}

void CALLBACK BASS_VoiceStateChange ( HSYNC handle, DWORD channel, DWORD data, void* user )
{
    // stub
}

void CVoiceManager::TriggerCallback ( const void *inputBuffer, void *outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
    // Lock our critical section
    m_CS.Lock ( );

    // check if we have a callback to trigger
    if ( m_pCallback )
    {
        // trigger our callback in deathmatch
        m_pCallback ( inputBuffer, outputBuffer, frameCount, timeInfo, statusFlags, userData );
    }

    // unlock our critical section
    m_CS.Unlock ( );
}

void CVoiceManager::HandleTestMode ( const void *inputBuffer, unsigned long frameCount )
{
    // lock our critical section
    m_CS.Lock ( );

    // make sure we have a valid playback stream
    if ( m_pBassPlaybackStream != NULL )
    {
        // push our buffer to the playback stream, each frame is 2 bytes.
        BASS_StreamPutData ( m_pBassPlaybackStream, inputBuffer, frameCount * 2 );
    }

    // unlock our critical section
    m_CS.Unlock ( );
}

void CVoiceManager::RegisterCallback ( PaStreamCallback * pCallback )
{
    // lock our critical section
    m_CS.Lock ( );

    // set our callback
    m_pCallback = pCallback;

    // unlock our critical section
    m_CS.Unlock ( );
}

void CVoiceManager::Init ( void* user, unsigned int uiServerSampleRate, unsigned int uiBitrate )
{
    // lock our critical section
    m_CS.Lock ( );

    // Convert the sample rate we received from the server (0-2) into an actual sample rate
    m_SampleRate = convertServerSampleRate( uiServerSampleRate );

    // Calculate how many frames we are storing and then the buffer size in bytes
    unsigned int iFramesPerBuffer = ( 2048 / ( 32000 / m_SampleRate ));
    m_uiBufferSizeBytes = iFramesPerBuffer * sizeof(short);

    // initialise our audio device
    Pa_Initialize();


    //int count = Pa_GetHostApiCount();

    // setup our stream parameters
    PaStreamParameters inputDevice;
    inputDevice.channelCount = 1;
    inputDevice.device = Pa_GetDefaultInputDevice();
    inputDevice.sampleFormat = paInt16;
    inputDevice.hostApiSpecificStreamInfo = NULL;
    inputDevice.suggestedLatency = 0;


    // open the audio stream
    Pa_OpenStream (
            &m_pAudioStream, 
            &inputDevice, 
            NULL, 
            m_SampleRate, 
            iFramesPerBuffer, 
            paNoFlag, 
            PACallback, 
            user );
    
    // start the stream
    Pa_StartStream( m_pAudioStream );

    // get our stream information
    const PaStreamInfo *pStreamInfo = Pa_GetStreamInfo ( m_pAudioStream );

    // set a sample rate
    double dSampleRate = m_SampleRate;

    // if we have our stream info
    if ( pStreamInfo )
    {
        // use that sample rate
        dSampleRate = pStreamInfo->sampleRate;
    }

    // Setup our BASS playback device
    m_pBassPlaybackStream = BASS_StreamCreate ( (DWORD)dSampleRate, 1, BASS_STREAM_AUTOFREE, STREAMPROC_PUSH, NULL );
    // not sure if this is strictly required to make it a stall stream
    BASS_ChannelSetSync ( m_pBassPlaybackStream, BASS_SYNC_STALL, 0, &BASS_VoiceStateChange, this );

    // play the stream
    BASS_ChannelPlay( m_pBassPlaybackStream, false );
    // set the volume
    BASS_ChannelSetAttribute( m_pBassPlaybackStream, BASS_ATTRIB_VOL, 1.0f );

    // unlock our critical section
    m_CS.Unlock ( );
}

void CVoiceManager::DeInit ( void )
{
    // Lock the critical section
    m_CS.Lock ( );

    // close our audio stream
    Pa_CloseStream( m_pAudioStream );


    // terminate port audio
    Pa_Terminate ( );

    // stop the playback stream
    BASS_ChannelStop( m_pBassPlaybackStream );
    // free the playback stream
    BASS_StreamFree( m_pBassPlaybackStream );

    // null everything
    m_pBassPlaybackStream = NULL;
    m_pAudioStream = NULL;
    m_pCallback = NULL;

    // test mode reset
    m_bInTestMode = true;

    // unlock the critical section
    m_CS.Unlock ( );
}

eSampleRate CVoiceManager::convertServerSampleRate( unsigned int uiServerSampleRate )
{
    // copied from CVoiceRecorder
    switch ( uiServerSampleRate )
    {
        case SERVERSAMPLERATE_NARROWBAND:
            return SAMPLERATE_NARROWBAND;
        case SERVERSAMPLERATE_WIDEBAND:
            return SAMPLERATE_WIDEBAND;
        case SERVERSAMPLERATE_ULTRAWIDEBAND:
            return SAMPLERATE_ULTRAWIDEBAND;
    }
    return SAMPLERATE_WIDEBAND;
}