/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CCustomDataPacket.cpp
 *  PURPOSE:     Custom element data packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCustomDataPacket.h"
#include "CCustomData.h"

CCustomDataPacket::CCustomDataPacket() noexcept
{
    m_szName = nullptr;
}

CCustomDataPacket::~CCustomDataPacket() noexcept
{
    delete[] m_szName;
    m_szName = nullptr;
}

bool CCustomDataPacket::Read(NetBitStreamInterface& BitStream) noexcept
{
    std::uint16_t usNameLength;
    if (!BitStream.Read(m_ElementID) || !BitStream.ReadCompressed(usNameLength))
        return false;

    if (usNameLength <= 0 || usNameLength > MAX_CUSTOMDATA_NAME_LENGTH)
        return false;

    m_szName = new char[usNameLength + 1];
    if (!BitStream.Read(m_szName, usNameLength))
        return false;

    m_szName[usNameLength] = 0;
    return m_Value.ReadFromBitStream(BitStream);
}

bool CCustomDataPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    return true;
}
