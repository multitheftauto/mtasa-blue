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
#include "Fileapi.h"

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
    if (IsUpgradeModelId(dwModelID))
    {
        DWORD dwFunc = FUNC_RequestVehicleUpgrade;
        _asm
        {
            push    dwFlags
            push    dwModelID
            call    dwFunc
            add     esp, 8
        }
    }
    else
    {
        DWORD dwFunction = FUNC_CStreaming__RequestModel;
        _asm
        {
            push    dwFlags
            push    dwModelID
            call    dwFunction
            add     esp, 8
        }
    }
}

void CStreamingSA::LoadAllRequestedModels(BOOL bOnlyPriorityModels, const char* szTag)
{
    TIMEUS startTime = GetTimeUs();

    DWORD dwFunction = FUNC_LoadAllRequestedModels;
    DWORD dwOnlyPriorityModels = bOnlyPriorityModels;
    _asm
    {
        push    dwOnlyPriorityModels
        call    dwFunction
        add     esp, 4
    }

    if (IS_TIMING_CHECKPOINTS())
    {
        uint deltaTimeMs = (GetTimeUs() - startTime) / 1000;
        if (deltaTimeMs > 2)
            TIMING_DETAIL(SString("LoadAllRequestedModels( %d, %s ) took %d ms", bOnlyPriorityModels, szTag, deltaTimeMs));
    }
}

BOOL CStreamingSA::HasModelLoaded(DWORD dwModelID)
{
    if (IsUpgradeModelId(dwModelID))
    {
        bool  bReturn;
        DWORD dwFunc = FUNC_CStreaming__HasVehicleUpgradeLoaded;
        _asm
        {
            push    dwModelID
            call    dwFunc
            add     esp, 0x4
            mov     bReturn, al
        }
        return bReturn;
    }
    else
    {
        DWORD dwFunc = FUNC_CStreaming__HasModelLoaded;
        BOOL  bReturn = 0;
        _asm
        {
            push    dwModelID
            call    dwFunc
            movzx   eax, al
            mov     bReturn, eax
            pop     eax
        }

        return bReturn;
    }
}

void CStreamingSA::RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel)
{
    DWORD dwFunc = FUNC_CStreaming_RequestSpecialModel;
    _asm
    {
        push    channel
        push    szTexture
        push    model
        call    dwFunc
        add     esp, 0xC
    }
}

CStreamingInfo* CStreamingSA::GetStreamingInfoFromModelId(uint id)
{
    CStreamingInfo* pItemInfo = (CStreamingInfo*)(ARRAY_StreamModelInfo);
    return pItemInfo + id;
}

bool CStreamingSA::SetModelStreamInfo(ushort id, uchar ucArchiveId, ushort usOffestInBlocks, ushort usSizeInBlocks)
{
    CStreamingInfo* pItemInfo = GetStreamingInfoFromModelId(id);
    pItemInfo->archiveId = ucArchiveId;
    pItemInfo->offsetInBlocks = usOffestInBlocks;
    pItemInfo->sizeInBlocks = usSizeInBlocks;
    // TODO CHANGE THIS INFO FOR PREV MODEL
    pItemInfo->nextInImg = -1;
    return true;
}

CStreamHandlerInfo* CStreamingSA::GetStreamingHandlerInfo(uint id)
{
    CStreamHandlerInfo* pHandlerInfo = (CStreamHandlerInfo*)(ARRAY_StreamHandlersInfo);
    return pHandlerInfo + id;
}


unsigned char CStreamingSA::AddStreamHandler(const char* szFilePath)
{
    // Get internal IMG id
    // By default gta sa has 8 IMG archives
    uchar ucArchiveId = 0;
    for (int i = 0; i < 8; i++)
    {
        CStreamHandlerInfo* info = GetStreamingHandlerInfo(i);
        if (!info->uiStreamHandleId)
        {
            ucArchiveId = i;
            break;
        }
    }

    if (ucArchiveId == 0)
        return 0;

    // Get free stream handler id
    HANDLE hFile = INVALID_HANDLE_VALUE;
    unsigned char ucStreamID = 0;
    
    for (unsigned char ID = 0; ID < VAR_StreamHandlersMaxCount; ++ID)
    {
        HANDLE hFile = *(HANDLE*)(ARRAY_StreamHandlers + (ID * sizeof(HANDLE)));
        if (!hFile)
        {
            ucStreamID = ID;
            break;
        }
    };

    if (ucStreamID == 0)
        return 0;

    //  Create new stream handler
    DWORD  dOpenFlags = *(DWORD*)(VAR_StreamHandlerCreateFlags) | FILE_ATTRIBUTE_READONLY | FILE_FLAG_RANDOM_ACCESS;

    hFile = CreateFileA(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, dOpenFlags, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return 0;

    // Register stream handler
    void* streamHandlerPos = (void*)(ARRAY_StreamHandlers + ucStreamID * sizeof(HANDLE));
    MemPutFast(streamHandlerPos, hFile);

    // Register archive data
    CStreamHandlerInfo* pNewArchiveInfo = GetStreamingHandlerInfo(ucArchiveId);
    pNewArchiveInfo->uiStreamHandleId = (ucStreamID << 24);

    return ucArchiveId;
}
