/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerSpawnPacket.h
 *  PURPOSE:     Player spawn packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../CCommon.h"
#include "CPacket.h"

class CPlayerSpawnPacket final : public CPacket
{
public:
    CPlayerSpawnPacket();
    CPlayerSpawnPacket(ElementID PlayerID, const CVector& vecSpawnPosition, float fSpawnRotation, unsigned short usPlayerSkin, unsigned char ucInterior,
                       unsigned short usDimension, ElementID Team, unsigned char ucTimeContext);

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_SPAWN; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID      GetPlayer() { return m_PlayerID; };
    CVector&       GetSpawnPosition() { return m_vecSpawnPosition; };
    float          GetSpawnRotation() { return m_fSpawnRotation; };
    unsigned short GetPlayerSkin() { return m_usPlayerSkin; };
    ElementID      GetTeam() { return m_Team; }
    unsigned char  GetInterior() { return m_ucInterior; }
    unsigned short GetDimension() { return m_usDimension; }

    void SetPlayer(ElementID PlayerID) { m_PlayerID = PlayerID; };
    void SetSpawnPosition(CVector& vecPosition) { m_vecSpawnPosition = vecPosition; };
    void SetSpawnRotation(float fRotation) { m_fSpawnRotation = fRotation; };
    void SetPlayerSkin(unsigned short usPlayerSkin) { m_usPlayerSkin = usPlayerSkin; };
    void SetTeam(ElementID TeamID) { m_Team = TeamID; }
    void SetInterior(unsigned char ucInterior) { m_ucInterior = ucInterior; }
    void SetDimension(unsigned short usDimension) { m_usDimension = usDimension; }

private:
    ElementID      m_PlayerID;
    CVector        m_vecSpawnPosition;
    float          m_fSpawnRotation;
    unsigned short m_usPlayerSkin;
    ElementID      m_Team;
    unsigned char  m_ucInterior;
    unsigned short m_usDimension;
    unsigned char  m_ucTimeContext;
};
