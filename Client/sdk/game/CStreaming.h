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
    WORD  prevId;
    WORD  nextId;
    WORD  nextInImg;
    unsigned char flg;
    unsigned char archiveId;
    DWORD offsetInBlocks;
    DWORD sizeInBlocks;
    DWORD loadState;
};

class CStreaming
{
public:
    virtual void RequestModel(DWORD dwModelID, DWORD dwFlags) = 0;
    virtual void LoadAllRequestedModels(BOOL bOnlyPriorityModels = 0, const char* szTag = NULL) = 0;
    virtual BOOL HasModelLoaded(DWORD dwModelID) = 0;
    virtual void RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel) = 0;
    virtual unsigned char AddStreamHandler(const char* szFilePath) = 0;
    //virtual BOOL RemoveStreamHandler(DWORD dwStreamHandler) = 0;
    virtual CStreamingInfo* GetStreamingInfoFromModelId(unsigned short id) = 0;
    virtual bool SetModelStreamInfo(unsigned short id, unsigned char ucArchiveId, unsigned short usOffestInBlocks, unsigned short usSizeInBlocks) = 0;
};
