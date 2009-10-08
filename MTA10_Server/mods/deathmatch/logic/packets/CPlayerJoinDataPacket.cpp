/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerJoinDataPacket.cpp
*  PURPOSE:     Player join data packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CPlayerJoinDataPacket::Read ( NetBitStreamInterface& BitStream )
{
    m_szNick [ MAX_NICK_LENGTH ] = 0;
    m_szSerialUser [MAX_SERIAL_LENGTH] = 0;

    // Read out the stuff
    if ( !BitStream.Read ( m_usNetVersion ) ||
         !BitStream.Read ( m_usMTAVersion ) )
        return false;

    if ( m_usMTAVersion < 0x0102 )
    {
        // Clients earlier than 1.0.2 do not have a bitstream version
        m_usBitStreamVersion = 0x01;
    }
    else
    {
        if ( !BitStream.Read ( m_usBitStreamVersion ) )
            return false;
    }

    return ( BitStream.Read ( m_ucGameVersion ) &&
             BitStream.Read ( m_szNick, MAX_NICK_LENGTH ) &&
             BitStream.Read ( reinterpret_cast < char* > ( &m_Password ), 16 ) &&
             BitStream.Read ( m_szSerialUser, MAX_SERIAL_LENGTH ) );
}
