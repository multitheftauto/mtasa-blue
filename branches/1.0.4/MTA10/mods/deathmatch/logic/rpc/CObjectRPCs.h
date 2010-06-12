/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CObjectRPCs.h
*  PURPOSE:     Header for object RPC class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CObjectRPCs_H
#define __CObjectRPCs_H

#include "CRPCFunctions.h"

class CObjectRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( DestroyAllObjects );
    DECLARE_RPC ( SetModel );
    DECLARE_RPC ( SetObjectRotation );
    DECLARE_RPC ( MoveObject );
    DECLARE_RPC ( StopObject );
};

#endif