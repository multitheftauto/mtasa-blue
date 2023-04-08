/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *  PURPOSE:
 *
 *  TODO:      - Handle tempo attributes when the sound moves in/out of range
 *             - Enable beat event for non streams
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CBassAudio.h"
#include <process.h>
#include <tags.h>
#include <bassmix.h>
#include <basswma.h>
#include <bass_fx.h>

void CALLBACK BPMCallback(int handle, float bpm, void* user);
void CALLBACK BeatCallback(DWORD chan, double beatpos, void* user);

#define INVALID_FX_HANDLE (-1)  // Hope that BASS doesn't use this as a valid Fx handle

namespace
{
    //
    // Use ids instead of points for callback arguments,
    // as it's easier to identify an invalid id
    //
    CCriticalSection             ms_CallbackCS;
    std::map<void*, CBassAudio*> ms_CallbackIdMap;
    uint                         ms_uiNextCallbackId = 1;

    // Get callback id for this CBassAudio
    void* AddCallbackId(CBassAudio* pBassAudio)
    {
        ms_CallbackCS.Lock();
        void* uiId = (void*)(++ms_uiNextCallbackId ? ms_uiNextCallbackId : ++ms_uiNextCallbackId);
        MapSet(ms_CallbackIdMap, uiId, pBassAudio);
        ms_CallbackCS.Unlock();
        return uiId;
    }

    // Mark callback id as no longer valid
    void RemoveCallbackId(void* uiId)
    {
        ms_CallbackCS.Lock();
        MapRemove(ms_CallbackIdMap, uiId);
        ms_CallbackCS.Unlock();
    }

    // Get pointer from id
    CBassAudio* LockCallbackId(void* uiId)
    {
        ms_CallbackCS.Lock();
        return MapFindRef(ms_CallbackIdMap, uiId);
    }

    // Finish with pointer
    void UnlockCallbackId() { ms_CallbackCS.Unlock(); }
}            // namespace

CBassAudio::CBassAudio(bool bStream, const SString& strPath, bool bLoop, bool bThrottle, bool b3D)
    : m_bStream(bStream), m_strPath(strPath), m_bLoop(bLoop), m_bThrottle(bThrottle), m_b3D(b3D)
{
    m_fVolume = 1.0f;
    m_fDefaultFrequency = 44100.0f;
    m_fMinDistance = 5.0f;
    m_fMaxDistance = 20.0f;
    m_fPlaybackSpeed = 1.0f;
    m_bPaused = false;
    m_bPan = true;
    m_uiCallbackId = AddCallbackId(this);
}

CBassAudio::CBassAudio(void* pBuffer, unsigned int uiBufferLength, bool bLoop, bool b3D)
    : m_bStream(false), m_pBuffer(pBuffer), m_uiBufferLength(uiBufferLength), m_bLoop(bLoop), m_bThrottle(false), m_b3D(b3D)
{
    m_fVolume = 1.0f;
    m_fDefaultFrequency = 44100.0f;
    m_fMinDistance = 5.0f;
    m_fMaxDistance = 20.0f;
    m_fPlaybackSpeed = 1.0f;
    m_bPaused = false;
    m_bPan = true;
    m_uiCallbackId = AddCallbackId(this);
}

CBassAudio::~CBassAudio()
{
    assert(m_uiCallbackId == 0);
    if (m_pSound)
    {
        BASS_ChannelStop(m_pSound);
    }

    SAFE_DELETE(m_pVars);
}

void CBassAudio::Destroy()
{
    RemoveCallbackId(m_uiCallbackId);
    m_uiCallbackId = 0;
    if (m_pSound)
    {
        // Remove all callbacks
        BASS_FX_BPM_BeatFree(m_pSound);
        BASS_FX_BPM_Free(m_pSound);
        BASS_ChannelRemoveSync(m_pSound, m_hSyncDownload);
        BASS_ChannelRemoveSync(m_pSound, m_hSyncEnd);
        BASS_ChannelRemoveSync(m_pSound, m_hSyncFree);
        BASS_ChannelRemoveSync(m_pSound, m_hSyncMeta);

        // Best way to minimize a freeze during BASS_ChannelStop:
        //   * BASS_ChannelPause
        //   * wait a little bit
        //   * BASS_ChannelStop
        BASS_ChannelPause(m_pSound);
        g_pClientGame->GetManager()->GetSoundManager()->QueueAudioStop(this);
    }
    else
        delete this;
}

//
// This will return false for non streams if the file is not correct
//
bool CBassAudio::BeginLoadingMedia()
{
    assert(!m_pSound && !m_bPendingPlay);

    // Calc the flags
    long lFlags = BASS_STREAM_AUTOFREE | BASS_SAMPLE_SOFTWARE;
#if 0   // Everything sounds better in ste-reo
    if ( m_b3D )
        lFlags |= BASS_SAMPLE_MONO;
#endif
    if (m_bLoop)
        lFlags |= BASS_SAMPLE_LOOP;

    if (m_bThrottle)
        lFlags |= BASS_STREAM_RESTRATE;

    if (m_bStream)
    {
        //
        // For streams, begin the connect sequence
        //
        assert(!m_pVars);
        m_pVars = new SSoundThreadVariables();
        m_pVars->strURL = m_strPath;
        m_pVars->lFlags = lFlags;
        CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&CBassAudio::PlayStreamIntern), m_uiCallbackId, 0, NULL);
        m_bPendingPlay = true;
        OutputDebugLine("[Bass]        stream connect started");
    }
    else
    {
        //
        // For non streams, try to load the sound file
        //
        // First x streams need to be decoders rather than "real" sounds but that's dependent on if we need streams or not so we need to adapt.
        /*
            We are the Borg. Lower your shields and surrender your ships.
            We will add your biological and technological distinctiveness to our own.
            Your culture will adapt to service us.
            Resistance is futile.
        */
        long lCreateFlags = BASS_MUSIC_PRESCAN | BASS_STREAM_DECODE;

        if (!m_pBuffer)
        {
            m_pSound = BASS_StreamCreateFile(false, FromUTF8(m_strPath), 0, 0, lCreateFlags | BASS_UNICODE);
            if (!m_pSound)
                m_pSound = BASS_MusicLoad(false, FromUTF8(m_strPath), 0, 0, BASS_MUSIC_RAMP | BASS_MUSIC_PRESCAN | BASS_STREAM_DECODE | BASS_UNICODE,
                                          0);            // Try again
            if (!m_pSound && m_b3D)
                m_pSound = ConvertFileToMono(m_strPath);            // Last try if 3D
        }
        else
        {
            m_pSound = BASS_StreamCreateFile(true, m_pBuffer, 0, m_uiBufferLength, lCreateFlags);
            if (!m_pSound)
                m_pSound = BASS_MusicLoad(true, m_pBuffer, 0, m_uiBufferLength, lCreateFlags, 0);
        }

        // Failed to load ?
        if (!m_pSound)
        {
            g_pCore->GetConsole()->Printf("BASS ERROR %d in LoadMedia  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), *m_strPath, m_b3D, m_bLoop);
            return false;
        }

        m_pSound = BASS_FX_ReverseCreate(m_pSound, 2.0f, BASS_STREAM_DECODE | BASS_FX_FREESOURCE | BASS_MUSIC_PRESCAN);
        BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_REVERSE_DIR, BASS_FX_RVS_FORWARD);
        // Sucks.
        /*if ( BASS_FX_BPM_CallbackSet ( m_pSound, (BPMPROC*)&BPMCallback, 1, 0, 0, m_uiCallbackId ) == false )
        {
            g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_FX_BPM_CallbackSet  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), *m_strPath, m_b3D, m_bLoop
        );
        }*/

        if (BASS_FX_BPM_BeatCallbackSet(m_pSound, (BPMBEATPROC*)&BeatCallback, m_uiCallbackId) == false)
        {
            g_pCore->GetConsole()->Printf("BASS ERROR %d in BASS_FX_BPM_BeatCallbackSet  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), *m_strPath, m_b3D,
                                          m_bLoop);
        }

        if (!m_pSound)
        {
            g_pCore->GetConsole()->Printf("BASS ERROR %d in BASS_FX_ReverseCreate  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), *m_strPath, m_b3D, m_bLoop);
            return false;
        }
        m_pSound = BASS_FX_TempoCreate(m_pSound, lFlags | BASS_FX_FREESOURCE);
        if (!m_pSound)
        {
            g_pCore->GetConsole()->Printf("BASS ERROR %d in CreateTempo  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), *m_strPath, m_b3D, m_bLoop);
            return false;
        }
        BASS_ChannelGetAttribute(m_pSound, BASS_ATTRIB_TEMPO, &m_fTempo);
        BASS_ChannelGetAttribute(m_pSound, BASS_ATTRIB_TEMPO_PITCH, &m_fPitch);
        BASS_ChannelGetAttribute(m_pSound, BASS_ATTRIB_TEMPO_FREQ, &m_fSampleRate);
        // Validation of some sort
        if (m_bLoop && BASS_ChannelFlags(m_pSound, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP) == -1)
            g_pCore->GetConsole()->Printf("BASS ERROR %d in LoadMedia ChannelFlags LOOP  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), *m_strPath, m_b3D,
                                          m_bLoop);

        BASS_ChannelGetAttribute(m_pSound, BASS_ATTRIB_FREQ, &m_fDefaultFrequency);
        m_bPendingPlay = true;
        SetFinishedCallbacks();
        OutputDebugLine("[Bass] sound loaded");
    }

    return true;
}

const char* CBassAudio::ErrorGetMessage()
{
    // As BASS has no function to retrieve the error messages we must do it manually
    // So this should be kept updated
    // Error messages copied directly from BASS
    switch (ErrorGetCode())
    {
        case BASS_OK:
            return "all is OK";
        case BASS_ERROR_MEM:
            return "memory error";
        case BASS_ERROR_FILEOPEN:
            return "can't open the file";
        case BASS_ERROR_DRIVER:
            return "can't find a free/valid driver";
        case BASS_ERROR_BUFLOST:
            return "the sample buffer was lost";
        case BASS_ERROR_HANDLE:
            return "invalid handle";
        case BASS_ERROR_FORMAT:
            return "unsupported sample format";
        case BASS_ERROR_POSITION:
            return "invalid position";
        case BASS_ERROR_INIT:
            return "BASS_Init has not been successfully called";
        case BASS_ERROR_START:
            return "BASS_Start has not been successfully called";
        case BASS_ERROR_SSL:
            return "SSL/HTTPS support isn't available";
        case BASS_ERROR_REINIT:
            return "device needs to be reinitialized";
        case BASS_ERROR_ALREADY:
            return "already initialized/paused/whatever";
        case BASS_ERROR_NOTAUDIO:
            return "file does not contain audio";
        case BASS_ERROR_NOCHAN:
            return "can't get a free channel";
        case BASS_ERROR_ILLTYPE:
            return "an illegal type was specified";
        case BASS_ERROR_ILLPARAM:
            return "an illegal parameter was specified";
        case BASS_ERROR_NO3D:
            return "no 3D support";
        case BASS_ERROR_NOEAX:
            return "no EAX support";
        case BASS_ERROR_DEVICE:
            return "illegal device number";
        case BASS_ERROR_NOPLAY:
            return "not playing";
        case BASS_ERROR_FREQ:
            return "illegal sample rate";
        case BASS_ERROR_NOTFILE:
            return "the stream is not a file stream";
        case BASS_ERROR_NOHW:
            return "no hardware voices available";
        case BASS_ERROR_EMPTY:
            return "the file has no sample data";
        case BASS_ERROR_NONET:
            return "no internet connection could be opened";
        case BASS_ERROR_CREATE:
            return "couldn't create the file";
        case BASS_ERROR_NOFX:
            return "effects are not available";
        case BASS_ERROR_NOTAVAIL:
            return "requested data/action is not available";
        case BASS_ERROR_DECODE:
            return "the channel is/isn't a \"decoding channel\"";
        case BASS_ERROR_DX:
            return "a sufficient DirectX version is not installed";
        case BASS_ERROR_TIMEOUT:
            return "connection timedout";
        case BASS_ERROR_FILEFORM:
            return "unsupported file format";
        case BASS_ERROR_SPEAKER:
            return "unavailable speaker";
        case BASS_ERROR_VERSION:
            return "invalid BASS version (used by add-ons)";
        case BASS_ERROR_CODEC:
            return "codec is not available/supported";
        case BASS_ERROR_ENDED:
            return "the channel/file has ended";
        case BASS_ERROR_BUSY:
            return "the device is busy";
        case BASS_ERROR_UNSTREAMABLE:
            return "unstreamable file";
        case BASS_ERROR_PROTOCOL:
            return "unsupported protocol";
        case BASS_ERROR_DENIED:
            return "access denied";
        case BASS_ERROR_UNKNOWN:
            return "some other mystery problem";

        // BASS may add new error codes in the future, which must be appended to the switch above.
        // Also, add-ons may introduce additional error codes.
        default:
            return "unknown error (?)";
    }
}

int CBassAudio::ErrorGetCode()
{
    return BASS_ErrorGetCode();
}

//
// Util use in BeginLoadingMedia
//
HSTREAM CBassAudio::ConvertFileToMono(const SString& strPath)
{
    HSTREAM decoder =
        BASS_StreamCreateFile(false, FromUTF8(strPath), 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_MONO | BASS_UNICODE);            // open file for decoding
    if (!decoder)
        return 0;                                                                                           // failed
    DWORD            length = static_cast<DWORD>(BASS_ChannelGetLength(decoder, BASS_POS_BYTE));            // get the length
    void*            data = malloc(length);                                                                 // allocate buffer for decoded data
    BASS_CHANNELINFO ci;
    BASS_ChannelGetInfo(decoder, &ci);            // get sample format
    if (ci.chans > 1)                             // not mono, downmix...
    {
        HSTREAM mixer = BASS_Mixer_StreamCreate(ci.freq, 1, BASS_STREAM_DECODE | BASS_MIXER_END);            // create mono mixer
        BASS_Mixer_StreamAddChannel(
            mixer, decoder, BASS_MIXER_DOWNMIX | BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE);            // plug-in the decoder (auto-free with the mixer)
        decoder = mixer;                                                                                 // decode from the mixer
    }
    length = BASS_ChannelGetData(decoder, data, length);                                                    // decode data
    BASS_StreamFree(decoder);                                                                               // free the decoder/mixer
    HSTREAM stream = BASS_StreamCreate(ci.freq, 1, BASS_STREAM_AUTOFREE, STREAMPROC_PUSH, NULL);            // create stream
    BASS_StreamPutData(stream, data, length);                                                               // set the stream data
    free(data);                                                                                             // free the buffer
    return stream;
}

//
// Thread callbacks
//

void CALLBACK DownloadSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
    CBassAudio* pBassAudio = LockCallbackId(user);
    if (pBassAudio)
    {
        pBassAudio->m_pVars->criticalSection.Lock();
        pBassAudio->m_pVars->onClientSoundFinishedDownloadQueue.push_back(pBassAudio->GetLength());
        pBassAudio->m_pVars->criticalSection.Unlock();
    }
    UnlockCallbackId();
}

// get stream title from metadata and send it as event
void CALLBACK MetaSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
    CBassAudio* pBassAudio = LockCallbackId(user);

    if (pBassAudio)
    {
        pBassAudio->m_pVars->criticalSection.Lock();
        DWORD pSound = pBassAudio->m_pVars->pSound;
        pBassAudio->m_pVars->criticalSection.Unlock();

        const char* szMeta = BASS_ChannelGetTags(pSound, BASS_TAG_META);
        if (szMeta)
        {
            SString strMeta = szMeta;
            if (!strMeta.empty())
            {
                pBassAudio->m_pVars->criticalSection.Lock();
                pBassAudio->m_pVars->onClientSoundChangedMetaQueue.push_back(strMeta);
                pBassAudio->m_pVars->criticalSection.Unlock();
            }
        }
    }
    UnlockCallbackId();
}

void CALLBACK BPMCallback(int handle, float bpm, void* user)
{
    CBassAudio* pBassAudio = LockCallbackId(user);
    if (pBassAudio)
    {
        if (pBassAudio->m_pVars)
        {
            pBassAudio->m_pVars->criticalSection.Lock();
            pBassAudio->m_pVars->onClientBPMQueue.push_back(bpm);
            pBassAudio->m_pVars->criticalSection.Unlock();
        }
        else
        {
            pBassAudio->SetSoundBPM(bpm);
        }
    }
    UnlockCallbackId();
}

void CALLBACK BeatCallback(DWORD chan, double beatpos, void* user)
{
    CBassAudio* pBassAudio = LockCallbackId(user);
    if (pBassAudio)
    {
        if (pBassAudio->m_pVars)
        {
            pBassAudio->m_pVars->criticalSection.Lock();
            pBassAudio->m_pVars->onClientBeatQueue.push_back(beatpos);
            pBassAudio->m_pVars->criticalSection.Unlock();
        }
    }
    UnlockCallbackId();
}

DWORD CBassAudio::PlayStreamIntern(LPVOID argument)
{
    CBassAudio* pBassAudio = LockCallbackId(argument);
    if (pBassAudio)
    {
        pBassAudio->m_pVars->criticalSection.Lock();
        SString strURL = pBassAudio->m_pVars->strURL;
        long    lFlags = pBassAudio->m_pVars->lFlags;
        pBassAudio->m_pVars->criticalSection.Unlock();
        UnlockCallbackId();

        // This can take a while
        HSTREAM pSound = BASS_StreamCreateURL(FromUTF8(strURL), 0, lFlags | BASS_UNICODE, NULL, NULL);

        CBassAudio* pBassAudio = LockCallbackId(argument);
        if (pBassAudio)
        {
            pBassAudio->m_pVars->criticalSection.Lock();
            pBassAudio->m_pVars->bStreamCreateResult = true;
            pBassAudio->m_pVars->pSound = pSound;
            pBassAudio->m_pVars->criticalSection.Unlock();
        }
        else
        {
            // Deal with unwanted pSound unless we're disconnecting already
            if (g_pClientGame != nullptr && !g_pClientGame->IsBeingDeleted())
                g_pClientGame->GetManager()->GetSoundManager()->QueueChannelStop(pSound);
        }
    }

    UnlockCallbackId();
    return 0;
}

//
// Called from the main thread during DoPulse
//
void CBassAudio::CompleteStreamConnect(HSTREAM pSound)
{
    SString strError;
    if (pSound)
    {
        m_pSound = pSound;

        BASS_ChannelGetAttribute(pSound, BASS_ATTRIB_FREQ, &m_fDefaultFrequency);
        BASS_ChannelSetAttribute(pSound, BASS_ATTRIB_FREQ, m_fPlaybackSpeed * m_fDefaultFrequency);
        if (!m_b3D)
            BASS_ChannelSetAttribute(pSound, BASS_ATTRIB_VOL, m_fVolume);
        ApplyFxEffects();
        // Set a Callback function for download finished or connection closed prematurely
        m_hSyncDownload = BASS_ChannelSetSync(pSound, BASS_SYNC_DOWNLOAD, 0, &DownloadSync, m_uiCallbackId);
        SetFinishedCallbacks();

        if (BASS_FX_BPM_CallbackSet(pSound, (BPMPROC*)&BPMCallback, 1, 0, 0, m_uiCallbackId) == false)
        {
            strError = "BASS ERROR in BASS_FX_BPM_CallbackSet";
        }
        if (BASS_FX_BPM_BeatCallbackSet(pSound, (BPMBEATPROC*)&BeatCallback, m_uiCallbackId) == false)
        {
            strError = "BASS ERROR in BASS_FX_BPM_BeatCallbackSet";
        }
        // get the broadcast name
        const char* szIcy;
        szIcy = BASS_ChannelGetTags(pSound, BASS_TAG_ICY);
        if ((szIcy = BASS_ChannelGetTags(pSound, BASS_TAG_ICY)) || (szIcy = BASS_ChannelGetTags(pSound, BASS_TAG_WMA)) ||
            (szIcy = BASS_ChannelGetTags(pSound, BASS_TAG_HTTP)))
        {
            if (szIcy)
            {
                for (; *szIcy; szIcy += strlen(szIcy) + 1)
                {
                    if (!strnicmp(szIcy, "icy-name:", 9))            // ICY / HTTP
                    {
                        m_strStreamName = szIcy + 9;
                        break;
                    }
                    else if (!strnicmp(szIcy, "title=", 6))            // WMA
                    {
                        m_strStreamName = szIcy + 6;
                        break;
                    }
                    // g_pCore->GetConsole()->Printf ( "BASS STREAM INFO  %s", szIcy );
                }
            }
        }

        const char* szMeta = BASS_ChannelGetTags(pSound, BASS_TAG_META);
        if (szMeta)
        {
            SString strMeta = szMeta;
            if (!strMeta.empty())
            {
                m_pVars->criticalSection.Lock();
                m_pVars->onClientSoundChangedMetaQueue.push_back(strMeta);
                m_pVars->criticalSection.Unlock();
            }
        }

        // set sync for stream titles
        m_hSyncMeta = BASS_ChannelSetSync(pSound, BASS_SYNC_META, 0, &MetaSync, m_uiCallbackId);            // Shoutcast
        // g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_SYNC_META", BASS_ErrorGetCode() );
        // BASS_ChannelSetSync(pSound,BASS_SYNC_OGG_CHANGE,0,&MetaSync,this); // Icecast/OGG
        // g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_SYNC_OGG_CHANGE", BASS_ErrorGetCode() );
        // BASS_ChannelSetSync(pSound,BASS_SYNC_WMA_META,0,&MetaSync,this); // script/mid-stream tags
        // g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_SYNC_WMA_META", BASS_ErrorGetCode() );
        // BASS_ChannelSetSync(pSound,BASS_SYNC_WMA_CHANGE,0,&WMAChangeSync,this); // server-side playlist changes
        // g_pCore->GetConsole()->Printf ( "BASS ERROR %d in BASS_SYNC_WMA_CHANGE", BASS_ErrorGetCode() );
    }
    else
        strError = SString("BASS ERROR %d in PlayStream  b3D = %s  path = %s", BASS_ErrorGetCode(), m_b3D ? "true" : "false", m_strPath.c_str());

    OutputDebugLine("[Bass]        stream connect complete");

    AddQueuedEvent(SOUND_EVENT_STREAM_RESULT, m_strStreamName, GetLength(), pSound ? true : false, strError);
}

//
// Finish detection
//
void CALLBACK EndSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
    CBassAudio* pBassAudio = LockCallbackId(user);
    if (pBassAudio)
    {
        pBassAudio->uiEndSyncCount++;
    }
    UnlockCallbackId();
}

void CALLBACK FreeSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
    CBassAudio* pBassAudio = LockCallbackId(user);
    if (pBassAudio)
    {
        pBassAudio->bFreeSync = true;
    }
    UnlockCallbackId();
}

void CBassAudio::SetFinishedCallbacks()
{
    m_hSyncEnd = BASS_ChannelSetSync(m_pSound, BASS_SYNC_END, 0, &EndSync, m_uiCallbackId);
    m_hSyncFree = BASS_ChannelSetSync(m_pSound, BASS_SYNC_FREE, 0, &FreeSync, m_uiCallbackId);
}

//
// CBassAudio::GetReachedEndCount
//
uint CBassAudio::GetReachedEndCount()
{
    // Return the number of times the sound has gotten to the end
    return uiEndSyncCount;
}

//
// CBassAudio::IsFreed
//
bool CBassAudio::IsFreed()
{
    // Check if BASS has freed the sound handle.
    // This could be some time after the actual sound has stopped.
    return bFreeSync;
}

//
//
// Lake of sets
//
//
void CBassAudio::SetPaused(bool bPaused)
{
    m_bPaused = bPaused;
    if (m_pSound)
    {
        if (bPaused)
            BASS_ChannelPause(m_pSound);
        else
            BASS_ChannelPlay(m_pSound, false);
    }
}

// Non-streams only
bool CBassAudio::SetPlayPosition(double dPosition)
{
    // Only relevant for non-streams, which are always ready if valid
    if (m_pSound)
    {
        // Make sure position is in range
        QWORD bytePosition = BASS_ChannelSeconds2Bytes(m_pSound, dPosition);
        QWORD byteLength = BASS_ChannelGetLength(m_pSound, BASS_POS_BYTE);
        return BASS_ChannelSetPosition(m_pSound, Clamp<QWORD>(0, bytePosition, byteLength - 1), BASS_POS_BYTE);
    }
    return false;
}

// Non-streams only
double CBassAudio::GetPlayPosition()
{
    // Only relevant for non-streams, which are always ready if valid
    if (m_pSound)
    {
        QWORD pos = BASS_ChannelGetPosition(m_pSound, BASS_POS_BYTE);
        if (pos != -1)
            return BASS_ChannelBytes2Seconds(m_pSound, pos);
    }
    return 0;
}

// Non-streams only
double CBassAudio::GetLength()
{
    // Only relevant for non-streams, which are always ready if valid
    if (m_pSound)
    {
        QWORD length = BASS_ChannelGetLength(m_pSound, BASS_POS_BYTE);
        if (length != -1)
            return BASS_ChannelBytes2Seconds(m_pSound, length);
    }
    return 0;
}

// Streams only
double CBassAudio::GetBufferLength()
{
    if (m_bStream && m_pSound)
    {
        QWORD length = BASS_ChannelGetLength(m_pSound, BASS_POS_BYTE);
        if (length != -1)
        {
            QWORD fileSize = BASS_StreamGetFilePosition(m_pSound, BASS_FILEPOS_SIZE);
            if (fileSize > 0)
            {
                QWORD bufferPosition = (BASS_StreamGetFilePosition(m_pSound, BASS_FILEPOS_START) + BASS_StreamGetFilePosition(m_pSound, BASS_FILEPOS_BUFFER));
                QWORD bufferLength = static_cast<QWORD>(static_cast<double>(length) / fileSize * bufferPosition);
                return BASS_ChannelBytes2Seconds(m_pSound, bufferLength);
            }
        }
    }
    return 0;
}

// Streams only
SString CBassAudio::GetMetaTags(const SString& strFormat)
{
    SString strMetaTags;
    if (strFormat == "streamName")
        strMetaTags = m_strStreamName;
    else if (strFormat == "streamTitle")
        strMetaTags = m_strStreamTitle;
    else if (m_pSound)
    {
        if (strFormat != "")
        {
            const char* szTags = TAGS_Read(m_pSound, strFormat);
            if (szTags)
            {
                strMetaTags = szTags;
                if (strMetaTags == "")
                {
                    // Try using data from from shoutcast meta
                    SString* pstrResult = MapFind(m_ConvertedTagMap, strFormat);
                    if (pstrResult)
                        strMetaTags = *pstrResult;
                }
            }
        }
    }
    return strMetaTags;
}

float CBassAudio::GetPan()
{
    if (m_pSound)
    {
        float fPan = 0.0f;
        BASS_ChannelGetAttribute(m_pSound, BASS_ATTRIB_PAN, &fPan);

        return fPan;
    }

    return 0.0f;
}

void CBassAudio::SetPan(float fPan)
{
    if (m_pSound)
        BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_PAN, fPan);
}

void CBassAudio::SetVolume(float fVolume)
{
    m_fVolume = fVolume;

    if (m_pSound && !m_b3D)
        BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_VOL, fVolume);
}

void CBassAudio::SetPlaybackSpeed(float fSpeed)
{
    m_fPlaybackSpeed = fSpeed;

    if (m_pSound)
        BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_FREQ, fSpeed * m_fDefaultFrequency);
}

bool CBassAudio::SetLooped(bool bLoop)
{
    if (!m_pSound)
        return false;

    m_bLoop = bLoop;

    return BASS_ChannelFlags(m_pSound, bLoop ? BASS_SAMPLE_LOOP : 0, BASS_SAMPLE_LOOP);
}

void CBassAudio::SetPosition(const CVector& vecPosition)
{
    m_vecPosition = vecPosition;
}

void CBassAudio::SetVelocity(const CVector& vecVelocity)
{
    m_vecVelocity = vecVelocity;
}

void CBassAudio::SetMinDistance(float fDistance)
{
    m_fMinDistance = fDistance;
}

void CBassAudio::SetMaxDistance(float fDistance)
{
    m_fMaxDistance = fDistance;
}

void CBassAudio::SetTempoValues(float fSampleRate, float fTempo, float fPitch, bool bReverse)
{
    if (fTempo != m_fTempo)
    {
        m_fTempo = fTempo;
    }
    if (fPitch != m_fPitch)
    {
        m_fPitch = fPitch;
    }
    if (fSampleRate != m_fSampleRate)
    {
        m_fSampleRate = fSampleRate;
    }
    m_bReversed = bReverse;

    // Update our attributes
    if (m_pSound)
    {
        // TODO: These are lost when the sound is not streamed in
        BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_TEMPO, m_fTempo);
        BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_TEMPO_PITCH, m_fPitch);
        BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_TEMPO_FREQ, m_fSampleRate);
        BASS_ChannelSetAttribute(BASS_FX_TempoGetSource(m_pSound), BASS_ATTRIB_REVERSE_DIR,
                                 (float)(bReverse == false ? BASS_FX_RVS_FORWARD : BASS_FX_RVS_REVERSE));
    }
}

float* CBassAudio::GetFFTData(int iLength)
{
    if (m_pSound)
    {
        long lFlags = BASS_DATA_FFT256;
        if (iLength == 256)
            lFlags = BASS_DATA_FFT256;
        else if (iLength == 512)
            lFlags = BASS_DATA_FFT512;
        else if (iLength == 1024)
            lFlags = BASS_DATA_FFT1024;
        else if (iLength == 2048)
            lFlags = BASS_DATA_FFT2048;
        else if (iLength == 4096)
            lFlags = BASS_DATA_FFT4096;
        else if (iLength == 8192)
            lFlags = BASS_DATA_FFT8192;
        else if (iLength == 16384)
            lFlags = BASS_DATA_FFT16384;
        else
            return NULL;

        float* pData = new float[iLength];
        if (BASS_ChannelGetData(m_pSound, pData, lFlags) != -1)
            return pData;
        else
        {
            delete[] pData;
            return NULL;
        }
    }
    return NULL;
}

float* CBassAudio::GetWaveData(int iLength)
{
    if (m_pSound)
    {
        long lFlags = 0;
        if (iLength == 128 || iLength == 256 || iLength == 512 || iLength == 1024 || iLength == 2048 || iLength == 4096 || iLength == 8192 || iLength == 16384)
        {
            lFlags = 4 * iLength | BASS_DATA_FLOAT;
        }
        else
            return NULL;

        float* pData = new float[iLength];
        if (BASS_ChannelGetData(m_pSound, pData, lFlags) != -1)
            return pData;
        else
        {
            delete[] pData;
            return NULL;
        }
    }
    return NULL;
}
DWORD CBassAudio::GetLevelData()
{
    if (m_pSound)
    {
        DWORD dwData = BASS_ChannelGetLevel(m_pSound);
        if (dwData != 0)
            return dwData;
    }
    return 0;
}

float CBassAudio::GetSoundBPM()
{
    if (m_fBPM == 0.0f && !m_bStream)
    {
        float fData = 0.0f;

        // open the same file as played but for bpm decoding detection
        DWORD bpmChan = BASS_StreamCreateFile(false, FromUTF8(m_strPath), 0, 0, BASS_STREAM_DECODE | BASS_UNICODE);

        if (!bpmChan)
        {
            bpmChan = BASS_MusicLoad(false, FromUTF8(m_strPath), 0, 0, BASS_MUSIC_DECODE | BASS_MUSIC_PRESCAN | BASS_UNICODE, 0);
        }

        if (bpmChan)
        {
            fData = BASS_FX_BPM_DecodeGet(bpmChan, 0, GetLength(), 0, BASS_FX_FREESOURCE, NULL, NULL);
            BASS_FX_BPM_Free(bpmChan);
        }

        if (BASS_ErrorGetCode() != BASS_OK)
        {
            g_pCore->GetConsole()->Printf("BASS ERROR %d in BASS_FX_BPM_DecodeGet  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), *m_strPath, m_b3D, m_bLoop);
        }
        else
        {
            m_fBPM = floor(fData);
        }
    }

    return m_fBPM;
}

//
// FxEffects
//
void CBassAudio::SetFxEffects(int* pEnabledEffects, uint iNumElements)
{
    // Update m_EnabledEffects array
    for (uint i = 0; i < NUMELMS(m_EnabledEffects); i++)
        m_EnabledEffects[i] = i < iNumElements ? pEnabledEffects[i] : 0;

    // Apply if active
    if (m_pSound)
        ApplyFxEffects();
}

//
// Copy state stored in m_EnabledEffects to actual BASS sound
//
void CBassAudio::ApplyFxEffects()
{
    for (uint i = 0; i < NUMELMS(m_FxEffects) && NUMELMS(m_EnabledEffects); i++)
    {
        if (m_EnabledEffects[i] && !m_FxEffects[i])
        {
            // Switch on
            m_FxEffects[i] = BASS_ChannelSetFX(m_pSound, i, 0);
            if (!m_FxEffects[i])
                m_FxEffects[i] = INVALID_FX_HANDLE;
        }
        else if (!m_EnabledEffects[i] && m_FxEffects[i])
        {
            // Switch off
            if (m_FxEffects[i] != INVALID_FX_HANDLE)
                BASS_ChannelRemoveFX(m_pSound, m_FxEffects[i]);
            m_FxEffects[i] = 0;
        }
    }
}

BOOL CBassAudio::SetFxParameters(uint iFxEffect, void* params)
{
    return BASS_FXSetParameters(m_FxEffects[iFxEffect], params);
}

BOOL CBassAudio::GetFxParameters(uint iFxEffect, void* params)
{
    return BASS_FXGetParameters(m_FxEffects[iFxEffect], params);
}

//
// Must be call every frame
//
void CBassAudio::DoPulse(const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt)
{
    // If the sound is a stream, handle results from other threads
    if (m_bStream)
        if (m_pVars)
            ServiceVars();

    // If the sound isn't ready, we stop here
    if (!m_pSound)
        return;

    // Update 3D attenuation and panning
    if (m_b3D)
        Process3D(vecPlayerPosition, vecCameraPosition, vecLookAt);

    // Apply any pending play request
    if (m_bPendingPlay)
    {
        m_bPendingPlay = false;
        if (!m_bPaused)
            BASS_ChannelPlay(m_pSound, false);
    }
}

void CBassAudio::Process3D(const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt)
{
    assert(m_b3D && m_pSound);

    float fDistance = DistanceBetweenPoints3D(vecCameraPosition, m_vecPosition);
    if (m_bPan)
    {
        // Limit panning when getting close to the min distance
        float fPanSharpness = UnlerpClamped(m_fMinDistance, fDistance, m_fMinDistance * 2);
        float fPanLimit = Lerp(0.35f, fPanSharpness, 1.0f);

        // Pan
        CVector vecLook = vecLookAt - vecCameraPosition;
        CVector vecSound = m_vecPosition - vecCameraPosition;
        vecLook.fZ = vecSound.fZ = 0.0f;
        vecLook.Normalize();
        vecSound.Normalize();

        vecLook.CrossProduct(&vecSound);
        // The length of the cross product (which is simply fZ in this case)
        // is equal to the sine of the angle between the vectors
        float fPan = Clamp(-fPanLimit, -vecLook.fZ, fPanLimit);

        BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_PAN, fPan);
    }
    else
    {
        // Revert to middle.
        BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_PAN, 0.0f);
    }

    // Volume
    float fDistDiff = m_fMaxDistance - m_fMinDistance;

    // Transform e^-x to suit our sound
    float fVolume;
    if (fDistance <= m_fMinDistance)
        fVolume = 1.0f;
    else if (fDistance >= m_fMaxDistance)
        fVolume = 0.0f;
    else
        fVolume = exp(-(fDistance - m_fMinDistance) * (CUT_OFF / fDistDiff));

    BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_VOL, fVolume * m_fVolume);
}

//
// Handle stored data from other threads
//
void CBassAudio::ServiceVars()
{
    // Temp
    DWORD              pSound = 0;
    bool               bStreamCreateResult = false;
    std::list<double>  onClientSoundFinishedDownloadQueue;
    std::list<SString> onClientSoundChangedMetaQueue;
    std::list<float>   onClientBPMQueue;
    std::list<double>  onClientBeatQueue;

    // Lock vars
    m_pVars->criticalSection.Lock();

    // Copy vars to temp
    pSound = m_pVars->pSound;
    bStreamCreateResult = m_pVars->bStreamCreateResult;
    onClientSoundFinishedDownloadQueue = m_pVars->onClientSoundFinishedDownloadQueue;
    onClientSoundChangedMetaQueue = m_pVars->onClientSoundChangedMetaQueue;
    onClientBPMQueue = m_pVars->onClientBPMQueue;
    onClientBeatQueue = m_pVars->onClientBeatQueue;

    // Clear vars
    m_pVars->bStreamCreateResult = false;
    m_pVars->onClientSoundFinishedDownloadQueue.clear();
    m_pVars->onClientSoundChangedMetaQueue.clear();
    m_pVars->onClientBPMQueue.clear();
    m_pVars->onClientBeatQueue.clear();

    // Unlock vars
    m_pVars->criticalSection.Unlock();

    // Process temp
    if (bStreamCreateResult)
        CompleteStreamConnect(pSound);

    // Handle onClientSoundFinishedDownload queue
    while (!onClientSoundFinishedDownloadQueue.empty())
    {
        AddQueuedEvent(SOUND_EVENT_FINISHED_DOWNLOAD, "", onClientSoundFinishedDownloadQueue.front());
        onClientSoundFinishedDownloadQueue.pop_front();
    }

    // Handle onClientSoundChangedMeta queue
    while (!onClientSoundChangedMetaQueue.empty())
    {
        SString strMeta = onClientSoundChangedMetaQueue.front();
        ParseShoutcastMeta(strMeta);
        AddQueuedEvent(SOUND_EVENT_CHANGED_META, m_strStreamTitle);
        onClientSoundChangedMetaQueue.pop_front();
    }

    // Handle bpm saving queue
    while (!onClientBPMQueue.empty())
    {
        float fBPM = onClientBPMQueue.front();
        m_fBPM = fBPM;
        onClientBPMQueue.pop_front();
    }

    // Handle onClientBeatQueue queue
    while (!onClientBeatQueue.empty())
    {
        double dBPM = onClientBeatQueue.front();
        AddQueuedEvent(SOUND_EVENT_BEAT, "", dBPM);
        onClientBeatQueue.pop_front();
    }
}

//
// Extract and map data from a shoutcast meta string
//
void CBassAudio::ParseShoutcastMeta(const SString& strMeta)
{
    // Get title
    int     startPos = strMeta.find("=");
    SString strStreamTitle = strMeta.SubStr(startPos + 2, strMeta.find(";") - startPos - 3);

    if (!strStreamTitle.empty())
        m_strStreamTitle = strStreamTitle;

    // Get url
    startPos = strMeta.find("=", startPos + 1);
    SString strStreamUrl = strMeta.SubStr(startPos + 2, strMeta.find(";", startPos) - startPos - 3);

    // Extract info from url
    CArgMap shoutcastInfo;
    shoutcastInfo.SetEscapeCharacter('%');
    shoutcastInfo.SetFromString(strStreamUrl);

    // Convert from shoutcast identifiers to map of tags
    static const char* convList[] = {
        // Mapable
        "%ARTI",
        "artist",
        "%TITL",
        "title",
        "%ALBM",
        "album",

        // Mapable, but possibly don't exist
        "%GNRE",
        "genre",
        "%YEAR",
        "year",
        "%CMNT",
        "comment",
        "%TRCK",
        "track",
        "%COMP",
        "composer",
        "%COPY",
        "copyright",
        "%SUBT",
        "subtitle",
        "%AART",
        "albumartist",

        // Not mapabale
        "%DURATION",
        "duration",
        "%SONGTYPE",
        "songtype",
        "%OVERLAY",
        "overlay",
        "%BUYCD",
        "buycd",
        "%WEBSITE",
        "website",
        "%PICTURE",
        "picture",
    };

    std::vector<SString> shoutcastKeyList;
    shoutcastInfo.GetKeys(shoutcastKeyList);

    // For each shoutcast pair
    for (std::vector<SString>::iterator iter = shoutcastKeyList.begin(); iter != shoutcastKeyList.end(); ++iter)
    {
        const SString& strKey = *iter;
        SString        strValue = shoutcastInfo.Get(strKey);

        // Find %TAG match
        for (uint i = 0; i < NUMELMS(convList) - 1; i += 2)
        {
            if (strKey == convList[i + 1])
            {
                MapSet(m_ConvertedTagMap, convList[i], strValue);
                break;
            }
        }
    }
}

//
// Add queued event from
//
void CBassAudio::AddQueuedEvent(eSoundEventType type, const SString& strString, double dNumber, bool bBool, const SString& strError)
{
    SSoundEventInfo info;
    info.type = type;
    info.strString = strString;
    info.dNumber = dNumber;
    info.bBool = bBool;
    info.strError = strError;
    m_EventQueue.push_back(info);
}

//
// Get next queued event
//
bool CBassAudio::GetQueuedEvent(SSoundEventInfo& info)
{
    if (m_EventQueue.empty())
        return false;

    info = m_EventQueue.front();
    m_EventQueue.pop_front();
    return true;
}
