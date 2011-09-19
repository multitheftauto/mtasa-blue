/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CPlayerRPCs.h
*  PURPOSE:     Header for player RPC class
*  DEVELOPERS:  Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPlayerRPCs_H
#define __CPlayerRPCs_H

#include "CRPCFunctions.h"

class CPlayerRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( SetPlayerMoney );       
    DECLARE_RPC ( ShowPlayerHudComponent );
    DECLARE_RPC ( ForcePlayerMap );
    DECLARE_ELEMENT_RPC ( SetPlayerNametagText );
    DECLARE_ELEMENT_RPC ( SetPlayerNametagColor );
    DECLARE_ELEMENT_RPC ( RemovePlayerNametagColor );
    DECLARE_ELEMENT_RPC ( SetPlayerNametagShowing );      
};

#endif