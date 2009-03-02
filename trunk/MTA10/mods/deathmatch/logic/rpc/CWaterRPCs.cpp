/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CWaterRPCs.cpp
*  PURPOSE:     Water remote procedure calls
*  DEVELOPERS:  arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CWaterRPCs.h"

void CWaterRPCs::LoadFunctions ( void )
{
    AddHandler ( SET_WATER_VERTEX_POSITION, SetWaterVertexPosition, "SetWaterVertexPosition" );
}

void CWaterRPCs::SetWaterVertexPosition ( NetBitStreamInterface& bitStream )
{
    ElementID waterID;
    unsigned char ucVertexID;
    short sX;
    short sY;
    float fZ;

    if ( bitStream.Read ( waterID ) &&
         bitStream.Read ( ucVertexID ) &&
         bitStream.Read ( sX ) &&
         bitStream.Read ( sY ) &&
         bitStream.Read ( fZ ) )
    {
        CClientWater* pWater = m_pWaterManager->Get ( waterID );
        if ( pWater && ucVertexID < pWater->GetNumVertices () )
        {
            CVector vecPosition ( sX, sY, fZ );
            pWater->SetVertexPosition ( ucVertexID, vecPosition );
        }
    }
}
