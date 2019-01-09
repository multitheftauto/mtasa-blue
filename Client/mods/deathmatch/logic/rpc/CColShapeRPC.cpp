/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CColShapeRPC.cpp
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
    unsigned char ucTimeContext;
    if (bitStream.Read(fFloor) && bitStream.Read(fCeil) && bitStream.Read(ucTimeContext))
    {
        if (pSource)
        {
            static_cast<CClientColPolygon*>(pSource)->SetHeight(fFloor, fCeil);
            pSource->SetSyncTimeContext(ucTimeContext);
        }
    }
}
