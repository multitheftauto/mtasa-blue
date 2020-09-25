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
#include "Common.h"
#include "Fileapi.h"

// Shoud be changed to interfaces
#define ARRAY_StreamHandlers                         0x8E4010 // [32]
#define ARRAY_StreamHandlersNames                    0x8E4098 // [32][64]
#define ARRAY_StreamHandlersInfo                     0x8E48D8 // [8][0x30]

#define ARRAY_StreamModelInfo                        0x8E4CC0 // size = 26316

#define VAR_StreamHandlersMaxCount                   32
#define VAR_StreamHandlerCreateFlags                 0x8E3FE0
#define VAR_StreamHandlersMaxCount                   32

#define FUNC_CStreaming__RequestModel                0x4087E0
#define FUNC_LoadAllRequestedModels                  0x40EA10
#define FUNC_CStreaming__HasVehicleUpgradeLoaded     0x407820
#define FUNC_CStreaming_RequestAnimations            0x407120
#define FUNC_CStreaming_RequestSpecialModel          0x409d10

#define FUNC_CStreaming_RequestFile                  0x406A20 // (DWORD streamNum, int lpBuffer, int streamIndex, int sectorCount)

#define FUNC_CStreaming_RemoveImages                 0x4066B3 // Remove all IMG`s from streaming
#define FUNC_CStreaming_RemoveImage                  0x4068A0

struct CStreamingInfo
{
    WORD prevId;
    WORD nextId;
    WORD  nextInImg;
    uchar flg;
    uchar archiveId;
    DWORD offsetInBlocks;
    DWORD sizeInBlocks;
    DWORD loadState;
};

struct CStreamHandlerInfo
{
    char szName[40];
    BYTE bUnknow = 1; // Only in player.img is 0. Maybe, it is DWORD value
    BYTE bUnused[3];
    DWORD uiStreamHandleId;
};


class CStreamingSA : public CStreaming
{
private:
    CStreamHandlerInfo* GetStreamingHandlerInfo(uint id);
public:
    bool SetModelStreamInfo(ushort id, uchar ucArchiveId, ushort usOffestInBlocks, ushort usSizeInBlocks);
    void RequestModel(DWORD dwModelID, DWORD dwFlags);
    void LoadAllRequestedModels(BOOL bOnlyPriorityModels = 0, const char* szTag = NULL);
    BOOL HasModelLoaded(DWORD dwModelID);
    void RequestSpecialModel(DWORD model, const char* szTexture, DWORD channel);

    CStreamingInfo* GetStreamingInfoFromModelId(uint id);
    unsigned char AddStreamHandler(const char* szFilePath);
    //BOOL RemoveStreamHandler(DWORD dwStreamHandler);
};
