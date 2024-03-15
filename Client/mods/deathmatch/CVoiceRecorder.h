/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVoiceRecorder.h
 *  PURPOSE:     Header for voice class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define VOICE_BUFFER_LENGTH             200000
#define VOICE_FREQUENCY                 44100
#define VOICE_SAMPLE_SIZE               2

#define FRAME_OUTGOING_BUFFER_COUNT 100
#define FRAME_INCOMING_BUFFER_COUNT 100

#include <mutex>
#include <speex/speex.h>
#include <speex/speex_preprocess.h>
#include <portaudio/portaudio.h>

enum eVoiceState
{
    VOICESTATE_AWAITING_INPUT = 0,
    VOICESTATE_RECORDING,
    VOICESTATE_RECORDING_LAST_PACKET,
};

enum eSampleRate
{
    SAMPLERATE_NARROWBAND = 8000,
    SAMPLERATE_WIDEBAND = 16000,
    SAMPLERATE_ULTRAWIDEBAND = 32000
};

enum eServerSampleRate
{
    SERVERSAMPLERATE_NARROWBAND = 0,
    SERVERSAMPLERATE_WIDEBAND,
    SERVERSAMPLERATE_ULTRAWIDEBAND
};

class CVoiceRecorder
{
public:
    CVoiceRecorder();
    ~CVoiceRecorder();

    void Init(bool bEnabled, std::uint32_t uiServerSampleRate, std::uint8_t ucQuality, std::uint32_t uiBitrate);

    bool IsEnabled() { return m_bEnabled; }

    void DoPulse();

    void SetPTTState(bool bState);
    bool GetPTTState();

    std::uint32_t  GetSampleRate() { return m_SampleRate; }
    std::uint8_t GetSampleQuality() { return m_ucQuality; }

    const SpeexMode* getSpeexModeFromSampleRate();

private:
    void DeInit();
    void SendFrame(const void* inputBuffer);

    static int PACallback(const void* inputBuffer, void* outputBuffer, std::uint32_t frameCount, const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags, void* userData);

    bool        m_bEnabled;
    eVoiceState m_VoiceState;

    PaStream* m_pAudioStream;

    void*                 m_pSpeexEncoderState;
    SpeexPreprocessState* m_pSpeexPreprocState;

    char*        m_pOutgoingBuffer;
    int          m_iSpeexOutgoingFrameSampleCount;
    std::uint32_t m_uiOutgoingReadIndex;
    std::uint32_t m_uiOutgoingWriteIndex;
    bool         m_bIsSendingVoiceData;

    std::uint32_t m_ulTimeOfLastSend;

    std::uint32_t m_uiBufferSizeBytes;
    eSampleRate  convertServerSampleRate(std::uint32_t uiServerSampleRate);

    eSampleRate   m_SampleRate;
    std::uint8_t m_ucQuality;

    std::list<SString> m_EventQueue;
    std::mutex         m_Mutex;
};
