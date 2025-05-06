/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CColShapeRPC.h
 *  PURPOSE:     Header for colshape RPC class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRPCFunctions.h"

class CColShapeRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions(void);

    DECLARE_ELEMENT_RPC(SetColShapeRadius);
    DECLARE_ELEMENT_RPC(SetColShapeSize);
    DECLARE_ELEMENT_RPC(UpdateColPolygonPoint);
    DECLARE_ELEMENT_RPC(AddColPolygonPoint);
    DECLARE_ELEMENT_RPC(RemoveColPolygonPoint);
    DECLARE_ELEMENT_RPC(SetColShapePolygonHeight);
};
