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

CCustomDataPacket::CCustomDataPacket()
{
}

CCustomDataPacket::~CCustomDataPacket()
{
}

bool CCustomDataPacket::Read(NetBitStreamInterface& BitStream)
{
    unsigned short usNameLength;
    if (BitStream.Read(m_ElementID) && BitStream.ReadCompressed(usNameLength) && usNameLength > 0 && usNameLength <= MAX_CUSTOMDATA_NAME_LENGTH)
    {
        m_strName.resize(usNameLength);

        if (BitStream.Read(m_strName.data(), usNameLength))
        {
            if (m_Value.ReadFromBitStream(BitStream))
            {
                return true;
            }
        }
    }

    return false;
}

bool CCustomDataPacket::Write(NetBitStreamInterface& BitStream) const
{
    return true;
}
