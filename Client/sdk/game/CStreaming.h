/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CStreaming.h
 *  PURPOSE:     Game streaming interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <stdint.h>

#define INVALID_ARCHIVE_ID 0xFF
#define INVALID_STREAM_ID 0xFF

struct CStreamingInfo
{
    uint16_t prevId = (uint16_t)-1;
    uint16_t nextId = (uint16_t)-1;
    uint16_t nextInImg = (uint16_t)-1;
    uint8_t  flg = 0u;
    uint8_t  archiveId = 0u;
    uint32_t offsetInBlocks = 0u;
    uint32_t sizeInBlocks = 0u;
    uint32_t loadState = 0u;
};
static_assert(sizeof(CStreamingInfo) == 0x14, "Invalid size for CStreamingInfo");

class CStreaming
{
public:
    virtual void            RequestModel(DWORD dwModelID, DWORD dwFlags) = 0;
    virtual void            RemoveModel(std::uint32_t model) = 0;
    virtual void            LoadAllRequestedModels(bool bOnlyPriorityModels = false, const char* szTag = NULL) = 0;
    virtual bool            HasModelLoaded(DWORD dwModelID) = 0;
    virtual void            RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel) = 0;
    virtual CStreamingInfo* GetStreamingInfo(uint32 id) = 0;
    virtual void            ReinitStreaming() = 0;
    virtual unsigned char   AddArchive(const wchar_t *szFilePath) = 0;
    virtual void            RemoveArchive(unsigned char ucArchiveID) = 0;
    virtual void   SetStreamingInfo(unsigned int id, unsigned char usStreamID, unsigned int uiOffset, unsigned short usSize, unsigned int uiNextInImg = -1) = 0;
    virtual bool   SetStreamingBufferSize(uint32 uiSize) = 0;
    virtual uint32 GetStreamingBufferSize() = 0;
    virtual void   MakeSpaceFor(std::uint32_t memoryToCleanInBytes) = 0;
    virtual std::uint32_t GetMemoryUsed() const = 0;
    virtual void          RemoveBigBuildings() = 0;
};
