/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/CVoiceRecorder.cpp
 *  PURPOSE:     Recording voice
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVoiceRecorder.h"

CVoiceRecorder::CVoiceRecorder() : m_enabled(true)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    // Recorder
    BASS_RecordInit(-1);
    m_recordHandle = BASS_RecordStart(SAMPLING_RATE, CHANNELS, BASS_RECORD_PAUSE, &BASSCallback, this);
    BASS_ChannelPlay(m_recordHandle, false);

    // Encoder
    m_encoder = opus_encoder_create(SAMPLING_RATE, CHANNELS, APPLICATION, nullptr);
    opus_encoder_ctl(m_encoder, OPUS_SET_BITRATE(BITRATE));
    opus_encoder_ctl(m_encoder, OPUS_SET_BANDWIDTH(BANDWIDTH));
    opus_encoder_ctl(m_encoder, OPUS_SET_VBR(VBR));
    opus_encoder_ctl(m_encoder, OPUS_SET_COMPLEXITY(COMPLEXITY));
    opus_encoder_ctl(m_encoder, OPUS_SET_DTX(DTX));

    m_inputBuffer = (unsigned char*)malloc(MAX_FRAME_SIZE * CHANNELS * sizeof(unsigned char));
    m_encodeBuffer = (unsigned char*)malloc(MAX_FRAME_SIZE * CHANNELS * sizeof(unsigned char));

    // Decoder
    m_decoder = opus_decoder_create(SAMPLING_RATE, CHANNELS, nullptr);
    opus_decoder_ctl(m_decoder, OPUS_SET_BITRATE(BITRATE));
    opus_decoder_ctl(m_decoder, OPUS_SET_BANDWIDTH(BANDWIDTH));
    opus_decoder_ctl(m_decoder, OPUS_SET_VBR(VBR));
    opus_decoder_ctl(m_decoder, OPUS_SET_COMPLEXITY(COMPLEXITY));
    opus_decoder_ctl(m_decoder, OPUS_SET_DTX(DTX));

    m_decodeBuffer = (opus_int16*)malloc(MAX_FRAME_SIZE * CHANNELS * sizeof(opus_int16));

    // Playback
    m_streamHandle = BASS_StreamCreate(SAMPLING_RATE, CHANNELS, BASS_STREAM_AUTOFREE, STREAMPROC_PUSH, nullptr);
    BASS_ChannelPlay(m_streamHandle, false);
}

CVoiceRecorder::~CVoiceRecorder()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    // Recorder
    BASS_ChannelStop(m_recordHandle);
    BASS_RecordFree();
    m_recordHandle = NULL;

    // Playback
    BASS_ChannelStop(m_streamHandle);
    BASS_StreamFree(m_streamHandle);
    m_streamHandle = NULL;

    // Encoder
    opus_encoder_destroy(m_encoder);
    m_encoder = nullptr;

    free(m_inputBuffer);
    SAFE_DELETE(m_inputBuffer);

    free(m_encodeBuffer);
    SAFE_DELETE(m_encodeBuffer);

    // Decoder
    opus_decoder_destroy(m_decoder);
    m_decoder = nullptr;

    /*
    free(m_encodeBuffer);
    SAFE_DELETE(m_encodeBuffer);

    free(m_frameBuffer);
    SAFE_DELETE(m_frameBuffer);

    free(m_outputBuffer);
    SAFE_DELETE(m_outputBuffer);

    free(m_decodeBuffer);
    SAFE_DELETE(m_decodeBuffer);
    */
}

int CVoiceRecorder::BASSCallback(HRECORD handle, const void* buffer, DWORD length, void* user)
{
    if (length == 0 || buffer == nullptr)
        return 1;

    // This assumes that BASSCallback will only be called when user is a valid CVoiceRecorder pointer
    CVoiceRecorder* voiceRecorder = static_cast<CVoiceRecorder*>(user);

    if (!voiceRecorder->IsEnabled())
        return 0;

    voiceRecorder->SendFrame(buffer, length);

    // Continue recording
    return 1;
}

void CVoiceRecorder::DoPulse()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (CClientTime::GetTime() - m_timeOfLastSend > 20)
    {
        opus_int32 result = opus_encode(m_encoder, (opus_int16*)m_inputBuffer, SAMPLING_RATE / 50, m_encodeBuffer, MAX_PACKET);

        g_pCore->GetConsole()->Printf("%i", m_writeIndex);

        opus_decode(m_decoder, m_encodeBuffer, result, m_decodeBuffer, SAMPLING_RATE / 50, 0);

        BASS_StreamPutData(m_streamHandle, (void*)m_decodeBuffer, result);

        m_timeOfLastSend = CClientTime::GetTime();
    }
}

void CVoiceRecorder::SendFrame(const void* buffer, DWORD length)
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (m_writeIndex + length > MAX_FRAME_SIZE * CHANNELS * sizeof(unsigned char))
        m_writeIndex = 0;

    memcpy(m_inputBuffer + m_writeIndex, buffer, length);
    m_writeIndex += length;
}
