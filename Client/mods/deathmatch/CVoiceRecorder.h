/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/CVoiceRecorder.h
 *  PURPOSE:     Header for voice class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define SAMPLING_RATE (opus_int32)48000
#define CHANNELS 1            // Mono
#define APPLICATION OPUS_APPLICATION_VOIP
#define MAX_FRAME_SIZE (48000 * CHANNELS)
#define FRAME_SIZE (SAMPLING_RATE / 50)            // 20 ms
#define BANDWIDTH OPUS_BANDWIDTH_WIDEBAND
#define BITRATE (opus_int32)192000
#define VBR (opus_int32)1
#define COMPLEXITY (opus_int32)10
#define MAX_PACKET 1500
#define MAX_PAYLOAD_BYTES MAX_PACKET
#define DTX (opus_int32)1

/// #define FRAME_DURATION 0.02
/// #define SAMPLES_PER_FRAME (int)(SAMPLE_RATE * FRAME_DURATION + 1)
/// #define BYTES_PER_SAMPLE sizeof(opus_int16)
/// #define BYTES_PER_FRAME (SAMPLES_PER_FRAME * BYTES_PER_SAMPLE)
/// #define BUFFER_LENGTH (BYTES_PER_FRAME * CHANNELS * sizeof(opus_int16))
/// #define BUFFER_LENGTH 4096

// Uncomment this to hear yourself speak locally
#define VOICE_DEBUG_LOCAL_PLAYBACK

#include <mutex>
#include <libopus/include/opus.h>
#include <bass/bass.h>

class CVoiceRecorder
{
public:
    CVoiceRecorder();
    ~CVoiceRecorder();

    bool IsEnabled() { return m_enabled; }

    void DoPulse();

private:
    void SendFrame(const void* buffer, DWORD length);

    static int __stdcall BASSCallback(HRECORD handle, const void* buffer, DWORD length, void* user);

    bool m_enabled;

    HRECORD m_recordHandle;
    HSTREAM m_streamHandle;

    OpusEncoder* m_encoder;
    OpusDecoder* m_decoder;

    unsigned char* m_inputBuffer;

    unsigned int   m_writeIndex;
    unsigned char* m_encodeBuffer;

    unsigned int m_readIndex;
    opus_int16*  m_decodeBuffer;

    unsigned long m_timeOfLastSend;

    std::mutex m_Mutex;
};
