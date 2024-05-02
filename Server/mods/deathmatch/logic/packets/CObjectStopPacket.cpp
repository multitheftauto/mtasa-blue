/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CObjectStopPacket.cpp
 *  PURPOSE:     Header for object stop packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectStopPacket.h"
#include <CObject.h>
#include <net/SyncStructures.h>

CObjectStopPacket::CObjectStopPacket() {}

CObjectStopPacket::CObjectStopPacket(CObject* pObject, bool bStoppedByScript) : m_ObjectID(pObject->GetID()), m_bStoppedByScript(bStoppedByScript) {}

bool CObjectStopPacket::Read(NetBitStreamInterface& BitStream)
{
    return BitStream.Read(m_ObjectID) && BitStream.Read(m_bStoppedByScript);
}

bool CObjectStopPacket::Write(NetBitStreamInterface& BitStream) const
{
    BitStream.Write(m_ObjectID);
    BitStream.Write(m_bStoppedByScript);

    return true;
}
