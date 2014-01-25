/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVoiceRecorder.h
*  PURPOSE:     Remote player voice chat playback
*  DEVELOPERS:  Philip Farquharson <philip@philipfarquharson.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTPLAYERVOICE_H
#define __CCLIENTPLAYERVOICE_H

/*
#define VOICE_FREQUENCY                 44100
#define VOICE_SAMPLE_SIZE               2
*/

#include <speex/speex.h>
#include <CClientPlayer.h>
#include <../deathmatch/CVoiceRecorder.h>
#include <bass.h>

class CClientPlayerVoice
{
public:
    ZERO_ON_NEW
                                        CClientPlayerVoice          ( CClientPlayer* pPlayer, CVoiceRecorder* pVoiceRecorder  );
                                        ~CClientPlayerVoice         ( void );
    void                                DecodeAndBuffer             ( char* pBuffer, unsigned int bytesWritten );
    void                                DoPulse                     ( void );

    bool                                m_bVoiceActive;

    std::list < SString >               m_EventQueue;
    CCriticalSection                    m_CS;

    void                    GetTempoValues          ( float &fSampleRate, float &fTempo, float &fPitch, bool &bReverse )        { fSampleRate = m_fSampleRate; fTempo = m_fTempo; fPitch = m_fPitch; bReverse = false; };

    void                    SetPaused               ( bool bPaused  );
    bool                    IsPaused                ( void );

    void                    SetPlayPosition         ( double dPosition );
    double                  GetPlayPosition         ( void );

    double                  GetLength               ( bool bAvoidLoad = false );

    void                    SetVolume               ( float fVolume, bool bStore = true );
    float                   GetVolume               ( void );

    void                    SetPlaybackSpeed        ( float fSpeed );
    float                   GetPlaybackSpeed        ( void );

    void                    ApplyFXModifications    ( float fSampleRate, float fTempo, float fPitch, bool bReversed );
    void                    GetFXModifications      ( float &fSampleRate, float &fTempo, float &fPitch, bool &bReversed );
    float*                  GetFFTData              ( int iLength );
    float*                  GetWaveData             ( int iLength );
    bool                    IsPanEnabled            ( void );
    bool                    SetPanEnabled           ( bool bPan );
    DWORD                   GetLevelData            ( void );
    float                   GetSoundBPM             ( void );

    bool                    SetPan                  ( float fPan );
    bool                    GetPan                  ( float& fPan );

    bool                    SetFxEffect             ( uint uiFxEffect, bool bEnable );
    bool                    IsFxEffectEnabled       ( uint uiFxEffect );
    bool                    IsActive                ( void )                            { return m_bVoiceActive; }
    
private:  
    void                                Init                        ( void );
    void                                DeInit                      ( void );
    void                                ServiceEventQueue           ( void );
    void                                ApplyFxEffects              ( void );
   
    CClientPlayer*                      m_pPlayer;
    CVoiceRecorder*                     m_pVoiceRecorder;
    unsigned int                        m_SampleRate;
    HSTREAM                             m_pBassPlaybackStream;
    void*                               m_pSpeexDecoderState;
    int                                 m_iSpeexIncomingFrameSampleCount;
    float                               m_fVolume;
    float                               m_fVolumeScale;

    // Playback altering stuff
    float                               m_fPitch;
    float                               m_fTempo;
    float                               m_fSampleRate;
    bool                                m_bPan;
    float                               m_fPan;


    // Playback state
    bool                                m_bPaused;
    float                               m_fPlaybackSpeed;
    float                               m_fDefaultFrequency;

    SFixedArray < int, 9 >              m_EnabledEffects;
    SFixedArray < HFX, 9 >              m_FxEffects;
};
#endif
