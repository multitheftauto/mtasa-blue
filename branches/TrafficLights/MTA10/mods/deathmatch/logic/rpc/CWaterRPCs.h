/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CWaterRPCs.h
*  PURPOSE:     Header for water RPC calls
*  DEVELOPERS:  arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWaterRPCs_H
#define __CWaterRPCs_H

#include "CRPCFunctions.h"

class CWaterRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( SetWaterLevel );
    DECLARE_RPC ( SetWaterVertexPosition );
};

#endif
