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
    m_pSpeexDecoderState = NULL;

    m_pOutgoingBuffer = NULL;
    m_iSpeexOutgoingFrameSampleCount = 0;
    m_uiOutgoingReadIndex = 0;
    m_uiOutgoingWriteIndex = 0;
    m_bIsSendingVoiceData = false;
    m_bBufferOutput = false;
    m_bCopiedOutputBufferToBufferedOutput = false;

    m_pIncomingBuffer = NULL;
    m_iSpeexIncomingFrameSampleCount = 0;
    m_uiIncomingReadIndex = 0;
    m_uiIncomingWriteIndex = 0;

    m_ulTimeOfLastSend = 0;

    m_uiBufferSizeBytes = 0;
    m_pBufferedOutput = NULL;
    m_uiBufferedOutputCount = 0;
    m_bZeroBufferedOutput = false;
    m_bCopiedOutgoingBufferToBufferedOutput = false;
}

CVoiceRecorder::~CVoiceRecorder( void )
{
    DeInit();
}

// TODO: Replace this with BASS
static int PACallback( void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, PaTimestamp outTime, void *userData )
{
    //CVoiceRecorder* pVoice = static_cast < CVoiceRecorder* > ( userData );
    CClientPlayerVoice* pVoice = static_cast < CClientPlayerVoice* > ( userData );

    if ( pVoice->IsEnabled() )
    {
        g_pClientGame->GetVoiceRecorder()->SendFrame(inputBuffer);
        //pVoice->ReceiveFrame(outputBuffer);
    }

    return 0;
}

void CVoiceRecorder::Init( bool bEnabled, unsigned int uiServerSampleRate )
{
    m_bEnabled = bEnabled;

    if ( !bEnabled ) // If we aren't enabled, don't bother continuing
        return;

    // Convert the sample rate we received from the server (0-2) into an actual sample rate
    m_SampleRate = convertServerSampleRate( uiServerSampleRate );

    // State is awaiting input
    m_VoiceState = VOICESTATE_AWAITING_INPUT;
 
    // Calculate how many frames we are storing and then the buffer size in bytes
    unsigned int iFramesPerBuffer = ( 2048 / ( 32000 / m_SampleRate ));
    m_uiBufferSizeBytes = iFramesPerBuffer * sizeof(short);

    // Create our buffered output buffer
    m_uiBufferedOutputCount = m_uiBufferSizeBytes / VOICE_SAMPLE_SIZE;
    m_pBufferedOutput = (float*) malloc(sizeof(float)*m_uiBufferedOutputCount);

    // Zero our buffered output
    for ( unsigned i = 0; i < m_uiBufferedOutputCount; i++ )
        m_pBufferedOutput[i] = 0.0f;

    m_bZeroBufferedOutput = false;

    // Time of last send, this is used to limit sending
    m_ulTimeOfLastSend = 0;

    // Get the relevant speex mode for the servers sample rate
    const SpeexMode* speexMode = getSpeexModeFromSampleRate();
    m_pSpeexEncoderState = speex_encoder_init(speexMode);
    m_pSpeexDecoderState = speex_decoder_init(speexMode);

    Pa_Initialize();
    Pa_OpenStream( &m_pAudioStream, Pa_GetDefaultInputDeviceID(), 1, paInt16, NULL, Pa_GetDefaultOutputDeviceID(), 1, paInt16, NULL, m_SampleRate, iFramesPerBuffer, 0, 0, PACallback, g_pClientGame->GetLocalPlayer()->GetVoice() );
    Pa_StartStream( m_pAudioStream );

    // Initialize our outgoing buffer
    speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_GET_FRAME_SIZE, &m_iSpeexOutgoingFrameSampleCount);
    m_pOutgoingBuffer = (char*) malloc(m_uiBufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT);
    m_uiOutgoingReadIndex = 0;
    m_uiOutgoingWriteIndex = 0;
    m_bBufferOutput = true;
    m_bCopiedOutgoingBufferToBufferedOutput = false;

    // Iniitalize our incoming buffer
    speex_decoder_ctl(m_pSpeexDecoderState, SPEEX_GET_FRAME_SIZE, &m_iSpeexIncomingFrameSampleCount);
    m_pIncomingBuffer = (char*) malloc(m_uiBufferSizeBytes * FRAME_INCOMING_BUFFER_COUNT);
    m_uiIncomingReadIndex=0;
    m_uiIncomingWriteIndex=0;
}

void CVoiceRecorder::DeInit( void )
{
    if ( m_bEnabled )
    {
        m_bEnabled = false;

        Pa_CloseStream( m_pAudioStream );
        Pa_Terminate();
        m_pAudioStream = NULL;

        m_iSpeexOutgoingFrameSampleCount = 0;

        speex_encoder_destroy(m_pSpeexEncoderState);
        m_pSpeexEncoderState = NULL;

        speex_decoder_destroy(m_pSpeexDecoderState);
        m_pSpeexDecoderState = NULL;

        free(m_pOutgoingBuffer);
        m_pOutgoingBuffer = NULL;

        free(m_pIncomingBuffer);
        m_pIncomingBuffer = NULL;

        free(m_pBufferedOutput);
        m_pBufferedOutput = NULL;

        m_VoiceState = VOICESTATE_AWAITING_INPUT;
        m_SampleRate = SAMPLERATE_WIDEBAND;

        m_pAudioStream = NULL;

        m_iSpeexOutgoingFrameSampleCount = 0;
        m_uiOutgoingReadIndex = 0;
        m_uiOutgoingWriteIndex = 0;
        m_bIsSendingVoiceData = false;
        m_bBufferOutput = false;
        m_bCopiedOutputBufferToBufferedOutput = false;
        m_iSpeexIncomingFrameSampleCount = 0;
        m_uiIncomingReadIndex = 0;
        m_uiIncomingWriteIndex = 0;
        m_ulTimeOfLastSend = 0;
        m_uiBufferSizeBytes = 0;
        m_uiBufferedOutputCount = 0;
        m_bZeroBufferedOutput = false;
        m_bCopiedOutgoingBufferToBufferedOutput = false;
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

CVoiceRecorder::eSampleRate CVoiceRecorder::convertServerSampleRate( unsigned int uiServerSampleRate )
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

    if ( uiState == 1 )
    {
        if ( m_VoiceState == VOICESTATE_AWAITING_INPUT )
        {
            // Call event on the local player for starting to talk
            if ( g_pClientGame->GetLocalPlayer () )
            {
                CLuaArguments Arguments;
                bool bEventTriggered = g_pClientGame->GetLocalPlayer ()->CallEvent ( "onClientPlayerVoiceStart", Arguments, true );
            
                if ( !bEventTriggered )
                {
                    return;
                }
                else
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
                CLuaArguments Arguments;
                g_pClientGame->GetLocalPlayer ()->CallEvent ( "onClientPlayerVoiceStop", Arguments, true );
            }
        }
    }
}

void CVoiceRecorder::DoPulse( void )
{
    char *pInputBuffer;
    char bufTempOutput[2048];
    unsigned int uiTotalBufferSize = m_uiBufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT;

    if ( m_bZeroBufferedOutput )
    {
        for (unsigned int i = 0; i < m_uiBufferedOutputCount; i++)
            m_pBufferedOutput[i] = 0.0f;

        m_bCopiedOutgoingBufferToBufferedOutput = false;
        m_bZeroBufferedOutput = false;
    }

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

                // Encode our audio stream with speex
                speex_encode_int(m_pSpeexEncoderState, (spx_int16_t*)pInputBuffer, &speexBits);

                m_uiOutgoingReadIndex = (m_uiOutgoingReadIndex + uiSpeexBlockSize)%uiTotalBufferSize;

                m_bIsSendingVoiceData = true;

                unsigned int uiBytesWritten = speex_bits_write(&speexBits, bufTempOutput, 2048);

                //#ifdef VOICE_DEBUG_LOCAL_PLAYBACK
                g_pClientGame->GetLocalPlayer()->GetVoice()->DecodeAndBuffer(bufTempOutput, uiBytesWritten);
                //#endif

                NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
                if ( pBitStream )
                {
                    CClientPlayer* pLocalPlayer = g_pClientGame->GetPlayerManager()->GetLocalPlayer();

                    if ( pLocalPlayer )
                    {
                        pBitStream->Write ( (unsigned short) uiBytesWritten ); // size of buffer / voice data
                        pBitStream->Write ( (char*) bufTempOutput, uiBytesWritten); // voice data

                        g_pNet->SendPacket(PACKET_ID_VOICE_DATA, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE, PACKET_ORDERING_GAME);
                        g_pNet->DeallocateNetBitStream( pBitStream );
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
                g_pCore->GetConsole()->Print("Last Packet!");
                g_pNet->SendPacket(PACKET_ID_VOICE_END, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE, PACKET_ORDERING_GAME);
                g_pNet->DeallocateNetBitStream( pBitStream );
            }
        }
    }

    // Buffer our output
    if (m_bCopiedOutgoingBufferToBufferedOutput == false)
    {
        unsigned int uiBytesWaitingToReturn = 0;

        // Calculate how many bytes we have spare
        if (m_uiIncomingReadIndex <= m_uiIncomingWriteIndex)
            uiBytesWaitingToReturn = m_uiIncomingWriteIndex - m_uiIncomingReadIndex;
        else
            uiBytesWaitingToReturn = uiTotalBufferSize-m_uiIncomingReadIndex + m_uiIncomingWriteIndex;

        // If we have none...
        if (uiBytesWaitingToReturn == 0)
        {
            // Start buffering the output
            m_bBufferOutput=true;
        }
        else if ( m_bBufferOutput==false || uiBytesWaitingToReturn > m_uiBufferSizeBytes*2 )
        {
            // Don't process call this function againn until we have recieved data
            m_bCopiedOutgoingBufferToBufferedOutput = true;

            // Stop buffering output
            m_bBufferOutput = false;

            // Make sure we don't exceed the buffer size, we can have less and fill the rest of the buffer with silence
            if (uiBytesWaitingToReturn > m_uiBufferSizeBytes)
            {
                uiBytesWaitingToReturn = m_uiBufferSizeBytes;
            }
            else
            {
                // Align the write index
                m_uiIncomingWriteIndex = m_uiIncomingReadIndex + m_uiBufferSizeBytes;

                // If we are at the end of the circular buffer, go back to the start
                if (m_uiIncomingWriteIndex == uiTotalBufferSize)
                    m_uiIncomingWriteIndex = 0;
            }

            // Get our input buffer ( buffer + read offset )
            short *pBufInput = (short *)(m_pIncomingBuffer + m_uiIncomingReadIndex);
            for (unsigned int j = 0; j < uiBytesWaitingToReturn / VOICE_SAMPLE_SIZE; j++)
            {
                // Write clamped to buffered output
                m_pBufferedOutput[j] += pBufInput[ j % (uiTotalBufferSize/VOICE_SAMPLE_SIZE) ];
            }

            // Align our read index
            m_uiIncomingReadIndex += m_uiBufferSizeBytes;

            // If we are at the end of the circular buffer, go back to the start
            if (m_uiIncomingReadIndex == uiTotalBufferSize)
                m_uiIncomingReadIndex=0;
        }
    }
}

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

void CVoiceRecorder::ReceiveFrame( void* outputBuffer )
{
    // Cast our output buffer to short
    short *pOutBuffer = (short*)outputBuffer;

    // Clamp floats to short
    for (unsigned int i = 0; i < m_uiBufferSizeBytes / VOICE_SAMPLE_SIZE; i++)
    {
        if (m_pBufferedOutput[i] > 32767.0f)
            pOutBuffer[i] = 32767;
        else if (m_pBufferedOutput[i]<-32768.0f)
            pOutBuffer[i] =- 32768;
        else
            pOutBuffer[i] = (short)m_pBufferedOutput[i];
    }

    // Zero all data
    m_bZeroBufferedOutput=true;
}

void CVoiceRecorder::DecodeAndBuffer(char* pBuffer, unsigned int bytesWritten )
{
    if ( m_bEnabled )
    {
        char pTempBuffer[2048];
        SpeexBits speexBits;
        speex_bits_init(&speexBits);

        speex_bits_read_from(&speexBits, (char*)(pBuffer), bytesWritten);
        speex_decode_int(m_pSpeexDecoderState, &speexBits, (spx_int16_t*)pTempBuffer);

        speex_bits_destroy(&speexBits);

        unsigned int remainingBufferSize = 0;

        unsigned int uiTotalBufferSize=m_uiBufferSizeBytes * FRAME_INCOMING_BUFFER_COUNT;
        if (m_uiIncomingWriteIndex >= m_uiIncomingReadIndex)
            remainingBufferSize = uiTotalBufferSize - (m_uiIncomingWriteIndex-m_uiIncomingReadIndex);
        else
            remainingBufferSize = m_uiIncomingReadIndex - m_uiIncomingWriteIndex;

        unsigned int uiSpeexBlockSize = m_iSpeexIncomingFrameSampleCount * VOICE_SAMPLE_SIZE;

        // Will it fit directly into the buffer? Otherwise wrap it
        if (m_uiIncomingWriteIndex + uiSpeexBlockSize <= uiTotalBufferSize)
        {
            memcpy(m_pIncomingBuffer + m_uiIncomingWriteIndex, pTempBuffer, uiSpeexBlockSize);
        }
        else
        {
            memcpy(m_pIncomingBuffer + m_uiIncomingWriteIndex, pTempBuffer, uiTotalBufferSize-m_uiIncomingWriteIndex);
            memcpy(m_pIncomingBuffer, pTempBuffer, uiSpeexBlockSize - ( uiTotalBufferSize - m_uiIncomingWriteIndex ));
        }
        m_uiIncomingWriteIndex=(m_uiIncomingWriteIndex+uiSpeexBlockSize) % uiTotalBufferSize;

        // Will we over run the buffer space available?
        if (m_uiBufferSizeBytes >= remainingBufferSize)
        {
            // Increment our read index by one block
            m_uiIncomingReadIndex += m_uiBufferSizeBytes;

            // If we have reached the end of the circular buffer, go back to the start
            if (m_uiIncomingReadIndex == uiTotalBufferSize)
                m_uiIncomingReadIndex=0;
        }
    }
}
