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
    ElementID m_TrailerID;
    CVector   m_TrailerPosition;
    CVector   m_TrailerRotationDeg;
};

class CSimVehiclePuresyncPacket : public CSimPacket
{
public:
    CSimVehiclePuresyncPacket(ElementID PlayerID, ushort usPlayerLatency, uchar ucPlayerSyncTimeContext, bool bPlayerHasOccupiedVehicle,
                              ushort usVehicleGotModel, uchar ucPlayerGotOccupiedVehicleSeat, uchar ucPlayerGotWeaponType, float fPlayerGotWeaponRange,
                              CControllerState& sharedControllerState, uint m_uiDamageInfoSendPhase, const SSimVehicleDamageInfo& damageInfo);

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_VEHICLE_PURESYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

private:
    void ReadVehicleSpecific(NetBitStreamInterface& BitStream);
    void WriteVehicleSpecific(NetBitStreamInterface& BitStream) const;

    bool CanUpdateSync(unsigned char ucRemote)
    {
        // We can update this element's sync only if the sync time
        // matches or either of them are 0 (ignore).
        return (m_ucPlayerSyncTimeContext == ucRemote || ucRemote == 0 || m_ucPlayerSyncTimeContext == 0);
    }

    // Set in constructor
    const ElementID              m_PlayerID;
    const ushort                 m_usPlayerLatency = 0;
    const uchar                  m_ucPlayerSyncTimeContext = 0;
    const bool                   m_bPlayerHasOccupiedVehicle = false;
    const ushort                 m_usVehicleGotModel = 0;
    const uchar                  m_ucPlayerGotOccupiedVehicleSeat = 0;
    const uchar                  m_ucPlayerGotWeaponType = 0;
    const float                  m_fPlayerGotWeaponRange = 0.0f;
    CControllerState&            m_sharedControllerState;
    const uint                   m_uiDamageInfoSendPhase = 0;
    const SSimVehicleDamageInfo& m_DamageInfo;

    // Set in Read()
    struct
    {
        uchar ucTimeContext = 0;

        int iModelID = 0;

        CVector PlrPosition;
        CVector vecCamPosition;
        CVector vecCamFwd;

        CVector VehPosition;            // Same as PlrPosition ?
        CVector VehRotationDeg;
        CVector BothVelocity;
        CVector VehTurnSpeed;

        float fVehHealth = 0.0f;

        std::vector<STrailerInfo> TrailerList;

        float                 fPlrHealth = 0.0f;
        float                 fArmor = 0.0f;
        SVehiclePuresyncFlags flags = {};

        uchar ucWeaponSlot = 0;

        ushort usAmmoInClip = 0;
        ushort usTotalAmmo = 0;

        float   fAimDirection = 0.0f;
        CVector vecSniperSource;
        CVector vecTargetting;

        uchar ucDriveByDirection = 0;

        float  fTurretX = 0.0f;
        float  fTurretY = 0.0f;
        ushort usAdjustableProperty = 0;

        float fRailPosition = 0.0f;
        uchar ucRailTrack  = 0;
        bool  bRailDirection = false;
        float fRailSpeed = 0.0f;

        SFixedArray<float, 4> fDoorOpenRatio = {};
    } m_Cache;
};
