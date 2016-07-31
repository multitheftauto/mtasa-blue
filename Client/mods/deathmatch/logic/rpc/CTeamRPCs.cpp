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
    AddHandler ( SET_TEAM_FRIENDLY_FIRE, SetTeamFriendlyFire, "SetTeamFriendlyFire" );
}


void CTeamRPCs::SetTeamName ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    SString strName;
    if ( bitStream.ReadString ( strName ) )
    {
        CClientTeam* pTeam = m_pTeamManager->GetTeam ( pSource->GetID () );
        if ( pTeam )
        {
            pTeam->SetTeamName ( strName );
        }
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