/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleInOutPacket.cpp
*  PURPOSE:     Vehicle enter/exit synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVehicleInOutPacket::CVehicleInOutPacket ( void )
{
    m_ID = INVALID_ELEMENT_ID;
    m_ucSeat = 0;
    m_ucAction = 0;
    m_ucFailReason = 0xFF;
    m_pCorrectVector = NULL;
    m_bOnWater = false;
    m_ucDoor = 0;
}


CVehicleInOutPacket::CVehicleInOutPacket ( ElementID ID,
                                           unsigned char ucSeat,
                                           unsigned char ucAction)
{
    m_ID = ID;
    m_ucSeat = ucSeat;
    m_ucAction = ucAction;
    m_ucFailReason = 0xFF;
    m_pCorrectVector = NULL;
    m_bOnWater = false;
    m_ucDoor = 0;
}

CVehicleInOutPacket::CVehicleInOutPacket ( ElementID ID,
                                           unsigned char ucSeat,
                                           unsigned char ucAction,
                                           unsigned char ucDoor)
{
    m_ID = ID;
    m_ucSeat = ucSeat;
    m_ucAction = ucAction;
    m_ucFailReason = 0xFF;
    m_pCorrectVector = NULL;
    m_bOnWater = false;
    m_ucDoor = ucDoor;
}

CVehicleInOutPacket::CVehicleInOutPacket ( ElementID ID,
                                           unsigned char ucSeat,
                                           unsigned char ucAction,
                                           ElementID PlayerIn,
                                           ElementID PlayerOut )
{
    m_ID = ID;
    m_ucSeat = ucSeat;
    m_ucAction = ucAction;
    m_PlayerIn = PlayerIn;
    m_PlayerOut = PlayerOut;
    m_ucStartedJacking = 0;
    m_ucFailReason = 0xFF;
    m_pCorrectVector = NULL;
    m_bOnWater = false;
    m_ucDoor = 0;
}


CVehicleInOutPacket::~CVehicleInOutPacket ( void )
{
    if ( m_pCorrectVector )
    {
        delete m_pCorrectVector;
        m_pCorrectVector = NULL;
    }
}


bool CVehicleInOutPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Read out the vehicle id
    m_ID = INVALID_ELEMENT_ID;
    BitStream.Read ( m_ID );
    if ( m_ID == INVALID_ELEMENT_ID )
    {
        return false;
    }

    // Read out the action id
    m_ucAction = 0xFF;
    if ( !BitStream.ReadBits ( &m_ucAction, 4 ) )
        return false;

    // If the action is requesting to get in, read out the "passenger" flag too
    if ( m_ucAction == CGame::VEHICLE_REQUEST_IN )
    {
        return BitStream.ReadBits ( &m_ucSeat, 3 ) &&
               BitStream.ReadBit  ( m_bOnWater ) &&
               BitStream.ReadBits ( &m_ucDoor, 3 );
    }
    else if ( m_ucAction == CGame::VEHICLE_NOTIFY_JACK_ABORT )
    {
        SDoorOpenRatioSync door;
        bool bStartedJacking;
        if ( BitStream.ReadBits ( &m_ucDoor, 3 ) &&
             BitStream.Read ( &door ) &&
             BitStream.ReadBit ( bStartedJacking ) )
        {
            m_ucStartedJacking = bStartedJacking;
            m_fDoorAngle = door.data.fRatio;
        }
        else
            return false;
    }
    else if ( m_ucAction == CGame::VEHICLE_NOTIFY_IN_ABORT )
    {
        SDoorOpenRatioSync door;
        if ( BitStream.ReadBits ( &m_ucDoor, 3 ) &&
             BitStream.Read ( &door ) )
        {
            m_fDoorAngle = door.data.fRatio;
        }
        else
            return false;
    }
    return true;
}


bool CVehicleInOutPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    if ( m_pSourceElement && m_ID != INVALID_ELEMENT_ID )
    {
        ElementID ID = m_pSourceElement->GetID ();
        BitStream.Write ( ID );

        BitStream.Write ( m_ID );
        BitStream.WriteBits ( &m_ucSeat, 3 );
        BitStream.WriteBits ( &m_ucAction, 4 );

        if ( m_ucAction == CGame::VEHICLE_REQUEST_IN_CONFIRMED || m_ucAction == CGame::VEHICLE_REQUEST_JACK_CONFIRMED )
        {
            BitStream.WriteBits ( &m_ucDoor, 3 );
        }
        // If the action id is VEHICLE_NOTIFY_JACK_RETURN, send the in/out player chars aswell
        if ( m_ucAction == CGame::VEHICLE_NOTIFY_JACK_RETURN )
        {
            BitStream.Write ( m_PlayerIn );
            BitStream.Write ( m_PlayerOut );
        }

        if ( m_ucAction == 9 /*VEHICLE_ATTEMPT_FAILED*/ )
        {
            BitStream.Write ( m_ucFailReason );
            if ( m_ucFailReason == 5 /*FAIL_DISTANCE*/ && m_pCorrectVector )
            {
                SPositionSync pos ( false );
                pos.data.vecPosition = *m_pCorrectVector;
                BitStream.Write ( &pos );
            }
        }

        if ( m_ucAction == CGame::VEHICLE_NOTIFY_IN_ABORT_RETURN )
        {
            BitStream.WriteBits ( &m_ucDoor, 3 );
            SDoorOpenRatioSync door;
            door.data.fRatio = m_fDoorAngle;
            BitStream.Write ( &door );
        }

        return true;
    }

    return false;
}
