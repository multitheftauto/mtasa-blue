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
    CPacket(void);
    virtual ~CPacket(void){};

    virtual bool            RequiresSourcePlayer(void) const { return true; }
    virtual bool            HasSimHandler(void) const { return false; }
    virtual ePacketID       GetPacketID(void) const = 0;
    virtual ePacketOrdering GetPacketOrdering(void) const { return PACKET_ORDERING_DEFAULT; }
    virtual unsigned long   GetFlags(void) const = 0;

    virtual bool Read(NetBitStreamInterface& BitStream) { return false; };
    virtual bool Write(NetBitStreamInterface& BitStream) const { return false; };

    void                     SetSourceElement(CElement* pSource) { m_pSourceElement = pSource; };
    CElement*                GetSourceElement(void) const { return m_pSourceElement; };
    CPlayer*                 GetSourcePlayer(void);
    void                     SetSourceSocket(const NetServerPlayerID& Source) { m_Source = Source; };
    const NetServerPlayerID& GetSourceSocket(void) const { return m_Source; };
    unsigned long            GetSourceIP(void) const { return m_Source.GetBinaryAddress(); };
    unsigned short           GetSourcePort(void) const { return m_Source.GetPort(); };

protected:
    CElement*         m_pSourceElement;
    NetServerPlayerID m_Source;
};
