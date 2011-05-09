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

#define VOICE_STOP_TIME 200 //Number of MS inactivity to assume voice has stopped

CClientPlayerVoice::CClientPlayerVoice( CClientPlayer* pPlayer, CVoiceRecorder* pVoiceRecorder )
{
    m_pPlayer = pPlayer;
    m_pVoiceRecorder = pVoiceRecorder;

    m_bVoiceActive = false;
    m_bWaitingToStop = false;
    m_ulVoiceLastActive = 0;

    m_SampleRate = SAMPLERATE_WIDEBAND;

    m_pPlaybackStream = NULL;

    m_pSpeexDecoderState = NULL;

    m_bBufferOutput = false;

    m_pIncomingBuffer = NULL;
    m_iSpeexIncomingFrameSampleCount = 0;
    m_uiIncomingReadIndex = 0;
    m_uiIncomingWriteIndex = 0;

    m_uiBufferSizeBytes = 0;
    m_pBufferedOutput = NULL;
    m_uiBufferedOutputCount = 0;

    Init ();
}

CClientPlayerVoice::~CClientPlayerVoice( void )
{
    DeInit();
}

// TODO: Replace this with BASS
int CClientPlayerVoice::PAPlaybackCallback( const void *inputBuffer, void *outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
    CClientPlayer* pPlayer = static_cast < CClientPlayer* > ( userData );
    CClientPlayerVoice* pVoice = pPlayer->GetVoice();

    pVoice->ReceiveFrame(outputBuffer);

    if ( pVoice->m_bVoiceActive && pVoice->m_bWaitingToStop )
    {
        short *pOutBuffer = (short*)outputBuffer;

        // Have we exhausted our buffer? if so, the voice has stopped
        for ( unsigned i = 0; i < pVoice->m_uiBufferedOutputCount; i++ )
            if ( pOutBuffer[i] )  //Buffer is not empty
                return 0;

        CLuaArguments Arguments;
        pPlayer->CallEvent ( "onClientPlayerVoiceStop", Arguments, true );

        pVoice->m_bVoiceActive = false;
        pVoice->m_bWaitingToStop = false;
        pVoice->m_ulVoiceLastActive = 0;
    }              

    return 0;
}

void CClientPlayerVoice::Init( void )
{
    // Grab our sample rate and quality
    m_SampleRate = m_pVoiceRecorder->GetSampleRate();
    unsigned char ucQuality = m_pVoiceRecorder->GetSampleQuality();

    // Calculate how many frames we are storing and then the buffer size in bytes
    unsigned int iFramesPerBuffer = ( 2048 / ( 32000 / m_SampleRate ));
    m_uiBufferSizeBytes = iFramesPerBuffer * sizeof(short);

    // Create our buffered output buffer
    m_uiBufferedOutputCount = m_uiBufferSizeBytes / VOICE_SAMPLE_SIZE;
    m_pBufferedOutput = (float*) malloc(sizeof(float)*m_uiBufferedOutputCount);

    // Zero our buffered output
    for ( unsigned i = 0; i < m_uiBufferedOutputCount; i++ )
        m_pBufferedOutput[i] = 0.0f;

    // Get the relevant speex mode for the servers sample rate
    const SpeexMode* speexMode = m_pVoiceRecorder->getSpeexModeFromSampleRate();
    m_pSpeexDecoderState = speex_decoder_init(speexMode);

    Pa_Initialize();

    PaStreamParameters outputDevice;
    outputDevice.channelCount = 1;
    outputDevice.device = Pa_GetDefaultOutputDevice();
    outputDevice.sampleFormat = paInt16;
    outputDevice.hostApiSpecificStreamInfo = NULL;
    outputDevice.suggestedLatency = 0;

    Pa_OpenStream (
            &m_pPlaybackStream, 
            NULL, 
            &outputDevice, 
            m_SampleRate, 
            iFramesPerBuffer, 
            paNoFlag, 
            PAPlaybackCallback, 
            m_pPlayer );

    Pa_StartStream( m_pPlaybackStream );

    m_bBufferOutput = true;

    // Iniitalize our incoming buffer
    speex_decoder_ctl(m_pSpeexDecoderState, SPEEX_GET_FRAME_SIZE, &m_iSpeexIncomingFrameSampleCount);
    speex_decoder_ctl(m_pSpeexDecoderState, SPEEX_SET_QUALITY, &ucQuality );
    m_pIncomingBuffer = (char*) malloc(m_uiBufferSizeBytes * FRAME_INCOMING_BUFFER_COUNT);
    m_uiIncomingReadIndex=0;
    m_uiIncomingWriteIndex=0;
}

void CClientPlayerVoice::DeInit( void )
{
    Pa_CloseStream( m_pPlaybackStream );
    Pa_Terminate();
    m_pPlaybackStream = NULL;

    speex_decoder_destroy(m_pSpeexDecoderState);
    m_pSpeexDecoderState = NULL;

    free(m_pIncomingBuffer);
    m_pIncomingBuffer = NULL;

    free(m_pBufferedOutput);
    m_pBufferedOutput = NULL;

    m_SampleRate = SAMPLERATE_WIDEBAND;

    m_pPlaybackStream = NULL;

    m_bBufferOutput = false;
    m_iSpeexIncomingFrameSampleCount = 0;
    m_uiIncomingReadIndex = 0;
    m_uiIncomingWriteIndex = 0;
    m_uiBufferSizeBytes = 0;
    m_uiBufferedOutputCount = 0;
}


void CClientPlayerVoice::DoPulse( void )
{
    if ( m_ulVoiceLastActive )  // If voice hasnt been recieved for 500ms, assume we're done
    {
        unsigned long ulTimePassed = CClientTime::GetTime() - m_ulVoiceLastActive;
        if ( ulTimePassed > VOICE_STOP_TIME )
        {
            m_bWaitingToStop = true;
            m_ulVoiceLastActive = 0;
        }
    }
}

void CClientPlayerVoice::ReceiveFrame( void* outputBuffer )
{
    // Cast our output buffer to short
    short *pOutBuffer = (short*)outputBuffer;

    float fVolume = 0.0f;
    if ( !g_pCore->GetCVars ()->Get ( "voicevolume", fVolume ) )
        fVolume = 1.0f;

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
    for (unsigned int i = 0; i < m_uiBufferedOutputCount; i++)
        m_pBufferedOutput[i] = 0.0f;


    unsigned int uiBytesWaitingToReturn = 0;

    unsigned int uiTotalBufferSize = m_uiBufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT;

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
            // Adjust the volume
            m_pBufferedOutput[j] = (short)((float)m_pBufferedOutput[j]*fVolume);
        }

        // Align our read index
        m_uiIncomingReadIndex += m_uiBufferSizeBytes;

        // If we are at the end of the circular buffer, go back to the start
        if (m_uiIncomingReadIndex == uiTotalBufferSize)
            m_uiIncomingReadIndex=0;
    }
}

void CClientPlayerVoice::DecodeAndBuffer(char* pBuffer, unsigned int bytesWritten )
{
    CLuaArguments Arguments;
    if ( !m_bVoiceActive )
    {
        if ( !m_pPlayer->CallEvent ( "onClientPlayerVoiceStart", Arguments, true ) )
            return;

        m_bVoiceActive = true;
    }

    m_ulVoiceLastActive = CClientTime::GetTime();

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
