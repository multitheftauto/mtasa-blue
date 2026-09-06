/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientWorldSoundManager.cpp
 *  PURPOSE:     Client-side world sound replacement (engineReplaceWorldSound)
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientWorldSoundManager.h"
#include "CClientEntity.h"
#include "CClientManager.h"
#include "CClientSoundManager.h"
#include <cmath>
#include <game/CPools.h>

CClientWorldSoundManager::CClientWorldSoundManager(CClientManager* pManager) : m_pManager(pManager), m_uiLastPruneTick(0)
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

    m_Replacements[MakeKey(uiGroup, uiIndex)] = {bIsRawData, strSound, std::max(0.0f, fMinDistance), std::max(0.0f, fMaxDistance)};
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
                iter = m_Replacements.erase(iter);
                bErased = true;
            }
            else
                ++iter;
        }
        return bErased;
    }

    return m_Replacements.erase(MakeKey(uiGroup, uiIndex)) > 0;
}

void CClientWorldSoundManager::RestoreAll()
{
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

bool CClientWorldSoundManager::HandleWorldSound(const SWorldSoundEvent& event, bool bAllowPlay)
{
    const SReplacement* pReplacement = nullptr;
    if (!FindReplacement(event.uiGroup, event.uiIndex, &pReplacement))
        return false;

    if (bAllowPlay)
    {
        const uint uiKey = MakeKey(event.uiGroup, event.uiIndex);

        bool bAlreadyPlayed = false;
        auto iter = m_LastPlayed.find(uiKey);
        if (iter != m_LastPlayed.end())
        {
            const uint    uiElapsed = GetTickCount32() - iter->second.uiTick;
            const CVector vecDelta = event.vecPosition - iter->second.vecPosition;
            if (uiElapsed < 40 && vecDelta.LengthSquared() < 2.25f)
                bAlreadyPlayed = true;
        }

        if (!bAlreadyPlayed)
        {
            CClientSound* pSound =
                m_pManager->GetSoundManager()->PlaySound3D(pReplacement->strSound, false, pReplacement->bRawData, event.vecPosition, false, true);
            if (!pSound)
            {
                return false;
            }

            float fMaxDistance = pReplacement->fMaxDistance > 0.0f ? pReplacement->fMaxDistance : event.fMaxDistance;
            if (fMaxDistance > 1.0f)
            {
                float fMinDistance = pReplacement->fMinDistance > 0.0f ? pReplacement->fMinDistance : std::min(5.0f, fMaxDistance * 0.25f);
                fMinDistance = std::max(0.1f, std::min(fMinDistance, fMaxDistance * 0.95f));
                pSound->SetMinDistance(fMinDistance);
                pSound->SetMaxDistance(fMaxDistance);
            }

            float fVolume = g_pCore->GetCVars()->GetValue<float>("sfxvolume", 1.0f);
            float fMtaVolume = 1.0f;
            if (g_pCore->GetCVars()->Get("mtavolume", fMtaVolume) && fMtaVolume > 0.0f)
                fVolume = std::min(4.0f, fVolume / fMtaVolume);

            if (g_pCore->IsWindowMinimized() &&
                (g_pCore->GetCVars()->GetValue<bool>("mute_master_when_minimized") || g_pCore->GetCVars()->GetValue<bool>("mute_sfx_when_minimized")))
            {
                fVolume = 0.0f;
            }

            pSound->SetVolume(fVolume);

            if (event.pGameEntity)
                m_FollowSounds.push_back({pSound, event.pGameEntity, GetTickCount32()});

            m_LastPlayed[uiKey] = {GetTickCount32(), event.vecPosition};
        }
    }

    return true;
}

void CClientWorldSoundManager::DoPulse()
{
    const uint uiNow = GetTickCount32();

    for (auto iter = m_FollowSounds.begin(); iter != m_FollowSounds.end();)
    {
        SFollowSound& entry = *iter;

        if (entry.pSound->IsFinished() || uiNow - entry.uiStartTick > 10000)
        {
            iter = m_FollowSounds.erase(iter);
            continue;
        }

        if (CClientEntity* pEntity = g_pGame->GetPools()->GetClientEntity((DWORD*)entry.pGameEntity))
        {
            CVector vecPosition;
            pEntity->GetPosition(vecPosition);
            entry.pSound->SetPosition(vecPosition);
        }

        ++iter;
    }

    if (uiNow - m_uiLastPruneTick > 500)
    {
        m_uiLastPruneTick = uiNow;
        for (auto iter = m_LastPlayed.begin(); iter != m_LastPlayed.end();)
        {
            if (uiNow - iter->second.uiTick > 1000)
                iter = m_LastPlayed.erase(iter);
            else
                ++iter;
        }
    }
}
