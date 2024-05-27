/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

//
// For bouncing a bullet sync packet
//
class CSimBulletsyncPacket : public CSimPacket
{
public:
    ZERO_ON_NEW

    CSimBulletsyncPacket(ElementID PlayerID) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_BULLETSYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    // Set in constructor
    const ElementID m_PlayerID;

    // Set in Read ()
    struct
    {
        eWeaponType  weaponType;
        CVector      vecStart;
        CVector      vecEnd;
        std::uint8_t ucOrderCounter;
        float        fDamage;
        std::uint8_t ucHitZone;
        ElementID    DamagedPlayerID;
    } m_Cache;
};
