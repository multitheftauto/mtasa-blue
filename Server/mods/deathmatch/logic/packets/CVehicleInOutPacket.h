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

class CVehicleInOutPacket : public CPacket
{
public:
    CVehicleInOutPacket();
    CVehicleInOutPacket(ElementID ID, unsigned char ucSeat, unsigned char ucAction);
    CVehicleInOutPacket(ElementID ID, unsigned char ucSeat, unsigned char ucAction, unsigned char ucDoor);
    CVehicleInOutPacket(ElementID ID, unsigned char ucSeat, unsigned char ucAction, ElementID PlayerIn, ElementID PlayerOut);
    virtual ~CVehicleInOutPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_VEHICLE_INOUT; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    unsigned char GetDoor() { return m_ucDoor; };
    bool          GetOnWater() { return m_bOnWater; };
    ElementID     GetID() { return m_ID; };
    unsigned char GetSeat() { return m_ucSeat; };
    float         GetDoorAngle() { return m_fDoorAngle; }
    unsigned char GetAction() { return m_ucAction; };
    ElementID     GetPlayerIn() { return m_PlayerIn; };
    ElementID     GetPlayerOut() { return m_PlayerOut; };
    unsigned char GetStartedJacking() { return m_ucStartedJacking; };

    void SetID(ElementID ID) { m_ID = ID; };
    void SetSeat(unsigned char ucSeat) { m_ucSeat = ucSeat; };
    void SetDoor(unsigned char ucDoor) { m_ucDoor = ucDoor; }
    void SetDoorAngle(float fDoorAngle) { m_fDoorAngle = fDoorAngle; }
    void SetAction(unsigned char ucAction) { m_ucAction = ucAction; };
    void SetPlayerIn(ElementID PlayerIn) { m_PlayerIn = PlayerIn; };
    void SetPlayerOut(ElementID PlayerOut) { m_PlayerOut = PlayerOut; };
    void SetFailReason(unsigned char ucReason) { m_ucFailReason = ucReason; }
    void SetCorrectVector(const CVector& vector) { m_pCorrectVector = new CVector(vector.fX, vector.fY, vector.fZ); }

private:
    ElementID     m_ID;
    unsigned char m_ucSeat;
    unsigned char m_ucAction;
    ElementID     m_PlayerIn;
    ElementID     m_PlayerOut;
    unsigned char m_ucStartedJacking;
    unsigned char m_ucFailReason;
    CVector*      m_pCorrectVector;
    bool          m_bOnWater;
    unsigned char m_ucDoor;
    float         m_fDoorAngle;
};
