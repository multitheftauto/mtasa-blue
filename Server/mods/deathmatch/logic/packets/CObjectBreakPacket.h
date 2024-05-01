/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CObjectBreakPacket.h
 *  PURPOSE:     Object break state class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CObject;

class CObjectBreakPacket final : public CPacket
{
public:
    CObjectBreakPacket();
    CObjectBreakPacket(CObject* pObject, CElement* pAttacker);

    ePacketID    GetPacketID() const { return PACKET_ID_OBJECT_BREAK; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID    m_ObjectID;
    ElementID    m_Attacker;
};
