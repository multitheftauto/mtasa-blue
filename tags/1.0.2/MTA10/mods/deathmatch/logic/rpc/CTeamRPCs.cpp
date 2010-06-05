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


void CTeamRPCs::SetTeamName ( NetBitStreamInterface& bitStream )
{
    ElementID TeamID;
    unsigned short usNameLength;
    if ( bitStream.Read ( TeamID ) &&
         bitStream.Read ( usNameLength ) )
    {
        char* szName = new char [ usNameLength + 1 ];
        szName [ usNameLength ] = NULL;

        if ( bitStream.Read ( szName, usNameLength ) )
        {
            CClientTeam* pTeam = m_pTeamManager->GetTeam ( TeamID );
            if ( pTeam )
            {
                pTeam->SetTeamName ( szName );
            }
        }
        delete [] szName;
    }
}


void CTeamRPCs::SetTeamColor ( NetBitStreamInterface& bitStream )
{
    ElementID TeamID;
    unsigned char ucRed, ucGreen, ucBlue;
    if ( bitStream.Read ( TeamID ) && bitStream.Read ( ucRed ) &&
         bitStream.Read ( ucGreen ) && bitStream.Read ( ucBlue ) )
    {
        CClientTeam* pTeam = m_pTeamManager->GetTeam ( TeamID );
        if ( pTeam )
        {
            pTeam->SetColor ( ucRed, ucGreen, ucBlue );
        }
    }
}


void CTeamRPCs::SetPlayerTeam ( NetBitStreamInterface& bitStream )
{
    ElementID PlayerID;
    ElementID TeamID;
    if ( bitStream.Read ( PlayerID ) &&
         bitStream.Read ( TeamID ) )
    {
        CClientPlayer* pPlayer = m_pPlayerManager->Get ( PlayerID );
        if ( pPlayer )
        {
            CClientTeam* pTeam = NULL;
            if ( TeamID != INVALID_ELEMENT_ID )
                pTeam = m_pTeamManager->GetTeam ( TeamID );

            pPlayer->SetTeam ( pTeam, true );
        }
    }
}


void CTeamRPCs::SetTeamFriendlyFire ( NetBitStreamInterface& bitStream )
{
    ElementID TeamID;
    unsigned char ucFriendlyFire;
    if ( bitStream.Read ( TeamID ) && bitStream.Read ( ucFriendlyFire ) )
    {
        CClientTeam* pTeam = m_pTeamManager->GetTeam ( TeamID );
        if ( pTeam )
        {
            pTeam->SetFriendlyFire ( ( ucFriendlyFire == 1 ) );
        }
    }
}