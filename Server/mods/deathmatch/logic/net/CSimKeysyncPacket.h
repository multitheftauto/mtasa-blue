/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

//
// For bouncing a key sync packet
//
class CSimKeysyncPacket : public CSimPacket
{
public:
    ZERO_ON_NEW

    CSimKeysyncPacket(ElementID PlayerID, bool bPlayerHasOccupiedVehicle, std::uint16_t usVehicleGotModel, std::uint8_t ucPlayerGotWeaponType, float fPlayerGotWeaponRange,
                      bool bVehicleHasHydraulics, bool bVehicleIsPlaneOrHeli, CControllerState& sharedControllerState) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_KEYSYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    // Set in constructor
    const ElementID     m_PlayerID;
    const bool          m_bPlayerHasOccupiedVehicle;
    const std::uint16_t m_usVehicleGotModel;
    const std::uint8_t  m_ucPlayerGotWeaponType;
    const float         m_fPlayerGotWeaponRange;
    const bool          m_bVehicleHasHydraulics;
    const bool          m_bVehicleIsPlaneOrHeli;
    CControllerState&   m_sharedControllerState;

    // Set in Read ()
    struct
    {
        float fPlayerRotation;
        float fCameraRotation;

        SKeysyncFlags flags;

        bool   bWeaponCorrect;
        std::uint8_t  ucWeaponSlot;            // Only valid if bWeaponCorrect
        std::uint16_t usAmmoInClip;            // Only valid if bWeaponCorrect

        float   fAimDirection;            // Only valid if bWeaponCorrect
        CVector vecSniperSource;
        CVector vecTargetting;

        std::uint8_t ucDriveByDirection;

        SVehicleTurretSync turretSync;
    } m_Cache;
};
