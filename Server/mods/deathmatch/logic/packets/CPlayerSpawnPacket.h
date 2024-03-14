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
    CPlayerSpawnPacket();
    CPlayerSpawnPacket(ElementID PlayerID, const CVector& vecSpawnPosition, float fSpawnRotation, std::uint16_t usPlayerSkin, std::uint8_t ucInterior,
                       std::uint16_t usDimension, ElementID Team, std::uint8_t ucTimeContext);

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_SPAWN; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID      GetPlayer() { return m_PlayerID; };
    CVector&       GetSpawnPosition() { return m_vecSpawnPosition; };
    float          GetSpawnRotation() { return m_fSpawnRotation; };
    std::uint16_t GetPlayerSkin() { return m_usPlayerSkin; };
    ElementID      GetTeam() { return m_Team; }
    std::uint8_t  GetInterior() { return m_ucInterior; }
    std::uint16_t GetDimension() { return m_usDimension; }

    void SetPlayer(ElementID PlayerID) { m_PlayerID = PlayerID; };
    void SetSpawnPosition(CVector& vecPosition) { m_vecSpawnPosition = vecPosition; };
    void SetSpawnRotation(float fRotation) { m_fSpawnRotation = fRotation; };
    void SetPlayerSkin(std::uint16_t usPlayerSkin) { m_usPlayerSkin = usPlayerSkin; };
    void SetTeam(ElementID TeamID) { m_Team = TeamID; }
    void SetInterior(std::uint8_t ucInterior) { m_ucInterior = ucInterior; }
    void SetDimension(std::uint16_t usDimension) { m_usDimension = usDimension; }

private:
    ElementID      m_PlayerID;
    CVector        m_vecSpawnPosition;
    float          m_fSpawnRotation;
    std::uint16_t m_usPlayerSkin;
    ElementID      m_Team;
    std::uint8_t  m_ucInterior;
    std::uint16_t m_usDimension;
    std::uint8_t  m_ucTimeContext;
};
