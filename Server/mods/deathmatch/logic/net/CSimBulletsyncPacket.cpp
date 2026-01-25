/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"

CSimBulletsyncPacket::CSimBulletsyncPacket(ElementID id) : m_id(id)
{
}

bool CSimBulletsyncPacket::Read(NetBitStreamInterface& stream)
{
    return m_cache.Read(stream);
}

bool CSimBulletsyncPacket::Write(NetBitStreamInterface& stream) const
{
    return m_cache.Write(stream, m_id);
}
