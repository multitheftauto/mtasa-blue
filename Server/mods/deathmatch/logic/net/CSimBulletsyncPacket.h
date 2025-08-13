/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

//
// For bouncing a bullet sync packet
//
class CSimBulletsyncPacket : public CSimPacket
{
public:
    ZERO_ON_NEW

    CSimBulletsyncPacket(ElementID PlayerID);

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_BULLETSYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    // Set in constructor
    const ElementID m_PlayerID;

    // Set in Read ()
    struct
    {
        eWeaponType weaponType;
        CVector     vecStart;
        CVector     vecEnd;
        uchar       ucOrderCounter;
        float       fDamage;
        uchar       ucHitZone;
        ElementID   DamagedPlayerID;
    } m_Cache;
};
