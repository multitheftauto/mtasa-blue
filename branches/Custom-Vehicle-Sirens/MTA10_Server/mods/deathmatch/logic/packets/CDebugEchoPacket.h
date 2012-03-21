/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CDebugEchoPacket.h
*  PURPOSE:     Debug message echo packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CDEBUGECHOPACKET_H
#define __PACKETS_CDEBUGECHOPACKET_H

#include "CPacket.h"
#include "../../Config.h"

#define DEBUGCOLOR_WARNING          255, 128, 0
#define DEBUGCOLOR_ERROR            255, 0, 0
#define DEBUGCOLOR_INFO             0, 255, 0

class CDebugEchoPacket : public CPacket
{
public:
    inline                  CDebugEchoPacket            ( const char* szMessage,
                                                          unsigned int uiLevel = 0,
                                                          unsigned char ucRed = 255,
                                                          unsigned char ucGreen = 255,
                                                          unsigned char ucBlue = 255                                                          
                                                           )    { strncpy ( m_szMessage, szMessage, MAX_DEBUGECHO_LENGTH ); m_szMessage [MAX_DEBUGECHO_LENGTH] = 0; m_ucRed = ucRed; m_ucGreen = ucGreen; m_ucBlue = ucBlue; m_uiLevel = uiLevel; };

    inline ePacketID                GetPacketID                 ( void ) const              { return PACKET_ID_DEBUG_ECHO; };
    inline unsigned long            GetFlags                    ( void ) const              { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };
    virtual ePacketOrdering         GetPacketOrdering           ( void ) const              { return PACKET_ORDERING_CHAT; }

    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

    inline const char*      GetMessage                  ( void )                    { return m_szMessage; };
    void                    SetMessage                  ( const char* szMessage )   { strncpy ( m_szMessage, szMessage, MAX_CHATECHO_LENGTH ); };
    inline unsigned int     GetLevel                    ( void )                    { return m_uiLevel; }
    inline void             SetLevel                    ( unsigned int uiLevel )    { m_uiLevel = uiLevel; }
    inline void             SetColor                    ( unsigned char ucRed,
                                                          unsigned char ucGreen,
                                                          unsigned char ucBlue )    { m_ucRed = ucRed; m_ucGreen = ucGreen; m_ucRed = ucRed; };

private:
    unsigned char           m_ucRed;
    unsigned char           m_ucGreen;
    unsigned char           m_ucBlue;
    unsigned int            m_uiLevel;
    char                    m_szMessage [MAX_DEBUGECHO_LENGTH + 1];
};

#endif
