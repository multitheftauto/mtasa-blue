/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CStreamingSA.h
 *  PURPOSE:     Header file for data streamer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CStreaming.h>
#include "Common.h"

#define FUNC_CStreaming__RequestModel                0x4087E0
#define FUNC_LoadAllRequestedModels                  0x40EA10
#define FUNC_CStreaming__HasVehicleUpgradeLoaded     0x407820
#define FUNC_CStreaming_RequestAnimations            0x407120
#define FUNC_CStreaming_RequestSpecialModel          0x409d10

class CStreamingSA final : public CStreaming
{
public:
    void            RequestModel(DWORD dwModelID, DWORD dwFlags);
    void            RemoveModel(std::uint32_t model) override;
    void            LoadAllRequestedModels(BOOL bOnlyPriorityModels = 0, const char* szTag = NULL);
    BOOL            HasModelLoaded(DWORD dwModelID);
    void            RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel);
    void            ReinitStreaming();
    CStreamingInfo* GetStreamingInfoFromModelId(uint32 id);
    void            MakeSpaceFor(std::uint32_t memoryToCleanInBytes) override;
    std::uint32_t   GetMemoryUsed() const override;

private:
    static CStreamingInfo* ms_aInfoForModel;            // count: 26316 in unmodified game
};
