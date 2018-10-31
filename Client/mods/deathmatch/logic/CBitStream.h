/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/Packets.h
 *  PURPOSE:     Header for bit stream class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "..\CClient.h"
extern CNet* g_pNet;

class CBitStream
{
public:
    CBitStream(void) { pBitStream = g_pNet->AllocateNetBitStream(); };
    ~CBitStream(void) { g_pNet->DeallocateNetBitStream(pBitStream); };

    NetBitStreamInterface* pBitStream;
};
