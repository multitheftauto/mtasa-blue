/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CChatEchoPacket.h
*  PURPOSE:     Chat message echo packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CCHATECHOPACKET_H
#define __PACKETS_CCHATECHOPACKET_H

#include "CPacket.h"
#include "../../Config.h"

#define CHATCOLOR_SAY           235, 221, 178
#define CHATCOLOR_TEAMSAY       235, 221, 178
#define CHATCOLOR_MESSAGE       255, 168, 0
#define CHATCOLOR_INFO          255, 100, 100
#define CHATCOLOR_ME            255, 0, 255
#define CHATCOLOR_ADMINSAY      131, 205, 241
#define CHATCOLOR_CONSOLESAY    223, 149, 232

class CChatEchoPacket : public CPacket
{
public:
    inline                  CChatEchoPacket             ( const char* szMessage,
                                                          unsigned char ucRed,
                                                          unsigned char ucGreen,
                                                          unsigned char ucBlue,
                                                          bool bColorCoded = false )    { strncpy ( m_szMessage, szMessage, MAX_CHATECHO_LENGTH ); m_szMessage [MAX_CHATECHO_LENGTH] = 0; m_ucRed = ucRed; m_ucGreen = ucGreen; m_ucBlue = ucBlue; m_bColorCoded = bColorCoded; };

    inline ePacketID        GetPacketID                 ( void ) const              { return PACKET_ID_CHAT_ECHO; };
    inline unsigned long    GetFlags                    ( void ) const              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                       ( NetServerBitStreamInterface& BitStream ) const;

    inline const char*      GetMessage                  ( void )                    { return m_szMessage; };
    void                    SetMessage                  ( const char* szMessage )   { strncpy ( m_szMessage, szMessage, MAX_CHATECHO_LENGTH ); };
    inline void             SetColor                    ( unsigned char ucRed,
                                                          unsigned char ucGreen,
                                                          unsigned char ucBlue )    { m_ucRed = ucRed; m_ucGreen = ucGreen; m_ucRed = ucRed; };
    inline void             SetColorCoded               ( bool bColorCoded )        { m_bColorCoded = bColorCoded; }
private:
    unsigned char           m_ucRed;
    unsigned char           m_ucGreen;
    unsigned char           m_ucBlue;
    char                    m_szMessage [MAX_CHATECHO_LENGTH + 1];
    bool                    m_bColorCoded;
};

#endif
