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
    // Read out the stuff
    if ( !BitStream.Read ( m_usNetVersion ) ||
         !BitStream.Read ( m_usMTAVersion ) )
        return false;

    if ( !BitStream.Read ( m_usBitStreamVersion ) )
        return false;

    BitStream.ReadString ( m_strPlayerVersion );

    m_bOptionalUpdateInfoRequired = BitStream.ReadBit ();

    if ( BitStream.Read ( m_ucGameVersion ) &&
             BitStream.ReadStringCharacters ( m_strNick, MAX_NICK_LENGTH ) &&
             BitStream.Read ( reinterpret_cast < char* > ( &m_Password ), 16 ) &&
             BitStream.ReadStringCharacters ( m_strSerialUser, MAX_SERIAL_LENGTH ) )
    {
        // Shrink string sizes to fit
        m_strNick = *m_strNick;
        m_strSerialUser = *m_strSerialUser;
        return true;
    }
    return false;
}
