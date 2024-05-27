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

class CSimPacket
{
public:
    CSimPacket() noexcept { DEBUG_CREATE_COUNT("CSimPacket"); }
    virtual ~CSimPacket() noexcept { DEBUG_DESTROY_COUNT("CSimPacket"); }

    virtual ePacketID       GetPacketID() const noexcept = 0;
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_DEFAULT; }
    virtual std::uint32_t   GetFlags() const noexcept = 0;

    virtual bool Read(NetBitStreamInterface& BitStream) noexcept { return false; }
    virtual bool Write(NetBitStreamInterface& BitStream) const noexcept{ return false; }
};

//
// For bouncing a player pure sync packet
//
class CSimPlayerPuresyncPacket : public CSimPacket
{
public:
    ZERO_ON_NEW

    CSimPlayerPuresyncPacket(ElementID PlayerID, std::uint16_t PlayerLatency,
        std::uint8_t PlayerSyncTimeContext, std::uint8_t PlayerGotWeaponType,
        float WeaponRange, CControllerState& sharedControllerState) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_PURESYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    bool CanUpdateSync(std::uint8_t ucRemote) noexcept
    {
        // We can update this element's sync only if the sync time
        // matches or either of them are 0 (ignore).
        return (m_PlayerSyncTimeContext == ucRemote || ucRemote == 0 || m_PlayerSyncTimeContext == 0);
    }

    // Set in constructor
    const ElementID   m_PlayerID;
    const std::uint16_t      m_PlayerLatency;
    const std::uint8_t       m_PlayerSyncTimeContext;
    const std::uint8_t       m_PlayerGotWeaponType;
    const float       m_WeaponRange;
    CControllerState& m_sharedControllerState;

    // Set in Read ()
    struct
    {
        std::uint8_t                ucTimeContext;
        SPlayerPuresyncFlags flags;
        ElementID            ContactElementID;
        CVector              Position;
        float                fRotation;
        CVector              Velocity;
        float                fHealth;
        float                fArmor;
        float                fCameraRotation;
        CVector              vecCamPosition;
        CVector              vecCamFwd;

        bool   bWeaponCorrect;
        std::uint8_t  ucWeaponSlot;            // Only valid if bWeaponCorrect
        std::uint16_t usAmmoInClip;            // Only valid if bWeaponCorrect
        std::uint16_t usTotalAmmo;             // Only valid if bWeaponCorrect

        bool    bIsAimFull;
        float   fAimDirection;              // Only valid if bWeaponCorrect
        CVector vecSniperSource;            // Only valid if bWeaponCorrect and bIsAimFull
        CVector vecTargetting;              // Only valid if bWeaponCorrect and bIsAimFull
    } m_Cache;
};
