/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVoiceManager.h
*  PURPOSE:     core based voice manager class header - Provides an API for Grabbing Audio data from Port Audio in Speex format
*  DEVELOPERS:  Cazomino05 <>
*
*****************************************************************************/

#ifndef __CVOICEMANAGER_H
#define __CVOICEMANAGER_H

#include <speex/speex.h>
#include <speex/speex_preprocess.h>
#include <portaudio/portaudio.h>

#include <core/CVoiceManagerInterface.h>

#define VOICE_BUFFER_LENGTH             200000
#define VOICE_FREQUENCY                 44100
#define VOICE_SAMPLE_SIZE               2

#define FRAME_OUTGOING_BUFFER_COUNT 100
#define FRAME_INCOMING_BUFFER_COUNT 100

// Uncomment this to hear yourself speak locally (Voice is still encoded & decoded to simulate network transmission)
#define VOICE_DEBUG_LOCAL_PLAYBACK
#define VOICE_DEBUG_LOCAL_PLAYBACK_LATENCY 15000
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

struct tVoiceManagerInitFlags
{
    tVoiceManagerInitFlags ( )
    {
        m_user = NULL;
        m_uiServerSampleRate = SAMPLERATE_ULTRAWIDEBAND;
        m_bInTestMode = false;
    }
    tVoiceManagerInitFlags ( void* user, unsigned int uiServerSampleRate, bool bInTestMode )
    {
        m_user = user;
        m_uiServerSampleRate = uiServerSampleRate;
        m_bInTestMode = bInTestMode;
    }
    void*           m_user;
    unsigned int    m_uiServerSampleRate;
    bool            m_bInTestMode;
};

class CVoiceManager : public CVoiceManagerInterface
{
public:
                    CVoiceManager               ( void );
                    ~CVoiceManager              ( void );

    bool            IsInTestMode                ( void )                { return m_bInTestMode; };
    void            SetTestMode                 ( bool bTestMode );
    void            BufferAudioFrame              ( const void *inputBuffer, unsigned long framecount );

    void            SetServerVoiceEnabled       ( bool bEnabled );
    bool            IsServerVoiceEnabled        ( void )                { return m_bServerVoiceEnabled; };

    bool            IsInitialised               ( void )                { return m_bInitialised; };
    void            Init                        ( void* user, unsigned int uiServerSampleRate, bool bTestMode );
    void            DeInit                      ( void );

    void            TriggerCallback             ( const void *inputBuffer, void *outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, 
                                                  PaStreamCallbackFlags statusFlags, void *userData );
    void            RegisterCallback            ( PaStreamCallback * pCallback );


    // Exported to deathmatch to share a critical section ensuring that the code is locked at the right times
    void            Lock                            ( void )            { m_CS.Lock ( ); };
    void            Unlock                          ( void )            { m_CS.Unlock ( ); };


    DWORD           GetLevelData                    ( void );

    std::map < int, SString > GetAvailableDevices   ( void );

    void                    SetAudioInputDevice             ( int iAudioDeviceID );
    void                    SetLocalPlaybackEnabled         ( bool bEnabled );

    static eSampleRate      convertServerSampleRate         ( unsigned int uiServerSampleRate );

    static int              PACallback                      ( const void *inputBuffer, void *outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, 
                                                             PaStreamCallbackFlags statusFlags, void *userData );

private:
    bool                                m_bInTestMode;

    
    PaStream*                           m_pAudioStream;

    eSampleRate                         m_SampleRate;
    
    PaStreamCallback   *                m_pCallback;

    CCriticalSection                    m_CS;

    CBassAudio *                        m_pBassAudioStream;

    HSTREAM                             m_pBassPlaybackStream;
    unsigned int                        m_uiBufferSizeBytes;

    bool                                m_bInitialised;
    bool                                m_bServerVoiceEnabled;
    bool                                m_bLocalPlaybackEnabled;

    PaDeviceIndex                       m_iDeviceIndex;

    tVoiceManagerInitFlags              m_tVoiceManagerInitFlags;

#ifdef VOICE_DEBUG_LOCAL_PLAYBACK
    bool                                m_bPaused;
    CElapsedTime                        m_tElapsedTime;
#endif

};

#endif