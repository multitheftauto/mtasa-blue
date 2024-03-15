/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVehicleInOutPacket.h
 *  PURPOSE:     Vehicle enter/exit synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>

class CVehicleInOutPacket final : public CPacket
{
public:
    CVehicleInOutPacket();
    CVehicleInOutPacket(ElementID PedID, ElementID VehicleID, std::uint8_t ucSeat, std::uint8_t ucAction);
    CVehicleInOutPacket(ElementID PedID, ElementID VehicleID, std::uint8_t ucSeat, std::uint8_t ucAction, std::uint8_t ucDoor);
    CVehicleInOutPacket(ElementID PedID, ElementID VehicleID, std::uint8_t ucSeat, std::uint8_t ucAction, ElementID PedIn, ElementID PedOut);
    virtual ~CVehicleInOutPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_VEHICLE_INOUT; };
    std::uint32_t GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    std::uint8_t GetDoor() { return m_ucDoor; };
    bool          GetOnWater() { return m_bOnWater; };
    ElementID     GetVehicleID() { return m_VehicleID; };
    ElementID     GetPedID() { return m_PedID; };
    std::uint8_t GetSeat() { return m_ucSeat; };
    float         GetDoorAngle() { return m_fDoorAngle; }
    std::uint8_t GetAction() { return m_ucAction; };
    ElementID     GetPedIn() { return m_PedIn; };
    ElementID     GetPedOut() { return m_PedOut; };
    std::uint8_t GetStartedJacking() { return m_ucStartedJacking; };

    void SetPedID(ElementID ID) { m_PedID = ID; };
    void SetVehicleID(ElementID ID) { m_VehicleID = ID; };
    void SetSeat(std::uint8_t ucSeat) { m_ucSeat = ucSeat; };
    void SetDoor(std::uint8_t ucDoor) { m_ucDoor = ucDoor; }
    void SetDoorAngle(float fDoorAngle) { m_fDoorAngle = fDoorAngle; }
    void SetAction(std::uint8_t ucAction) { m_ucAction = ucAction; };
    void SetPedIn(ElementID PedIn) { m_PedIn = PedIn; };
    void SetPedOut(ElementID PedOut) { m_PedOut = PedOut; };
    void SetFailReason(std::uint8_t ucReason) { m_ucFailReason = ucReason; }
    void SetCorrectVector(const CVector& vector) { m_pCorrectVector = new CVector(vector.fX, vector.fY, vector.fZ); }

private:
    ElementID     m_PedID;                       // The ped
    ElementID     m_VehicleID;                   // The vehicle
    std::uint8_t m_ucSeat;                      // The seat
    std::uint8_t m_ucAction;                    // The action, see CGame.h
    ElementID     m_PedIn;                       // The ped jacking
    ElementID     m_PedOut;                      // The ped getting jacked
    std::uint8_t m_ucStartedJacking;            // 1 = client reports he started jacking
    std::uint8_t m_ucFailReason;                // eFailReasons
    CVector*      m_pCorrectVector;              // Ped position reported by client
    bool          m_bOnWater;                    // Vehicle in water reported by client
    std::uint8_t m_ucDoor;                      // Door ID
    float         m_fDoorAngle;                  // Door angle
};
