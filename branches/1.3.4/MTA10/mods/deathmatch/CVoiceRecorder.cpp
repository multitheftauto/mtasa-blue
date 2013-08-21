/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVoiceRecorderRecorder.cpp
*  PURPOSE:     Transfer box GUI
*  DEVELOPERS:  Philip Farquharson <philip@philipfarquharson.co.uk>
*               Talidan <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CVoiceRecorder.h"

CVoiceRecorder::CVoiceRecorder( void )
{
    m_bEnabled = false;

    m_VoiceState = VOICESTATE_AWAITING_INPUT;
    m_SampleRate = SAMPLERATE_WIDEBAND;

    m_pAudioStream = NULL;

    m_pSpeexEncoderState = NULL;

    m_pOutgoingBuffer = NULL;
    m_iSpeexOutgoingFrameSampleCount = 0;
    m_uiOutgoingReadIndex = 0;
    m_uiOutgoingWriteIndex = 0;
    m_bIsSendingVoiceData = false;

    m_ulTimeOfLastSend = 0;

    m_uiBufferSizeBytes = 0;
}

CVoiceRecorder::~CVoiceRecorder( void )
{
    DeInit();
}

// TODO: Replace this with BASS
int CVoiceRecorder::PACallback( const void *inputBuffer, void *outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
    // This assumes that PACallback will only be called when userData is a valid CVoiceRecorder pointer
    CVoiceRecorder* pVoiceRecorder = static_cast < CVoiceRecorder* > ( userData );
    pVoiceRecorder->m_CS.Lock ();

    if ( pVoiceRecorder->IsEnabled() )
        pVoiceRecorder->SendFrame(inputBuffer);

    pVoiceRecorder->m_CS.Unlock ();
    return 0;
}

void CVoiceRecorder::Init( bool bEnabled, unsigned int uiServerSampleRate, unsigned char ucQuality, unsigned int uiBitrate )
{
    m_bEnabled = bEnabled;

    if ( !bEnabled ) // If we aren't enabled, don't bother continuing
        return;

    m_CS.Lock ();

    // Convert the sample rate we received from the server (0-2) into an actual sample rate
    m_SampleRate = convertServerSampleRate( uiServerSampleRate );
    m_ucQuality = ucQuality;

    // State is awaiting input
    m_VoiceState = VOICESTATE_AWAITING_INPUT;
 
    // Calculate how many frames we are storing and then the buffer size in bytes
    unsigned int iFramesPerBuffer = ( 2048 / ( 32000 / m_SampleRate ));
    m_uiBufferSizeBytes = iFramesPerBuffer * sizeof(short);

    // Time of last send, this is used to limit sending
    m_ulTimeOfLastSend = 0;

    // Get the relevant speex mode for the servers sample rate
    const SpeexMode* speexMode = getSpeexModeFromSampleRate();
    m_pSpeexEncoderState = speex_encoder_init(speexMode);

    Pa_Initialize();

    int count = Pa_GetHostApiCount();

    PaStreamParameters inputDevice;
    inputDevice.channelCount = 1;
    inputDevice.device = Pa_GetDefaultInputDevice();
    inputDevice.sampleFormat = paInt16;
    inputDevice.hostApiSpecificStreamInfo = NULL;
    inputDevice.suggestedLatency = 0;


    Pa_OpenStream (
            &m_pAudioStream, 
            &inputDevice, 
            NULL, 
            m_SampleRate, 
            iFramesPerBuffer, 
            paNoFlag, 
            PACallback, 
            this );
    
    Pa_StartStream( m_pAudioStream );

    // Initialize our outgoing buffer
    speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_GET_FRAME_SIZE, &m_iSpeexOutgoingFrameSampleCount);
    speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_SET_QUALITY, &m_ucQuality);
    int iBitRate = (int)uiBitrate;
    if ( iBitRate )
        speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_SET_BITRATE, &iBitRate );

    m_pOutgoingBuffer = (char*) malloc(m_uiBufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT);
    m_uiOutgoingReadIndex = 0;
    m_uiOutgoingWriteIndex = 0;

    // Initialise the speex preprocessor
    int iSamplingRate;
    speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_GET_SAMPLING_RATE, &iSamplingRate );
    m_pSpeexPreprocState = speex_preprocess_state_init(m_iSpeexOutgoingFrameSampleCount, iSamplingRate);

    // Set our preprocessor parameters
    int iEnable = 1; int iDisable = 0;
    speex_preprocess_ctl(m_pSpeexPreprocState, SPEEX_PREPROCESS_SET_AGC, &iEnable );
    speex_preprocess_ctl(m_pSpeexPreprocState, SPEEX_PREPROCESS_SET_DENOISE, &iEnable );
    speex_preprocess_ctl(m_pSpeexPreprocState, SPEEX_PREPROCESS_SET_DEREVERB, &iEnable );
    speex_preprocess_ctl(m_pSpeexPreprocState, SPEEX_PREPROCESS_SET_VAD, &iDisable );
    speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_SET_DTX, &iEnable );

    speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_GET_BITRATE, &iBitRate );

    g_pCore->GetConsole()->Printf( "Server Voice Chat Quality [%i];  Sample Rate: [%ikHz]; Bitrate [%ibps]", m_ucQuality, iSamplingRate, iBitRate );

    m_CS.Unlock ();
}

void CVoiceRecorder::DeInit( void )
{
    if ( m_bEnabled )
    {
        m_bEnabled = false;

        Pa_CloseStream( m_pAudioStream );
        Pa_Terminate();

        // Assumes now that PACallback will not be called in this context
        m_CS.Lock ();
        m_CS.Unlock ();
        // Assumes now that PACallback is not executing in this context

        m_pAudioStream = NULL;

        m_iSpeexOutgoingFrameSampleCount = 0;

        speex_encoder_destroy(m_pSpeexEncoderState);
        m_pSpeexEncoderState = NULL;

        speex_preprocess_state_destroy(m_pSpeexPreprocState);
        m_pSpeexPreprocState = NULL;

        free(m_pOutgoingBuffer);
        m_pOutgoingBuffer = NULL;

        m_VoiceState = VOICESTATE_AWAITING_INPUT;
        m_SampleRate = SAMPLERATE_WIDEBAND;

        m_pAudioStream = NULL;

        m_iSpeexOutgoingFrameSampleCount = 0;
        m_uiOutgoingReadIndex = 0;
        m_uiOutgoingWriteIndex = 0;
        m_bIsSendingVoiceData = false;
        m_ulTimeOfLastSend = 0;
        m_uiBufferSizeBytes = 0;
    }
}

const SpeexMode* CVoiceRecorder::getSpeexModeFromSampleRate( void )
{
    switch (m_SampleRate)
    {
        case SAMPLERATE_NARROWBAND:
            return &speex_nb_mode;
        case SAMPLERATE_WIDEBAND:
            return &speex_wb_mode;
        case SAMPLERATE_ULTRAWIDEBAND:
            return &speex_uwb_mode;
    }
    return &speex_wb_mode;
}

eSampleRate CVoiceRecorder::convertServerSampleRate( unsigned int uiServerSampleRate )
{
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

void CVoiceRecorder::UpdatePTTState( unsigned int uiState )
{
    if ( !m_bEnabled )
        return;

    m_CS.Lock ();

    if ( uiState == 1 )
    {
        if ( m_VoiceState == VOICESTATE_AWAITING_INPUT )
        {
            // Call event on the local player for starting to talk
            if ( g_pClientGame->GetLocalPlayer () )
            {
                m_CS.Unlock ();
                CLuaArguments Arguments;
                bool bEventTriggered = g_pClientGame->GetLocalPlayer ()->CallEvent ( "onClientPlayerVoiceStart", Arguments, true );

                if ( !bEventTriggered )
                {
                    return;
                }
                m_CS.Lock ();
                m_VoiceState = VOICESTATE_RECORDING;
            }
        }
    }
    else if ( uiState == 0 )
    {
        if ( m_VoiceState == VOICESTATE_RECORDING )
        {
            m_VoiceState = VOICESTATE_RECORDING_LAST_PACKET;

            // Call event on the local player for stopping to talk
            if ( g_pClientGame->GetLocalPlayer () )
            {
                m_CS.Unlock ();
                CLuaArguments Arguments;
                g_pClientGame->GetLocalPlayer ()->CallEvent ( "onClientPlayerVoiceStop", Arguments, true );
                m_CS.Lock ();
            }
        }
    }
    m_CS.Unlock ();
}

void CVoiceRecorder::DoPulse( void )
{
    m_CS.Lock ();

    char *pInputBuffer;
    char bufTempOutput[2048];
    unsigned int uiTotalBufferSize = m_uiBufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT;

    // Only send every 100 ms
    if ( CClientTime::GetTime () - m_ulTimeOfLastSend > 100 && m_VoiceState != VOICESTATE_AWAITING_INPUT)
    {
        m_bIsSendingVoiceData = false;
        unsigned int uiBytesAvailable = 0;

        if (m_uiOutgoingWriteIndex >= m_uiOutgoingReadIndex)
            uiBytesAvailable = m_uiOutgoingWriteIndex - m_uiOutgoingReadIndex;
        else
            uiBytesAvailable = m_uiOutgoingWriteIndex + (uiTotalBufferSize - m_uiOutgoingReadIndex);

        unsigned int uiSpeexBlockSize = m_iSpeexOutgoingFrameSampleCount * VOICE_SAMPLE_SIZE;

        unsigned int uiSpeexFramesAvailable = uiBytesAvailable / uiSpeexBlockSize;

        if (uiSpeexFramesAvailable > 0)
        {
            SpeexBits speexBits;
            speex_bits_init(&speexBits);

            while ( uiSpeexFramesAvailable-- > 0 )
            {
                speex_bits_reset(&speexBits);

                // Does the input data wrap around the buffer? Copy it first then
                if (m_uiOutgoingReadIndex + uiSpeexBlockSize >= uiTotalBufferSize)
                {
                    unsigned t;
                    for (t=0; t < uiSpeexBlockSize; t++)
                        bufTempOutput[t] = m_pOutgoingBuffer[t%uiTotalBufferSize];
                    pInputBuffer=bufTempOutput;
                }
                else
                    pInputBuffer = m_pOutgoingBuffer + m_uiOutgoingReadIndex;

                // Run through our preprocessor (noise/echo cancelation)
                speex_preprocess_run(m_pSpeexPreprocState, (spx_int16_t*)pInputBuffer );

                // Encode our audio stream with speex
                speex_encode_int(m_pSpeexEncoderState, (spx_int16_t*)pInputBuffer, &speexBits);

                m_uiOutgoingReadIndex = (m_uiOutgoingReadIndex + uiSpeexBlockSize)%uiTotalBufferSize;

                m_bIsSendingVoiceData = true;

                unsigned int uiBytesWritten = speex_bits_write(&speexBits, bufTempOutput, 2048);

                g_pClientGame->GetLocalPlayer()->GetVoice()->DecodeAndBuffer(bufTempOutput, uiBytesWritten);

                NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
                if ( pBitStream )
                {
                    CClientPlayer* pLocalPlayer = g_pClientGame->GetPlayerManager()->GetLocalPlayer();

                    if ( pLocalPlayer )
                    {
                        pBitStream->Write ( (unsigned short) uiBytesWritten ); // size of buffer / voice data
                        pBitStream->Write ( (char*) bufTempOutput, uiBytesWritten); // voice data

                        g_pNet->SendPacket ( PACKET_ID_VOICE_DATA, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED, PACKET_ORDERING_VOICE );
                        g_pNet->DeallocateNetBitStream ( pBitStream );
                    }
                }
            }
            speex_bits_destroy(&speexBits);

            m_ulTimeOfLastSend = CClientTime::GetTime ();
        }
    }

    if ( m_VoiceState == VOICESTATE_RECORDING_LAST_PACKET ) // End of voice data (for events)
    {
        m_VoiceState = VOICESTATE_AWAITING_INPUT;

        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
        if ( pBitStream )
        {
            CClientPlayer* pLocalPlayer = g_pClientGame->GetPlayerManager()->GetLocalPlayer();

            if ( pLocalPlayer )
            {
                g_pNet->SendPacket ( PACKET_ID_VOICE_END, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED, PACKET_ORDERING_VOICE );
                g_pNet->DeallocateNetBitStream ( pBitStream );
            }
        }
    }
    m_CS.Unlock ();
}

// Called from other thread. Critical section is already locked.
void CVoiceRecorder::SendFrame( const void* inputBuffer )
{
    if ( m_VoiceState != VOICESTATE_AWAITING_INPUT && m_bEnabled && inputBuffer )
    {
        unsigned int remainingBufferSize = 0;
        unsigned int uiTotalBufferSize = m_uiBufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT;

        // Calculate how much of our buffer is remaining
        if ( m_uiOutgoingWriteIndex >= m_uiOutgoingReadIndex )
            remainingBufferSize = uiTotalBufferSize - (m_uiOutgoingWriteIndex-m_uiOutgoingReadIndex);
        else
            remainingBufferSize = m_uiOutgoingReadIndex - m_uiOutgoingWriteIndex;

        // Copy from our input buffer to our outgoing buffer at write index
        memcpy(m_pOutgoingBuffer + m_uiOutgoingWriteIndex, inputBuffer, m_uiBufferSizeBytes);

        // Re-align our write index
        m_uiOutgoingWriteIndex += m_uiBufferSizeBytes;

        // If we have reached the end of the buffer, go back to the start
        if ( m_uiOutgoingWriteIndex == uiTotalBufferSize )
            m_uiOutgoingWriteIndex = 0;

        // Wrap around the buffer?
        if ( m_uiBufferSizeBytes >= remainingBufferSize )
            m_uiOutgoingReadIndex = ( m_uiOutgoingReadIndex + m_iSpeexOutgoingFrameSampleCount * VOICE_SAMPLE_SIZE ) % uiTotalBufferSize;
    }
}
