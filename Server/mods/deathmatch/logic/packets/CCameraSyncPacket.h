/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CCameraSyncPacket.h
 *  PURPOSE:     Camera synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../CCommon.h"
#include "CPacket.h"
#include "../CElement.h"

class CCameraSyncPacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const { return PACKET_ID_CAMERA_SYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);

    bool      m_bFixed;
    CVector   m_vecPosition, m_vecLookAt;
    ElementID m_TargetID;
};
