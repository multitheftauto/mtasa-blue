/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPad.h"

struct SSimVehicleDamageInfo
{
    SFixedArray<unsigned char, MAX_DOORS>  m_ucDoorStates;
    SFixedArray<unsigned char, MAX_WHEELS> m_ucWheelStates;
    SFixedArray<unsigned char, MAX_PANELS> m_ucPanelStates;
    SFixedArray<unsigned char, MAX_LIGHTS> m_ucLightStates;
};

//
// Copy of enough data from CPlayer to enable autonomous relaying of pure sync packets
//
class CSimPlayer
{
public:
    ZERO_ON_NEW
    CSimPlayer() { DEBUG_CREATE_COUNT("CSimPlayer"); }
    ~CSimPlayer() { DEBUG_DESTROY_COUNT("CSimPlayer"); }

    bool                                      IsJoined() { return m_bIsJoined; };
    const std::multimap<ushort, CSimPlayer*>& GetPuresyncSendList();
    unsigned short                            GetBitStreamVersion() { return m_usBitStreamVersion; };
    NetServerPlayerID&                        GetSocket() { return m_PlayerSocket; };

    // General synced vars
    bool                               m_bIsJoined;
    unsigned short                     m_usBitStreamVersion;
    NetServerPlayerID                  m_PlayerSocket;
    std::vector<CSimPlayer*>           m_PuresyncSendListFlat;
    std::multimap<ushort, CSimPlayer*> m_PuresyncSendListGrouped;  // Send list grouped by bitstream version
    bool                               m_bSendListChanged;
    bool                               m_bHasOccupiedVehicle;
    bool                               m_bIsExitingVehicle;
    CControllerState                   m_sharedControllerState;  // Updated by CSim*Packet code

    // Used in CSimPlayerPuresyncPacket and CSimVehiclePuresyncPacket
    ElementID m_PlayerID;
    ushort    m_usLatency;
    uchar     m_ucSyncTimeContext;
    uchar     m_ucWeaponType;
    bool      m_hasJetPack;

    // Used in CSimVehiclePuresyncPacket
    ushort                m_usVehicleModel;
    uchar                 m_ucOccupiedVehicleSeat;
    float                 m_fWeaponRange;
    uint                  m_uiVehicleDamageInfoSendPhase;
    SSimVehicleDamageInfo m_VehicleDamageInfo;

    // Used in bullet sync validation (HandleBulletSync)
    CVector      m_vecPosition;
    CElapsedTime m_BulletSyncRateTimer;

    // Bullet sync validation snapshot, refreshed by UpdateSimPlayer on the
    // main thread. The sim thread must not read live player state, so the
    // sim path's possession, ammo and range checks run against these copies.
    static constexpr std::size_t WEAPON_SLOT_COUNT = 13;  // Matches WEAPON_SLOTS in CPed.h
    bool                         m_bIsSpawned{};
    bool                         m_bIsDead{};
    unsigned char                m_WeaponTypes[WEAPON_SLOT_COUNT]{};
    unsigned short               m_WeaponTotalAmmo[WEAPON_SLOT_COUNT]{};
    // Widest-tier range per weapon slot, for the range gate in
    // HandleBulletSync. The gate measures the packet's weapon, so keying by
    // slot keeps a shooter from relaying shots the main path rejects when
    // the held weapon outranges the fired one, and from dropping shots of a
    // weapon selected after the last refresh. Computed on the main thread
    // because the sim thread cannot consult the mutable weapon stat tables.
    float m_fBulletSyncRangeHighest[WEAPON_SLOT_COUNT]{};

    // Used in CSimKeysyncPacket
    bool m_bVehicleHasHydraulics;
    bool m_bVehicleIsPlaneOrHeli;

    // Flag
    bool m_bDoneFirstUpdate;

    // Interlink to CPlayer object
    CPlayer* m_pRealPlayer;
};
