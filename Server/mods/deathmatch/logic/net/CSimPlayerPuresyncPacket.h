/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CSimPacket
{
public:
    CSimPacket() { DEBUG_CREATE_COUNT("CSimPacket"); }
    virtual ~CSimPacket() { DEBUG_DESTROY_COUNT("CSimPacket"); }

    virtual ePacketID       GetPacketID() const = 0;
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_DEFAULT; }
    virtual unsigned long   GetFlags() const = 0;

    virtual bool Read(NetBitStreamInterface& BitStream) { return false; };
    virtual bool Write(NetBitStreamInterface& BitStream) const { return false; };
};

//
// For bouncing a player pure sync packet
//
class CSimPlayerPuresyncPacket : public CSimPacket
{
public:
    CSimPlayerPuresyncPacket(ElementID PlayerID, ushort PlayerLatency, uchar PlayerSyncTimeContext, uchar PlayerGotWeaponType, float WeaponRange,
                             CControllerState& sharedControllerState);

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_PURESYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    bool CanUpdateSync(unsigned char ucRemote)
    {
        // We can update this element's sync only if the sync time
        // matches or either of them are 0 (ignore).
        return (m_PlayerSyncTimeContext == ucRemote || ucRemote == 0 || m_PlayerSyncTimeContext == 0);
    }

    // Set in constructor
    const ElementID   m_PlayerID;
    const ushort      m_PlayerLatency = 0;
    const uchar       m_PlayerSyncTimeContext = 0;
    const uchar       m_PlayerGotWeaponType = 0;
    const float       m_WeaponRange = 0.0f;
    CControllerState& m_sharedControllerState;

    // Set in Read ()
    struct
    {
        uchar                ucTimeContext = 0;
        SPlayerPuresyncFlags flags = {};
        ElementID            ContactElementID;
        CVector              Position;
        float                fRotation = 0.0f;
        CVector              Velocity;
        float                fHealth = 0.0f;
        float                fArmor = 0.0f; 
        float                fCameraRotation = 0.0f;
        CVector              vecCamPosition;
        CVector              vecCamFwd;

        bool   bWeaponCorrect = false;
        uchar  ucWeaponSlot = 0;            // Only valid if bWeaponCorrect
        ushort usAmmoInClip = 0;            // Only valid if bWeaponCorrect
        ushort usTotalAmmo = 0;             // Only valid if bWeaponCorrect

        bool    bIsAimFull = false;
        float   fAimDirection = 0.0f;              // Only valid if bWeaponCorrect
        CVector vecSniperSource;            // Only valid if bWeaponCorrect and bIsAimFull
        CVector vecTargetting;              // Only valid if bWeaponCorrect and bIsAimFull
    } m_Cache;
};
