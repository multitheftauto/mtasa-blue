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
    // start off uninitialised
    m_bInitialised = false;
    // start off in test mode
    m_bInTestMode = true;
    // device index is -1 for default
    m_iDeviceIndex = -1;
    // initialise port audio
    Pa_Initialize();
    // assume that voice isn't enabled
    m_bServerVoiceEnabled = false;
    // not de-initializing
    m_bDeinitialising = false;
}

CVoiceManager::~CVoiceManager ( void )
{
    // deinitialise
    DeInit ( );
    // terminate port audio
    Pa_Terminate ( );
}

int CVoiceManager::PACallback( const void *inputBuffer, void *outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
    // This assumes that PACallback will only be called when userData is a valid CVoiceRecorder pointer
    CVoiceManager* pVoiceManager = g_pCore->GetVoiceManager ( );

    // if we are shutting down don't carry on and risk locking the critical section
    if ( pVoiceManager->IsDeinitialising ( ) )
        return 0;

    // handle test mode/local only playback
    pVoiceManager->BufferAudioFrame ( inputBuffer, frameCount );

    // if we are not in test mode
    if ( !pVoiceManager->IsInTestMode ( ) )
    {
        // trigger the callback in deathmatch
        pVoiceManager->TriggerCallback ( inputBuffer, outputBuffer, frameCount, timeInfo, statusFlags, userData );
    }
#ifdef MTA_DEBUG
    if ( statusFlags != 0 )
    {
        g_pCore->GetConsole()->Printf("[Voice Error] Status flags %i", statusFlags);
    }
#endif

    return 0;
}

void CALLBACK BASS_VoiceStateChange ( HSYNC handle, DWORD channel, DWORD data, void* user )
{
    // stub
#ifdef MTA_DEBUG
    g_pCore->GetConsole()->Printf("[Voice Error] Voice state change %i", data);
#endif
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

void CVoiceManager::SetTestMode ( bool bTestMode )
{
    // lock our critical section
    m_CS.Lock ( );

    // set test mode to bTestMode and redirect input to the menu
    m_bInTestMode = bTestMode;

    // unlock our critical section
    m_CS.Unlock ( );
}

void CVoiceManager::BufferAudioFrame ( const void *inputBuffer, unsigned long frameCount )
{
    // ensure we have an input buffer, apparently this can happen but hasn't so far
    if ( inputBuffer == NULL )
        return;

    // lock our critical section
    m_CS.Lock ( );

    // make sure we have a valid playback stream
    if ( m_pBassPlaybackStream != NULL )
    {
        // local playback or test mode
        if ( m_bLocalPlaybackEnabled || m_bInTestMode )
        {
            // push our buffer to the playback stream, each frame is 2 bytes.
            BASS_StreamPutData ( m_pBassPlaybackStream, inputBuffer, frameCount * 4 );
#ifdef MTA_DEBUG
            if ( frameCount * 4 != m_uiBufferSizeBytes )
            {
                g_pCore->GetConsole()->Printf("[Voice Error] Buffer mismatch %i %i", frameCount * 4, m_uiBufferSizeBytes);
            }
#endif
            #ifdef VOICE_DEBUG_LOCAL_PLAYBACK
            // if we are paused
            if ( m_bPaused )
            {
                // pause playback for x seconds, this is called frequently enough we know that it will unpause
                if ( m_tElapsedTime.Get ( ) >= VOICE_DEBUG_LOCAL_PLAYBACK_LATENCY )
                {
                    // unpause, don't reset
                    BASS_ChannelPlay ( m_pBassPlaybackStream, false );
                }
                m_bPaused = false;
            }
            #endif
        }
    }

    // unlock our critical section
    m_CS.Unlock ( );
}

void CVoiceManager::SetServerVoiceEnabled ( bool bEnabled )
{
    // set our voice enabled state
    m_bServerVoiceEnabled = bEnabled;
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

void CVoiceManager::Init ( void* user, unsigned int uiServerSampleRate, bool bTestMode )
{
    // lock our critical section
    m_CS.Lock ( );

    // make sure we aren't already initialised
    if ( m_bInitialised )
    {
        // unlock our critical section
        m_CS.Unlock ( );
        // already initialised
        return;
    }

    // set test mode
    m_bInTestMode = bTestMode;
    
    // save our init flags
    m_tVoiceManagerInitFlags = tVoiceManagerInitFlags ( user, uiServerSampleRate, bTestMode );

    // set us as initialised
    m_bInitialised = true;

    // Convert the sample rate we received from the server (0-2) into an actual sample rate
    m_SampleRate = convertServerSampleRate( uiServerSampleRate );

    // Calculate how many frames we are storing and then the buffer size in bytes
    unsigned int iFramesPerBuffer = ( 4096 / ( 44100 / m_SampleRate ));
    m_uiBufferSizeBytes = iFramesPerBuffer * 4;

    // initialise our audio device - Moved to constructor so that we can get device info neatly
    //Pa_Initialize();
    
    // start off using our default device
    PaDeviceIndex deviceIndex = Pa_GetDefaultInputDevice ( );

    // we have a device to try
    if ( m_iDeviceIndex != -1 )
    {
        // set our device index
        deviceIndex = (PaDeviceIndex) m_iDeviceIndex;
        // validate the device id by checking if it has device info
        if ( Pa_GetDeviceInfo ( deviceIndex ) == NULL )
        {
            // looks like it's invalid so use the default again
            deviceIndex = Pa_GetDefaultInputDevice ( );
            // don't try again
            m_iDeviceIndex = (PaDeviceIndex) -1;
        }
    }

    const PaDeviceInfo * pDeviceInfo = Pa_GetDeviceInfo ( deviceIndex );

    //int count = Pa_GetHostApiCount();

    // setup our stream parameters
    PaStreamParameters inputDevice;
    inputDevice.channelCount = 1;
    inputDevice.device = deviceIndex;
    inputDevice.sampleFormat = paFloat32;
    inputDevice.hostApiSpecificStreamInfo = NULL;
    inputDevice.suggestedLatency = 1;


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


#ifdef VOICE_USE_FLOAT
    // Setup our BASS playback device
    m_pBassPlaybackStream = BASS_StreamCreate ( (DWORD)pStreamInfo->sampleRate, 1, BASS_STREAM_AUTOFREE|BASS_SAMPLE_FLOAT|BASS_SAMPLE_MONO, STREAMPROC_PUSH, NULL );
#else
    // Setup our BASS playback device
    m_pBassPlaybackStream = BASS_StreamCreate ( m_SampleRate * 2, 1, BASS_STREAM_AUTOFREE|BASS_SAMPLE_MONO, STREAMPROC_PUSH, NULL );
#endif

    // not sure if this is strictly required to make it a stall stream
    BASS_ChannelSetSync ( m_pBassPlaybackStream, BASS_SYNC_STALL, 0, &BASS_VoiceStateChange, this );

    // play the stream
    BASS_ChannelPlay( m_pBassPlaybackStream, false );

    // set the volume
    BASS_ChannelSetAttribute( m_pBassPlaybackStream, BASS_ATTRIB_VOL, m_bLocalPlaybackEnabled == true ? 1.0f : 0.0f );

    // not de-initializing
    m_bDeinitialising = false;

    // unlock our critical section
    m_CS.Unlock ( );
}

void CVoiceManager::DeInit ( void )
{
    // De-initialising
    m_bDeinitialising = true;

    // Lock the critical section
    m_CS.Lock ( );

    // Stop the stream - if you find a freeze here it's because another thread is currently triggering the callback and flushing the critical section
    // did not work, I've tested it but threading issues suck so an easy fix would be keeping track of if we are in the callback and waiting here
    Pa_StopStream ( m_pAudioStream );

    // close our audio stream
    Pa_CloseStream( m_pAudioStream );

    // terminate port audio - Moved to destructor
    //Pa_Terminate ( );

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

    // not initialised
    m_bInitialised = false;

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

DWORD CVoiceManager::GetLevelData ( void )
{
    // get our return value (level data in both channels) and store it temporarily so we can unlock before we return
    DWORD dwReturn = BASS_ChannelGetLevel ( m_pBassPlaybackStream );

    // return our level data
    return dwReturn;
}


std::map < int, SString > CVoiceManager::GetAvailableDevices ( void )
{
    // returns a map containing the device ID and device names of the input devices available
    std::map < int, SString > mapDevices = std::map < int, SString > ( );
    // loop through the device count
    for ( int i = 0; i < Pa_GetDeviceCount ( ); i++ )
    {
        // check if it has device info
        const PaDeviceInfo * pDeviceInfo = Pa_GetDeviceInfo ( (PaDeviceIndex)i );
        // if the device info is valid and we have input channels (channels we can record) add it
        if ( pDeviceInfo && pDeviceInfo->maxInputChannels > 0 )
        {
            // add it
            mapDevices[i] = pDeviceInfo->name;
        }
    }
    // return our map
    return mapDevices;
}


void CVoiceManager::SetAudioInputDevice ( int iAudioDeviceID )
{
    // check it's a valid device now
    const PaDeviceInfo * pDeviceInfo = Pa_GetDeviceInfo ( (PaDeviceIndex)iAudioDeviceID );
    // device info should return null if not valid
    if ( pDeviceInfo )
    {
        // update our device ID to use
        m_iDeviceIndex = iAudioDeviceID;
        // if we are initialized we need to reinitialize quickly
        if ( m_bInitialised )
        {
            // save the test mode flag
            bool bTestMode = m_bInTestMode;
            // save the callback
            PaStreamCallback * pCallback = m_pCallback;

            // de-initialise
            DeInit ( );

            // initialise
            Init ( m_tVoiceManagerInitFlags.m_user, m_tVoiceManagerInitFlags.m_uiServerSampleRate, m_tVoiceManagerInitFlags.m_bInTestMode );

            // restore test mode
            m_bInTestMode = bTestMode;
            // restore the callback
            m_pCallback = pCallback;
        }
    }
}

void CVoiceManager::SetLocalPlaybackEnabled ( bool bEnabled )
{
    // lock the critical section
    m_CS.Lock ( );

    // update our client variables
    CVARS_SET ( "localvoiceplayback", bEnabled );

    // Local state
    m_bLocalPlaybackEnabled = bEnabled;

    // check if we have a stream yet
    if ( m_pBassPlaybackStream != NULL )
    {
        // set the volume
        BASS_ChannelSetAttribute( m_pBassPlaybackStream, BASS_ATTRIB_VOL, bEnabled == true ? 1.0f : 0.0f );
    }

    // unlock the critical section
    m_CS.Unlock ( );
}