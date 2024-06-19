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

#define VAR_DefaultStreamHandlersMaxCount                   32
#define VAR_DefaultMaxArchives                              8

#define FUNC_CStreaming__RequestModel                0x4087E0
#define FUNC_LoadAllRequestedModels                  0x40EA10
#define FUNC_CStreaming__HasVehicleUpgradeLoaded     0x407820
#define FUNC_CStreaming_RequestSpecialModel          0x409d10

struct CArchiveInfo
{
    char  szName[40];
    BYTE  bUnknow = 1;            // Only in player.img is 0. Maybe, it is DWORD value
    BYTE  bUnused[3];
    DWORD uiStreamHandleId{};
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

struct SStreamName
{
    char szName[64];
};

class CStreamingSA final : public CStreaming
{
public:
    CStreamingSA();

    void SetArchivesNum(size_t imagesNum);

    void RequestModel(DWORD dwModelID, DWORD dwFlags);
    void RemoveModel(std::uint32_t model) override;
    void LoadAllRequestedModels(bool bOnlyPriorityModels = false, const char* szTag = NULL);
    bool HasModelLoaded(DWORD dwModelID);
    void RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel);
    void ReinitStreaming();
    void RemoveBigBuildings() override;

    CStreamingInfo* GetStreamingInfo(uint32 id);
    void            SetStreamingInfo(uint32 modelid, unsigned char usStreamID, uint uiOffset, ushort usSize, uint uiNextInImg = -1);
    unsigned char   GetUnusedArchive();
    unsigned char   GetUnusedStreamHandle();
    unsigned char   AddArchive(const wchar_t* szFilePath);
    void            RemoveArchive(unsigned char ucStreamHandler);
    bool            SetStreamingBufferSize(uint32 uiSize);
    uint32          GetStreamingBufferSize() { return ms_streamingHalfOfBufferSizeBlocks * 2048 * 2; }; // In bytes

    void          MakeSpaceFor(std::uint32_t memoryToCleanInBytes) override;
    std::uint32_t GetMemoryUsed() const override;

private:
    void AllocateArchive();

    std::vector<CArchiveInfo> m_Imgs;
    std::vector<HANDLE> m_StreamHandles;
    std::vector<SStreamName> m_StreamNames;

    static void* (&ms_pStreamingBuffer)[2];
    static uint32(&ms_streamingHalfOfBufferSizeBlocks);
    static CStreamingInfo (&ms_aInfoForModel)[26316];            // count: 26316 in unmodified game
};
