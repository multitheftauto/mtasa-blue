/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CTeamRPCs.h
*  PURPOSE:     Header for team RPC class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTeamRPCs_H
#define __CTeamRPCs_H

#include "CRPCFunctions.h"

class CTeamRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( RemoveTeam );
    DECLARE_RPC ( SetTeamName );
    DECLARE_RPC ( SetTeamColor );
    DECLARE_RPC ( SetPlayerTeam );
    DECLARE_RPC ( SetTeamFriendlyFire );
};

#endif