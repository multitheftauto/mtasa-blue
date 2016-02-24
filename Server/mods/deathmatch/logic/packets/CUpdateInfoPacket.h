/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CUpdateInfoPacket.h
*  PURPOSE:     
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CUPDATEINFOPACKET_H
#define __PACKETS_CUPDATEINFOPACKET_H


class CUpdateInfoPacket : public CPacket
{
public:
                            CUpdateInfoPacket           ( void );
                            CUpdateInfoPacket           ( const SString& m_strType, const SString& strData );

    inline ePacketID        GetPacketID                 ( void ) const              { return PACKET_ID_UPDATE_INFO; };
    inline unsigned long    GetFlags                    ( void ) const              { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

private:
    SString                 m_strType;
    SString                 m_strData;
};

#endif
