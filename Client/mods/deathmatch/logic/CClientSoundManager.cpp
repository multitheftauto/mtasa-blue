/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientSoundManager.cpp
 *  PURPOSE:     Sound manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CSettings.h>
#include "CBassAudio.h"

using SharedUtil::CalcMTASAPath;
using std::list;

extern CCoreInterface* g_pCore;

CClientSoundManager::CClientSoundManager(CClientManager* pClientManager)
{
    m_pClientManager = pClientManager;

    // Initialize BASS audio library
    if (!BASS_Init(-1, 44100, NULL, NULL, NULL))
        g_pCore->GetConsole()->Printf("BASS ERROR %d in Init", BASS_ErrorGetCode());

    // Load the Plugins
    if (!BASS_PluginLoad("basswma.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf("BASS ERROR %d in PluginLoad WMA", BASS_ErrorGetCode());
    if (!BASS_PluginLoad("bassflac.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf("BASS ERROR %d in PluginLoad FLAC", BASS_ErrorGetCode());
    if (!BASS_PluginLoad("bassmidi.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf("BASS ERROR %d in PluginLoad MIDI", BASS_ErrorGetCode());
    if (!BASS_PluginLoad("bass_aac.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf("BASS ERROR %d in PluginLoad AAC", BASS_ErrorGetCode());
    if (!BASS_PluginLoad("bass_ac3.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf("BASS ERROR %d in PluginLoad AC3", BASS_ErrorGetCode());
    if (!BASS_PluginLoad("bassopus.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf("BASS ERROR %d in PluginLoad OPUS", BASS_ErrorGetCode());
    if (!BASS_PluginLoad("basswebm.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf("BASS ERROR %d in PluginLoad WEBM", BASS_ErrorGetCode());

    BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);
    BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);            // Allow playlists

    m_strUserAgent = SString("MTA:SA Server %s - See http://mtasa.com/agent/", g_pNet->GetConnectedServer(true));
    BASS_SetConfigPtr(BASS_CONFIG_NET_AGENT, (void*)*m_strUserAgent);

    UpdateVolume();

    m_FxEffectNames["chorus"] = BASS_FX_DX8_CHORUS;
    m_FxEffectNames["compressor"] = BASS_FX_DX8_COMPRESSOR;
    m_FxEffectNames["distortion"] = BASS_FX_DX8_DISTORTION;
    m_FxEffectNames["echo"] = BASS_FX_DX8_ECHO;
    m_FxEffectNames["flanger"] = BASS_FX_DX8_FLANGER;
    m_FxEffectNames["gargle"] = BASS_FX_DX8_GARGLE;
    m_FxEffectNames["i3dl2reverb"] = BASS_FX_DX8_I3DL2REVERB;
    m_FxEffectNames["parameq"] = BASS_FX_DX8_PARAMEQ;
    m_FxEffectNames["reverb"] = BASS_FX_DX8_REVERB;

    // Validate audio container on startup
    for (int i = 0; i < 9; i++)
    {
        m_aValidatedSFX[i] = g_pGame->GetAudioContainer()->ValidateContainer(static_cast<eAudioLookupIndex>(i));
    }
}

CClientSoundManager::~CClientSoundManager()
{
    ProcessStopQueues(true);
    BASS_Stop();
    BASS_Free();
}

void CClientSoundManager::DoPulse()
{
    UpdateVolume();

    CClientCamera* pCamera = m_pClientManager->GetCamera();

    CVector vecCameraPosition, vecPosition, vecLookAt, vecFront, vecVelocity;
    pCamera->GetPosition(vecCameraPosition);
    pCamera->GetFixedTarget(vecLookAt);
    vecFront = vecLookAt - vecCameraPosition;

    CClientPlayer* p_LocalPlayer = m_pClientManager->GetPlayerManager()->GetLocalPlayer();
    if (p_LocalPlayer)
    {
        p_LocalPlayer->GetMoveSpeed(vecVelocity);

        if (pCamera->IsInFixedMode())
            vecPosition = vecCameraPosition;
        else
            p_LocalPlayer->GetPosition(vecPosition);
    }
    else
    {
        // Make sure the players position at least has something (I'd be surprised if we get here though)
        vecPosition = vecCameraPosition;
    }

    // Update volume position and velocity from all sounds
    list<CClientSound*>::iterator iter = m_Sounds.begin();
    for (; iter != m_Sounds.end(); ++iter)
    {
        CClientSound* pSound = *iter;
        pSound->Process3D(vecPosition, vecCameraPosition, vecLookAt);

        // Delete sound if finished
        if (pSound->IsFinished())
        {
            // call onClientSoundStopped
            CLuaArguments Arguments;
            Arguments.PushString("finished");            // Reason
            pSound->CallEvent("onClientSoundStopped", Arguments, false);
            g_pClientGame->GetElementDeleter()->Delete(pSound);
        }
    }
    UpdateDistanceStreaming(vecCameraPosition);
    ProcessStopQueues();
}

void CClientSoundManager::SetDimension(unsigned short usDimension)
{
    m_usDimension = usDimension;
}

CClientSound* CClientSoundManager::PlaySound2D(const SString& strSound, bool bIsURL, bool bIsRawData, bool bLoop, bool bThrottle)
{
    CClientSound* pSound = new CClientSound(m_pClientManager, INVALID_ELEMENT_ID);
    if (bIsURL)
    {
        pSound->PlayStream(strSound, bLoop, bThrottle);
        return pSound;
    }
    else if (bIsRawData)
    {
        size_t size = strSound.size();
        void*  pMemory = new char[size];
        memcpy(pMemory, strSound.data(), size);
        if (pSound->Play((void*)pMemory, size, bLoop))
            return pSound;
    }
    else if (pSound->Play(strSound, bLoop))
        return pSound;

    delete pSound;
    return NULL;
}

CClientSound* CClientSoundManager::PlaySound2D(void* pMemory, unsigned int uiLength, bool bLoop)
{
    CClientSound* pSound = new CClientSound(m_pClientManager, INVALID_ELEMENT_ID);

    if (pSound->Play(pMemory, uiLength, bLoop))
        return pSound;

    delete pSound;
    return NULL;
}

CClientSound* CClientSoundManager::PlaySound3D(const SString& strSound, bool bIsURL, bool bIsRawData, const CVector& vecPosition, bool bLoop, bool bThrottle)
{
    CClientSound* pSound = new CClientSound(m_pClientManager, INVALID_ELEMENT_ID);

    if (bIsURL)
    {
        pSound->PlayStream(strSound, bLoop, bThrottle, true);
        pSound->SetPosition(vecPosition);
        return pSound;
    }
    else if (bIsRawData)
    {
        size_t size = strSound.size();
        void*  pMemory = new char[size];
        memcpy(pMemory, strSound.data(), size);
        if (pSound->Play3D((void*)pMemory, size, bLoop))
        {
            pSound->SetPosition(vecPosition);
            return pSound;
        }
    }
    else if (pSound->Play3D(strSound, bLoop))
    {
        pSound->SetPosition(vecPosition);
        return pSound;
    }

    delete pSound;
    return NULL;
}

CClientSound* CClientSoundManager::PlaySound3D(void* pMemory, unsigned int uiLength, const CVector& vecPosition, bool bLoop)
{
    CClientSound* pSound = new CClientSound(m_pClientManager, INVALID_ELEMENT_ID);

    if (pSound->Play3D(pMemory, uiLength, bLoop))
    {
        pSound->SetPosition(vecPosition);
        return pSound;
    }

    delete pSound;
    return NULL;
}

CClientSound* CClientSoundManager::PlayGTASFX(eAudioLookupIndex containerIndex, int iBankIndex, int iAudioIndex, bool bLoop)
{
    void*        pAudioData;
    unsigned int uiAudioLength;

    if (containerIndex == AUDIO_LOOKUP_RADIO)
    {
        if (!g_pGame->GetAudioContainer()->GetRadioAudioData((eRadioStreamIndex)iBankIndex, iAudioIndex, pAudioData, uiAudioLength))
            return NULL;
    }
    else
    {
        if (!GetSFXStatus(containerIndex))
            return NULL;

        if (!g_pGame->GetAudioContainer()->GetAudioData(containerIndex, iBankIndex, iAudioIndex, pAudioData, uiAudioLength))
            return NULL;
    }

    CClientSound* pSound = PlaySound2D(pAudioData, uiAudioLength, bLoop);
    if (pSound)
    {
        CGameSettings* gameSettings = g_pGame->GetSettings();
        pSound->SetVolume(gameSettings->GetSFXVolume() / 255.0f);
    }
    return pSound;
}

CClientSound* CClientSoundManager::PlayGTASFX3D(eAudioLookupIndex containerIndex, int iBankIndex, int iAudioIndex, const CVector& vecPosition, bool bLoop)
{
    void*        pAudioData;
    unsigned int uiAudioLength;

    if (containerIndex == AUDIO_LOOKUP_RADIO)
    {
        if (!g_pGame->GetAudioContainer()->GetRadioAudioData((eRadioStreamIndex)iBankIndex, iAudioIndex, pAudioData, uiAudioLength))
            return NULL;
    }
    else
    {
        if (!GetSFXStatus(containerIndex))
            return NULL;

        if (!g_pGame->GetAudioContainer()->GetAudioData(containerIndex, iBankIndex, iAudioIndex, pAudioData, uiAudioLength))
            return NULL;
    }

    CClientSound* pSound = PlaySound3D(pAudioData, uiAudioLength, vecPosition, bLoop);
    if (pSound)
    {
        CGameSettings* gameSettings = g_pGame->GetSettings();
        pSound->SetVolume(gameSettings->GetSFXVolume() / 255.0f);
    }
    return pSound;
}

bool CClientSoundManager::GetSFXStatus(eAudioLookupIndex containerIndex)
{
    return m_aValidatedSFX[static_cast<int>(containerIndex)];
}

int CClientSoundManager::GetFxEffectFromName(const std::string& strEffectName)
{
    std::map<std::string, int>::iterator it;
    it = m_FxEffectNames.find(strEffectName);

    if (it != m_FxEffectNames.end())
    {
        return it->second;
    }
    return -1;
}

void CClientSoundManager::UpdateVolume()
{
    // set our mta sound volume if mtavolume or mastervolume cvar changed
    float fValue = 0.0f;

    if (!m_bMinimizeMuted)
    {
        float fMasterVolume = g_pCore->GetCVars()->GetValue<float>("mastervolume", 1.0f);

        if (g_pCore->GetCVars()->Get("mtavolume", fValue))
        {
            fValue *= fMasterVolume;

            if (fValue * 10000 == BASS_GetConfig(BASS_CONFIG_GVOL_STREAM))
                return;

            fValue = std::max(0.0f, std::min(1.0f, fValue));
        }
        else
        {
            fValue = fMasterVolume;
        }
    }

    BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, static_cast<DWORD>(fValue * 10000));
    BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, static_cast<DWORD>(fValue * 10000));
}

//
// Lists
//
void CClientSoundManager::AddToList(CClientSound* pSound)
{
    m_Sounds.push_back(pSound);
}

void CClientSoundManager::RemoveFromList(CClientSound* pSound)
{
    m_Sounds.remove(pSound);
    MapRemove(m_DistanceStreamedInMap, pSound);
}

//
// Distance streaming
//
void CClientSoundManager::OnDistanceStreamIn(CClientSound* pSound)
{
    assert(!MapContains(m_DistanceStreamedInMap, pSound));
    m_DistanceStreamedInMap.insert(pSound);
}

void CClientSoundManager::OnDistanceStreamOut(CClientSound* pSound)
{
    assert(MapContains(m_DistanceStreamedInMap, pSound));
    MapRemove(m_DistanceStreamedInMap, pSound);
}

//
// Distance streaming like what is done with visible objects
//
void CClientSoundManager::UpdateDistanceStreaming(const CVector& vecListenerPosition)
{
    //
    // Make a copy of the current list of sounds that are active
    //
    std::set<CClientSound*> considerMap = m_DistanceStreamedInMap;

    //
    // Mix in all sounds near enough to the listener to be heard
    //
    {
        // Find all entities overlapping the listener position
        CClientEntityResult result;
        GetClientSpatialDatabase()->SphereQuery(result, CSphere(vecListenerPosition, 0));

        // Extract relevant types
        for (CClientEntityResult::const_iterator iter = result.begin(); iter != result.end(); ++iter)
        {
            if (CClientSound* pSound = DynamicCast<CClientSound>(*iter))
            {
                if (pSound->IsSound3D())
                {
                    // Add to consider map
                    considerMap.insert(pSound);
                }
            }
        }
    }

    //
    // Step through each sound
    //  If the sound is more than 40 units away (or in another dimension), make sure it is deactivated
    //  If the sound is less than 20 units away, make sure it is activated
    //
    for (CClientSound* pSound : considerMap)
    {
        // Calculate distance to the edge of the sphere
        CSphere sphere = pSound->GetWorldBoundingSphere();
        float   fDistance = (vecListenerPosition - sphere.vecPosition).Length() - sphere.fRadius;

        if (fDistance > 40 || m_usDimension != pSound->GetDimension())
        {
            if (MapContains(m_DistanceStreamedInMap, pSound))
                pSound->DistanceStreamOut();
        }
        else if (fDistance < 20)
            pSound->DistanceStreamIn();
    }
}

//
// Add a BASS sound to be stopped
//
void CClientSoundManager::QueueChannelStop(DWORD pSound)
{
    // Always not main thread
    dassert(!IsMainThread());
    m_CS.Lock();
    m_ChannelStopQueue.push_back(pSound);
    m_CS.Unlock();
}

//
// Add a CBassAudio to be stopped
//
void CClientSoundManager::QueueAudioStop(CBassAudio* pAudio)
{
    // Always main thread
    dassert(IsMainThread());
    MapSet(m_AudioStopQueue, pAudio, CElapsedTime());
}

//
// Process pending sounds to be stopped
//
void CClientSoundManager::ProcessStopQueues(bool bFlush)
{
    if (!m_ChannelStopQueue.empty())
    {
        m_CS.Lock();
        std::vector<DWORD> channelStopList = m_ChannelStopQueue;
        m_ChannelStopQueue.clear();
        m_CS.Unlock();
        for (unsigned int i = 0; i < channelStopList.size(); i++)
        {
            BASS_ChannelStop(channelStopList[i]);
        }
    }

    for (std::map<CBassAudio*, CElapsedTime>::iterator iter = m_AudioStopQueue.begin(); iter != m_AudioStopQueue.end();)
    {
        if (iter->second.Get() > 100 || bFlush)
        {
            delete iter->first;            // This will cause BASS_ChannelStop
            m_AudioStopQueue.erase(iter++);
        }
        else
            ++iter;
    }
}
