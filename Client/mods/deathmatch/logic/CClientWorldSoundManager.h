/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientWorldSoundManager.h
 *  PURPOSE:     Client-side world sound replacement
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <list>
#include <unordered_map>
#include <vector>
#include <game/CAudioEngine.h>

class CClientManager;
class CClientSound;

class CClientWorldSoundManager
{
public:
    CClientWorldSoundManager(CClientManager* pManager);
    ~CClientWorldSoundManager();

    bool ReplaceSound(uint uiGroup, uint uiIndex, const SString& strSound, bool bIsRawData, float fMinDistance = 0.0f, float fMaxDistance = 0.0f,
                      SString* pOutError = nullptr);
    bool RestoreSound(uint uiGroup, uint uiIndex);
    void RestoreAll();

    bool IsSoundReplaced(uint uiGroup, uint uiIndex) const;

    bool HandleWorldSound(const SWorldSoundEvent& event, bool bAllowPlay);

    void DoPulse();

private:
    struct SReplacement
    {
        bool    bRawData;
        SString strSound;
        float   fMinDistance;
        float   fMaxDistance;

        bool              bNativeWanted = false;
        bool              bNativeApplied = false;
        std::vector<char> pcmData;
        std::vector<char> originalPcm;
        uint              uiNativeLastTryTick = 0;
    };

    struct SLastPlayed
    {
        uint    uiTick;
        CVector vecPosition;
    };

    struct SFollowSound
    {
        CClientSound*       pSound;
        CEntitySAInterface* pGameEntity;
        uint                uiStartTick;
    };

    static uint MakeKey(uint uiGroup, uint uiIndex) { return (uiGroup << 16) | (uiIndex & 0xFFFF); }

    bool FindReplacement(uint uiGroup, uint uiIndex, const SReplacement** ppOutReplacement) const;

    bool TryApplyNativeReplacement(SReplacement& replacement, uint uiGroup, uint uiIndex);
    bool RestoreSoundBuffer(const SReplacement& replacement, uint uiGroup, uint uiIndex);
    void ApplyNativeReplacements();

    CClientManager*                        m_pManager;
    std::unordered_map<uint, SReplacement> m_Replacements;
    std::unordered_map<uint, SLastPlayed>  m_LastPlayed;
    std::list<SFollowSound>                m_FollowSounds;
    uint                                   m_uiLastPruneTick;
};
