/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CObjectStartSyncPacket.cpp
*  PURPOSE:     Object start sync packet class
*  DEVELOPERS:  Stanislav Izmalkov <izstas@live.ru>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CObjectStartSyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    if ( !m_pObject ) return false;

    // Write the ID
    BitStream.Write ( m_pObject->GetID () );

    // Write the position
    SPositionSync position;
    position.data.vecPosition = m_pObject->GetPosition ();
    BitStream.Write ( &position );

    // Write the rotation
    SRotationRadiansSync rotation;
    m_pObject->GetRotation ( rotation.data.vecRotation );
    BitStream.Write ( &rotation );

    // Write the health
    SObjectHealthSync health;
    health.data.fValue = m_pObject->GetHealth ();
    BitStream.Write ( &health );

    return true;
}
