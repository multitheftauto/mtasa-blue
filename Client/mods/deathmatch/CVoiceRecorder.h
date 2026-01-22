/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVoiceRecorder.h
 *  PURPOSE:     Header for voice class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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

    void Init(bool bEnabled, unsigned int uiServerSampleRate, unsigned char ucQuality, unsigned int uiBitrate);

    bool IsEnabled() { return m_bEnabled; }

    void DoPulse();

    void SetPTTState(bool bState);
    bool GetPTTState();

    unsigned int  GetSampleRate() { return m_SampleRate; }
    unsigned char GetSampleQuality() { return m_ucQuality; }

    const SpeexMode* getSpeexModeFromSampleRate();

private:
    void DeInit();
    void SendFrame(const void* inputBuffer);

    static int PACallback(const void* inputBuffer, void* outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags, void* userData);

    bool        m_bEnabled;
    eVoiceState m_VoiceState;

    PaStream* m_pAudioStream;

    void*                 m_pSpeexEncoderState;
    SpeexPreprocessState* m_pSpeexPreprocState;

    char*        m_pOutgoingBuffer;
    int          m_iSpeexOutgoingFrameSampleCount;
    unsigned int m_uiOutgoingReadIndex;
    unsigned int m_uiOutgoingWriteIndex;
    bool         m_bIsSendingVoiceData;

    unsigned long m_ulTimeOfLastSend;

    unsigned int m_uiBufferSizeBytes;
    eSampleRate  convertServerSampleRate(unsigned int uiServerSampleRate);

    eSampleRate   m_SampleRate;
    unsigned char m_ucQuality;

    std::list<SString> m_EventQueue;
    std::mutex         m_Mutex;
};
