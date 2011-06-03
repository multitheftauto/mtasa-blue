/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CTeamRPCs.cpp
*  PURPOSE:     Team remote procedure calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CTeamRPCs.h"

void CTeamRPCs::LoadFunctions ( void )
{
    AddHandler ( SET_TEAM_NAME, SetTeamName, "SetTeamName" );
    AddHandler ( SET_TEAM_COLOR, SetTeamColor, "SetTeamColor" );
    AddHandler ( SET_PLAYER_TEAM, SetPlayerTeam, "SetPlayerTeam" );
    AddHandler ( SET_TEAM_FRIENDLY_FIRE, SetTeamFriendlyFire, "SetTeamFriendlyFire" );
}


void CTeamRPCs::SetTeamName ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned short usNameLength;
    if ( bitStream.Read ( usNameLength ) )
    {
        char* szName = new char [ usNameLength + 1 ];
        szName [ usNameLength ] = NULL;

        if ( bitStream.Read ( szName, usNameLength ) )
        {
            CClientTeam* pTeam = m_pTeamManager->GetTeam ( pSource->GetID () );
            if ( pTeam )
            {
                pTeam->SetTeamName ( szName );
            }
        }
        delete [] szName;
    }
}


void CTeamRPCs::SetTeamColor ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned char ucRed, ucGreen, ucBlue;
    if ( bitStream.Read ( ucRed ) &&
         bitStream.Read ( ucGreen ) &&
         bitStream.Read ( ucBlue ) )
    {
        CClientTeam* pTeam = m_pTeamManager->GetTeam ( pSource->GetID () );
        if ( pTeam )
        {
            pTeam->SetColor ( ucRed, ucGreen, ucBlue );
        }
    }
}


void CTeamRPCs::SetPlayerTeam ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    ElementID PlayerID;
    if ( bitStream.ReadCompressed ( PlayerID ) )
    {
        CClientPlayer* pPlayer = m_pPlayerManager->Get ( PlayerID );
        CClientTeam* pTeam = NULL;
        if ( pSource != NULL && pSource->GetType () == CCLIENTTEAM )
            pTeam = static_cast < CClientTeam * > ( pSource );

        if ( pPlayer )
        {
            pPlayer->SetTeam ( pTeam, true );
        }
    }
}


void CTeamRPCs::SetTeamFriendlyFire ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    bool bFriendlyFire;
    if ( bitStream.ReadBit ( bFriendlyFire ) )
    {
        CClientTeam* pTeam = m_pTeamManager->GetTeam ( pSource->GetID () );
        if ( pTeam )
        {
            pTeam->SetFriendlyFire ( bFriendlyFire );
        }
    }
}