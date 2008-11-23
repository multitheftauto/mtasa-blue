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
    m_ucAttached = ( bAttached ) ? 1 : 0;
    m_vecPosition = pTrailer->GetPosition ();
    pTrailer->GetRotationDegrees ( m_vecRotationDegrees );
    m_vecTurnSpeed = pTrailer->GetTurnSpeed ();
}

bool CVehicleTrailerPacket::Read ( NetServerBitStreamInterface& BitStream )
{
    return ( BitStream.Read ( m_Vehicle ) &&
             BitStream.Read ( m_AttachedVehicle ) &&
             BitStream.Read ( m_ucAttached ) &&
             ( m_ucAttached == 0 ||
               ( BitStream.Read ( m_vecPosition.fX ) &&
                 BitStream.Read ( m_vecPosition.fY ) &&
                 BitStream.Read ( m_vecPosition.fZ ) &&
                 BitStream.Read ( m_vecRotationDegrees.fX ) &&
                 BitStream.Read ( m_vecRotationDegrees.fY ) &&
                 BitStream.Read ( m_vecRotationDegrees.fZ ) &&
                 BitStream.Read ( m_vecTurnSpeed.fX ) &&
                 BitStream.Read ( m_vecTurnSpeed.fY ) &&
                 BitStream.Read ( m_vecTurnSpeed.fZ ) ) ) );
}


bool CVehicleTrailerPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    BitStream.Write ( m_Vehicle );
    BitStream.Write ( m_AttachedVehicle );
    BitStream.Write ( m_ucAttached );
    BitStream.Write ( m_vecPosition.fX );
    BitStream.Write ( m_vecPosition.fY );
    BitStream.Write ( m_vecPosition.fZ );
    BitStream.Write ( m_vecRotationDegrees.fX );
    BitStream.Write ( m_vecRotationDegrees.fY );
    BitStream.Write ( m_vecRotationDegrees.fZ );
    BitStream.Write ( m_vecTurnSpeed.fX );
    BitStream.Write ( m_vecTurnSpeed.fY );
    BitStream.Write ( m_vecTurnSpeed.fZ );

    return true;
}
