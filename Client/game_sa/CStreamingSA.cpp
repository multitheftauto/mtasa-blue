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
HANDLE* phStreamingThread = (HANDLE*)0x8E4008;
uint32(&CStreamingSA::ms_streamingHalfOfBufferSizeBlocks) = *(uint32*)0x8E4CA8;
void* (&CStreamingSA::ms_pStreamingBuffer)[2] = *(void* (*)[2])0x8E4CAC;

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

    constexpr size_t RESERVED_STREAMS_NUM = 10; // GTA3 + 9 SFX archives(FEET, GENRL, PAIN_A, SCRIPT, SPC_EA, SPC_FA, SPC_GA, SPC_NA, SPC_PA)
    constexpr size_t MAX_STREAMS_NUM = 255;
    constexpr size_t MAX_IMAGES_NUM = MAX_STREAMS_NUM - RESERVED_STREAMS_NUM;
    constexpr size_t MIN_IMAGES_NUM = 6; // GTA3(yes, it is presented twice), GTA_INT, CARREC, SCRIPT, CUTSCENE, PLAYER
} // namespace

bool IsUpgradeModelId(DWORD dwModelID)
{
    return dwModelID >= 1000 && dwModelID <= 1193;
}

CStreamingSA::CStreamingSA()
{
    // Allocate the default number of archives in order to keep modded games working as before.
    SetArchivesNum(VAR_DefaultMaxArchives);

    // Copy the default data
    HANDLE (&defaultStreamingHandlers)[32] = *(HANDLE(*)[32])0x8E4010;
    SStreamName (&defaultStreamingNames)[32] = *(SStreamName(*)[32])0x8E4098;
    CArchiveInfo (&defaultAchiveInfo)[8] = *(CArchiveInfo(*)[8])0x8E48D8;

    std::memcpy(m_StreamHandles.data(), defaultStreamingHandlers,
        sizeof(HANDLE) * std::min(m_StreamHandles.size(), (size_t)VAR_DefaultStreamHandlersMaxCount));
    std::memcpy(m_StreamNames.data(), defaultStreamingNames,
        sizeof(SStreamName) *  std::min(m_StreamNames.size(), (size_t)VAR_DefaultStreamHandlersMaxCount));
    std::memcpy(m_Imgs.data(), defaultAchiveInfo,
        sizeof(CArchiveInfo) * std::min(m_Imgs.size(), (size_t)VAR_DefaultMaxArchives));
}

void CStreamingSA::SetArchivesNum(size_t imagesNum)
{
    if (imagesNum < MIN_IMAGES_NUM || imagesNum > MAX_IMAGES_NUM)
        return;

    /*
        IMGs
    */
    if (m_Imgs.size() != imagesNum)
    {
        try
        {
            m_Imgs.resize(imagesNum);
        }
        catch (const std::bad_alloc&)
        {
            return;
        }

        const auto pImgs = m_Imgs.data();
        const size_t uiImgsSize = sizeof(CArchiveInfo) * m_Imgs.size();

        // CStreaming::AddImageToList
        MemPutFast<DWORD>((void*)0x1567B94, (DWORD)pImgs);
        MemPutFast<DWORD>((void*)0x1567BA2, (DWORD)pImgs + uiImgsSize);
        MemPutFast<DWORD>((void*)0x1567BBA, (DWORD)pImgs);
        MemPutFast<DWORD>((void*)0x1567BD6, (DWORD)pImgs + 0x2C);
        MemPutFast<DWORD>((void*)0x1567BE3, (DWORD)pImgs + 0x28);

        // CStreaming::InitImageList
        MemPut<DWORD>((void*)0x4083C1, (DWORD)pImgs + 0x2C);
        MemPut<DWORD>((void*)0x4083DE, (DWORD)pImgs + 0x2C + uiImgsSize);
        MemPut<DWORD>((void*)0x4083E9, (DWORD)pImgs);
        MemPut<DWORD>((void*)0x4083FA, (DWORD)pImgs + uiImgsSize);
        MemPut<DWORD>((void*)0x40840B, (DWORD)pImgs);
        MemPut<DWORD>((void*)0x40841A, (DWORD)pImgs + uiImgsSize);
        MemPut<DWORD>((void*)0x40843B, (DWORD)pImgs);
        MemPut<DWORD>((void*)0x40845B, (DWORD)pImgs + 0x2C);
        MemPut<DWORD>((void*)0x408461, (DWORD)pImgs + 0x28);
        MemPut<DWORD>((void*)0x408479, (DWORD)pImgs);
        MemPut<DWORD>((void*)0x4084A2, (DWORD)pImgs + 0x2C);
        MemPut<DWORD>((void*)0x4084A8, (DWORD)pImgs + 0x28);

        // CStreaming::loadArchives
        MemPut<DWORD>((void*)0x5B82F1, (DWORD)pImgs);
        MemPut<DWORD>((void*)0x5B82FD, (DWORD)pImgs);
        MemPut<DWORD>((void*)0x5B8303, (DWORD)pImgs + uiImgsSize);

        // CStreamingInfo::GetCdPosn
        MemPut<DWORD>((void*)0x40757F, (DWORD)pImgs + 0x2C);

        // CStreaming::GetNextFileOnCd
        MemPut<DWORD>((void*)0x408FDC, (DWORD)pImgs + 0x2C);

        // CStreaming::requestSpecialModel
        MemPut<DWORD>((void*)0x409D5A, (DWORD)pImgs + 0x2C);

        // CStreaming::RequestModelStream
        MemPut<DWORD>((void*)0x40CC54, (DWORD)pImgs + 0x2C);
        MemPut<DWORD>((void*)0x40CCC7, (DWORD)pImgs + 0x2C);

        // CStreamingInfo::GetCdPosnAndSize
        MemPutFast<DWORD>((void*)0x1560E68, (DWORD)pImgs + 0x2C);

        // sub_40A080
        MemPutFast<DWORD>((void*)0x15663E7, (DWORD)pImgs + 0x2C);
    }

    /*
        Stream handles
    */
    const size_t handlesNum = Clamp((size_t)VAR_DefaultStreamHandlersMaxCount, imagesNum + RESERVED_STREAMS_NUM, MAX_STREAMS_NUM);
    if (m_StreamHandles.size() != handlesNum)
    {
        try
        {
            m_StreamHandles.resize(handlesNum);
            m_StreamNames.resize(handlesNum);
        }
        catch (const std::bad_alloc&)
        {
            return;
        }

        const auto pStreamHandles = m_StreamHandles.data();
        const auto pStreamNames = m_StreamNames.data();

        // _GetFileSizeOfTheFirstStream
        MemPutFast<DWORD>((void*)0x15700D1, (DWORD)pStreamHandles);

        // _closeAllStreams
        MemPut<DWORD>((void*)0x4066C7, (DWORD)pStreamNames);
        MemPut<DWORD>((void*)0x4066D6, (DWORD)pStreamHandles);
        MemPut<DWORD>((void*)0x4066ED, (DWORD)pStreamHandles);

        // _sub_406710
        MemPut<DWORD>((void*)0x406737, (DWORD)pStreamHandles);

        // _sub_406750
        MemPut<DWORD>((void*)0x40676C, (DWORD)pStreamNames);
        MemPut<DWORD>((void*)0x406797, (DWORD)pStreamHandles);

        // _openStream
        DWORD dwExeCodePtr = (DWORD)0x01564A94;

        MemPutFast<WORD>((void*)(dwExeCodePtr), (WORD)0x048B); // mov     eax, _streamHandles[esi*4]
        MemPutFast<BYTE>((void*)(dwExeCodePtr + 2), (BYTE)0xB5);
        MemPutFast<DWORD>((void*)(dwExeCodePtr + 3), (DWORD)pStreamHandles);

        MemPutFast<WORD>((void*)(dwExeCodePtr + 7), (WORD)0xC085);

        MemPutFast<WORD>((void*)(dwExeCodePtr + 9), (WORD)0x840F);
        MemPutFast<DWORD>((void*)(dwExeCodePtr + 11), (DWORD)(0x01564B31 - (dwExeCodePtr+15)));

        MemPutFast<BYTE>((void*)(dwExeCodePtr + 15), (BYTE)0x46);
        MemPutFast<WORD>((void*)(dwExeCodePtr + 16), (WORD)0xFE81);
        MemPutFast<DWORD>((void*)(dwExeCodePtr + 18), (DWORD)(handlesNum - 1)); // MAX_NUMBER_OF_STREAM_HANDLES

        MemPutFast<BYTE>((void*)(dwExeCodePtr + 22), (BYTE)0x7C);
        MemPutFast<BYTE>((void*)(dwExeCodePtr + 23), (BYTE)(dwExeCodePtr - (dwExeCodePtr+24)));

        MemPutFast<BYTE>((void*)(dwExeCodePtr + 24), (BYTE)0xE9);
        MemPutFast<DWORD>((void*)(dwExeCodePtr + 25), (DWORD)(0x01564B31 - (dwExeCodePtr+29)));
        // end of loop creation

        MemPutFast<DWORD>((void*)0x1564B74, (DWORD)pStreamHandles);
        MemPutFast<DWORD>((void*)0x1564B8C, (DWORD)pStreamNames);

        // _sub_4068A0
        MemPut<DWORD>((void*)0x4068AB, (DWORD)pStreamHandles);
        MemPut<DWORD>((void*)0x4068C2, (DWORD)pStreamHandles);
        MemPut<DWORD>((void*)0x4068D0, (DWORD)pStreamHandles);
        MemPut<DWORD>((void*)0x4068DD, (DWORD)pStreamNames);

        // _readStream
        MemPutFast<DWORD>((void*)0x156C2E8, (DWORD)pStreamHandles);

        // _initStreaming
        MemPut<DWORD>((void*)0x406B7C, (DWORD)pStreamHandles);
        MemPut<DWORD>((void*)0x406B81, (DWORD)pStreamNames);
        MemPut<DWORD>((void*)0x406B98, (DWORD)(pStreamNames + sizeof(SStreamName) * (handlesNum - 1)));
    }
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
        bool  bReturn = 0;
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
    for (size_t i = 6; i < m_Imgs.size(); i++)
    {
        if (!m_Imgs[i].uiStreamHandleId)
            return (unsigned char)i;
    }
    return INVALID_ARCHIVE_ID;
}

unsigned char CStreamingSA::GetUnusedStreamHandle()
{
    for (size_t i = 0; i < m_StreamHandles.size(); i++)
    {
        if (!m_StreamHandles[i])
            return (unsigned char)i;
    }
    return INVALID_STREAM_ID;
}

unsigned char CStreamingSA::AddArchive(const wchar_t* szFilePath)
{
    auto ucArchiveId = GetUnusedArchive();
    if (ucArchiveId == INVALID_ARCHIVE_ID)
    {
        // Allocate some extra archives
        AllocateArchive();

        // Give it a second try
        ucArchiveId = GetUnusedArchive();
        if (ucArchiveId == INVALID_ARCHIVE_ID)
            return INVALID_ARCHIVE_ID;
    }

    // Get free stream handler id
    const auto ucStreamID = GetUnusedStreamHandle();
    if (ucStreamID == INVALID_STREAM_ID)
        return INVALID_ARCHIVE_ID;

    // Create new stream handler
    const auto streamCreateFlags = *(DWORD*)0x8E3FE0;
    HANDLE     hFile = CreateFileW(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                               streamCreateFlags | FILE_ATTRIBUTE_READONLY | FILE_FLAG_RANDOM_ACCESS, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return INVALID_ARCHIVE_ID;

    // Register stream handler
    m_StreamHandles[ucStreamID] = hFile;

    // Register archive data
    m_Imgs[ucArchiveId].uiStreamHandleId = (ucStreamID << 24);

    return ucArchiveId;
}

void CStreamingSA::RemoveArchive(unsigned char ucArchiveID)
{
    unsigned int uiStreamHandlerID = m_Imgs[ucArchiveID].uiStreamHandleId >> 24;
    if (!uiStreamHandlerID)
        return;

    m_Imgs[ucArchiveID].uiStreamHandleId = 0;

    CloseHandle(m_StreamHandles[uiStreamHandlerID]);
    m_StreamHandles[uiStreamHandlerID] = NULL;
}

bool CStreamingSA::SetStreamingBufferSize(uint32 numBlocks)
{
    numBlocks += numBlocks % 2; // Make sure number is even by "rounding" it upwards. [Otherwise it can't be split in half properly]
    
    // Check if the size is the same already
    if (numBlocks == ms_streamingHalfOfBufferSizeBlocks * 2)
        return true;

    if (ms_pStreamingBuffer[0] == nullptr || ms_pStreamingBuffer[1] == nullptr)
        return false;

    // First of all, allocate the new buffer
    // NOTE: Due to a bug in the `MallocAlign` code the function will just *crash* instead of returning nullptr on alloc. failure :D
    typedef void*(__cdecl * Function_CMemoryMgr_MallocAlign)(uint32 uiCount, uint32 uiAlign);
    void* pNewBuffer = ((Function_CMemoryMgr_MallocAlign)(0x72F4C0))(numBlocks * 2048, 2048);
    if (!pNewBuffer) // ...so this code is useless for now
        return false;

    int pointer = *(int*)0x8E3FFC;
    SGtaStream(&streaming)[5] = *(SGtaStream(*)[5])(pointer);

    // Wait while streaming thread ends tasks
    while (streaming[0].bInUse || streaming[1].bInUse);

    // Suspend streaming thread [otherwise data might become corrupted]
    SuspendThread(*phStreamingThread);

    // Calculate new buffer pointers
    void* const pNewBuff0 = pNewBuffer;
    void* const pNewBuff1 = (void*)(reinterpret_cast<uintptr_t>(pNewBuffer) + 2048u * (numBlocks / 2));

    // Copy data from old buffer to new buffer
    const auto copySizeBytes = std::min(ms_streamingHalfOfBufferSizeBlocks, numBlocks / 2) * 2048;
    MemCpyFast(pNewBuff0, ms_pStreamingBuffer[0], copySizeBytes);
    MemCpyFast(pNewBuff1, ms_pStreamingBuffer[1], copySizeBytes);

    // Now, we can deallocate the old buffer safely
    typedef void(__cdecl * Function_CMemoryMgr_FreeAlign)(void* pos);
    ((Function_CMemoryMgr_FreeAlign)(0x72F4F0))(ms_pStreamingBuffer[0]);

    // Update the buffer size now
    ms_streamingHalfOfBufferSizeBlocks = numBlocks / 2;

    // Update internal pointers too
    streaming[0].pBuffer = ms_pStreamingBuffer[0] = pNewBuff0;
    streaming[1].pBuffer = ms_pStreamingBuffer[1] = pNewBuff1;

    // Now we can resume streaming
    ResumeThread(*phStreamingThread);

    return true;
}

void CStreamingSA::MakeSpaceFor(std::uint32_t memoryToCleanInBytes)
{
    (reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x40E120))(memoryToCleanInBytes);
}

std::uint32_t CStreamingSA::GetMemoryUsed() const
{
    return *reinterpret_cast<std::uint32_t*>(0x8E4CB4);
}

void CStreamingSA::AllocateArchive()
{
    // Preallocate some extra archives to skip this procedure for the next several archives
    const size_t archivesNum = std::min(m_Imgs.size() + m_Imgs.size() * 2 + 1, MAX_IMAGES_NUM);
    SetArchivesNum(archivesNum);
}

void CStreamingSA::RemoveBigBuildings()
{
    (reinterpret_cast<void(__cdecl*)()>(0x4093B0))();
}
