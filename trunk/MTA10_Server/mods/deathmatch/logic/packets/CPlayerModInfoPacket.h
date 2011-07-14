/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerModInfoPacket.h
*  PURPOSE:
*  DEVELOPERS:  bidiot
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPlayerModInfoPacket : public CPacket
{
public:

    inline ePacketID                        GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_MODINFO; };
    unsigned long                           GetFlags                    ( void ) const                  { return PACKET_SEQUENCED; };

    bool                                    Read                        ( NetBitStreamInterface& BitStream );

    SString                                 m_strInfoType;
    std::vector < ushort >                  m_IdList;
    std::vector < SString >                 m_NameList;
};
