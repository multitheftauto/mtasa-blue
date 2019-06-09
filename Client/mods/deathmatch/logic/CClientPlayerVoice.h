/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVoiceRecorder.h
 *  PURPOSE:     Remote player voice chat playback
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
    CClientPlayerVoice(CClientPlayer* pPlayer, CVoiceRecorder* pVoiceRecorder);
    ~CClientPlayerVoice();
    void DecodeAndBuffer(char* pBuffer, unsigned int bytesWritten);
    void DoPulse();

    bool m_bVoiceActive;

    std::list<SString> m_EventQueue;
    std::mutex         m_Mutex;            // Only for m_EventQueue and m_bVoiceActive

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
    bool IsActive() { return m_bVoiceActive; }

private:
    void Init();
    void DeInit();
    void ServiceEventQueue();
    void ApplyFxEffects();

    CClientPlayer*  m_pPlayer = nullptr;
    CVoiceRecorder* m_pVoiceRecorder = nullptr;
    unsigned int    m_SampleRate = 0;
    HSTREAM         m_pBassPlaybackStream = 0;
    void*           m_pSpeexDecoderState = nullptr;
    int             m_iSpeexIncomingFrameSampleCount = 0;
    float           m_fVolume = 0.0f;
    float           m_fVolumeScale = 0.0f;

    // Playback altering stuff
    float m_fPitch = 0.0f;
    float m_fTempo = 0.0f;
    float m_fSampleRate = 0.0f;
    bool  m_bPan = false;
    float m_fPan = 0.0f;

    // Playback state
    bool  m_bPaused = false;
    float m_fPlaybackSpeed = 0.0f;
    float m_fDefaultFrequency = 0.0f;

    SFixedArray<int, 9> m_EnabledEffects = {};
    SFixedArray<HFX, 9> m_FxEffects = {};
};
