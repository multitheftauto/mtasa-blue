/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CLuaEventPacket.cpp
*  PURPOSE:     Lua event packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CLuaEventPacket::CLuaEventPacket ( void )
{
    m_szName [ 0 ] = 0;
    m_ElementID = INVALID_ELEMENT_ID;
}


CLuaEventPacket::CLuaEventPacket ( const char* szName, ElementID ID, CLuaArguments& Arguments )
{
    strncpy ( m_szName, szName, MAX_EVENT_NAME_LENGTH );
    if ( MAX_EVENT_NAME_LENGTH )
        m_szName [ MAX_EVENT_NAME_LENGTH - 1 ] = 0;
    m_ElementID = ID;
    m_Arguments = Arguments;
}


bool CLuaEventPacket::Read ( NetBitStreamInterface& BitStream )
{
    unsigned short usNameLength;
    if ( BitStream.ReadCompressed ( usNameLength ) )
    {
        if ( usNameLength < (MAX_EVENT_NAME_LENGTH - 1) && BitStream.Read ( m_szName, usNameLength ) && BitStream.ReadCompressed ( m_ElementID ) )
        {
            m_szName [ usNameLength ] = 0;
            m_Arguments = CLuaArguments ( BitStream );

            return true;
        }
    }

    return false;
}


bool CLuaEventPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    unsigned short usNameLength = static_cast < unsigned short > ( strlen ( m_szName ) );
    BitStream.WriteCompressed ( usNameLength );
    BitStream.Write ( const_cast < char* > ( m_szName ), usNameLength );
    BitStream.WriteCompressed ( m_ElementID );
    m_Arguments.WriteToBitStream ( BitStream );

    return true;
}
