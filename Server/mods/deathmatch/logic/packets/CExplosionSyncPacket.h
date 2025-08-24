/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CExplosionSyncPacket.h
 *  PURPOSE:     Explosion synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>

class CExplosionSyncPacket final : public CPacket
{
public:
    enum eExplosionType
    {
        EXPLOSION_GRENADE,
        EXPLOSION_MOLOTOV,
        EXPLOSION_ROCKET,
        EXPLOSION_ROCKET_WEAK,
        EXPLOSION_CAR,
        EXPLOSION_CAR_QUICK,
        EXPLOSION_BOAT,
        EXPLOSION_HELI,
        EXPLOSION_MINE,
        EXPLOSION_OBJECT,
        EXPLOSION_TANK_GRENADE,
        EXPLOSION_SMALL,
        EXPLOSION_TINY,
    };

    CExplosionSyncPacket();
    CExplosionSyncPacket(const CVector& vecPosition, unsigned char ucType);

    ePacketID     GetPacketID() const { return PACKET_ID_EXPLOSION; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID     m_OriginID;
    CVector       m_vecPosition;
    unsigned char m_ucType;

    bool m_isVehicleResponsible = false;
    bool m_blowVehicleWithoutExplosion = false;
};
