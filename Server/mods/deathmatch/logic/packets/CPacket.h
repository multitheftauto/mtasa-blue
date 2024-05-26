/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPacket.h
 *  PURPOSE:     Packet base class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPacket;

#pragma once

#include "../../Config.h"
#include <net/CNetServer.h>
#include <net/Packets.h>

class CElement;
class CPlayer;

enum
{
    PACKET_RELIABLE = 1,
    PACKET_SEQUENCED = 2,
    PACKET_HIGH_PRIORITY = 4,
    PACKET_LOW_PRIORITY = 8,

    PACKET_UNRELIABLE = 0,
    PACKET_MEDIUM_PRIORITY = 0,
};

class CPacket
{
public:
    CPacket() noexcept;
    virtual ~CPacket() noexcept {};

    virtual bool            RequiresSourcePlayer() const noexcept { return true; }
    virtual bool            HasSimHandler() const noexcept { return false; }
    virtual ePacketID       GetPacketID() const noexcept = 0;
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_DEFAULT; }
    virtual std::uint32_t   GetFlags() const noexcept = 0;

    virtual bool Read(NetBitStreamInterface& BitStream) noexcept { return false; }
    virtual bool Write(NetBitStreamInterface& BitStream) const noexcept { return false; }

    void                     SetSourceElement(CElement* pSource) noexcept { m_pSourceElement = pSource; }
    CElement*                GetSourceElement() const noexcept { return m_pSourceElement; }
    CPlayer*                 GetSourcePlayer();
    void                     SetSourceSocket(const NetServerPlayerID& Source) noexcept { m_Source = Source; }
    const NetServerPlayerID& GetSourceSocket() const noexcept { return m_Source; }
    std::uint32_t            GetSourceIP() const noexcept { return m_Source.GetBinaryAddress(); }
    std::uint16_t            GetSourcePort() const noexcept { return m_Source.GetPort(); }

protected:
    CElement*         m_pSourceElement{nullptr};
    NetServerPlayerID m_Source;
};
