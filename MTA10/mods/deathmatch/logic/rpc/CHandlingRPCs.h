/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CHandlingRPCs.h
*  PURPOSE:     Header for handling RPC class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CHandlingRPCs_H
#define __CHandlingRPCs_H

#include "CRPCFunctions.h"

class CHandlingRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( HandlingAddDefault );
    DECLARE_RPC ( HandlingRemoveDefault );
    DECLARE_RPC ( HandlingSetProperty );
};

#endif