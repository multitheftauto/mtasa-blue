/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CObjectBreakPacket.cpp
 *  PURPOSE:     Header for object break packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectBreakPacket.h"
#include <CObject.h>
#include <net/SyncStructures.h>

CObjectBreakPacket::CObjectBreakPacket() {}

CObjectBreakPacket::CObjectBreakPacket(CObject* pObject, CElement* pAttacker) : m_ObjectID(pObject->GetID()), m_Attacker(pAttacker ? pAttacker->GetID() : INVALID_ELEMENT_ID) {}

bool CObjectBreakPacket::Read(NetBitStreamInterface& BitStream)
{
    return BitStream.Read(m_ObjectID) && BitStream.Read(m_Attacker);
}

bool CObjectBreakPacket::Write(NetBitStreamInterface& BitStream) const
{
    BitStream.Write(m_ObjectID);
    BitStream.Write(m_Attacker);

    return true;
}
