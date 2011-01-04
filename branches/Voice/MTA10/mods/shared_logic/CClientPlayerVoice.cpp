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

    m_bEnabled = false;

    m_bUsingVoice = false;

    m_SampleRate = CVoiceRecorder::SAMPLERATE_WIDEBAND;

    m_pPlaybackStream = NULL;

    m_pSpeexDecoderState = NULL;

    m_bBufferOutput = false;
    m_bCopiedOutputBufferToBufferedOutput = false;

    m_pIncomingBuffer = NULL;
    m_iSpeexIncomingFrameSampleCount = 0;
    m_uiIncomingReadIndex = 0;
    m_uiIncomingWriteIndex = 0;

    m_uiBufferSizeBytes = 0;
    m_pBufferedOutput = NULL;
    m_uiBufferedOutputCount = 0;
    m_bZeroBufferedOutput = false;
    m_bCopiedOutgoingBufferToBufferedOutput = false;

    Init ( true );
}

CClientPlayerVoice::~CClientPlayerVoice( void )
{
    DeInit();
}

// TODO: Replace this with BASS
static int PAPlaybackCallback( void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, PaTimestamp outTime, void *userData )
{
    g_pCore->DebugEcho("CALLBACK");
    CClientPlayerVoice* pVoice = static_cast < CClientPlayerVoice* > ( userData );

    //if ( pVoice->IsEnabled() )
    {
        pVoice->ReceiveFrame(outputBuffer);
    }

    return 0;
}

void CClientPlayerVoice::Init( bool bEnabled )
{
    m_bEnabled = bEnabled;

    if ( !bEnabled ) // If we aren't enabled, don't bother continuing
        return;

    // Grab our sample rate
    m_SampleRate = m_pVoiceRecorder->GetSampleRate();

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

    // Get the relevant speex mode for the servers sample rate
    const SpeexMode* speexMode = m_pVoiceRecorder->getSpeexModeFromSampleRate();
    m_pSpeexDecoderState = speex_decoder_init(speexMode);

    Pa_Initialize();
    //Pa_OpenStream( &m_pPlaybackStream, NULL, 0, paInt16, NULL, Pa_GetDefaultOutputDeviceID(), 1, paInt16, NULL, m_SampleRate, iFramesPerBuffer, 0, 0, PAPlaybackCallback, this );
    //Pa_OpenStream( &m_pPlaybackStream, paNoDevice, 0, paInt16, NULL, Pa_GetDefaultOutputDeviceID(), 1, paInt16, NULL, m_SampleRate, iFramesPerBuffer, 0, 0, PAPlaybackCallback, this ); 

    Pa_OpenDefaultStream( &m_pPlaybackStream,
				0,
				2,
				paInt16,
				m_SampleRate,
				iFramesPerBuffer,
				1,
				PAPlaybackCallback,
				this );

    Pa_StartStream( m_pPlaybackStream );

    m_bBufferOutput = true;
    m_bCopiedOutgoingBufferToBufferedOutput = false;

    // Iniitalize our incoming buffer
    speex_decoder_ctl(m_pSpeexDecoderState, SPEEX_GET_FRAME_SIZE, &m_iSpeexIncomingFrameSampleCount);
    m_pIncomingBuffer = (char*) malloc(m_uiBufferSizeBytes * FRAME_INCOMING_BUFFER_COUNT);
    m_uiIncomingReadIndex=0;
    m_uiIncomingWriteIndex=0;
}

void CClientPlayerVoice::DeInit( void )
{
    if ( m_bEnabled )
    {
        m_bEnabled = false;

        Pa_CloseStream( m_pPlaybackStream );
        Pa_Terminate();
        m_pPlaybackStream = NULL;

        speex_decoder_destroy(m_pSpeexDecoderState);
        m_pSpeexDecoderState = NULL;

        free(m_pIncomingBuffer);
        m_pIncomingBuffer = NULL;

        free(m_pBufferedOutput);
        m_pBufferedOutput = NULL;

        m_SampleRate = CVoiceRecorder::SAMPLERATE_WIDEBAND;

        m_pPlaybackStream = NULL;

        m_bBufferOutput = false;
        m_bCopiedOutputBufferToBufferedOutput = false;
        m_iSpeexIncomingFrameSampleCount = 0;
        m_uiIncomingReadIndex = 0;
        m_uiIncomingWriteIndex = 0;
        m_uiBufferSizeBytes = 0;
        m_uiBufferedOutputCount = 0;
        m_bZeroBufferedOutput = false;
    }
}


void CClientPlayerVoice::DoPulse( void )
{
    unsigned int uiTotalBufferSize = m_uiBufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT;

    if ( m_bZeroBufferedOutput )
    {
        for (unsigned int i = 0; i < m_uiBufferedOutputCount; i++)
            m_pBufferedOutput[i] = 0.0f;

        m_bCopiedOutgoingBufferToBufferedOutput = false;
        m_bZeroBufferedOutput = false;

        if ( m_bUsingVoice )
        {
            CLuaArguments Arguments;
            m_pPlayer->CallEvent ( "onClientPlayerVoiceStop", Arguments, true );
            m_bUsingVoice = false;
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

void CClientPlayerVoice::ReceiveFrame( void* outputBuffer )
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

void CClientPlayerVoice::DecodeAndBuffer(char* pBuffer, unsigned int bytesWritten )
{
    //if ( m_bEnabled )
    {
        g_pCore->DebugEcho("DECODE");
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

        if ( !m_bUsingVoice )
        {
            //Trigger our lua voice event
            CLuaArguments Arguments;
            m_pPlayer->CallEvent ( "onClientPlayerVoiceStart", Arguments, true );
            m_bUsingVoice = true;
        }
    }
}
