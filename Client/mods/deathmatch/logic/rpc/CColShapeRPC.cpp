/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CObjectRPCs.cpp
*  PURPOSE:     Colshapes remote procedure calls
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CColShapeRPCs.h"

void CColShapeRPCs::LoadFunctions(void)
{
    AddHandler(COLSHAPE_POLYGON_SET_HEIGHT, SetColShapePolygonHeight, "SetColShapePolygonHeight");
}

void CColShapeRPCs::SetColShapePolygonHeight(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float fFloor;
    float fCeil;

    bitStream.Read(fFloor);
    bitStream.Read(fCeil);
    if (pSource)
    {
        static_cast<CClientColPolygon*>(pSource)->SetHeight(fFloor, fCeil);
    }
}
