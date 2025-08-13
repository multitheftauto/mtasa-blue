/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CConsoleEchoPacket.h
 *  PURPOSE:     Console message echo packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "../../Config.h"

class CConsoleEchoPacket final : public CPacket
{
public:
    CConsoleEchoPacket(const char* szMessage) { m_strMessage.AssignLeft(szMessage, MAX_CONSOLEECHO_LENGTH); }

    ePacketID               GetPacketID() const { return PACKET_ID_CONSOLE_ECHO; };
    unsigned long           GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_CHAT; }

    bool Write(NetBitStreamInterface& BitStream) const;

    const char* GetMessage() { return m_strMessage; };
    void        SetMessage(const char* szMessage) { m_strMessage.AssignLeft(szMessage, MAX_CONSOLEECHO_LENGTH); }

private:
    SString m_strMessage;
};
