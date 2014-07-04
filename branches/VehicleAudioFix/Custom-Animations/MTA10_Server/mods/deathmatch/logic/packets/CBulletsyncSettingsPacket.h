/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CBulletsyncSettingsPacket.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


class CBulletsyncSettingsPacket : public CPacket
{
public:
    inline                                  CBulletsyncSettingsPacket           ( void )                        {};
                                            CBulletsyncSettingsPacket           ( const std::set < eWeaponType >& weaponTypesUsingBulletSync );

    inline ePacketID                        GetPacketID                 ( void ) const                  { return PACKET_ID_BULLETSYNC_SETTINGS; };
    unsigned long                           GetFlags                    ( void ) const                  { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                                    Read                        ( NetBitStreamInterface& BitStream );
    bool                                    Write                       ( NetBitStreamInterface& BitStream ) const;

    std::set < eWeaponType >    m_weaponTypesUsingBulletSync;
};

