/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDebugEchoPacket.h
 *  PURPOSE:     Debug message echo packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "../../Config.h"

#define DEBUGCOLOR_WARNING          255, 128, 0
#define DEBUGCOLOR_ERROR            255, 0, 0
#define DEBUGCOLOR_INFO             0, 255, 0

class CDebugEchoPacket final : public CPacket
{
public:
    CDebugEchoPacket(const char* szMessage, unsigned int uiLevel = 0, unsigned char ucRed = 255, unsigned char ucGreen = 255, unsigned char ucBlue = 255)
    {
        m_strMessage.AssignLeft(szMessage, MAX_DEBUGECHO_LENGTH);
        m_ucRed = ucRed;
        m_ucGreen = ucGreen;
        m_ucBlue = ucBlue;
        m_uiLevel = uiLevel;
    }

    ePacketID               GetPacketID() const { return PACKET_ID_DEBUG_ECHO; };
    unsigned long           GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_CHAT; }

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    unsigned char m_ucRed;
    unsigned char m_ucGreen;
    unsigned char m_ucBlue;
    unsigned int  m_uiLevel;
    SString       m_strMessage;
};
