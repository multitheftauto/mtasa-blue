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

#include <unordered_map>
#include <vector>
#include <game/CAudioEngine.h>

class CClientManager;

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
    bool HandleWorldSound(const SWorldSoundEvent& event);

    void DoPulse();

private:
    struct SReplacement
    {
        bool    bRawData;
        SString strSound;
        float   fMinDistance;
        float   fMaxDistance;

        bool                                        bWholeGroup = false;
        bool                                        bNativeWanted = false;
        bool                                        bNativeApplied = false;
        bool                                        bResultLogged = false;
        std::unordered_map<uint, std::vector<char>> pcmByRate;
        std::unordered_map<uint, std::vector<char>> originalPcm;
        std::unordered_map<uint, ushort>            originalRate;
        uint                                        uiNativeLastTryTick = 0;
    };

    static uint MakeKey(uint uiGroup, uint uiIndex) { return (uiGroup << 16) | (uiIndex & 0xFFFF); }

    bool FindReplacement(uint uiGroup, uint uiIndex, const SReplacement** ppOutReplacement) const;

    bool TryApplyNativeReplacement(SReplacement& replacement, uint uiGroup, uint uiIndex);
    bool PatchSoundBufferIndex(SReplacement& replacement, uint uiGroup, uint uiIndex);
    bool RestoreSoundBuffer(const SReplacement& replacement, uint uiGroup);
    void ApplyNativeReplacements();
    void LogResult(SReplacement& replacement, uint uiGroup, uint uiIndex, const SString& strResult, bool bWarning);

    CClientManager*                        m_pManager;
    std::unordered_map<uint, SReplacement> m_Replacements;
};
