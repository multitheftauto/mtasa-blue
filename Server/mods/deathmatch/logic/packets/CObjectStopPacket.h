/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CObjectStopPacket.h
 *  PURPOSE:     Object stop state class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CObject;

class CObjectStopPacket final : public CPacket
{
public:
    CObjectStopPacket();
    CObjectStopPacket(CObject* pObject, bool bStoppedByScript);

    ePacketID     GetPacketID() const { return PACKET_ID_OBJECT_STOP; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID m_ObjectID;
    int      m_bStoppedByScript;
};
