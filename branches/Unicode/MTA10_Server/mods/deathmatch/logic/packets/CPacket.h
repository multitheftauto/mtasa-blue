/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPacket.h
*  PURPOSE:     Packet base class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kent Simon <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPacket;

#ifndef __CPACKET_H
#define __CPACKET_H

#include "../../Config.h"
#include "net/Packets.h"
#include <net/CNetServer.h>

class CElement;
class CPlayer;

enum
{
    PACKET_RELIABLE = 1,
    PACKET_SEQUENCED = 2,
    PACKET_HIGH_PRIORITY = 4,
    PACKET_LOW_PRIORITY = 8,
};

class CPacket
{
public:
                                        CPacket             ( void );
    virtual                             ~CPacket            ( void ) {};

    virtual ePacketID                   GetPacketID         ( void ) const = 0;
    virtual unsigned long               GetFlags            ( void ) const = 0;

    virtual bool                        Read                ( NetBitStreamInterface& BitStream )                { return false; };
    virtual bool                        Write               ( NetBitStreamInterface& BitStream ) const          { return false; };

    inline void                         SetSourceElement    ( CElement* pSource )                               { m_pSourceElement = pSource; };
    inline CElement*                    GetSourceElement    ( void ) const                                      { return m_pSourceElement; };
    CPlayer*                            GetSourcePlayer     ( void );
    inline void                         SetSourceSocket     ( const NetServerPlayerID& Source )                 { m_Source = Source; };
    inline const NetServerPlayerID&     GetSourceSocket     ( void ) const                                      { return m_Source; };
    inline unsigned long                GetSourceIP         ( void ) const                                      { return m_Source.GetBinaryAddress (); };
    inline unsigned short               GetSourcePort       ( void ) const                                      { return m_Source.GetPort (); };

    virtual void                        Send                ( CPlayer* pPlayer ) const;
private:
    
protected:
    CElement*                           m_pSourceElement;
    NetServerPlayerID                   m_Source;    
};

#endif
