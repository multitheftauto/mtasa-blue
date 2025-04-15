/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CLuaEventPacket.cpp
 *  PURPOSE:     Lua event packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
    unsigned short usNameLength;
    if (BitStream.ReadCompressed(usNameLength))
    {
        if (usNameLength < (MAX_EVENT_NAME_LENGTH - 1) && BitStream.ReadStringCharacters(m_strName, usNameLength) && BitStream.Read(m_ElementID))
        {
            // Faster than using a constructor
            m_ArgumentsStore.DeleteArguments();
            if (!m_ArgumentsStore.ReadFromBitStream(BitStream))
                return false;
            m_pArguments = &m_ArgumentsStore;

            return true;
        }
    }

    return false;
}

bool CLuaEventPacket::Write(NetBitStreamInterface& BitStream) const
{
    BitStream.WriteCompressed(static_cast<unsigned short>(m_strName.length()));
    BitStream.WriteStringCharacters(m_strName);
    BitStream.Write(m_ElementID);
    m_pArguments->WriteToBitStream(BitStream);

    return true;
}
