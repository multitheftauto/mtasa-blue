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

    DECLARE_ELEMENT_RPC ( SetElementParent );
    DECLARE_ELEMENT_RPC ( SetElementData );
    DECLARE_ELEMENT_RPC ( RemoveElementData );
    DECLARE_ELEMENT_RPC ( SetElementPosition );
    DECLARE_ELEMENT_RPC ( SetElementVelocity );
    DECLARE_ELEMENT_RPC ( SetElementInterior );
    DECLARE_ELEMENT_RPC ( SetElementDimension );
    DECLARE_ELEMENT_RPC ( AttachElements );
    DECLARE_ELEMENT_RPC ( DetachElements );
    DECLARE_ELEMENT_RPC ( SetElementAlpha );
    DECLARE_ELEMENT_RPC ( SetElementName );
    DECLARE_ELEMENT_RPC ( SetElementHealth );
    DECLARE_ELEMENT_RPC ( SetElementModel );
    DECLARE_ELEMENT_RPC ( SetElementAttachedOffsets );
    DECLARE_ELEMENT_RPC ( SetElementDoubleSided );
    DECLARE_ELEMENT_RPC ( SetElementCollisionsEnabled );
    DECLARE_ELEMENT_RPC ( SetElementFrozen );
    DECLARE_ELEMENT_RPC ( SetLowLodElement );
};

#endif
