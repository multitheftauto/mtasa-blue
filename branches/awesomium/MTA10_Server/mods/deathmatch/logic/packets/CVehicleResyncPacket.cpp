/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleResyncPacket.cpp
*  PURPOSE:     Vehicle re-synchronization packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

bool CVehicleResyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Only the server sends these.
    return false;
}

bool CVehicleResyncPacket::Write ( NetBitStreamInterface& BitStream ) const 
{
    if ( !m_pVehicle )
        return false;

    BitStream.Write ( m_pVehicle->GetID() );

    // Write vehicle position and rotation
    SPositionSync position ( false );
    position.data.vecPosition = m_pVehicle->GetPosition();
    BitStream.Write ( &position );

    SRotationDegreesSync rotation;
    m_pVehicle->GetRotationDegrees ( rotation.data.vecRotation );
    BitStream.Write ( &rotation );

    // Read out the movespeed
    SVelocitySync velocity;
    velocity.data.vecVelocity = m_pVehicle->GetVelocity ();
    BitStream.Write ( &velocity );

    // Read out the turnspeed
    SVelocitySync turnSpeed;
    turnSpeed.data.vecVelocity = m_pVehicle->GetTurnSpeed ();
    BitStream.Write ( &turnSpeed );

    // Read out the vehicle health
    SVehicleHealthSync health;
    health.data.fValue = m_pVehicle->GetHealth ();
    BitStream.Write ( &health );

    return true;
}
