/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerDamageCancelledPacket.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerDamageCancelledPacket.h"

bool CPlayerDamageCancelledPacket::Read(NetBitStreamInterface& stream) noexcept
{
    stream.Read(m_attacker);
    stream.Read(m_cause);
    stream.Read(m_bodypart);
    stream.Read(m_loss);

    return true;
}
