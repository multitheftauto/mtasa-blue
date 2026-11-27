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
    ushort  usId = 0;
    uint    uiHash = 0;
    SString strName;
    bool    bHasSize = false;
    CVector vecSize;
    CVector vecOriginalSize;
    bool    bHasHashInfo = false;
    uint    uiShortBytes = 0;
    SString strShortMd5;
    SString strShortSha256;
    uint    uiLongBytes = 0;
    SString strLongMd5;
    SString strLongSha256;
};

class CPlayerModInfoPacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_MODINFO; };
    unsigned long GetFlags() const { return 0; };  // Not used

    bool Read(NetBitStreamInterface& BitStream);

    SString                   m_strInfoType;
    std::vector<SModInfoItem> m_ModInfoItemList;
};
