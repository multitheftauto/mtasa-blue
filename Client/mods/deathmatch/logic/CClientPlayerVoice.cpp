/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVoiceRecorder.cpp
 *  PURPOSE:     Remote player voice chat playback
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientPlayerVoice.h"
#include "CBassAudio.h"
#include <process.h>
#include <tags.h>
#include <bassmix.h>
#include <basswma.h>
#include <bass_fx.h>

void CALLBACK BPMCallback(int handle, float bpm, void* user);
void CALLBACK BeatCallback(DWORD chan, double beatpos, void* user);

#define INVALID_FX_HANDLE (-1)  // Hope that BASS doesn't use this as a valid Fx handle

// A stalled stream is only reported as stopped after this many milliseconds of
// silence, so brief delivery gaps do not fire a false stop/start pair
constexpr unsigned long VOICE_STALL_STOP_GRACE_MS = 250;

// Pending frames kept when the per-frame decode cap is hit (bounded memory)
constexpr unsigned int VOICE_PENDING_FRAME_LIMIT = 64;

// Frames decoded per rendered frame to bound CPU from relay floods
constexpr unsigned int VOICE_DECODES_PER_FRAME = 6;

CClientPlayerVoice::CClientPlayerVoice(CClientPlayer* pPlayer, CVoiceRecorder* pVoiceRecorder)
{
    m_pPlayer = pPlayer;
    m_pVoiceRecorder = pVoiceRecorder;
    m_bVoiceActive = false;
    m_SampleRate = SAMPLERATE_WIDEBAND;
    m_pSpeexDecoderState = NULL;
    m_iSpeexIncomingFrameSampleCount = 0;

    // Get initial voice volume
    m_fVolume = 1.0f;
    g_pCore->GetCVars()->Get("voicevolume", m_fVolumeScale);
    m_fVolumeScale *= g_pCore->GetCVars()->GetValue<float>("mastervolume", 1.0f);

    // The user scale is applied to BASS at play time (m_fVolume * m_fVolumeScale),
    // so m_fVolume must stay at the neutral value here

    if (pPlayer->IsLocalPlayer() == true)
    {
        m_fVolume = 0.0f;
    }
    m_fPlaybackSpeed = 1.0f;
    Init();
}
CClientPlayerVoice::~CClientPlayerVoice()
{
    DeInit();
}

void CALLBACK BASS_VoiceStateChange(HSYNC handle, DWORD channel, DWORD data, void* user)
{
    if (data == 0)
    {
        CClientPlayerVoice* pVoice = static_cast<CClientPlayerVoice*>(user);
        pVoice->OnVoiceStall();
    }
}

void CClientPlayerVoice::Init()
{
    // Grab our sample rate
    m_SampleRate = m_pVoiceRecorder->GetSampleRate();

    // Setup our BASS playback device
    m_pBassPlaybackStream = BASS_StreamCreate(m_SampleRate / VOICE_SAMPLE_SIZE, 2, BASS_STREAM_AUTOFREE, STREAMPROC_PUSH, NULL);
    m_hStallSync = BASS_ChannelSetSync(m_pBassPlaybackStream, BASS_SYNC_STALL, 0, &BASS_VoiceStateChange, this);

    // Cap the queue BASS keeps for push streams at one second of audio, so a
    // paused or flooded stream cannot grow memory without bound; frames past
    // the limit are refused and then held back by the pending queue instead
    BASS_ChannelSetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_PUSH_LIMIT, static_cast<float>(m_SampleRate * 2));

    BASS_ChannelPlay(m_pBassPlaybackStream, false);

    // Fallback if the attribute read fails
    m_fDefaultFrequency = static_cast<float>(m_SampleRate) / VOICE_SAMPLE_SIZE;
    BASS_ChannelGetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_FREQ, &m_fDefaultFrequency);
    BASS_ChannelSetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_VOL, m_fVolume * m_fVolumeScale);

    // Get the relevant speex mode for the servers sample rate
    const SpeexMode* speexMode = m_pVoiceRecorder->getSpeexModeFromSampleRate();
    m_pSpeexDecoderState = speex_decoder_init(speexMode);

    // A failed decoder creation leaves a null handle, so skip the config chain;
    // the frame paths drop undecodable frames the same way
    if (m_pSpeexDecoderState)
    {
        // Initialize our speex decoder
        speex_decoder_ctl(m_pSpeexDecoderState, SPEEX_GET_FRAME_SIZE, &m_iSpeexIncomingFrameSampleCount);
        int iQuality = static_cast<int>(m_pVoiceRecorder->GetSampleQuality());
        speex_decoder_ctl(m_pSpeexDecoderState, SPEEX_SET_QUALITY, &iQuality);
    }
}

void CClientPlayerVoice::DeInit()
{
    if (m_pBassPlaybackStream)
    {
        // Remove the stall callback before freeing the stream so BASS cannot
        // call back into this object after the stream is gone
        BASS_ChannelRemoveSync(m_pBassPlaybackStream, m_hStallSync);
        BASS_ChannelStop(m_pBassPlaybackStream);
        BASS_StreamFree(m_pBassPlaybackStream);
    }

    m_pBassPlaybackStream = NULL;
    m_hStallSync = 0;

    if (m_pSpeexDecoderState)
        speex_decoder_destroy(m_pSpeexDecoderState);
    m_pSpeexDecoderState = NULL;

    m_SampleRate = SAMPLERATE_WIDEBAND;
}

void CClientPlayerVoice::DoPulse()
{
    // Reset before draining, so the drain gets first call on the shared decode
    // budget and the next frame's packet intake only uses what the drain leaves
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_voiceFramesThisPulse = 0;
    }

    // A stalled stream counts as ended only after the grace period, and only
    // once every queued frame has been decoded, so a starved decode budget
    // cannot cut a burst short while audio is still waiting to play
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        // The stop also needs packet silence: a held but silent push-to-talk
        // sends DTX packets and no audio, so the stall alone would fire a
        // false stop/start pair mid-hold
        const unsigned long ulNow = CClientTime::GetTime();
        if (m_bStallPending && (m_ulTimeOfLastFrame == 0 || ulNow - m_ulTimeOfLastFrame > VOICE_STALL_STOP_GRACE_MS) &&
            ulNow - m_ulTimeOfLastPacket > VOICE_STALL_STOP_GRACE_MS && m_PendingVoiceFrames.empty())
        {
            m_bStallPending = false;
            if (m_bVoiceActive)
            {
                m_EventQueue.push_back("onClientPlayerVoiceStop");
                m_bVoiceActive = false;
            }
        }

        // Stalls do not end a muted burst, so clear the ignore flag once the
        // speaker stops sending packets
        if (m_bIgnoreStart && CClientTime::GetTime() - m_ulTimeOfLastPacket > VOICE_STALL_STOP_GRACE_MS)
            m_bIgnoreStart = false;
    }

    // Dispatch queued events
    ServiceEventQueue();

    // Decode frames held back by the per-frame cap, using the same cap
    while (true)
    {
        std::vector<unsigned char> frame;
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            if (m_PendingVoiceFrames.empty() || m_voiceFramesThisPulse >= VOICE_DECODES_PER_FRAME)
                break;
            frame = std::move(m_PendingVoiceFrames.front());
            m_PendingVoiceFrames.pop_front();
            ++m_voiceFramesThisPulse;
        }

        if (ProcessFrame(frame.data(), static_cast<unsigned int>(frame.size())) == EVoiceFrameResult::Deferred)
        {
            // BASS refused the frame, so hold it back instead of losing it
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_PendingVoiceFrames.push_front(std::move(frame));
            break;
        }
    }

    float fPreviousVolume = 0.0f;
    g_pCore->GetCVars()->Get("voicevolume", fPreviousVolume);
    fPreviousVolume *= g_pCore->GetCVars()->GetValue<float>("mastervolume", 1.0f);

    if (fPreviousVolume != m_fVolumeScale && m_pPlayer->IsLocalPlayer() == false)
    {
        m_fVolumeScale = fPreviousVolume;
        float fScaledVolume = m_fVolume * m_fVolumeScale;
        if (m_pBassPlaybackStream)
            BASS_ChannelSetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_VOL, fScaledVolume);
    }
}

void CClientPlayerVoice::DecodeAndBuffer(const unsigned char* voiceBuffer, unsigned int voiceBufferLength)
{
    if (!voiceBuffer || !voiceBufferLength || voiceBufferLength > 2048)
        return;

    // Packets are handled on the game thread, and the BASS stall callback runs
    // on the audio thread, so every shared member is handled under the mutex;
    // the decode itself stays outside the lock
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        // Stamp before the uniform-byte skip so DTX silence keeps the burst alive
        // for the ignore flag, which must outlast the whole held talk burst
        m_ulTimeOfLastPacket = CClientTime::GetTime();

        // Skip uniform-byte noise before costly Speex decode
        if (voiceBufferLength >= 4 && voiceBuffer[0] == voiceBuffer[1] && voiceBuffer[0] == voiceBuffer[2] && voiceBuffer[0] == voiceBuffer[3])
            return;

        if (!m_pSpeexDecoderState)
            return;

        // A canceled start event silences the rest of the talk burst
        if (m_bIgnoreStart)
            return;

        // Limit decodes per frame to bound CPU from relay floods, and queue
        // behind anything already waiting so playback keeps its order
        if (m_voiceFramesThisPulse >= VOICE_DECODES_PER_FRAME || !m_PendingVoiceFrames.empty())
        {
            QueueVoiceFrame(voiceBuffer, voiceBufferLength);
            return;
        }
        ++m_voiceFramesThisPulse;
    }

    if (ProcessFrame(voiceBuffer, voiceBufferLength) == EVoiceFrameResult::Deferred)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        QueueVoiceFrame(voiceBuffer, voiceBufferLength);
    }
}

void CClientPlayerVoice::QueueVoiceFrame(const unsigned char* voiceBuffer, unsigned int voiceBufferLength)
{
    // Caller must hold m_Mutex: the deque is shared with the pulse drain
    if (m_PendingVoiceFrames.size() >= VOICE_PENDING_FRAME_LIMIT)
    {
        // Recycle the oldest slot, keep the most recent audio
        std::vector<unsigned char> recycled = std::move(m_PendingVoiceFrames.front());
        m_PendingVoiceFrames.pop_front();
        recycled.assign(voiceBuffer, voiceBuffer + voiceBufferLength);
        m_PendingVoiceFrames.push_back(std::move(recycled));
    }
    else
        m_PendingVoiceFrames.emplace_back(voiceBuffer, voiceBuffer + voiceBufferLength);
}

CClientPlayerVoice::EVoiceFrameResult CClientPlayerVoice::ProcessFrame(const unsigned char* voiceBuffer, unsigned int voiceBufferLength)
{
    // Guard: a muted burst must never feed BASS, whatever path a frame arrives by
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_bIgnoreStart)
            return EVoiceFrameResult::Suppressed;
    }

    // A stream that never opened can never accept frames, so drop before any
    // start event or active state is published; that state would otherwise stay
    // stuck active until destruction, with no BASS stall signal to end it
    if (!m_pBassPlaybackStream)
        return EVoiceFrameResult::Suppressed;

    if (!m_pSpeexDecoderState)
        return EVoiceFrameResult::Suppressed;

    char      pTempBuffer[2048];
    SpeexBits speexBits;
    speex_bits_init(&speexBits);

    speex_bits_read_from(&speexBits, reinterpret_cast<const char*>(voiceBuffer), voiceBufferLength);
    const int decodeResult = speex_decode_int(m_pSpeexDecoderState, &speexBits, (spx_int16_t*)pTempBuffer);

    speex_bits_destroy(&speexBits);

    // Do not retry frames that can never decode, or they would block the queue
    if (decodeResult < 0)
        return EVoiceFrameResult::Suppressed;

    m_Mutex.lock();

    if (!m_bVoiceActive)
    {
        m_Mutex.unlock();

        ServiceEventQueue();

        CLuaArguments Arguments;
        if (!m_pPlayer->CallEvent("onClientPlayerVoiceStart", Arguments, true))
        {
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_bIgnoreStart = true;

                // Frames queued before the cancel belong to the canceled burst, so
                // drop them now; they would otherwise play after the ignore window
                m_PendingVoiceFrames.clear();
            }

            return EVoiceFrameResult::Suppressed;
        }

        m_Mutex.lock();
        m_bVoiceActive = true;
        m_bStallPending = false;  // A fresh burst starts with no stale stall signal
        m_Mutex.unlock();
    }
    else
    {
        m_Mutex.unlock();
    }

    unsigned int uiSpeexBlockSize = m_iSpeexIncomingFrameSampleCount * VOICE_SAMPLE_SIZE;

    // BASS queues any data that does not fit the playback buffer itself, so
    // every non-error return keeps the frame. Only a refused frame is held
    // back for a later pulse
    if (BASS_StreamPutData(m_pBassPlaybackStream, (void*)pTempBuffer, uiSpeexBlockSize) != static_cast<DWORD>(-1))
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_bStallPending = false;
        m_ulTimeOfLastFrame = CClientTime::GetTime();
        return EVoiceFrameResult::Accepted;
    }

    return EVoiceFrameResult::Deferred;
}

void CClientPlayerVoice::ServiceEventQueue()
{
    std::list<SString> eventQueue;
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        std::swap(eventQueue, m_EventQueue);
    }

    for (const SString& strEvent : eventQueue)
    {
        CLuaArguments Arguments;
        m_pPlayer->CallEvent(strEvent, Arguments, true);
    }
}

////////////////////////////////////////////////////////////
//
// CClientPlayerVoice:: Sea of sets 'n' gets
//
//
//
////////////////////////////////////////////////////////////
void CClientPlayerVoice::SetPlayPosition(double dPosition)
{
    // Only relevant for non-streams, which are always ready if valid
    if (m_pBassPlaybackStream)
    {
        // Make sure position is in range
        QWORD bytePosition = BASS_ChannelSeconds2Bytes(m_pBassPlaybackStream, dPosition);
        QWORD byteLength = BASS_ChannelGetLength(m_pBassPlaybackStream, BASS_POS_BYTE);
        BASS_ChannelSetPosition(m_pBassPlaybackStream, Clamp<QWORD>(0, bytePosition, byteLength - 1), BASS_POS_BYTE);
    }
}

double CClientPlayerVoice::GetPlayPosition()
{
    if (m_pBassPlaybackStream)
    {
        QWORD pos = BASS_ChannelGetPosition(m_pBassPlaybackStream, BASS_POS_BYTE);
        if (pos != -1)
            return BASS_ChannelBytes2Seconds(m_pBassPlaybackStream, pos);
    }
    return 0.0;
}

double CClientPlayerVoice::GetLength(bool bAvoidLoad)
{
    if (m_pBassPlaybackStream)
    {
        QWORD length = BASS_ChannelGetLength(m_pBassPlaybackStream, BASS_POS_BYTE);
        if (length != -1)
            return BASS_ChannelBytes2Seconds(m_pBassPlaybackStream, length);
    }
    return 0;
}

float CClientPlayerVoice::GetVolume()
{
    return m_fVolume;
}

void CClientPlayerVoice::SetVolume(float fVolume, bool bStore)
{
    m_fVolume = fVolume;

    if (m_pBassPlaybackStream && m_pPlayer->IsLocalPlayer() == false)
    {
        float fScaledVolume = m_fVolume * m_fVolumeScale;
        BASS_ChannelSetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_VOL, fScaledVolume);
    }
}

float CClientPlayerVoice::GetPlaybackSpeed()
{
    return m_fPlaybackSpeed;
}

void CClientPlayerVoice::SetPlaybackSpeed(float fSpeed)
{
    m_fPlaybackSpeed = fSpeed;

    if (m_pBassPlaybackStream)
        BASS_ChannelSetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_FREQ, fSpeed * m_fDefaultFrequency);
}

void CClientPlayerVoice::ApplyFXModifications(float fSampleRate, float fTempo, float fPitch, bool bReversed)
{
    m_fSampleRate = fSampleRate;
    m_fTempo = fTempo;
    m_fPitch = fPitch;
    if (m_pBassPlaybackStream)
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

        // Update our attributes
        BASS_ChannelSetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_TEMPO, m_fTempo);
        BASS_ChannelSetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_TEMPO_PITCH, m_fPitch);
        BASS_ChannelSetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_TEMPO_FREQ, m_fSampleRate);
    }
}

void CClientPlayerVoice::GetFXModifications(float& fSampleRate, float& fTempo, float& fPitch, bool& bReversed)
{
    if (m_pBassPlaybackStream)
    {
        GetTempoValues(fSampleRate, fTempo, fPitch, bReversed);
    }
}

float* CClientPlayerVoice::GetFFTData(int iLength)
{
    if (m_pBassPlaybackStream)
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
        if (BASS_ChannelGetData(m_pBassPlaybackStream, pData, lFlags) != -1)
            return pData;
        else
        {
            delete[] pData;
            return NULL;
        }
    }
    return NULL;
}

float* CClientPlayerVoice::GetWaveData(int iLength)
{
    if (m_pBassPlaybackStream)
    {
        long lFlags = 0;
        if (iLength == 128 || iLength == 256 || iLength == 512 || iLength == 1024 || iLength == 2048 || iLength == 4096 || iLength == 8192 || iLength == 16384)
        {
            lFlags = 4 * iLength | BASS_DATA_FLOAT;
        }
        else
            return NULL;

        float* pData = new float[iLength];
        if (BASS_ChannelGetData(m_pBassPlaybackStream, pData, lFlags) != -1)
            return pData;
        else
        {
            delete[] pData;
            return NULL;
        }
    }
    return NULL;
}

DWORD CClientPlayerVoice::GetLevelData()
{
    if (m_pBassPlaybackStream)
    {
        DWORD dwData = BASS_ChannelGetLevel(m_pBassPlaybackStream);
        if (dwData != 0)
            return dwData;
    }
    return 0;
}

////////////////////////////////////////////////////////////
//
// CClientSound::SetFxEffect
//
//
//
////////////////////////////////////////////////////////////
bool CClientPlayerVoice::SetFxEffect(uint uiFxEffect, bool bEnable)
{
    if (uiFxEffect >= NUMELMS(m_EnabledEffects))
        return false;

    m_EnabledEffects[uiFxEffect] = bEnable;

    // Apply if active
    if (m_pBassPlaybackStream)
    {
        ApplyFxEffects();
        // ApplyFxEffects clears m_EnabledEffects[i] when BASS rejects the effect
        // (e.g. I3DL2REVERB on Windows 11 24H2, #4259), so report the truth.
        return (m_EnabledEffects[uiFxEffect] != 0) == bEnable;
    }

    return true;
}

//
// Copy state stored in m_EnabledEffects to actual BASS sound
//
void CClientPlayerVoice::ApplyFxEffects()
{
    for (uint i = 0; i < NUMELMS(m_FxEffects) && NUMELMS(m_EnabledEffects); i++)
    {
        if (m_EnabledEffects[i] && !m_FxEffects[i])
        {
            // Switch on
            m_FxEffects[i] = BASS_ChannelSetFX(m_pBassPlaybackStream, i, 0);
            if (!m_FxEffects[i])
            {
                // Effect could not be wired up by BASS (e.g. Windows 11 24H2
                // removed BASS_FX_DX8_I3DL2REVERB at the OS level, #4259).
                g_pCore->GetConsole()->Printf("BASS ERROR %d in BASS_ChannelSetFX (effect %u)", BASS_ErrorGetCode(), i);
                m_FxEffects[i] = INVALID_FX_HANDLE;
            }
        }
        else
        {
            if (!m_EnabledEffects[i] && m_FxEffects[i])
            {
                // Switch off
                if (m_FxEffects[i] != INVALID_FX_HANDLE)
                    BASS_ChannelRemoveFX(m_pBassPlaybackStream, m_FxEffects[i]);
                m_FxEffects[i] = 0;
            }
        }

        // Mirror failure into m_EnabledEffects so IsFxEffectEnabled() reports
        // the truth and re-enable requests don't silently leave a dangling handle.
        if (m_FxEffects[i] == INVALID_FX_HANDLE)
            m_EnabledEffects[i] = 0;
    }
}

bool CClientPlayerVoice::IsFxEffectEnabled(uint uiFxEffect)
{
    if (uiFxEffect >= NUMELMS(m_EnabledEffects))
        return false;

    return m_EnabledEffects[uiFxEffect] ? true : false;
}

bool CClientPlayerVoice::SetFxEffectParameters(std::uint32_t uiFxEffect, void* params)
{
    if (IsFxEffectEnabled(uiFxEffect))
        return BASS_FXSetParameters(m_FxEffects[uiFxEffect], params);
    return false;
}

bool CClientPlayerVoice::GetFxEffectParameters(std::uint32_t uiFxEffect, void* params)
{
    if (IsFxEffectEnabled(uiFxEffect))
        return BASS_FXGetParameters(m_FxEffects[uiFxEffect], params);
    return false;
}

bool CClientPlayerVoice::GetPan(float& fPan)
{
    fPan = 0.0f;
    if (m_pBassPlaybackStream)
    {
        BASS_ChannelGetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_PAN, &fPan);
        return true;
    }
    return false;
}

bool CClientPlayerVoice::SetPan(float fPan)
{
    if (m_pBassPlaybackStream)
    {
        BASS_ChannelSetAttribute(m_pBassPlaybackStream, BASS_ATTRIB_PAN, fPan);

        return true;
    }

    return false;
}

void CClientPlayerVoice::SetPaused(bool bPaused)
{
    if (m_bPaused != bPaused)
    {
        if (bPaused)
        {
            // call onClientPlayerVoicePause
            CLuaArguments Arguments;
            Arguments.PushString("paused");  // Reason
            m_pPlayer->CallEvent("onClientPlayerVoicePause", Arguments, false);
        }
        else
        {
            // call onClientPlayerVoiceResumed
            CLuaArguments Arguments;
            Arguments.PushString("resumed");  // Reason
            m_pPlayer->CallEvent("onClientPlayerVoiceResumed", Arguments, false);
        }
    }

    m_bPaused = bPaused;

    if (m_pBassPlaybackStream)
    {
        if (bPaused)
            BASS_ChannelPause(m_pBassPlaybackStream);
        else
            BASS_ChannelPlay(m_pBassPlaybackStream, false);
    }
}

bool CClientPlayerVoice::IsPaused()
{
    return m_bPaused;
}
