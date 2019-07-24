/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CServerInfo.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CServerInfo::CServerInfo() : m_MaxPlayersCount(0)
{

}

void CServerInfo::SetMaxPlayers(uint set)
{
    m_MaxPlayersCount = set;
    g_pCore->GetDiscordManager()->SetState(SString("%i/%i Players", g_pClientGame->GetPlayerManager()->Count(), g_pClientGame->GetServerInfo()->GetMaxPlayers()), [](EDiscordRes) {});
}
