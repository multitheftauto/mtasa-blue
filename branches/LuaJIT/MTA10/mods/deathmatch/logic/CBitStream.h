/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/Packets.h
*  PURPOSE:     Header for bit stream class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CBITSTREAM_H
#define __CBITSTREAM_H

#include "..\CClient.h"
extern CNet * g_pNet;

class CBitStream
{
public:
    inline                          CBitStream  ( void )    { pBitStream = g_pNet->AllocateNetBitStream (); };
    inline                          ~CBitStream ( void )    { g_pNet->DeallocateNetBitStream ( pBitStream ); };

    NetBitStreamInterface*          pBitStream;
};

#endif
