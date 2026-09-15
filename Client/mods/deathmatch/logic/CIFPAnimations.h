/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CIFPAnimations.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <bitset>
#include "CClientIFP.h"

class CIFPAnimations
{
public:
    struct SAnimation
    {
        SString                              Name;
        unsigned int                         uiNameHash;
        std::unique_ptr<CAnimBlendHierarchy> pHierarchy;
        BYTE*                                pSequencesMemory;

        // Bit i is set when bone i (see CClientIFP::m_karruBoneIds) is actually driven by this
        // animation's own IFP data, as opposed to being padded out with a fixed pose because the
        // file doesn't define that bone.
        std::bitset<32> AnimatedBonesMask;
    };

    std::vector<SAnimation> vecAnimations;

public:
    ~CIFPAnimations();
    void DeleteAnimations();
};
