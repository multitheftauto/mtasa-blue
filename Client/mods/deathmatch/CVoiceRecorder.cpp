/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVoiceRecorderRecorder.cpp
 *  PURPOSE:     Transfer box GUI
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVoiceRecorder.h"

CVoiceRecorder::CVoiceRecorder()
{
    m_bEnabled = false;

    m_VoiceState = VOICESTATE_AWAITING_INPUT;
    m_SampleRate = SAMPLERATE_WIDEBAND;
    m_ucQuality = 0;

    m_pAudioStream = nullptr;

    m_pSpeexEncoderState = nullptr;
    m_pSpeexPreprocState = nullptr;

    m_pOutgoingBuffer = nullptr;
    m_iSpeexOutgoingFrameSampleCount = 0;
    m_uiOutgoingReadIndex = 0;
    m_uiOutgoingWriteIndex = 0;
    m_bOutgoingBufferFull = false;

    m_ulTimeOfLastSend = 0;

    m_uiBufferSizeBytes = 0;
}

CVoiceRecorder::~CVoiceRecorder()
{
    DeInit();
}

// TODO: Replace this with BASS
int CVoiceRecorder::PACallback(const void* inputBuffer, void* outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags, void* userData)
{
    // This assumes that PACallback will only be called when userData is a valid CVoiceRecorder pointer
    CVoiceRecorder* pVoiceRecorder = static_cast<CVoiceRecorder*>(userData);

    if (pVoiceRecorder->IsEnabled())
        pVoiceRecorder->SendFrame(inputBuffer);

    return 0;
}

void CVoiceRecorder::Init(bool bEnabled, unsigned int uiServerSampleRate, unsigned char ucQuality, unsigned int uiBitrate)
{
    // A re-init shouldnt leak the previous session's stream, encoder, preprocessor
    // and buffer, and disabling must still clean up the old stream
    if (m_bEnabled)
        DeInit();

    m_bEnabled = bEnabled;

    if (!bEnabled)  // If we aren't enabled, don't bother continuing
        return;

    std::unique_lock<std::mutex> lock(m_Mutex);

    // Convert the sample rate we received from the server (0-2) into an actual sample rate
    m_SampleRate = convertServerSampleRate(uiServerSampleRate);
    m_ucQuality = ucQuality;

    // State is awaiting input
    m_VoiceState = VOICESTATE_AWAITING_INPUT;

    // Calculate how many frames we are storing and then the buffer size in bytes
    unsigned int iFramesPerBuffer = (2048 / (32000 / m_SampleRate));
    m_uiBufferSizeBytes = iFramesPerBuffer * sizeof(short);

    // Time of last send, this is used to limit sending
    m_ulTimeOfLastSend = 0;

    // Get the relevant speex mode for the servers sample rate
    const SpeexMode* speexMode = getSpeexModeFromSampleRate();
    m_pSpeexEncoderState = speex_encoder_init(speexMode);

    PaError error = Pa_Initialize();

    PaStreamParameters inputDevice;
    inputDevice.channelCount = 1;
    inputDevice.device = Pa_GetDefaultInputDevice();
    inputDevice.sampleFormat = paInt16;
    inputDevice.hostApiSpecificStreamInfo = nullptr;
    inputDevice.suggestedLatency = 0;

    if (error == paNoError)
        error = Pa_OpenStream(&m_pAudioStream, &inputDevice, NULL, m_SampleRate, iFramesPerBuffer, paNoFlag, PACallback, this);
    if (error == paNoError && m_pAudioStream)
        error = Pa_StartStream(m_pAudioStream);

    // A recorder that looks enabled but never captures is worse than one that
    // reports the failure, so shut down cleanly when the mic stream cannot be
    // opened or started. The stream close must run outside the mutex, same as
    // DeInit, in case a host API leaves the callback running after a failed start
    if (error != paNoError)
    {
        m_bEnabled = false;
        lock.unlock();
        if (m_pAudioStream)
        {
            Pa_CloseStream(m_pAudioStream);
            m_pAudioStream = nullptr;
        }
        Pa_Terminate();
        lock.lock();
        if (m_pSpeexEncoderState)
        {
            speex_encoder_destroy(m_pSpeexEncoderState);
            m_pSpeexEncoderState = nullptr;
        }
        g_pCore->GetConsole()->Printf("Voice: cannot open microphone stream (%s)", Pa_GetErrorText(error));
        return;
    }

    // Initialize our outgoing buffer. A failed encoder creation would leave these
    // calls on a null handle, so the whole config chain is skipped when the
    // encoder is missing; DoPulse drops frames the same way
    int iBitRate = 0;
    if (m_pSpeexEncoderState)
    {
        speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_GET_FRAME_SIZE, &m_iSpeexOutgoingFrameSampleCount);
        int iQuality = static_cast<int>(m_ucQuality);
        speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_SET_QUALITY, &iQuality);
        iBitRate = (int)uiBitrate;
        if (iBitRate)
            speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_SET_BITRATE, &iBitRate);
    }

    m_pOutgoingBuffer = (unsigned char*)malloc(m_uiBufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT);
    if (!m_pOutgoingBuffer)
    {
        // Without the ring the recorder would look enabled but never send,
        // so shut down cleanly instead. The stream close must run outside the
        // mutex: the callback can be waiting on it, and Pa_CloseStream waits
        // for the callback to return
        m_bEnabled = false;
        lock.unlock();
        if (m_pAudioStream)
        {
            Pa_CloseStream(m_pAudioStream);
            m_pAudioStream = nullptr;
        }
        Pa_Terminate();
        lock.lock();
        if (m_pSpeexEncoderState)
        {
            speex_encoder_destroy(m_pSpeexEncoderState);
            m_pSpeexEncoderState = nullptr;
        }
        g_pCore->GetConsole()->Printf("Voice: could not allocate the voice buffer");
        return;
    }
    m_uiOutgoingReadIndex = 0;
    m_uiOutgoingWriteIndex = 0;
    m_bOutgoingBufferFull = false;

    // Initialise the speex preprocessor
    int iSamplingRate = 0;
    if (m_pSpeexEncoderState)
    {
        speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_GET_SAMPLING_RATE, &iSamplingRate);
        m_pSpeexPreprocState = speex_preprocess_state_init(m_iSpeexOutgoingFrameSampleCount, iSamplingRate);
    }

    // Set our preprocessor parameters, but only when the preprocessor exists
    int iEnable = 1;
    int iDisable = 0;
    if (m_pSpeexPreprocState)
    {
        speex_preprocess_ctl(m_pSpeexPreprocState, SPEEX_PREPROCESS_SET_AGC, &iEnable);
        speex_preprocess_ctl(m_pSpeexPreprocState, SPEEX_PREPROCESS_SET_DENOISE, &iEnable);
        speex_preprocess_ctl(m_pSpeexPreprocState, SPEEX_PREPROCESS_SET_DEREVERB, &iEnable);
        speex_preprocess_ctl(m_pSpeexPreprocState, SPEEX_PREPROCESS_SET_VAD, &iDisable);
    }
    if (m_pSpeexEncoderState)
    {
        speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_SET_DTX, &iEnable);
        speex_encoder_ctl(m_pSpeexEncoderState, SPEEX_GET_BITRATE, &iBitRate);
    }

    g_pCore->GetConsole()->Printf("Server Voice Chat Quality [%i];  Sample Rate: [%iHz]; Bitrate [%ibps]", m_ucQuality, iSamplingRate, iBitRate);
}

void CVoiceRecorder::DeInit()
{
    if (!m_bEnabled)
        return;

    // The audio callback can be mid-frame here; it must be able to finish
    // before the stream closes, so take the mutex only after the stream teardown
    m_bEnabled = false;

    if (m_pAudioStream)
        Pa_CloseStream(m_pAudioStream);
    Pa_Terminate();

    m_pAudioStream = nullptr;

    std::lock_guard<std::mutex> lock(m_Mutex);

    m_iSpeexOutgoingFrameSampleCount = 0;

    if (m_pSpeexEncoderState)
        speex_encoder_destroy(m_pSpeexEncoderState);
    m_pSpeexEncoderState = nullptr;

    if (m_pSpeexPreprocState)
        speex_preprocess_state_destroy(m_pSpeexPreprocState);
    m_pSpeexPreprocState = nullptr;

    free(m_pOutgoingBuffer);
    m_pOutgoingBuffer = nullptr;

    m_VoiceState = VOICESTATE_AWAITING_INPUT;
    m_SampleRate = SAMPLERATE_WIDEBAND;

    m_pAudioStream = nullptr;

    m_iSpeexOutgoingFrameSampleCount = 0;
    m_uiOutgoingReadIndex = 0;
    m_uiOutgoingWriteIndex = 0;
    m_bOutgoingBufferFull = false;
    m_ulTimeOfLastSend = 0;
    m_uiBufferSizeBytes = 0;
}

const SpeexMode* CVoiceRecorder::getSpeexModeFromSampleRate()
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

eSampleRate CVoiceRecorder::convertServerSampleRate(unsigned int uiServerSampleRate)
{
    switch (uiServerSampleRate)
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

void CVoiceRecorder::SetPTTState(bool bState)
{
    if (!m_bEnabled)
        return;

    m_Mutex.lock();

    if (bState)
    {
        // A re-press while the last packet is still draining continues the burst
        if (m_VoiceState == VOICESTATE_AWAITING_INPUT || m_VoiceState == VOICESTATE_RECORDING_LAST_PACKET)
        {
            // Call event on the local player for starting to talk
            if (g_pClientGame->GetLocalPlayer())
            {
                m_Mutex.unlock();
                CLuaArguments Arguments;
                bool          bEventTriggered = g_pClientGame->GetLocalPlayer()->CallEvent("onClientPlayerVoiceStart", Arguments, true);

                if (!bEventTriggered)
                    return;

                m_Mutex.lock();

                if (m_VoiceState == VOICESTATE_AWAITING_INPUT || m_VoiceState == VOICESTATE_RECORDING_LAST_PACKET)
                    m_VoiceState = VOICESTATE_RECORDING;
            }
        }
    }
    else
    {
        if (m_VoiceState == VOICESTATE_RECORDING)
        {
            m_VoiceState = VOICESTATE_RECORDING_LAST_PACKET;

            // Call event on the local player for stopping to talk
            if (g_pClientGame->GetLocalPlayer())
            {
                m_Mutex.unlock();
                CLuaArguments Arguments;
                g_pClientGame->GetLocalPlayer()->CallEvent("onClientPlayerVoiceStop", Arguments, true);
                return;
            }
        }
    }

    m_Mutex.unlock();
}

bool CVoiceRecorder::GetPTTState()
{
    // The key is up while the last packet drains, so only count active recording
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_VoiceState == VOICESTATE_RECORDING;
}

void CVoiceRecorder::DoPulse()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    // A missing buffer still needs the flush below to run, so it can reset the
    // last-packet state and send the voice end packet
    if (!m_pOutgoingBuffer && m_VoiceState != VOICESTATE_RECORDING_LAST_PACKET)
        return;

    // Only send every 100 ms, or flush the last packet right away on release
    if ((CClientTime::GetTime() - m_ulTimeOfLastSend > 100 || m_VoiceState == VOICESTATE_RECORDING_LAST_PACKET) && m_VoiceState != VOICESTATE_AWAITING_INPUT)
    {
        unsigned char* pInputBuffer;
        unsigned char  audioBuffer[2048]{};
        // One frame at the highest Speex rate (32 kHz ultra-wideband) is 1280 bytes;
        // the check keeps the local buffer in step with the largest Speex frame
        static_assert(640 * sizeof(short) <= sizeof(audioBuffer), "Voice frame exceeds the local audio buffer");
        unsigned int uiTotalBufferSize = m_uiBufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT;

        unsigned int uiBytesAvailable = 0;

        if (m_bOutgoingBufferFull)
            uiBytesAvailable = uiTotalBufferSize;
        else if (m_uiOutgoingWriteIndex >= m_uiOutgoingReadIndex)
            uiBytesAvailable = m_uiOutgoingWriteIndex - m_uiOutgoingReadIndex;
        else
            uiBytesAvailable = m_uiOutgoingWriteIndex + (uiTotalBufferSize - m_uiOutgoingReadIndex);

        unsigned int uiSpeexBlockSize = m_iSpeexOutgoingFrameSampleCount * VOICE_SAMPLE_SIZE;

        // A missing encoder leaves the frame size at zero; skip the division so
        // the pulse degrades instead of dividing by zero
        unsigned int uiSpeexFramesAvailable = uiSpeexBlockSize ? uiBytesAvailable / uiSpeexBlockSize : 0;

        if (uiSpeexFramesAvailable > 0)
        {
            SpeexBits speexBits;
            speex_bits_init(&speexBits);

            while (uiSpeexFramesAvailable-- > 0)
            {
                speex_bits_reset(&speexBits);

                // Does the input data wrap around the buffer? Copy it first then
                if (m_uiOutgoingReadIndex + uiSpeexBlockSize >= uiTotalBufferSize)
                {
                    unsigned t;
                    for (t = 0; t < uiSpeexBlockSize; t++)
                        audioBuffer[t] = m_pOutgoingBuffer[(m_uiOutgoingReadIndex + t) % uiTotalBufferSize];
                    pInputBuffer = audioBuffer;
                }
                else
                    pInputBuffer = m_pOutgoingBuffer + m_uiOutgoingReadIndex;

                // Run through our preprocessor (noise/echo cancelation)
                if (m_pSpeexPreprocState)
                    speex_preprocess_run(m_pSpeexPreprocState, (spx_int16_t*)pInputBuffer);

                // A missing encoder leaves voice without a codec; drop the frame
                // the same way an encode error is handled
                if (!m_pSpeexEncoderState)
                {
                    m_uiOutgoingReadIndex = (m_uiOutgoingReadIndex + uiSpeexBlockSize) % uiTotalBufferSize;
                    continue;
                }

                // Encode our audio stream with speex
                const int encodeResult = speex_encode_int(m_pSpeexEncoderState, (spx_int16_t*)pInputBuffer, &speexBits);

                m_uiOutgoingReadIndex = (m_uiOutgoingReadIndex + uiSpeexBlockSize) % uiTotalBufferSize;

                // A failed encode produces no usable bits, so drop the frame
                if (encodeResult < 0)
                    continue;

                unsigned int audioBufferLength = speex_bits_write(&speexBits, reinterpret_cast<char*>(audioBuffer), 2048);

                NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
                if (pBitStream)
                {
                    CClientPlayer* pLocalPlayer = g_pClientGame->GetPlayerManager()->GetLocalPlayer();

                    if (pLocalPlayer)
                    {
                        pBitStream->Write((unsigned short)audioBufferLength);
                        pBitStream->Write(reinterpret_cast<char*>(audioBuffer), audioBufferLength);

                        g_pNet->SendPacket(PACKET_ID_VOICE_DATA, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED,
                                           PACKET_ORDERING_VOICE);
                    }
                    g_pNet->DeallocateNetBitStream(pBitStream);
                }
            }
            speex_bits_destroy(&speexBits);

            m_ulTimeOfLastSend = CClientTime::GetTime();
            m_bOutgoingBufferFull = false;
        }
    }

    if (m_VoiceState == VOICESTATE_RECORDING_LAST_PACKET)  // End of voice data (for events)
    {
        // The flush sends whole frames only; discard the unsent partial frame so
        // the next talk burst does not start with stale audio
        m_uiOutgoingReadIndex = m_uiOutgoingWriteIndex;
        m_bOutgoingBufferFull = false;

        m_VoiceState = VOICESTATE_AWAITING_INPUT;

        if (g_pClientGame->GetPlayerManager()->GetLocalPlayer())
        {
            NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();

            if (pBitStream)
            {
                g_pNet->SendPacket(PACKET_ID_VOICE_END, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE_SEQUENCED, PACKET_ORDERING_VOICE);
                g_pNet->DeallocateNetBitStream(pBitStream);
            }
        }
    }
}

void CVoiceRecorder::SendFrame(const void* inputBuffer)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (m_VoiceState == VOICESTATE_AWAITING_INPUT || !m_bEnabled || !inputBuffer || !m_pOutgoingBuffer)
        return;

    unsigned int remainingBufferSize = 0;
    unsigned int uiTotalBufferSize = m_uiBufferSizeBytes * FRAME_OUTGOING_BUFFER_COUNT;

    // Calculate how much of our buffer is remaining
    if (m_bOutgoingBufferFull)
        remainingBufferSize = 0;
    else if (m_uiOutgoingWriteIndex >= m_uiOutgoingReadIndex)
        remainingBufferSize = uiTotalBufferSize - (m_uiOutgoingWriteIndex - m_uiOutgoingReadIndex);
    else
        remainingBufferSize = m_uiOutgoingReadIndex - m_uiOutgoingWriteIndex;

    // Copy from our input buffer to our outgoing buffer at write index
    memcpy(m_pOutgoingBuffer + m_uiOutgoingWriteIndex, inputBuffer, m_uiBufferSizeBytes);

    // Re-align our write index
    m_uiOutgoingWriteIndex += m_uiBufferSizeBytes;

    // If we have reached the end of the buffer, go back to the start
    if (m_uiOutgoingWriteIndex == uiTotalBufferSize)
        m_uiOutgoingWriteIndex = 0;

    // Wrap around the buffer? Skip exactly the bytes the incoming chunk overwrote.
    // If the indices meet, the ring is exactly full; the fullness flag keeps that
    // state distinct from empty for the flush
    if (m_uiBufferSizeBytes >= remainingBufferSize)
        m_uiOutgoingReadIndex = (m_uiOutgoingReadIndex + m_uiBufferSizeBytes - remainingBufferSize) % uiTotalBufferSize;

    m_bOutgoingBufferFull = (m_uiOutgoingWriteIndex == m_uiOutgoingReadIndex);
}
