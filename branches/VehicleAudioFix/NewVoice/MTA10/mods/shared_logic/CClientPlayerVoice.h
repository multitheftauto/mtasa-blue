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
#define VOICE_BUFFER_LENGTH             200000
#define VOICE_FREQUENCY                 44100
#define VOICE_SAMPLE_SIZE               2

#define FRAME_OUTGOING_BUFFER_COUNT 100
#define FRAME_INCOMING_BUFFER_COUNT 100*/

#include <speex/speex.h>
#include <portaudio/portaudio.h>
#include <CClientPlayer.h>
#include <../deathmatch/CVoiceRecorder.h>

class CClientPlayerVoice
{
public:
                                        CClientPlayerVoice                      ( CClientPlayer* pPlayer, CVoiceRecorder* pVoiceRecorder  );
                                        ~CClientPlayerVoice                     ( void );

    void                                Init                        ( void );
    void                                DeInit                      ( void );

    void                                ReceiveFrame                ( void* outputBuffer );  

    bool                                IsActive                    ( void )                        { return m_bVoiceActive; }

    void                                DecodeAndBuffer             ( char* pBuffer, unsigned int bytesWritten );

    void                                DoPulse                     ( void );

    static int                          PAPlaybackCallback          ( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData );
    static void                         PAFinishCallback            ( void *userData );
    
    bool                                m_bVoiceActive;
    bool                                m_bWaitingToStop;
    unsigned long                       m_ulVoiceLastActive;

protected:
    unsigned int                        m_uiBufferedOutputCount;
    bool                                m_bBufferOutput;

private:  
    CClientPlayer*                      m_pPlayer;
    CVoiceRecorder*                     m_pVoiceRecorder;
    unsigned int                        m_SampleRate;

    PaStream*                           m_pPlaybackStream;

    void*                               m_pSpeexDecoderState;

    char*                               m_pIncomingBuffer;
    int                                 m_iSpeexIncomingFrameSampleCount;
    unsigned int                        m_uiIncomingReadIndex;
    unsigned int                        m_uiIncomingWriteIndex;

    unsigned int                        m_uiBufferSizeBytes;
    float*                              m_pBufferedOutput;
};
#endif

