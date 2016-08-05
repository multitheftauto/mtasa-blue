/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/common/CBitStream.h
*  PURPOSE:     Network bitstream class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CBITSTREAM_H
#define __CBITSTREAM_H

#include "../CCommon.h"

class CBitStream
{
public:
    inline                          CBitStream  ( const void* pData = nullptr, uint uiDataSize = 0, bool bCopyData = false )    { pBitStream = g_pNetServer->AllocateNetServerBitStream ( 0, pData, uiDataSize, bCopyData ); }
    inline                          ~CBitStream ( void )    { g_pNetServer->DeallocateNetServerBitStream ( (NetBitStreamInterface*)pBitStream ); };
    NetBitStreamInterfaceNoVersion* operator->  ( void )    { return pBitStream; }

    NetBitStreamInterfaceNoVersion* pBitStream;
};

// Use this if Version() is required - Make sure the player is the once receiving/sending the data
class CPlayerBitStream
{
public:
                                    CPlayerBitStream  ( class CPlayer* pPlayer );//  { pBitStream = g_pNetServer->AllocateNetServerBitStream ( pPlayer->GetBitStreamVersion() ); };
    inline                          ~CPlayerBitStream ( void )              { g_pNetServer->DeallocateNetServerBitStream ( (NetBitStreamInterface*)pBitStream ); };
    NetBitStreamInterface*          operator->        ( void )              { return pBitStream; }

    NetBitStreamInterface* pBitStream;
};


#endif
