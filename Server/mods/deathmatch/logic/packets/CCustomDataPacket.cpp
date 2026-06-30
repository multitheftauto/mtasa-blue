/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CCustomDataPacket.cpp
 *  PURPOSE:     Custom element data packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCustomDataPacket.h"
#include "CCustomData.h"

CCustomDataPacket::CCustomDataPacket()
{
    m_szName = NULL;
    m_isDelete = false;
}

CCustomDataPacket::~CCustomDataPacket()
{
    delete[] m_szName;
    m_szName = NULL;
}

bool CCustomDataPacket::Read(NetBitStreamInterface& bitStream)
{
    std::uint16_t nameLength;
    if (bitStream.Read(m_ElementID) && bitStream.ReadCompressed(nameLength) && nameLength > 0 && nameLength <= MAX_CUSTOMDATA_NAME_LENGTH)
    {
        m_szName = new char[nameLength + 1];
        if (bitStream.Read(m_szName, nameLength))
        {
            m_szName[nameLength] = 0;
            if (m_Value.ReadFromBitStream(bitStream))
                return true;

            // Clients leave out the value when requesting deletion, so the existing custom-data packet can also serve as RemoveElementData.
            m_isDelete = true;
            return true;
        }
    }

    return false;
}

bool CCustomDataPacket::Write(NetBitStreamInterface& bitStream) const
{
    return true;
}
