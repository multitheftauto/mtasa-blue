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

struct CStreamingInfo
{
    uint16 prevId;
    uint16 nextId;
    uint16 nextInImg;
    uint8  flg;
    uint8  archiveId;
    uint32 offsetInBlocks;
    uint32 sizeInBlocks;
    uint32 loadState;

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

class CStreaming
{
public:
    virtual void RequestModel(DWORD dwModelID, DWORD dwFlags) = 0;
    virtual void LoadAllRequestedModels(BOOL bOnlyPriorityModels = 0, const char* szTag = NULL) = 0;
    virtual BOOL HasModelLoaded(DWORD dwModelID) = 0;
    virtual void RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel) = 0;
    virtual CStreamingInfo* GetStreamingInfoFromModelId(unsigned int uiID) = 0;
    virtual void ReinitStreaming() = 0;
    virtual unsigned char AddArchive(const char* szFilePath) = 0;
    virtual void RemoveArchive(unsigned char ucArchiveID) = 0;
    virtual void SetStreamingInfoForModelId(unsigned int id, unsigned char usStreamID, unsigned int uiOffset, unsigned short usSize, unsigned int uiNextInImg = -1) = 0;
};
