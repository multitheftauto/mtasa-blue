/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerSpawnPacket.h
 *  PURPOSE:     Player spawn packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../CCommon.h"
#include "CPacket.h"

class CPlayerSpawnPacket final : public CPacket
{
public:
    CPlayerSpawnPacket() noexcept;
    CPlayerSpawnPacket(ElementID PlayerID, const CVector& vecSpawnPosition, float fSpawnRotation, std::uint16_t usPlayerSkin, std::uint8_t ucInterior,
                       std::uint16_t usDimension, ElementID Team, std::uint8_t ucTimeContext) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_SPAWN; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    ElementID      GetPlayer() const noexcept { return m_PlayerID; }
    CVector&       GetSpawnPosition() noexcept { return m_vecSpawnPosition; }
    const CVector& GetSpawnPosition() const noexcept { return m_vecSpawnPosition; }
    float          GetSpawnRotation() const noexcept { return m_fSpawnRotation; }
    std::uint16_t  GetPlayerSkin() const noexcept { return m_usPlayerSkin; }
    ElementID      GetTeam() const noexcept { return m_Team; }
    std::uint8_t   GetInterior() const noexcept { return m_ucInterior; }
    std::uint16_t  GetDimension() const noexcept { return m_usDimension; }

    void SetPlayer(const ElementID PlayerID) noexcept { m_PlayerID = PlayerID; }
    void SetSpawnPosition(const CVector& vecPosition) noexcept { m_vecSpawnPosition = vecPosition; }
    void SetSpawnRotation(const float fRotation) noexcept { m_fSpawnRotation = fRotation; }
    void SetPlayerSkin(const std::uint16_t usPlayerSkin) noexcept { m_usPlayerSkin = usPlayerSkin; }
    void SetTeam(const ElementID TeamID) noexcept { m_Team = TeamID; }
    void SetInterior(const std::uint8_t ucInterior) noexcept { m_ucInterior = ucInterior; }
    void SetDimension(const std::uint16_t usDimension) noexcept { m_usDimension = usDimension; }

private:
    CVector       m_vecSpawnPosition;
    ElementID     m_PlayerID{0};
    float         m_fSpawnRotation{0};
    std::uint16_t m_usPlayerSkin{0};
    ElementID     m_Team{0};
    std::uint8_t  m_ucInterior{0};
    std::uint16_t m_usDimension{0};
    std::uint8_t  m_ucTimeContext{0};
};
