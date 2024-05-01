/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CObjectDamagePacket.h
 *  PURPOSE:     Object damage state class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CObject;

class CObjectDamagePacket final : public CPacket
{
public:
    CObjectDamagePacket();
    CObjectDamagePacket(CObject* pObject, float fLoss, CElement* pAttacker);

    ePacketID     GetPacketID() const { return PACKET_ID_OBJECT_DAMAGE; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID   m_ObjectID;
    float       m_fLoss; 
    ElementID   m_Attacker;
};
