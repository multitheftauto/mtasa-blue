/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVehicleTrailerPacket.h
 *  PURPOSE:     Vehicle trailer synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../CCommon.h"
#include "CPacket.h"

class CVehicle;

class CVehicleTrailerPacket final : public CPacket
{
public:
    CVehicleTrailerPacket(){};
    CVehicleTrailerPacket(CVehicle* pVehicle, CVehicle* pTrailer, bool bAttached);

    ePacketID     GetPacketID() const { return PACKET_ID_VEHICLE_TRAILER; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID GetVehicle() { return m_Vehicle; };
    ElementID GetAttachedVehicle() { return m_AttachedVehicle; };
    bool      GetAttached() { return m_bAttached; }
    CVector   GetPosition() { return m_vecPosition; }
    CVector   GetRotationDegrees() { return m_vecRotationDegrees; }
    CVector   GetTurnSpeed() { return m_vecTurnSpeed; }

private:
    ElementID m_Vehicle;
    ElementID m_AttachedVehicle;
    bool      m_bAttached;
    CVector   m_vecPosition;
    CVector   m_vecRotationDegrees;
    CVector   m_vecTurnSpeed;
};
