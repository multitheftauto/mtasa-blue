/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CServerTextItemPacket.h
*  PURPOSE:     Server text item packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kent Simon <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSERVERTEXTITEMPACKET_H
#define __CSERVERTEXTITEMPACKET_H

#include "CPacket.h"

class CServerTextItemPacket : public CPacket
{
public:
                            CServerTextItemPacket       ( void );
                            CServerTextItemPacket       ( unsigned long ulUniqueId, bool bDeleteable, float fX, float fY, float fScale, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, unsigned char format, char* szText );
                            ~CServerTextItemPacket      ( void );

    inline ePacketID        GetPacketID                 ( void ) const              { return PACKET_ID_TEXT_ITEM; };
    inline unsigned long    GetFlags                    ( void ) const              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                       ( NetServerBitStreamInterface& BitStream ) const;
private:
    char *                  m_szText;
    float                   m_fX;
    float                   m_fY;
    unsigned char           m_red;
    unsigned char           m_green;
    unsigned char           m_blue;
    unsigned char           m_alpha;
    float                   m_fScale;
    unsigned char           m_ucFormat;
    unsigned long           m_ulUniqueId;
    bool                    m_bDeletable;
};

#endif
