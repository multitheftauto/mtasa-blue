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
#include <core/CCoreInterface.h>
#include "CStreamingSA.h"
#include "CModelInfoSA.h"
#include "Fileapi.h"
#include "processthreadsapi.h"

extern CCoreInterface* g_pCore;

// count: 26316 in unmodified game
CStreamingInfo (&CStreamingSA::ms_aInfoForModel)[26316] = *(CStreamingInfo(*)[26316])0x8E4CC0;
HANDLE (&CStreamingSA::m_aStreamingHandlers)[32] = *(HANDLE(*)[32])0x8E4010; // Contains open files
CArchiveInfo (&CStreamingSA::ms_aAchiveInfo)[8] = *(CArchiveInfo(*)[8])0x8E48D8; // [8][0x30]
HANDLE* phStreamingThread = (HANDLE*)0x8E4008;
uint32  (&CStreamingSA::ms_streamingHalfOfBufferSize) = *(uint32*)0x8E4CA8;
void* (&CStreamingSA::ms_pStreamingBuffer)[2] = *(void*(*)[2])0x8E4CAC;

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

void CStreamingSA::RemoveModel(std::uint32_t model)
{
    using Signature = void(__cdecl*)(std::uint32_t);
    const auto function = reinterpret_cast<Signature>(0x4089A0);
    function(model);
}

void CStreamingSA::LoadAllRequestedModels(bool bOnlyPriorityModels, const char* szTag)
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

bool CStreamingSA::HasModelLoaded(DWORD dwModelID)
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
        bool bReturn = 0;
        _asm
        {
            push    dwModelID
            call    dwFunc
            mov     bReturn, al
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

void CStreamingSA::ReinitStreaming()
{
    typedef int(__cdecl * Function_ReInitStreaming)();
    ((Function_ReInitStreaming)(0x40E560))();
}

// ReinitStreaming should be called after this.
// Otherwise the model wont be restreamed
// TODO: Somehow restream a single model instead of the whole world
void CStreamingSA::SetStreamingInfo(uint modelid, unsigned char usStreamID, uint uiOffset, ushort usSize, uint uiNextInImg)
{
    CStreamingInfo* pItemInfo = GetStreamingInfo(modelid);

    // Change nextInImg field for prev model
    for (CStreamingInfo& info : ms_aInfoForModel)
    {
        if (info.archiveId == pItemInfo->archiveId)
        {
            // Check if the block after `info` is the beginning of `pItemInfo`'s block
            if (info.offsetInBlocks + info.sizeInBlocks == pItemInfo->offsetInBlocks)
            {
                info.nextInImg = -1;
                break;
            }
        }
    }

    pItemInfo->archiveId = usStreamID;
    pItemInfo->offsetInBlocks = uiOffset;
    pItemInfo->sizeInBlocks = usSize;
    pItemInfo->nextInImg = uiNextInImg;
}

CStreamingInfo* CStreamingSA::GetStreamingInfo(uint modelid)
{
    return &ms_aInfoForModel[modelid];
}

unsigned char CStreamingSA::GetUnusedArchive()
{
    // Get internal IMG id
    // By default gta sa uses 6 of 8 IMG archives
    for (size_t i = 6; i < 8; i++)
    {
        if (!GetArchiveInfo(i)->uiStreamHandleId)
            return (unsigned char)i;
    }
    return -1;
}

unsigned char CStreamingSA::GetUnusedStreamHandle()
{
    for (size_t i = 0; i < VAR_StreamHandlersMaxCount; i++)
    {
        if (m_aStreamingHandlers[i])
            return (unsigned char)i;
    }
    return -1;
}

unsigned char CStreamingSA::AddArchive(const char* szFilePath)
{
    const auto ucArchiveId = GetUnusedArchive();
    if (ucArchiveId == -1)
        return -1;

    // Get free stream handler id
    const auto ucStreamID = GetUnusedStreamHandle();
    if (ucStreamID == -1)
        return -1;

    // Create new stream handler
    const auto streamCreateFlags = *(DWORD*)0x8E3FE0;
    HANDLE hFile = CreateFileA(
        szFilePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        streamCreateFlags | FILE_ATTRIBUTE_READONLY | FILE_FLAG_RANDOM_ACCESS,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE)
        return -1;

    // Register stream handler
    m_aStreamingHandlers[ucStreamID] = hFile;

    // Register archive data
    ms_aAchiveInfo[ucArchiveId].uiStreamHandleId = (ucStreamID << 24);

    return ucArchiveId;
}

void CStreamingSA::RemoveArchive(unsigned char ucArhiveID)
{
    unsigned int uiStreamHandlerID = ms_aAchiveInfo[ucArhiveID].uiStreamHandleId >> 24;
    if (!uiStreamHandlerID)
        return;

    ms_aAchiveInfo[ucArhiveID].uiStreamHandleId = 0;

    CloseHandle(m_aStreamingHandlers[uiStreamHandlerID]);
    m_aStreamingHandlers[uiStreamHandlerID] = NULL;
}

void CStreamingSA::SetStreamingBufferSize(uint32 uiBlockSize)
{
    if (uiBlockSize == ms_streamingHalfOfBufferSize * 2)
        return;

    int pointer = *(int*)0x8E3FFC;
    SGtaStream(&streaming)[5] = *(SGtaStream(*)[5])(pointer);

    // Wait while streaming threads ends tasks
    while (streaming[0].bInUse && streaming[1].bInUse)

    // Suspend streaming handle
    SuspendThread(*phStreamingThread);

    // Create new buffer
    if (uiBlockSize & 1)
        uiBlockSize++;

    typedef void*(__cdecl * Function_CMemoryMgr_MallocAlign)(uint32 uiCount, uint32 uiAlign);
    void* pNewBuffer = ((Function_CMemoryMgr_MallocAlign)(0x72F4C0))(uiBlockSize << 11, 2048);

    // Copy data from old buffer to new buffer
    uint uiCopySize = std::min(ms_streamingHalfOfBufferSize, uiBlockSize / 2);
    MemCpyFast(pNewBuffer, (void*)ms_pStreamingBuffer[0], uiCopySize);
    MemCpyFast((void*)(reinterpret_cast<int>(pNewBuffer) + 1024 * uiBlockSize), (void*)ms_pStreamingBuffer[1], uiCopySize);

    typedef void(__cdecl * Function_CMemoryMgr_FreeAlign)(void* pos);
    ((Function_CMemoryMgr_FreeAlign)(0x72F4F0))(ms_pStreamingBuffer[0]);

    ms_streamingHalfOfBufferSize = uiBlockSize / 2;

    ms_pStreamingBuffer[0] = pNewBuffer;
    ms_pStreamingBuffer[1] = (void*)(reinterpret_cast<int>(pNewBuffer) + 1024 * uiBlockSize);

    streaming[0].pBuffer = ms_pStreamingBuffer[0];
    streaming[1].pBuffer = ms_pStreamingBuffer[1];

    // Well done
    ResumeThread(*phStreamingThread);
}

void CStreamingSA::MakeSpaceFor(std::uint32_t memoryToCleanInBytes)
{
    (reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x40E120))(memoryToCleanInBytes);
}

std::uint32_t CStreamingSA::GetMemoryUsed() const
{
    return *reinterpret_cast<std::uint32_t*>(0x8E4CB4);
}
