/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CCustomDataPacket.h
*  PURPOSE:     Custom element data packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CCUSTOMDATAPACKET_H
#define __PACKETS_CCUSTOMDATAPACKET_H

#include "CPacket.h"
#include "../lua/CLuaArguments.h"

class CCustomDataPacket : public CPacket
{
public:
                            CCustomDataPacket               ( void );
                            ~CCustomDataPacket              ( void );

    inline ePacketID        GetPacketID                     ( void ) const              { return PACKET_ID_CUSTOM_DATA; };
    inline unsigned long    GetFlags                        ( void ) const              { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Read                            ( NetBitStreamInterface& BitStream );
    bool                    Write                           ( NetBitStreamInterface& BitStream ) const;

    inline ElementID        GetElementID                    ( void )                    { return m_ElementID; }
    inline char*            GetName                         ( void )                    { return m_szName; }
    inline CLuaArgument&    GetValue                        ( void )                    { return m_Value; }

private:
    ElementID               m_ElementID;
    char*                   m_szName;
    CLuaArgument            m_Value;
};

#endif
