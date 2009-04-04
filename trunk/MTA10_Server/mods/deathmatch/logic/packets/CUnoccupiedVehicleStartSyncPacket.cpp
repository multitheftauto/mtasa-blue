/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CUnoccupiedVehicleStartSyncPacket.cpp
*  PURPOSE:     Unoccupied vehicle start synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CUnoccupiedVehicleStartSyncPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    if ( !m_pVehicle ) return false;

    BitStream.Write ( m_pVehicle->GetID () );

    CVector vecTemp;
    vecTemp = m_pVehicle->GetPosition ();
    BitStream.Write ( vecTemp.fX );
    BitStream.Write ( vecTemp.fY );
    BitStream.Write ( vecTemp.fZ );

    m_pVehicle->GetRotationDegrees ( vecTemp );
    BitStream.Write ( vecTemp.fX );
    BitStream.Write ( vecTemp.fY );
    BitStream.Write ( vecTemp.fZ );

    vecTemp = m_pVehicle->GetVelocity ();
    BitStream.Write ( vecTemp.fX );
    BitStream.Write ( vecTemp.fY );
    BitStream.Write ( vecTemp.fZ );

    vecTemp = m_pVehicle->GetTurnSpeed ();
    BitStream.Write ( vecTemp.fX );
    BitStream.Write ( vecTemp.fY );
    BitStream.Write ( vecTemp.fZ );

    BitStream.Write ( m_pVehicle->GetHealth () );

    return true;
}
