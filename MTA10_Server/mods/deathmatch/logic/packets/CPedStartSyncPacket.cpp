/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPedStartSyncPacket.cpp
*  PURPOSE:     Ped start synchronization packet class
*  DEVELOPERS:  lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CPedStartSyncPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    if ( !m_pPed ) return false;

    BitStream.Write ( m_pPed->GetID () );

    CVector vecTemp;
    float fTemp = 0.0f;

    vecTemp = m_pPed->GetPosition ();
    BitStream.Write ( vecTemp.fX );
    BitStream.Write ( vecTemp.fY );
    BitStream.Write ( vecTemp.fZ );

    BitStream.Write ( m_pPed->GetRotation () );

    m_pPed->GetVelocity ( vecTemp );
    BitStream.Write ( vecTemp.fX );
    BitStream.Write ( vecTemp.fY );
    BitStream.Write ( vecTemp.fZ );

    BitStream.Write ( m_pPed->GetHealth () );
    BitStream.Write ( m_pPed->GetArmor () );

    return true;
}
