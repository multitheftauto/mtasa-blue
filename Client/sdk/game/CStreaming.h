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

struct CStreamingInfo
{
    uint16_t prevId;
    uint16_t nextId;
    uint16_t nextInImg;
    uint8_t  flg;
    uint8_t  archiveId;
    uint32_t offsetInBlocks;
    uint32_t sizeInBlocks;
    uint32_t loadState;

public:
    void Reset()
    {
        this->loadState = 0;
        this->nextInImg = -1;
        this->nextId = -1;
        this->prevId = -1;
        this->archiveId = 0;
        this->flg = 0;
        this->offsetInBlocks = 0;
        this->sizeInBlocks = 0;
    };
};
static_assert(sizeof(CStreamingInfo) == 0x14, "Invalid size for CStreamingInfo");

class CStreaming
{
public:
    virtual void            RequestModel(DWORD dwModelID, DWORD dwFlags) = 0;
    virtual void            RemoveModel(std::uint32_t model) = 0;
    virtual void            LoadAllRequestedModels(BOOL bOnlyPriorityModels = 0, const char* szTag = NULL) = 0;
    virtual BOOL            HasModelLoaded(DWORD dwModelID) = 0;
    virtual void            RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel) = 0;
    virtual CStreamingInfo* GetStreamingInfoFromModelId(uint32 id) = 0;
    virtual void            ReinitStreaming() = 0;
    virtual void            MakeSpaceFor(std::uint32_t memoryToCleanInBytes) = 0;
    virtual std::uint32_t   GetMemoryUsed() const = 0;
};
