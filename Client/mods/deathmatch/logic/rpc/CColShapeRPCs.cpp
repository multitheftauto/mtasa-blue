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
    AddHandler(UPDATE_COLPOLYGON_POINT, UpdateColPolygonPoint, "UpdateColPolygonPoint");
    AddHandler(ADD_COLPOLYGON_POINT, AddColPolygonPoint, "AddColPolygonPoint");
    AddHandler(REMOVE_COLPOLYGON_POINT, RemoveColPolygonPoint, "RemoveColPolygonPoint");
}

void CColShapeRPCs::SetColShapeRadius(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float         fRadius;
    unsigned char ucTimeContext;
    if (bitStream.Read(fRadius) && bitStream.Read(ucTimeContext))
    {
        CClientColShape* pColShape = static_cast<CClientColShape*>(pSource);
        CStaticFunctionDefinitions::SetColShapeRadius(pColShape, fRadius);
        pSource->SetSyncTimeContext(ucTimeContext);
    }
}

void CColShapeRPCs::SetColShapeSize(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CVector       vecSize;
    unsigned char ucTimeContext;
    if (bitStream.ReadVector(vecSize.fX, vecSize.fY, vecSize.fZ) && bitStream.Read(ucTimeContext))
    {
        CClientColShape* pColShape = static_cast<CClientColShape*>(pSource);
        CStaticFunctionDefinitions::SetColShapeSize(pColShape, vecSize);
        pSource->SetSyncTimeContext(ucTimeContext);
    }
}

void CColShapeRPCs::UpdateColPolygonPoint(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    SPosition2DSync size(false);
    unsigned int    uiPointIndex;

    unsigned char   ucTimeContext;
    if (bitStream.Read(&size) && bitStream.Read(uiPointIndex) && bitStream.Read(ucTimeContext))
    {
        CClientColPolygon* pColShape = static_cast<CClientColPolygon*>(pSource);
        CStaticFunctionDefinitions::SetColPolygonPointPosition(pColShape, uiPointIndex, size.data.vecPosition);
        pSource->SetSyncTimeContext(ucTimeContext);
    }
}

void CColShapeRPCs::AddColPolygonPoint(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    SPosition2DSync size(false);
    int             iPointIndex;

    unsigned char ucTimeContext;
    if (bitStream.Read(&size) && bitStream.Read(iPointIndex) && bitStream.Read(ucTimeContext))
    {
        CClientColPolygon* pColShape = static_cast<CClientColPolygon*>(pSource);
        CStaticFunctionDefinitions::AddColPolygonPoint(pColShape, iPointIndex, size.data.vecPosition);
        pSource->SetSyncTimeContext(ucTimeContext);
    }
}

void CColShapeRPCs::RemoveColPolygonPoint(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    unsigned int    uiPointIndex;

    unsigned char ucTimeContext;
    if (bitStream.Read(uiPointIndex) && bitStream.Read(ucTimeContext))
    {
        CClientColPolygon* pColShape = static_cast<CClientColPolygon*>(pSource);
        CStaticFunctionDefinitions::RemoveColPolygonPoint(pColShape, uiPointIndex);
        pSource->SetSyncTimeContext(ucTimeContext);
    }
}
