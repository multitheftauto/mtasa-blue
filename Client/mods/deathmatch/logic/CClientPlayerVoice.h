/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVoiceRecorder.h
 *  PURPOSE:     Remote player voice chat playback
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

/*
#define VOICE_FREQUENCY                 44100
#define VOICE_SAMPLE_SIZE               2
*/

#include <mutex>
#include <speex/speex.h>
#include <CClientPlayer.h>
#include <../deathmatch/CVoiceRecorder.h>
#include <bass.h>

class CClientPlayerVoice
{
public:
    ZERO_ON_NEW
    CClientPlayerVoice(CClientPlayer* pPlayer, CVoiceRecorder* pVoiceRecorder);
    ~CClientPlayerVoice();
    void DecodeAndBuffer(const unsigned char* voiceBuffer, unsigned int voiceBufferLength);
    void DoPulse();

    bool m_bVoiceActive;

    std::list<SString> m_EventQueue;
    std::mutex         m_Mutex;  // Only for m_EventQueue and m_bVoiceActive

    void GetTempoValues(float& fSampleRate, float& fTempo, float& fPitch, bool& bReverse)
    {
        fSampleRate = m_fSampleRate;
        fTempo = m_fTempo;
        fPitch = m_fPitch;
        bReverse = false;
    };

    void SetPaused(bool bPaused);
    bool IsPaused();

    void   SetPlayPosition(double dPosition);
    double GetPlayPosition();

    double GetLength(bool bAvoidLoad = false);

    void  SetVolume(float fVolume, bool bStore = true);
    float GetVolume();

    void  SetPlaybackSpeed(float fSpeed);
    float GetPlaybackSpeed();

    void   ApplyFXModifications(float fSampleRate, float fTempo, float fPitch, bool bReversed);
    void   GetFXModifications(float& fSampleRate, float& fTempo, float& fPitch, bool& bReversed);
    float* GetFFTData(int iLength);
    float* GetWaveData(int iLength);
    bool   IsPanEnabled();
    bool   SetPanEnabled(bool bPan);
    DWORD  GetLevelData();
    float  GetSoundBPM();

    bool SetPan(float fPan);
    bool GetPan(float& fPan);

    bool SetFxEffect(uint uiFxEffect, bool bEnable);
    bool IsFxEffectEnabled(uint uiFxEffect);

    bool SetFxEffectParameters(std::uint32_t uiFxEffect, void* params);
    bool GetFxEffectParameters(std::uint32_t uiFxEffect, void* params);

    bool IsActive() { return m_bVoiceActive; }

private:
    void Init();
    void DeInit();
    bool TakeDecodeCredit();
    void ServiceEventQueue();
    void ApplyFxEffects();

    CClientPlayer*  m_pPlayer;
    CVoiceRecorder* m_pVoiceRecorder;
    unsigned int    m_SampleRate;
    HSTREAM         m_pBassPlaybackStream;
    void*           m_pSpeexDecoderState;
    int             m_iSpeexIncomingFrameSampleCount;
    float           m_fVolume;
    float           m_fVolumeScale;

    // Playback altering stuff
    float m_fPitch;
    float m_fTempo;
    float m_fSampleRate;
    bool  m_bPan;
    float m_fPan;

    // Playback state
    bool  m_bPaused;
    float m_fPlaybackSpeed;
    float m_fDefaultFrequency;

    SFixedArray<int, 9> m_EnabledEffects;
    SFixedArray<HFX, 9> m_FxEffects;

    // Decode throttle. Credit accrues at the real-time frame rate (one frame per
    // VOICE_FRAME_DURATION_MS) up to the server-configured burst ceiling, so a
    // speaker who is merely ahead of a slow client frame is never dropped, while a
    // flood is capped at the ceiling and then held to real-time.
    float         m_fDecodeCredit;
    unsigned long m_ulDecodeCreditTime;
};
