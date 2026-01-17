/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVehicleInOutPacket.h
 *  PURPOSE:     Vehicle enter/exit synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>

class CVehicleInOutPacket final : public CPacket
{
public:
    CVehicleInOutPacket();
    CVehicleInOutPacket(ElementID PedID, ElementID VehicleID, unsigned char ucSeat, unsigned char ucAction);
    CVehicleInOutPacket(ElementID PedID, ElementID VehicleID, unsigned char ucSeat, unsigned char ucAction, unsigned char ucDoor);
    CVehicleInOutPacket(ElementID PedID, ElementID VehicleID, unsigned char ucSeat, unsigned char ucAction, ElementID PedIn, ElementID PedOut);
    virtual ~CVehicleInOutPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_VEHICLE_INOUT; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    unsigned char GetDoor() { return m_ucDoor; };
    bool          GetOnWater() { return m_bOnWater; };
    ElementID     GetVehicleID() { return m_VehicleID; };
    ElementID     GetPedID() { return m_PedID; };
    unsigned char GetSeat() { return m_ucSeat; };
    float         GetDoorAngle() { return m_fDoorAngle; }
    unsigned char GetAction() { return m_ucAction; };
    ElementID     GetPedIn() { return m_PedIn; };
    ElementID     GetPedOut() { return m_PedOut; };
    unsigned char GetStartedJacking() { return m_ucStartedJacking; };

    void SetPedID(ElementID ID) { m_PedID = ID; };
    void SetVehicleID(ElementID ID) { m_VehicleID = ID; };
    void SetSeat(unsigned char ucSeat) { m_ucSeat = ucSeat; };
    void SetDoor(unsigned char ucDoor) { m_ucDoor = ucDoor; }
    void SetDoorAngle(float fDoorAngle) { m_fDoorAngle = fDoorAngle; }
    void SetAction(unsigned char ucAction) { m_ucAction = ucAction; };
    void SetPedIn(ElementID PedIn) { m_PedIn = PedIn; };
    void SetPedOut(ElementID PedOut) { m_PedOut = PedOut; };
    void SetFailReason(unsigned char ucReason) { m_ucFailReason = ucReason; }
    void SetCorrectVector(const CVector& vector) { m_pCorrectVector = new CVector(vector.fX, vector.fY, vector.fZ); }

private:
    ElementID     m_PedID;                       // The ped
    ElementID     m_VehicleID;                   // The vehicle
    unsigned char m_ucSeat;                      // The seat
    unsigned char m_ucAction;                    // The action, see CGame.h
    ElementID     m_PedIn;                       // The ped jacking
    ElementID     m_PedOut;                      // The ped getting jacked
    unsigned char m_ucStartedJacking;            // 1 = client reports he started jacking
    unsigned char m_ucFailReason;                // eFailReasons
    CVector*      m_pCorrectVector;              // Ped position reported by client
    bool          m_bOnWater;                    // Vehicle in water reported by client
    unsigned char m_ucDoor;                      // Door ID
    float         m_fDoorAngle;                  // Door angle
};
