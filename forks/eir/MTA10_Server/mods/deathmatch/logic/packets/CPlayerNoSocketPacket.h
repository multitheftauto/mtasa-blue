/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerNoSocketPacket.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPlayerNoSocketPacket : public CPacket
{
public:
    virtual bool            RequiresSourcePlayer    ( void ) const                              { return true; }
    ePacketID               GetPacketID             ( void ) const                              { return PACKET_ID_PLAYER_NO_SOCKET; };
    unsigned long           GetFlags                ( void ) const                              { assert( 0 ); return 0; };

    bool                    Read                    ( NetBitStreamInterface& BitStream )        { return true; };
};
