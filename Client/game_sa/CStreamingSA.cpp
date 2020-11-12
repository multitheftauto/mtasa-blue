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

CStreamingInfo (&CStreamingSA::ms_aInfoForModel)[26316] = *(CStreamingInfo(*)[26316])0x8E4CC0;
HANDLE (&CStreamingSA::m_aStreamingHandlers)[32] = *(HANDLE(*)[32])0x8E4010; // Contains open files
CArchiveInfo (&CStreamingSA::ms_aAchiveInfo)[8] = *(CArchiveInfo(*)[8])0x8E48D8; // [8][0x30]

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

void CStreamingSA::ReinitStreaming()
{
    typedef int(__cdecl * Function_ReInitStreaming)();
    Function_ReInitStreaming reinitStreaming = (Function_ReInitStreaming)(0x40E560);

    reinitStreaming();
}

void CStreamingSA::SetStreamingInfoForModelId(uint id, unsigned char usStreamID, uint uiOffset, ushort usSize, uint uiNextInImg)
{
    CStreamingInfo* pItemInfo = GetStreamingInfoFromModelId(id);

    // Change nextInImg filed for prev model
    for ( unsigned int i = 0; i < 26316; i++)
    {
        if (ms_aInfoForModel[i].archiveId == pItemInfo->archiveId &&
            (ms_aInfoForModel[i].offsetInBlocks + ms_aInfoForModel[i].sizeInBlocks) == pItemInfo->offsetInBlocks)
        {
            ms_aInfoForModel[i].nextInImg = -1;
            break;
        }
    }

    pItemInfo->archiveId = usStreamID;
    pItemInfo->offsetInBlocks = uiOffset;
    pItemInfo->sizeInBlocks = usSize;
    pItemInfo->nextInImg = uiNextInImg;
}

CStreamingInfo* CStreamingSA::GetStreamingInfoFromModelId(uint id)
{
    return &ms_aInfoForModel[id];
}

unsigned char CStreamingSA::AddArchive(const char* szFilePath)
{
    // Get internal IMG id
    // By default gta sa uses 6 of 8 IMG archives
    uchar ucArchiveId = -1;
    for (int i = 6; i < 8; i++)
    {
        CArchiveInfo* info = GetArchiveInfo(i);
        if (!info->uiStreamHandleId)
        {
            ucArchiveId = i;
            break;
        }
    }

    if (ucArchiveId == -1)
        return -1;

    // Get free stream handler id
    HANDLE hFile = INVALID_HANDLE_VALUE;
    unsigned char ucStreamID = 0;
    
    for (unsigned char ID = 0; ID < VAR_StreamHandlersMaxCount; ++ID)
    {
        HANDLE hFile = m_aStreamingHandlers[ID];
        if (!hFile)
        {
            ucStreamID = ID;
            break;
        }
    };

    if (ucStreamID == 0)
        return -1;

    //  Create new stream handler
    DWORD  dOpenFlags = *(DWORD*)(VAR_StreamHandlerCreateFlags) | FILE_ATTRIBUTE_READONLY | FILE_FLAG_RANDOM_ACCESS;

    hFile = CreateFileA(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, dOpenFlags, NULL);

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
