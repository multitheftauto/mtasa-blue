/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CObjectRPCs.h
*  PURPOSE:     Header for colshape RPC class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#ifndef __CColShapeRPCs_H
#define __CColShapeRPCs_H

#include "CRPCFunctions.h"

class CColShapeRPCs : public CRPCFunctions
{
public:
    static void LoadFunctions(void);

    DECLARE_ELEMENT_RPC(SetColShapePolygonHeight);
};

#endif
