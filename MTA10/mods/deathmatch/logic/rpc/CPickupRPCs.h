/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CPickupRPCs.h
*  PURPOSE:     Header for pickups RPC class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPickupRPCs_H
#define __CPickupRPCs_H

#include "CRPCFunctions.h"

class CPickupRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( DestroyAllPickups );
    DECLARE_RPC ( SetPickupType );
    DECLARE_RPC ( SetPickupVisible );
};

#endif