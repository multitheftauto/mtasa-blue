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
    virtual ePacketID                   GetPacketID         ( void ) const = 0;
    virtual unsigned long               GetFlags            ( void ) const = 0;

    virtual bool                        Read                ( NetBitStreamInterface& BitStream )                { return false; };
    virtual bool                        Write               ( NetBitStreamInterface& BitStream ) const          { return false; };
};


//
// For bouncing a player pure sync packet
//
class CSimPlayerPuresyncPacket : public CSimPacket
{
public:
    ZERO_ON_NEW

                      CSimPlayerPuresyncPacket              ( ElementID PlayerID,
                                                              ushort PlayerLatency,
                                                              uchar PlayerSyncTimeContext,
                                                              uchar PlayerGotWeaponType,
                                                              float WeaponRange );

    ePacketID               GetPacketID                     ( void ) const                  { return PACKET_ID_PLAYER_PURESYNC; };
    unsigned long           GetFlags                        ( void ) const                  { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };

    bool                    Read                            ( NetBitStreamInterface& BitStream );
    bool                    Write                           ( NetBitStreamInterface& BitStream ) const;


    bool CanUpdateSync ( unsigned char ucRemote )
    {
        // We can update this element's sync only if the sync time
        // matches or either of them are 0 (ignore).
        return ( m_PlayerSyncTimeContext == ucRemote ||
                 ucRemote == 0 ||
                 m_PlayerSyncTimeContext == 0 );
    }


    // Set in constructor
    const ElementID m_PlayerID;
    const ushort    m_PlayerLatency;
    const uchar     m_PlayerSyncTimeContext;
    const uchar     m_PlayerGotWeaponType;
    const float     m_WeaponRange;

    // Set in Read ()
    struct
    {
        uchar           ucTimeContext;
        CControllerState        ControllerState;
        SPlayerPuresyncFlags    flags;
        ElementID       ContactElementID;
        CVector         Position;
        float           fRotation;
        CVector         Velocity;
        float           fHealth;
        float           fArmor;
        float           fCameraRotation;
        CVector         vecCamPosition;
        CVector         vecCamFwd;

        bool            bWeaponCorrect;
        uchar           ucWeaponSlot;       // Only valid if bWeaponCorrect
        ushort          usAmmoInClip;      // Only valid if bWeaponCorrect
        ushort          usTotalAmmo;       // Only valid if bWeaponCorrect

        bool            bIsAimFull;
        float           fAimDirection;      // Only valid if bWeaponCorrect
        CVector         vecSniperSource;  // Only valid if bWeaponCorrect and bIsAimFull
        CVector         vecTargetting;    // Only valid if bWeaponCorrect and bIsAimFull
    } m_Cache;

};


