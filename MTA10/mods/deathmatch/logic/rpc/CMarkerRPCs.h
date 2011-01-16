/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CMarkerRPCs.h
*  PURPOSE:     Header for marker RPC class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMarkerRPCs_H
#define __CMarkerRPCs_H

#include "CRPCFunctions.h"

class CMarkerRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_ELEMENT_RPC ( SetMarkerType );
    DECLARE_ELEMENT_RPC ( SetMarkerColor );
    DECLARE_ELEMENT_RPC ( SetMarkerSize );
    DECLARE_ELEMENT_RPC ( SetMarkerTarget );
    DECLARE_ELEMENT_RPC ( SetMarkerIcon );
};

#endif