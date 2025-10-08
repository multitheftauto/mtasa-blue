/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/Packets.h
 *  PURPOSE:     Header for bit stream class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "..\CClient.h"
extern CNet* g_pNet;

class CBitStream
{
public:
    CBitStream() { pBitStream = g_pNet->AllocateNetBitStream(); };
    ~CBitStream() { g_pNet->DeallocateNetBitStream(pBitStream); };

    NetBitStreamInterface* pBitStream;
};
