/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.h
 *  PURPOSE:     Voice end packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CVoiceEndPacket final : public CPacket
{
public:
    CVoiceEndPacket(class CPlayer* pPlayer = NULL);
    ~CVoiceEndPacket();

    ePacketID               GetPacketID() const { return PACKET_ID_VOICE_END; }
    unsigned long           GetFlags() const { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_VOICE; }

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID GetPlayer();
    void      SetPlayer(ElementID PlayerID);

private:
    ElementID m_PlayerID;
};
