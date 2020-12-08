/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CStreamingSA.cpp
 *  PURPOSE:     Data streamer
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "CModelInfoSA.h"

// count: 26316 in unmodified game
CStreamingInfo* CStreamingSA::ms_aInfoForModel = (CStreamingInfo*)CStreaming__ms_aInfoForModel;

namespace
{
    //
    // Used in LoadAllRequestedModels to record state
    //
    struct SPassStats
    {
        bool  bLoadingBigModel;
        DWORD numPriorityRequests;
        DWORD numModelsRequested;
        DWORD memoryUsed;

        void Record()
        {
            #define VAR_CStreaming_bLoadingBigModel         0x08E4A58
            #define VAR_CStreaming_numPriorityRequests      0x08E4BA0
            #define VAR_CStreaming_numModelsRequested       0x08E4CB8
            #define VAR_CStreaming_memoryUsed               0x08E4CB4

            bLoadingBigModel = *(BYTE*)VAR_CStreaming_bLoadingBigModel != 0;
            numPriorityRequests = *(DWORD*)VAR_CStreaming_numPriorityRequests;
            numModelsRequested = *(DWORD*)VAR_CStreaming_numModelsRequested;
            memoryUsed = *(DWORD*)VAR_CStreaming_memoryUsed;
        }
    };
}            // namespace

bool IsUpgradeModelId(DWORD dwModelID)
{
    return dwModelID >= 1000 && dwModelID <= 1193;
}

void CStreamingSA::RequestModel(DWORD dwModelID, DWORD dwFlags)
{
    typedef void(__cdecl* RequestFn_t)(uint, int);
    if (IsUpgradeModelId(dwModelID))
        ((RequestFn_t)(FUNC_RequestVehicleUpgrade))(dwModelID, dwFlags);
    else
        ((RequestFn_t)(FUNC_CStreaming__RequestModel))(dwModelID, dwFlags);
}

void CStreamingSA::LoadAllRequestedModels(BOOL bOnlyPriorityModels, const char* szTag)
{
    TIMEUS startTime = GetTimeUs();

    typedef void(__fastcall* LoadAllRequestedModelsFn_t)(bool);
    ((LoadAllRequestedModelsFn_t)FUNC_LoadAllRequestedModels)(bOnlyPriorityModels);

    if (IS_TIMING_CHECKPOINTS())
    {
        uint deltaTimeMs = (GetTimeUs() - startTime) / 1000;
        if (deltaTimeMs > 2)
            TIMING_DETAIL(SString("LoadAllRequestedModels( %d, %s ) took %d ms", bOnlyPriorityModels, szTag, deltaTimeMs));
    }
}

BOOL CStreamingSA::HasModelLoaded(DWORD dwModelID)
{
    typedef bool(__cdecl* IsLoadedFn_t)(uint);
    if (IsUpgradeModelId(dwModelID))
        return ((IsLoadedFn_t)FUNC_CStreaming__HasVehicleUpgradeLoaded)(dwModelID);
    else
        return ((IsLoadedFn_t)FUNC_CStreaming__HasModelLoaded)(dwModelID);
}

void CStreamingSA::RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel)
{
    typedef void(__fastcall* RequestSpecialModelFn_t)(int, const char*, int);
    ((RequestSpecialModelFn_t)FUNC_CStreaming_RequestSpecialModel)(model, szTexture, channel);
}

CStreamingInfo* CStreamingSA::GetStreamingInfoFromModelId(ushort id)
{
    return &ms_aInfoForModel[id];
}

void CStreamingSA::ReinitStreaming()
{
    typedef int(__cdecl * ReInitStreamingFn_t)();
    ((ReInitStreamingFn_t)0x40E560)();
}
