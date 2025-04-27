/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CChatEchoPacket.cpp
 *  PURPOSE:     Chat message echo packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CChatEchoPacket.h"
#include "CElement.h"

bool CChatEchoPacket::Write(NetBitStreamInterface& BitStream) const
{
    // Write the color
    BitStream.Write(m_ucRed);
    BitStream.Write(m_ucGreen);
    BitStream.Write(m_ucBlue);
    BitStream.WriteBit(m_bColorCoded);

    // Write the client's ID
    if (BitStream.Can(eBitStreamVersion::OnClientChatMessage_PlayerSource))
    {
        BitStream.Write(GetSourceElement() ? GetSourceElement()->GetID() : INVALID_ELEMENT_ID);
    }

    // Too short?
    size_t sizeMessage = m_strMessage.length();
    if (sizeMessage >= MIN_CHATECHO_LENGTH)
    {
        if (BitStream.Can(eBitStreamVersion::OnClientChatMessage_MessageType))
        {
            // Write the message type
            BitStream.Write(m_ucMessageType);
        }

        // Write the string
        BitStream.Write(m_strMessage.c_str(), sizeMessage);

        return true;
    }

    return false;
}
