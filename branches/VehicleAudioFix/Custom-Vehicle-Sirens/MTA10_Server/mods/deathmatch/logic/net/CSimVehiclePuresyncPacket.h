/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

struct STrailerInfo
{
    ElementID   m_TrailerID;
    CVector     m_TrailerPosition;
    CVector     m_TrailerRotationDeg;
};


class CSimVehiclePuresyncPacket : public CSimPacket
{
public:
    ZERO_ON_NEW
                             CSimVehiclePuresyncPacket      ( ElementID PlayerID,
                                                              ushort usPlayerLatency,
                                                              uchar ucPlayerSyncTimeContext,
                                                              bool bPlayerHasOccupiedVehicle,
                                                              ushort usVehicleGotModel,
                                                              uchar ucPlayerGotOccupiedVehicleSeat,
                                                              uchar ucPlayerGotWeaponType,
                                                              float fPlayerGotWeaponRange );

    inline ePacketID            GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_VEHICLE_PURESYNC; };
    inline unsigned long        GetFlags                    ( void ) const                  { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool                        Read                        ( NetBitStreamInterface& BitStream );
    bool                        Write                       ( NetBitStreamInterface& BitStream ) const;

private:
    void                        ReadVehicleSpecific         ( NetBitStreamInterface& BitStream );
    void                        WriteVehicleSpecific        ( NetBitStreamInterface& BitStream ) const;


    bool CanUpdateSync ( unsigned char ucRemote )
    {
        // We can update this element's sync only if the sync time
        // matches or either of them are 0 (ignore).
        return ( m_ucPlayerSyncTimeContext == ucRemote ||
                 ucRemote == 0 ||
                 m_ucPlayerSyncTimeContext == 0 );
    }

    // Set in constructor 
    const ElementID m_PlayerID;
    const ushort    m_usPlayerLatency;
    const uchar     m_ucPlayerSyncTimeContext;
    const bool      m_bPlayerHasOccupiedVehicle;
    const ushort    m_usVehicleGotModel;
    const uchar     m_ucPlayerGotOccupiedVehicleSeat;
    const uchar     m_ucPlayerGotWeaponType;
    const float     m_fPlayerGotWeaponRange;

    // Set in Read()
    struct
    {
        uchar           ucTimeContext;

        CControllerState ControllerState;
        CVector         PlrPosition;
        CVector         vecCamPosition;
        CVector         vecCamFwd;

        CVector         VehPosition;      // Same as PlrPosition ?
        CVector         VehRotationDeg;
        CVector         BothVelocity;
        CVector         VehTurnSpeed;

        float           fVehHealth;

        std::vector < STrailerInfo > TrailerList;     // Unused

        float           fPlrHealth;
        float           fArmor;
        SVehiclePuresyncFlags flags;

        uchar           ucWeaponSlot;

        ushort          usAmmoInClip;

        float           fAimDirection;
        CVector         vecSniperSource;
        CVector         vecTargetting;

        uchar           ucDriveByDirection;

        float           fTurretX;
        float           fTurretY;
        ushort          usAdjustableProperty;

        float           fDoorOpenRatio[4];
    } m_Cache;
};

