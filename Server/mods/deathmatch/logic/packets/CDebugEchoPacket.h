/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDebugEchoPacket.h
 *  PURPOSE:     Debug message echo packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
    CDebugEchoPacket(const char* szMessage, std::uint32_t uiLevel = 0,
        std::uint8_t ucRed = 255, std::uint8_t ucGreen = 255, std::uint8_t ucBlue = 255) noexcept
    {
        m_strMessage.AssignLeft(szMessage, MAX_DEBUGECHO_LENGTH);
        m_ucRed = ucRed;
        m_ucGreen = ucGreen;
        m_ucBlue = ucBlue;
        m_uiLevel = uiLevel;
    }

    ePacketID               GetPacketID() const noexcept { return PACKET_ID_DEBUG_ECHO; }
    std::uint32_t           GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_CHAT; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    std::uint8_t  m_ucRed;
    std::uint8_t  m_ucGreen;
    std::uint8_t  m_ucBlue;
    std::uint32_t m_uiLevel;
    SString       m_strMessage;
};
