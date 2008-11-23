/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CElementRPCs.h
*  PURPOSE:     Header for element RPC class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CElementRPCs_H
#define __CElementRPCs_H

#include "CRPCFunctions.h"

class CElementRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( SetElementParent );
    DECLARE_RPC ( SetElementData );
    DECLARE_RPC ( RemoveElementData );
    DECLARE_RPC ( SetElementPosition );
    DECLARE_RPC ( SetElementVelocity );
    DECLARE_RPC ( SetElementInterior );
    DECLARE_RPC ( SetElementDimension );
    DECLARE_RPC ( AttachElements );
    DECLARE_RPC ( DetachElements );
    DECLARE_RPC ( SetElementAlpha );
	DECLARE_RPC ( SetElementName );
    DECLARE_RPC ( SetElementHealth );
    DECLARE_RPC ( SetElementModel );
};

#endif