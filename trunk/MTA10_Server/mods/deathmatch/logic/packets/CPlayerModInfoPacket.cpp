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

        BitStream.Read ( item.usId );
        BitStream.Read ( item.uiHash );
        BitStream.ReadString ( item.strName );

        int iHasSize;
        BitStream.Read ( iHasSize );
        item.bHasSize = iHasSize != 0;

        BitStream.Read ( item.vecSize.fX );
        BitStream.Read ( item.vecSize.fY );
        BitStream.Read ( item.vecSize.fZ );

        BitStream.Read ( item.vecOriginalSize.fX );
        BitStream.Read ( item.vecOriginalSize.fY );
        BitStream.Read ( item.vecOriginalSize.fZ );

        int iHashInfo;
        BitStream.Read ( iHashInfo );
        item.bHasHashInfo = iHashInfo != 0;

        BitStream.Read ( item.uiShortBytes );
        BitStream.ReadString ( item.strShortMd5 );
        BitStream.ReadString ( item.strShortSha256 );
        BitStream.Read ( item.uiLongBytes );
        BitStream.ReadString ( item.strLongMd5 );
        if ( !BitStream.ReadString ( item.strLongSha256 ) )
            return false;

        m_ModInfoItemList.push_back ( item );
    }

    return true;
}
