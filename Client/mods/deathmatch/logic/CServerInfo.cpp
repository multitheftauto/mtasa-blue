/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CServerInfo.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CServerInfo.h"

CServerInfo::CServerInfo() : m_MaxPlayersCount(0)
{
}

void CServerInfo::SetMaxPlayers(uint set)
{
    m_MaxPlayersCount = set;
}
