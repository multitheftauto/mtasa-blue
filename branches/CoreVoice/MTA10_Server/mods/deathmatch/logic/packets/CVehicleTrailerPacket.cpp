/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleTrailerPacket.cpp
*  PURPOSE:     Vehicle trailer synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVehicleTrailerPacket::CVehicleTrailerPacket ( CVehicle* pVehicle,
                                               CVehicle* pTrailer,
                                               bool bAttached )
{
    m_Vehicle = pVehicle->GetID ();
    m_AttachedVehicle = pTrailer->GetID ();
    m_bAttached = bAttached;
    m_vecPosition = pTrailer->GetPosition ();
    pTrailer->GetRotationDegrees ( m_vecRotationDegrees );
    m_vecTurnSpeed = pTrailer->GetTurnSpeed ();
}

bool CVehicleTrailerPacket::Read ( NetBitStreamInterface& BitStream )
{
    SPositionSync position ( false );
    SRotationDegreesSync rotation ( false );
    SVelocitySync turn;

    if ( BitStream.Read ( m_Vehicle ) &&
         BitStream.Read ( m_AttachedVehicle ) &&
         BitStream.ReadBit ( m_bAttached ) &&
         ( !m_bAttached ||
           ( BitStream.Read ( &position ) &&
             BitStream.Read ( &rotation ) &&
             BitStream.Read ( &turn )
           )
         )
       )
    {
        if ( m_bAttached )
        {
            m_vecPosition = position.data.vecPosition;
            m_vecRotationDegrees = rotation.data.vecRotation;
            m_vecTurnSpeed = turn.data.vecVelocity;
        }
        return true;
    }
    return false;
}


bool CVehicleTrailerPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    BitStream.Write ( m_Vehicle );
    BitStream.Write ( m_AttachedVehicle );
    BitStream.WriteBit ( m_bAttached );

    if ( m_bAttached )
    {
        SPositionSync position ( false );
        position.data.vecPosition = m_vecPosition;
        BitStream.Write ( &position );

        SRotationDegreesSync rotation ( false );
        rotation.data.vecRotation = m_vecRotationDegrees;
        BitStream.Write ( &rotation );

        SVelocitySync turn;
        turn.data.vecVelocity = m_vecTurnSpeed;
        BitStream.Write ( &turn );
    }

    return true;
}
