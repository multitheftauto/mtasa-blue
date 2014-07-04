/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CBulletsyncPacket.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


class CBulletsyncPacket : public CPacket
{
public:
    inline                                  CBulletsyncPacket           ( void )                        {};
                                            CBulletsyncPacket           ( class CPlayer* pPlayer );

    inline ePacketID                        GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_BULLETSYNC; };
    unsigned long                           GetFlags                    ( void ) const                  { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool                                    Read                        ( NetBitStreamInterface& BitStream );
    bool                                    Write                       ( NetBitStreamInterface& BitStream ) const;

    eWeaponType             m_WeaponType;
    CVector                 m_vecStart;
    CVector                 m_vecEnd;
};

