/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerModInfoPacket.cpp
*  PURPOSE:
*  DEVELOPERS:  bidiot
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CPlayerModInfoPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Read type
    if ( !BitStream.ReadString ( m_strInfoType ) )
        return false;

    // Read amount of items
    ushort usCount;
    if ( !BitStream.Read ( usCount ) )
        return false;

    // Read each item
    for ( uint i = 0 ; i < usCount ; i++ )
    {
        ushort usId;
        if ( !BitStream.Read ( usId ) )
            return false;

        SString strName;
        if ( !BitStream.ReadString ( strName ) )
            return false;

        m_IdList.push_back ( usId );
        m_NameList.push_back ( strName );
    }

    return true;
}
