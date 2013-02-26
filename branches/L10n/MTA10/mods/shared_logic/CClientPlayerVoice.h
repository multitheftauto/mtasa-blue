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
                                        CClientPlayerVoice          ( CClientPlayer* pPlayer, CVoiceRecorder* pVoiceRecorder  );
                                        ~CClientPlayerVoice         ( void );
    void                                DecodeAndBuffer             ( char* pBuffer, unsigned int bytesWritten );
    void                                DoPulse                     ( void );

    bool                                m_bVoiceActive;

    std::list < SString >               m_EventQueue;
    CCriticalSection                    m_CS;

private:  
    void                                Init                        ( void );
    void                                DeInit                      ( void );
    bool                                IsActive                    ( void )                        { return m_bVoiceActive; }
    void                                ServiceEventQueue           ( void );
   
    CClientPlayer*                      m_pPlayer;
    CVoiceRecorder*                     m_pVoiceRecorder;
    unsigned int                        m_SampleRate;
    HSTREAM                             m_pBassPlaybackStream;
    void*                               m_pSpeexDecoderState;
    int                                 m_iSpeexIncomingFrameSampleCount;
    float                               m_fVolume;
};
#endif
