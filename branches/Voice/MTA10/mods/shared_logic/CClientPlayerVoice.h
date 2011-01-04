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

                                        /*
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
                                        };*/

    void                                Init                        ( bool bEnabled );
    void                                DeInit                      ( void );

    void                                ReceiveFrame                ( void* outputBuffer );  

    bool                                IsEnabled                   ( void )                        { return m_bEnabled; }

    void                                DecodeAndBuffer             ( char* pBuffer, unsigned int bytesWritten );

    void                                DoPulse                     ( void );

private:  
    CClientPlayer*                      m_pPlayer;
    CVoiceRecorder*                     m_pVoiceRecorder;
    bool                                m_bEnabled;
    bool                                m_bUsingVoice;
    unsigned int                        m_SampleRate;

    PortAudioStream*                    m_pPlaybackStream;

    void*                               m_pSpeexDecoderState;

    bool                                m_bBufferOutput;
    bool                                m_bCopiedOutputBufferToBufferedOutput;

    char*                               m_pIncomingBuffer;
    int                                 m_iSpeexIncomingFrameSampleCount;
    unsigned int                        m_uiIncomingReadIndex;
    unsigned int                        m_uiIncomingWriteIndex;

    unsigned int                        m_uiBufferSizeBytes;
    float*                              m_pBufferedOutput;
    unsigned int                        m_uiBufferedOutputCount;
    bool                                m_bZeroBufferedOutput;
    bool                                m_bCopiedOutgoingBufferToBufferedOutput;

};
#endif

