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
                            CServerTextItemPacket       ( unsigned long ulUniqueId, bool bDeleteable, float fX, float fY, float fScale, const SColor color, unsigned char format, unsigned char ucShadowAlpha, const char* szText );

    inline ePacketID        GetPacketID                 ( void ) const              { return PACKET_ID_TEXT_ITEM; };
    inline unsigned long    GetFlags                    ( void ) const              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;
private:
    SString                 m_strText;
    float                   m_fX;
    float                   m_fY;
    SColor                  m_Color;
    float                   m_fScale;
    unsigned char           m_ucFormat;
    unsigned char           m_ucShadowAlpha;
    unsigned long           m_ulUniqueId;
    bool                    m_bDeletable;
};

#endif
