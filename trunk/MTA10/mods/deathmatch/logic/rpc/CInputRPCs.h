/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CInputRPCs.h
*  PURPOSE:     Header for input RPC class
*  DEVELOPERS:  Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CInputRPCs_H
#define __CInputRPCs_H

#include "CRPCFunctions.h"

class CInputRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( BindKey );
    DECLARE_RPC ( UnbindKey );
    DECLARE_RPC ( ToggleControl );
    DECLARE_RPC ( ToggleAllControls );
    DECLARE_RPC ( SetControlState );
    DECLARE_RPC ( ForceReconnect );
    DECLARE_RPC ( ShowCursor );
	DECLARE_RPC ( ShowChat );
};

#endif