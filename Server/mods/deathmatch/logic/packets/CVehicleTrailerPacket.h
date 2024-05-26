/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVehicleTrailerPacket.h
 *  PURPOSE:     Vehicle trailer synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../CCommon.h"
#include "CPacket.h"

class CVehicle;

class CVehicleTrailerPacket final : public CPacket
{
public:
    CVehicleTrailerPacket() noexcept {};
    CVehicleTrailerPacket(CVehicle* pVehicle, CVehicle* pTrailer, bool bAttached) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_VEHICLE_TRAILER; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    ElementID GetVehicle() const noexcept { return m_Vehicle; }
    ElementID GetAttachedVehicle() const noexcept { return m_AttachedVehicle; }
    bool      GetAttached() const noexcept { return m_bAttached; }
    CVector   GetPosition() const noexcept { return m_vecPosition; }
    CVector   GetRotationDegrees() const noexcept { return m_vecRotationDegrees; }
    CVector   GetTurnSpeed() const noexcept { return m_vecTurnSpeed; }

private:
    ElementID m_Vehicle;
    ElementID m_AttachedVehicle;
    bool      m_bAttached;
    CVector   m_vecPosition;
    CVector   m_vecRotationDegrees;
    CVector   m_vecTurnSpeed;
};
