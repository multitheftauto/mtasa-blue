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
    uint16_t prevId = -1;
    uint16_t nextId = -1;
    uint16_t nextInImg = -1;
    uint8_t  flg = 0;
    uint8_t  archiveId = 0;
    uint32_t offsetInBlocks = 0;
    uint32_t sizeInBlocks = 0;
    uint32_t loadState = 0;
};
static_assert(sizeof(CStreamingInfo) == 0x14, "Invalid size for CStreamingInfo");

class CStreaming
{
public:
    virtual void RequestModel(DWORD dwModelID, DWORD dwFlags) = 0;
    virtual void LoadAllRequestedModels(BOOL bOnlyPriorityModels = 0, const char* szTag = NULL) = 0;
    virtual BOOL HasModelLoaded(DWORD dwModelID) = 0;
    virtual void RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel) = 0;
    virtual CStreamingInfo* GetStreamingInfo(unsigned int uiID) = 0;
    virtual void ReinitStreaming() = 0;
    virtual unsigned char AddArchive(const char* szFilePath) = 0;
    virtual void RemoveArchive(unsigned char ucArchiveID) = 0;
    virtual void SetStreamingInfo(unsigned int id, unsigned char usStreamID, unsigned int uiOffset, unsigned short usSize, unsigned int uiNextInImg = -1) = 0;
};
