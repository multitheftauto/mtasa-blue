/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CObjectDamagePacket.cpp
 *  PURPOSE:     Header for object damage packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectDamagePacket.h"
#include <CObject.h>
#include <net/SyncStructures.h>

CObjectDamagePacket::CObjectDamagePacket() {}

CObjectDamagePacket::CObjectDamagePacket(CObject* pObject, float fLoss, CElement* pAttacker) : m_ObjectID(pObject->GetID()), m_fLoss(fLoss), m_Attacker(pAttacker ? pAttacker->GetID() : INVALID_ELEMENT_ID) {}

bool CObjectDamagePacket::Read(NetBitStreamInterface& BitStream)
{
    return BitStream.Read(m_ObjectID) && BitStream.Read(m_fLoss) && BitStream.Read(m_Attacker);
}

bool CObjectDamagePacket::Write(NetBitStreamInterface& BitStream) const
{
    BitStream.Write(m_ObjectID);
    BitStream.Write(m_fLoss);
    BitStream.Write(m_Attacker);

    return true;
}
