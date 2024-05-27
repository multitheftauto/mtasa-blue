/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <net/SyncStructures.h>

struct STrailerInfo
{
    ElementID m_TrailerID;
    CVector   m_TrailerPosition;
    CVector   m_TrailerRotationDeg;
};

enum class eVehicleAimDirection : std::uint8_t;

class CSimVehiclePuresyncPacket : public CSimPacket
{
public:
    ZERO_ON_NEW
    CSimVehiclePuresyncPacket(ElementID PlayerID, std::uint16_t usPlayerLatency, std::uint8_t ucPlayerSyncTimeContext, bool bPlayerHasOccupiedVehicle,
                              std::uint16_t usVehicleGotModel, std::uint8_t ucPlayerGotOccupiedVehicleSeat, std::uint8_t ucPlayerGotWeaponType, float fPlayerGotWeaponRange,
                              CControllerState& sharedControllerState, std::uint32_t m_uiDamageInfoSendPhase, const SSimVehicleDamageInfo& damageInfo) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_VEHICLE_PURESYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    void ReadVehicleSpecific(NetBitStreamInterface& BitStream) noexcept;
    void WriteVehicleSpecific(NetBitStreamInterface& BitStream) const noexcept;

    bool CanUpdateSync(std::uint8_t ucRemote) noexcept
    {
        // We can update this element's sync only if the sync time
        // matches or either of them are 0 (ignore).
        return (m_ucPlayerSyncTimeContext == ucRemote || ucRemote == 0 || m_ucPlayerSyncTimeContext == 0);
    }

    // Set in constructor
    const ElementID              m_PlayerID;
    const std::uint16_t          m_usPlayerLatency;
    const std::uint8_t           m_ucPlayerSyncTimeContext;
    const bool                   m_bPlayerHasOccupiedVehicle;
    const std::uint16_t          m_usVehicleGotModel;
    const std::uint8_t           m_ucPlayerGotOccupiedVehicleSeat;
    const std::uint8_t           m_ucPlayerGotWeaponType;
    const float                  m_fPlayerGotWeaponRange;
    CControllerState&            m_sharedControllerState;
    const std::uint32_t          m_uiDamageInfoSendPhase;
    const SSimVehicleDamageInfo& m_DamageInfo;

    // Set in Read()
    struct
    {
        std::uint8_t ucTimeContext;

        int iModelID;

        CVector PlrPosition;
        CVector vecCamPosition;
        CVector vecCamFwd;

        CVector VehPosition;            // Same as PlrPosition ?
        CVector VehRotationDeg;
        CVector BothVelocity;
        CVector VehTurnSpeed;

        float fVehHealth;

        std::vector<STrailerInfo> TrailerList;

        float                 fPlrHealth;
        float                 fArmor;
        SVehiclePuresyncFlags flags;

        std::uint8_t ucWeaponSlot;

        std::uint16_t usAmmoInClip;
        std::uint16_t usTotalAmmo;

        float   fAimDirection;
        CVector vecSniperSource;
        CVector vecTargetting;

        eVehicleAimDirection ucDriveByDirection;

        float  fTurretX;
        float  fTurretY;
        std::uint16_t usAdjustableProperty;

        float fRailPosition;
        std::uint8_t ucRailTrack;
        bool  bRailDirection;
        float fRailSpeed;

        SFixedArray<float, 4> fDoorOpenRatio;
    } m_Cache;
};
