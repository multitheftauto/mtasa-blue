/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPacket.h
 *  PURPOSE:     Packet base class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
    CPacket();
    virtual ~CPacket(){};

    virtual bool            RequiresSourcePlayer() const { return true; }
    virtual bool            HasSimHandler() const { return false; }
    virtual ePacketID       GetPacketID() const = 0;
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_DEFAULT; }
    virtual unsigned long   GetFlags() const = 0;

    virtual bool Read(NetBitStreamInterface& BitStream) { return false; };
    virtual bool Write(NetBitStreamInterface& BitStream) const { return false; };

    void                     SetSourceElement(CElement* pSource) { m_pSourceElement = pSource; };
    CElement*                GetSourceElement() const { return m_pSourceElement; };
    CPlayer*                 GetSourcePlayer();
    void                     SetSourceSocket(const NetServerPlayerID& Source) { m_Source = Source; };
    const NetServerPlayerID& GetSourceSocket() const { return m_Source; };
    unsigned long            GetSourceIP() const { return m_Source.GetBinaryAddress(); };
    unsigned short           GetSourcePort() const { return m_Source.GetPort(); };

protected:
    CElement*         m_pSourceElement;
    NetServerPlayerID m_Source;
};
