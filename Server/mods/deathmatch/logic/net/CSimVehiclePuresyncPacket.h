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

enum class eVehicleAimDirection : unsigned char;

class CSimVehiclePuresyncPacket : public CSimPacket
{
public:
    ZERO_ON_NEW
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
    const ushort                 m_usPlayerLatency;
    const uchar                  m_ucPlayerSyncTimeContext;
    const bool                   m_bPlayerHasOccupiedVehicle;
    const ushort                 m_usVehicleGotModel;
    const uchar                  m_ucPlayerGotOccupiedVehicleSeat;
    const uchar                  m_ucPlayerGotWeaponType;
    const float                  m_fPlayerGotWeaponRange;
    CControllerState&            m_sharedControllerState;
    const uint                   m_uiDamageInfoSendPhase;
    const SSimVehicleDamageInfo& m_DamageInfo;

    // Set in Read()
    struct
    {
        uchar ucTimeContext;

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

        uchar ucWeaponSlot;

        ushort usAmmoInClip;
        ushort usTotalAmmo;

        float   fAimDirection;
        CVector vecSniperSource;
        CVector vecTargetting;

        eVehicleAimDirection ucDriveByDirection;

        float  fTurretX;
        float  fTurretY;
        ushort usAdjustableProperty;

        float fRailPosition;
        uchar ucRailTrack;
        bool  bRailDirection;
        float fRailSpeed;

        SFixedArray<float, 4> fDoorOpenRatio;

        bool isOnFire{false};
    } m_Cache;
};
