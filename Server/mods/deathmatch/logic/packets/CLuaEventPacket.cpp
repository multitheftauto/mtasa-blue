/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CLuaEventPacket.cpp
 *  PURPOSE:     Lua event packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaEventPacket.h"

CLuaEventPacket::CLuaEventPacket()
{
    m_ElementID = INVALID_ELEMENT_ID;
    m_pArguments = &m_ArgumentsStore;
}

CLuaEventPacket::CLuaEventPacket(const char* szName, ElementID ID, CLuaArguments* pArguments)
{
    m_strName.AssignLeft(szName, MAX_EVENT_NAME_LENGTH);
    m_ElementID = ID;
    m_pArguments = pArguments;            // Use a pointer to save copying the arguments
}

bool CLuaEventPacket::Read(NetBitStreamInterface& BitStream)
{
    std::uint16_t usNameLength;
    if (!BitStream.ReadCompressed(usNameLength))
        return false;

    if (usNameLength >= (MAX_EVENT_NAME_LENGTH - 1))
        return false;

    if (!BitStream.ReadStringCharacters(m_strName, usNameLength))
        return false;

    if (!BitStream.Read(m_ElementID))
        return false;

    // Faster than using a constructor
    m_ArgumentsStore.DeleteArguments();
    if (!m_ArgumentsStore.ReadFromBitStream(BitStream))
        return false;

    m_pArguments = &m_ArgumentsStore;

    return true;
}

bool CLuaEventPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    BitStream.WriteCompressed(static_cast<std::uint16_t>(m_strName.length()));
    BitStream.WriteStringCharacters(m_strName);
    BitStream.Write(m_ElementID);
    m_pArguments->WriteToBitStream(BitStream);

    return true;
}
