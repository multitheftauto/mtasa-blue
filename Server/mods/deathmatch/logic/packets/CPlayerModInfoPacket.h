/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerModInfoPacket.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

struct SModInfoItem
{
    ushort  usId;
    uint    uiHash;
    SString strName;
    bool    bHasSize;
    CVector vecSize;
    CVector vecOriginalSize;
    bool    bHasHashInfo;
    uint    uiShortBytes;
    SString strShortMd5;
    SString strShortSha256;
    uint    uiLongBytes;
    SString strLongMd5;
    SString strLongSha256;
};

class CPlayerModInfoPacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_MODINFO; };
    unsigned long GetFlags() const { return 0; };            // Not used

    bool Read(NetBitStreamInterface& BitStream);

    SString                   m_strInfoType;
    std::vector<SModInfoItem> m_ModInfoItemList;
};
