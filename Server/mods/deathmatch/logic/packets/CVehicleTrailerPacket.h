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

#include "../CVehicle.h"
#include "../CCommon.h"
#include "CPacket.h"

class CVehicleTrailerPacket : public CPacket
{
public:
    CVehicleTrailerPacket(void){};
    CVehicleTrailerPacket(CVehicle* pVehicle, CVehicle* pTrailer, bool bAttached);

    ePacketID     GetPacketID(void) const { return PACKET_ID_VEHICLE_TRAILER; };
    unsigned long GetFlags(void) const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID GetVehicle(void) { return m_Vehicle; };
    ElementID GetAttachedVehicle(void) { return m_AttachedVehicle; };
    bool      GetAttached(void) { return m_bAttached; }
    CVector   GetPosition(void) { return m_vecPosition; }
    CVector   GetRotationDegrees(void) { return m_vecRotationDegrees; }
    CVector   GetTurnSpeed(void) { return m_vecTurnSpeed; }

private:
    ElementID m_Vehicle;
    ElementID m_AttachedVehicle;
    bool      m_bAttached;
    CVector   m_vecPosition;
    CVector   m_vecRotationDegrees;
    CVector   m_vecTurnSpeed;
};
