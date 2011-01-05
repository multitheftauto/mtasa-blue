/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CBlipRPCs.h
*  PURPOSE:     Header for blip RPC class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CBlipRPCs_H
#define __CBlipRPCs_H

#include "CRPCFunctions.h"

class CBlipRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( DestroyAllBlips );
    DECLARE_RPC ( SetBlipIcon );
    DECLARE_RPC ( SetBlipSize );
    DECLARE_RPC ( SetBlipColor );
    DECLARE_RPC ( SetBlipOrdering );
    DECLARE_RPC ( SetBlipVisibleDistance );

};

#endif