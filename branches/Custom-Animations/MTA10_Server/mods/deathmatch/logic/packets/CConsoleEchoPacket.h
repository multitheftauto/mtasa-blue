/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CConsoleEchoPacket.h
*  PURPOSE:     Console message echo packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CCONSOLEECHOPACKET_H
#define __PACKETS_CCONSOLEECHOPACKET_H

#include "CPacket.h"
#include "../../Config.h"

class CConsoleEchoPacket : public CPacket
{
public:
    inline                  CConsoleEchoPacket          ( const char* szMessage )   { m_strMessage.AssignLeft ( szMessage, MAX_CONSOLEECHO_LENGTH ); }

    inline ePacketID                GetPacketID                 ( void ) const              { return PACKET_ID_CONSOLE_ECHO; };
    inline unsigned long            GetFlags                    ( void ) const              { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };
    virtual ePacketOrdering         GetPacketOrdering           ( void ) const              { return PACKET_ORDERING_CHAT; }

    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

    inline const char*      GetMessage                  ( void )                    { return m_strMessage; };
    void                    SetMessage                  ( const char* szMessage )   { m_strMessage.AssignLeft ( szMessage, MAX_CONSOLEECHO_LENGTH ); }

private:
    SString                 m_strMessage;
};

#endif
