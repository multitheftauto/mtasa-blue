/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CChatEchoPacket.h
 *  PURPOSE:     Chat message echo packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "../../Config.h"

#define CHATCOLOR_SAY           235, 221, 178
#define CHATCOLOR_TEAMSAY       235, 221, 178
#define CHATCOLOR_MESSAGE       255, 168, 0
#define CHATCOLOR_INFO          255, 100, 100
#define CHATCOLOR_ME            255, 0, 255
#define CHATCOLOR_ADMINSAY      131, 205, 241
#define CHATCOLOR_CONSOLESAY    223, 149, 232

enum eMessageType
{
    MESSAGE_TYPE_PLAYER,
    MESSAGE_TYPE_ACTION,
    MESSAGE_TYPE_TEAM,
    MESSAGE_TYPE_PRIVATE,
    MESSAGE_TYPE_INTERNAL
};

class CChatEchoPacket final : public CPacket
{
public:
    CChatEchoPacket(SString strMessage, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, bool bColorCoded = false,
                    unsigned char ucMessageType = MESSAGE_TYPE_PLAYER)
    {
        m_strMessage = strMessage;
        m_ucRed = ucRed;
        m_ucGreen = ucGreen;
        m_ucBlue = ucBlue;
        m_bColorCoded = bColorCoded;
        m_ucMessageType = ucMessageType;
    };

    // Chat uses low priority channel to avoid getting queued behind large map downloads #6877
    ePacketID               GetPacketID() const { return PACKET_ID_CHAT_ECHO; };
    unsigned long           GetFlags() const { return PACKET_LOW_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_CHAT; }

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    unsigned char m_ucRed;
    unsigned char m_ucGreen;
    unsigned char m_ucBlue;
    SString       m_strMessage;
    bool          m_bColorCoded;
    unsigned char m_ucMessageType;
};
