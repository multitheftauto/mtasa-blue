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
    AddHandler(SET_COLSHAPE_RADIUS, SetColShapeRadius, "SetColShapeRadius");
    AddHandler(SET_COLSHAPE_SIZE, SetColShapeSize, "SetColShapeSize");
}

void CColShapeRPCs::SetColShapeRadius(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float         fRadius;
    unsigned char ucTimeContext;
    if (bitStream.Read(fRadius) && bitStream.Read(ucTimeContext))
    {
        if (pSource)
        {
            CClientColShape* pColShape = static_cast<CClientColShape*>(pSource);
            CStaticFunctionDefinitions::SetColShapeRadius(pColShape, fRadius);
            pSource->SetSyncTimeContext(ucTimeContext);
        }
    }
}

void CColShapeRPCs::SetColShapeSize(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CVector       vecSize;
    unsigned char ucTimeContext;
    if (bitStream.ReadVector(vecSize.fX, vecSize.fY, vecSize.fZ) && bitStream.Read(ucTimeContext))
    {
        if (pSource)
        {
            CClientColShape* pColShape = static_cast<CClientColShape*>(pSource);
            CStaticFunctionDefinitions::SetColShapeSize(pColShape, vecSize);
            pSource->SetSyncTimeContext(ucTimeContext);
        }
    }
}
