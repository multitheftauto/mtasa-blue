/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleDeltasyncPacket.cpp
*  PURPOSE:     Vehicle dekta synchronization packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

extern CGame* g_pGame;

bool CVehicleDeltasyncPacket::Read ( NetBitStreamInterface& bitStream )
{
    return false;
}

bool CVehicleDeltasyncPacket::Write ( NetBitStreamInterface& bitStream ) const
{
    return false;
}

void CVehicleDeltasyncPacket::PrepareToSendDeltaSync ()
{
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );
        m_delta = pSourcePlayer->GetSentDeltaSyncData ();
    }
}
