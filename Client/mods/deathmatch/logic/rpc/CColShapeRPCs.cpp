/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CColShapeRPC.cpp
 *  PURPOSE:     Colshapes remote procedure calls
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
    AddHandler(SET_COLPOLYGON_HEIGHT, SetColShapePolygonHeight, "SetColShapePolygonHeight");
}

void CColShapeRPCs::SetColShapeRadius(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float fRadius;
    if (bitStream.Read(fRadius))
    {
        CClientColShape* pColShape = static_cast<CClientColShape*>(pSource);
        CStaticFunctionDefinitions::SetColShapeRadius(pColShape, fRadius);
    }
}

void CColShapeRPCs::SetColShapeSize(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CVector vecSize;
    if (bitStream.ReadVector(vecSize.fX, vecSize.fY, vecSize.fZ))
    {
        CClientColShape* pColShape = static_cast<CClientColShape*>(pSource);
        CStaticFunctionDefinitions::SetColShapeSize(pColShape, vecSize);
    }
}

void CColShapeRPCs::UpdateColPolygonPoint(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    SPosition2DSync size(false);
    unsigned int    uiPointIndex;
    if (bitStream.Read(&size) && bitStream.Read(uiPointIndex))
    {
        CClientColPolygon* pColShape = static_cast<CClientColPolygon*>(pSource);
        CStaticFunctionDefinitions::SetColPolygonPointPosition(pColShape, uiPointIndex, size.data.vecPosition);
    }
}

void CColShapeRPCs::AddColPolygonPoint(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    SPosition2DSync size(false);
    unsigned int    uiPointIndex;
    if (bitStream.Read(&size))
    {
        CClientColPolygon* pColShape = static_cast<CClientColPolygon*>(pSource);
        if (bitStream.Read(uiPointIndex))
        {
            CStaticFunctionDefinitions::AddColPolygonPoint(pColShape, uiPointIndex, size.data.vecPosition);
        }
        else
        {
            CStaticFunctionDefinitions::AddColPolygonPoint(pColShape, size.data.vecPosition);
        }
    }
}

void CColShapeRPCs::RemoveColPolygonPoint(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    unsigned int uiPointIndex;
    if (bitStream.Read(uiPointIndex))
    {
        CClientColPolygon* pColShape = static_cast<CClientColPolygon*>(pSource);
        CStaticFunctionDefinitions::RemoveColPolygonPoint(pColShape, uiPointIndex);
    }
}

void CColShapeRPCs::SetColShapePolygonHeight(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float fFloor, fCeil;
    if (bitStream.Read(fFloor) && bitStream.Read(fCeil))
    {
        CClientColPolygon* pColPolygon = static_cast<CClientColPolygon*>(pSource);
        if (pColPolygon->SetHeight(fFloor, fCeil))
            CStaticFunctionDefinitions::RefreshColShapeColliders(pColPolygon);
    }
}
