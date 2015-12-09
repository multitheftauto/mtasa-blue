/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerACInfoPacket.h
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPlayerACInfoPacket : public CPacket
{
public:

    inline ePacketID                        GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_ACINFO; };
    unsigned long                           GetFlags                    ( void ) const                  { return 0; };  // Not used

    bool                                    Read                        ( NetBitStreamInterface& BitStream );

    std::vector < uchar >                   m_IdList;
    uint                                    m_uiD3d9Size;
    SString                                 m_strD3d9MD5;
    SString                                 m_strD3d9SHA256;
};

