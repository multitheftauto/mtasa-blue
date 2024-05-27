/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVoiceEndPacket.cpp
 *  PURPOSE:     Voice end packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVoiceEndPacket.h"
#include "CPlayer.h"

CVoiceEndPacket::CVoiceEndPacket(CPlayer* pPlayer) noexcept
{
    m_PlayerID = INVALID_ELEMENT_ID;
    if (pPlayer)
        SetSourceElement(pPlayer);
}

CVoiceEndPacket::~CVoiceEndPacket() noexcept
{
}

bool CVoiceEndPacket::Read(NetBitStreamInterface& BitStream) noexcept
{
    return true;
}

bool CVoiceEndPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    if (!m_pSourceElement)
        return false;

    // Write the source player
    ElementID ID = m_pSourceElement->GetID();
    BitStream.Write(ID);
    return true;
}
