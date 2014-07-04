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
    inline                  CConsoleEchoPacket          ( const char* szMessage )   { strncpy ( m_szMessage, szMessage, MAX_CONSOLEECHO_LENGTH ); m_szMessage [MAX_CONSOLEECHO_LENGTH] = 0; };

    inline ePacketID                GetPacketID                 ( void ) const              { return PACKET_ID_CONSOLE_ECHO; };
    inline unsigned long            GetFlags                    ( void ) const              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

    inline const char*      GetMessage                  ( void )                    { return m_szMessage; };
    void                    SetMessage                  ( const char* szMessage )   { strncpy ( m_szMessage, szMessage, MAX_CONSOLEECHO_LENGTH ); };

private:
    char                    m_szMessage [MAX_CONSOLEECHO_LENGTH + 1];
};

#endif
