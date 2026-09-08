/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientWorldSoundManager.cpp
 *  PURPOSE:     Client-side world sound replacement
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientWorldSoundManager.h"
#include "CClientGame.h"
#include "CClientManager.h"
#include "CClientSoundManager.h"
#include <cmath>
#include <game/CAEAudioHardware.h>

CClientWorldSoundManager::CClientWorldSoundManager(CClientManager* pManager) : m_pManager(pManager)
{
}

CClientWorldSoundManager::~CClientWorldSoundManager()
{
    RestoreAll();
}

bool CClientWorldSoundManager::ReplaceSound(uint uiGroup, uint uiIndex, const SString& strSound, bool bIsRawData, float fMinDistance, float fMaxDistance,
                                            SString* pOutError)
{
    if (uiIndex != static_cast<uint>(-1) && uiIndex > 399)
    {
        if (pOutError)
            *pOutError = SString("invalid sound index %u (maximum is 399)", uiIndex);
        return false;
    }

    if (!std::isfinite(fMinDistance) || !std::isfinite(fMaxDistance))
    {
        if (pOutError)
            *pOutError = "minDistance/maxDistance must be finite numbers (no NaN or infinity)";
        return false;
    }
    if (fMinDistance > 0.0f && fMaxDistance > 0.0f && fMinDistance > fMaxDistance)
    {
        if (pOutError)
            *pOutError = SString("minDistance (%.2f) cannot be greater than maxDistance (%.2f)", fMinDistance, fMaxDistance);
        return false;
    }

    if (!m_pManager->GetSoundManager()->ValidateSound(strSound, bIsRawData, pOutError))
        return false;

    SReplacement replacement;
    replacement.bRawData = bIsRawData;
    replacement.strSound = strSound;
    replacement.fMinDistance = std::max(0.0f, fMinDistance);
    replacement.fMaxDistance = std::max(0.0f, fMaxDistance);
    replacement.bWholeGroup = (uiIndex == static_cast<uint>(-1));
    replacement.bNativeWanted = true;
    replacement.uiNativeLastTryTick = 0;

    const uint uiKey = MakeKey(uiGroup, uiIndex);
    auto       iterExisting = m_Replacements.find(uiKey);
    if (iterExisting != m_Replacements.end())
    {
        replacement.originalPcm = std::move(iterExisting->second.originalPcm);
        replacement.pcmByRate = std::move(iterExisting->second.pcmByRate);
    }

    m_Replacements[uiKey] = std::move(replacement);
    m_Replacements[uiKey].bNativeApplied = TryApplyNativeReplacement(m_Replacements[uiKey], uiGroup, uiIndex);

    return true;
}

bool CClientWorldSoundManager::RestoreSound(uint uiGroup, uint uiIndex)
{
    if (uiIndex == static_cast<uint>(-1))
    {
        bool bErased = false;
        for (auto iter = m_Replacements.begin(); iter != m_Replacements.end();)
        {
            if ((iter->first >> 16) == uiGroup)
            {
                RestoreSoundBuffer(iter->second, uiGroup);
                iter = m_Replacements.erase(iter);
                bErased = true;
            }
            else
                ++iter;
        }
        return bErased;
    }

    auto iter = m_Replacements.find(MakeKey(uiGroup, uiIndex));
    if (iter == m_Replacements.end())
        return false;

    RestoreSoundBuffer(iter->second, uiGroup);
    m_Replacements.erase(iter);
    return true;
}

void CClientWorldSoundManager::RestoreAll()
{
    if (g_pGame)
    {
        for (auto& iter : m_Replacements)
            RestoreSoundBuffer(iter.second, iter.first >> 16);
    }
    m_Replacements.clear();
}

bool CClientWorldSoundManager::IsSoundReplaced(uint uiGroup, uint uiIndex) const
{
    if (uiIndex == static_cast<uint>(-1))
    {
        for (const auto& iter : m_Replacements)
        {
            if ((iter.first >> 16) == uiGroup)
                return true;
        }
        return false;
    }

    const SReplacement* pReplacement = nullptr;
    return FindReplacement(uiGroup, uiIndex, &pReplacement);
}

bool CClientWorldSoundManager::FindReplacement(uint uiGroup, uint uiIndex, const SReplacement** ppOutReplacement) const
{
    auto iter = m_Replacements.find(MakeKey(uiGroup, uiIndex));
    if (iter != m_Replacements.end())
    {
        *ppOutReplacement = &iter->second;
        return true;
    }

    iter = m_Replacements.find(MakeKey(uiGroup, static_cast<uint>(-1)));
    if (iter != m_Replacements.end())
    {
        *ppOutReplacement = &iter->second;
        return true;
    }

    return false;
}

bool CClientWorldSoundManager::TryApplyNativeReplacement(SReplacement& replacement, uint uiGroup, uint uiIndex)
{
    if (!g_pGame)
        return false;

    CAEAudioHardware* pAudioHardware = g_pGame->GetAEAudioHardware();
    if (!pAudioHardware)
        return false;

    if (replacement.bWholeGroup)
    {
        const uint uiNumSounds = pAudioHardware->GetNumSoundsInBankSlot(static_cast<ushort>(uiGroup));
        bool       bAnyApplied = false;
        for (uint i = 0; i < uiNumSounds; ++i)
        {
            if (PatchSoundBufferIndex(replacement, uiGroup, i))
                bAnyApplied = true;
        }
        return bAnyApplied;
    }

    return PatchSoundBufferIndex(replacement, uiGroup, uiIndex);
}

bool CClientWorldSoundManager::PatchSoundBufferIndex(SReplacement& replacement, uint uiGroup, uint uiIndex)
{
    if (!g_pGame)
        return false;

    CAEAudioHardware* pAudioHardware = g_pGame->GetAEAudioHardware();
    if (!pAudioHardware)
        return false;

    void* pPcmData = nullptr;
    uint  uiPcmSize = 0;
    uint  uiCurrentRate = 0;
    int   iLoopStartOffset = -1;
    if (!pAudioHardware->GetLoadedSoundInfo(static_cast<ushort>(uiGroup), static_cast<ushort>(uiIndex), pPcmData, uiPcmSize, uiCurrentRate, iLoopStartOffset))
        return false;

    if (uiCurrentRate == 0)
    {
        LogResult(replacement, uiGroup, uiIndex, "invalid bank sample rate 0", false);
        return false;
    }

    const uint uiBaseRate = replacement.originalRate.contains(uiIndex) ? replacement.originalRate[uiIndex] : uiCurrentRate;

    std::vector<char>& decoded = replacement.pcmByRate[uiBaseRate];
    if (decoded.empty() && !m_pManager->GetSoundManager()->DecodeToPcm(replacement.strSound, replacement.bRawData, uiBaseRate, decoded))
    {
        LogResult(replacement, uiGroup, uiIndex, SString("failed to decode '%s'", *replacement.strSound), false);
        return false;
    }

    const uint uiSourceSize = static_cast<uint>(decoded.size());
    uint       uiPatchRate = uiBaseRate;

    if (uiSourceSize > uiPcmSize)
    {
        if (iLoopStartOffset >= 0)
        {
            const uint uiRequiredRate = static_cast<uint>(uiBaseRate * (uiPcmSize * 0.98f) / uiSourceSize);
            if (uiRequiredRate < 8000)
            {
                LogResult(
                    replacement, uiGroup, uiIndex,
                    SString("replacement too long for looping sound: needs %u Hz to fit %u bytes into %u byte slot", uiRequiredRate, uiSourceSize, uiPcmSize),
                    false);
                return false;
            }

            uiPatchRate = uiRequiredRate;
            std::vector<char>& fitted = replacement.pcmByRate[uiPatchRate];
            if (fitted.empty() && !m_pManager->GetSoundManager()->DecodeToPcm(replacement.strSound, replacement.bRawData, uiPatchRate, fitted))
            {
                LogResult(replacement, uiGroup, uiIndex, SString("failed to decode '%s' at %u Hz", *replacement.strSound, uiPatchRate), false);
                return false;
            }
            if (fitted.size() > uiPcmSize)
            {
                LogResult(replacement, uiGroup, uiIndex,
                          SString("replacement too long for looping sound: still %u bytes at %u Hz", static_cast<uint>(fitted.size()), uiPatchRate), false);
                return false;
            }
        }
        else
        {
            decoded.resize(uiPcmSize);

            const uint uiTotalSamples = uiPcmSize / 2;
            const uint uiFadeSamples = std::min<uint>(uiTotalSamples, std::max<uint>(1, uiBaseRate / 125));
            short*     pSamples = reinterpret_cast<short*>(decoded.data());
            const uint uiFadeStart = uiTotalSamples - uiFadeSamples;
            for (uint i = 0; i < uiFadeSamples; ++i)
            {
                const float fGain = static_cast<float>(uiFadeSamples - i) / static_cast<float>(uiFadeSamples);
                pSamples[uiFadeStart + i] = static_cast<short>(static_cast<float>(pSamples[uiFadeStart + i]) * fGain);
            }
        }
    }

    if (!replacement.originalPcm.contains(uiIndex))
    {
        replacement.originalPcm[uiIndex].assign(static_cast<const char*>(pPcmData), static_cast<const char*>(pPcmData) + uiPcmSize);
        replacement.originalRate[uiIndex] = static_cast<ushort>(uiCurrentRate);
    }

    if (!pAudioHardware->PatchSoundBuffer(static_cast<ushort>(uiGroup), static_cast<ushort>(uiIndex), replacement.pcmByRate[uiPatchRate].data(),
                                          static_cast<uint>(replacement.pcmByRate[uiPatchRate].size())))
    {
        LogResult(replacement, uiGroup, uiIndex, "failed to patch bank slot", false);
        return false;
    }

    if (uiPatchRate != uiCurrentRate &&
        !pAudioHardware->SetSoundSampleRate(static_cast<ushort>(uiGroup), static_cast<ushort>(uiIndex), static_cast<ushort>(uiPatchRate)))
    {
        LogResult(replacement, uiGroup, uiIndex, "failed to set bank sample rate", false);
        return false;
    }

    if (uiSourceSize > uiPcmSize)
    {
        if (iLoopStartOffset >= 0)
            LogResult(replacement, uiGroup, uiIndex,
                      SString("looping sound rate-lowered to fit: %u bytes at %u Hz (source was %u bytes at %u Hz)", uiPcmSize, uiPatchRate, uiSourceSize,
                              uiBaseRate),
                      true);
        else
            LogResult(replacement, uiGroup, uiIndex,
                      SString("sound trimmed to fit: %u bytes at %u Hz (source was %u bytes)", uiPcmSize, uiBaseRate, uiSourceSize), true);
    }

    return true;
}

bool CClientWorldSoundManager::RestoreSoundBuffer(const SReplacement& replacement, uint uiGroup)
{
    if (replacement.originalPcm.empty() || !g_pGame)
        return false;

    CAEAudioHardware* pAudioHardware = g_pGame->GetAEAudioHardware();
    if (!pAudioHardware)
        return false;

    bool bRestored = false;
    for (const auto& entry : replacement.originalPcm)
    {
        const ushort usIndex = static_cast<ushort>(entry.first);
        if (pAudioHardware->PatchSoundBuffer(static_cast<ushort>(uiGroup), usIndex, entry.second.data(), static_cast<uint>(entry.second.size())))
        {
            auto iterRate = replacement.originalRate.find(entry.first);
            if (iterRate != replacement.originalRate.end())
                pAudioHardware->SetSoundSampleRate(static_cast<ushort>(uiGroup), usIndex, iterRate->second);
            bRestored = true;
        }
    }
    return bRestored;
}

void CClientWorldSoundManager::LogResult(SReplacement& replacement, uint uiGroup, uint uiIndex, const SString& strResult, bool bWarning)
{
    if (replacement.bResultLogged)
        return;

    replacement.bResultLogged = true;
    if (g_pClientGame && g_pClientGame->GetScriptDebugging())
    {
        if (bWarning)
            g_pClientGame->GetScriptDebugging()->LogWarning(NULL, "WorldSound: %s (group %u index %u)", *strResult, uiGroup, uiIndex);
        else
            g_pClientGame->GetScriptDebugging()->LogError(NULL, "WorldSound: %s (group %u index %u)", *strResult, uiGroup, uiIndex);
    }
}

void CClientWorldSoundManager::ApplyNativeReplacements()
{
    const uint uiNow = GetTickCount32();

    for (auto& iter : m_Replacements)
    {
        SReplacement& replacement = iter.second;
        if (!replacement.bNativeWanted)
            continue;

        const uint uiStoredIndex = iter.first & 0xFFFF;

        if (!replacement.bNativeApplied)
        {
            if (uiNow - replacement.uiNativeLastTryTick < 500)
                continue;
            replacement.uiNativeLastTryTick = uiNow;
            replacement.bNativeApplied = TryApplyNativeReplacement(replacement, iter.first >> 16, uiStoredIndex);
        }
        else if (uiNow - replacement.uiNativeLastTryTick > 2000)
        {
            replacement.uiNativeLastTryTick = uiNow;
            TryApplyNativeReplacement(replacement, iter.first >> 16, uiStoredIndex);
        }
    }
}

bool CClientWorldSoundManager::HandleWorldSound(const SWorldSoundEvent& event)
{
    SReplacement* pReplacement = nullptr;
    auto          iter = m_Replacements.find(MakeKey(event.uiGroup, event.uiIndex));
    if (iter != m_Replacements.end())
        pReplacement = &iter->second;
    else
    {
        iter = m_Replacements.find(MakeKey(event.uiGroup, static_cast<uint>(-1)));
        if (iter != m_Replacements.end())
            pReplacement = &iter->second;
    }

    if (!pReplacement)
        return false;

    const uint uiNow = GetTickCount32();
    if (!pReplacement->bNativeApplied && uiNow - pReplacement->uiNativeLastTryTick >= 500)
    {
        pReplacement->uiNativeLastTryTick = uiNow;
        pReplacement->bNativeApplied = TryApplyNativeReplacement(*pReplacement, event.uiGroup, event.uiIndex);
    }

    if (pReplacement->fMaxDistance > 0.0f && event.pAESound && g_pGame && g_pGame->GetAudioEngine())
        g_pGame->GetAudioEngine()->SetWorldSoundMaxDistance(event.pAESound, pReplacement->fMaxDistance);

    return false;
}

void CClientWorldSoundManager::DoPulse()
{
    ApplyNativeReplacements();
}
