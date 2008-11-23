/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CResourceStopPacket.cpp
*  PURPOSE:     Resource stop packet handler class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CResourceStopPacket::CResourceStopPacket ( unsigned short usID )
{
    m_usID = usID;
}


CResourceStopPacket::~CResourceStopPacket ( void )
{

}


bool CResourceStopPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    if ( m_usID )
    {
        // Write the resource id
        BitStream.Write ( m_usID );

        return true;
    }

    return false;
}
