/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CCustomDataPacket.cpp
*  PURPOSE:     Custom element data packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CCustomDataPacket::CCustomDataPacket ( void )
{
    m_szName = NULL;
}


CCustomDataPacket::~CCustomDataPacket ( void )
{
    delete [] m_szName;
    m_szName = NULL;
}


bool CCustomDataPacket::Read ( NetBitStreamInterface& BitStream )
{
    unsigned short usNameLength;
    if ( BitStream.Read ( m_ElementID ) &&
         BitStream.Read ( usNameLength ) && 
         usNameLength > 0 )
    {
        m_szName = new char [ usNameLength + 1 ];
        if ( BitStream.Read ( m_szName, usNameLength ) )
        {
            m_szName [ usNameLength ] = 0;
            if ( m_Value.ReadFromBitStream ( BitStream ) )
            {
                return true;
            }
        }
    }

    return false;
}


bool CCustomDataPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    return true;
}

