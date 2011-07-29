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
    uint uiCount;
    if ( !BitStream.Read ( uiCount ) )
        return false;

    // Read each item
    for ( uint i = 0 ; i < uiCount ; i++ )
    {
        SModInfoItem item;

        if ( !BitStream.Read ( item.usId ) )
            return false;

        if ( !BitStream.Read ( item.uiHash ) )
            return false;

        if ( !BitStream.ReadString ( item.strName ) )
            return false;

        int iHasSize;
        if ( !BitStream.Read ( iHasSize ) )
            return false;
        item.bHasSize = iHasSize != 0;

        if ( !BitStream.Read ( item.vecSize.fX ) )
            return false;

        if ( !BitStream.Read ( item.vecSize.fY ) )
            return false;

        if ( !BitStream.Read ( item.vecSize.fZ ) )
            return false;

        m_ModInfoItemList.push_back ( item );
    }

    return true;
}
