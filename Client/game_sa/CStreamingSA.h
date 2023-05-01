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

#define VAR_StreamHandlersMaxCount                   32
#define VAR_MaxArchives                              8

#define FUNC_CStreaming__RequestModel                0x4087E0
#define FUNC_LoadAllRequestedModels                  0x40EA10
#define FUNC_CStreaming__HasVehicleUpgradeLoaded     0x407820
#define FUNC_CStreaming_RequestSpecialModel          0x409d10

struct CArchiveInfo
{
    char  szName[40];
    BYTE  bUnknow = 1;            // Only in player.img is 0. Maybe, it is DWORD value
    BYTE  bUnused[3];
    DWORD uiStreamHandleId;
};

struct SGtaStream
{
    uint32_t nSectorsOffset;
    uint32_t nSectorsToRead;
    void*    pBuffer;
    uint8_t  bUnknow1;
    uint8_t  bLocked;
    uint8_t  bInUse;
    uint8_t  bUnknow2;
    uint32_t uiStatus;
    uint32_t handle;
    uint32_t file;
    uint8_t  pad[20];
};
static_assert(sizeof(SGtaStream) == 0x30, "Invalid size for SGtaStream");

class CStreamingSA final : public CStreaming
{
private:
    static CArchiveInfo* GetArchiveInfo(uint id) { return &ms_aAchiveInfo[id]; };

public:
    void RequestModel(DWORD dwModelID, DWORD dwFlags);
    void RemoveModel(std::uint32_t model) override;
    void LoadAllRequestedModels(bool bOnlyPriorityModels = false, const char* szTag = NULL);
    bool HasModelLoaded(DWORD dwModelID);
    void RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel);
    void ReinitStreaming();

    CStreamingInfo* GetStreamingInfo(uint32 id);
    void            SetStreamingInfo(uint32 modelid, unsigned char usStreamID, uint uiOffset, ushort usSize, uint uiNextInImg = -1);
    unsigned char   GetUnusedArchive();
    unsigned char   GetUnusedStreamHandle();
    unsigned char   AddArchive(const char* szFilePath);
    void            RemoveArchive(unsigned char ucStreamHandler);
    void            SetStreamingBufferSize(uint32 uiSize);
    uint32          GetStreamingBufferSize() { return ms_streamingHalfOfBufferSize * 2; };

    void          MakeSpaceFor(std::uint32_t memoryToCleanInBytes) override;
    std::uint32_t GetMemoryUsed() const override;

private:
    static void* (&ms_pStreamingBuffer)[2];
    static uint32(&ms_streamingHalfOfBufferSize);
    static CStreamingInfo (&ms_aInfoForModel)[26316];            // count: 26316 in unmodified game
    static HANDLE (&m_aStreamingHandlers)[32];
    static CArchiveInfo (&ms_aAchiveInfo)[8];
};
