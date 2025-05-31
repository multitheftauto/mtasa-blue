/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CWaterRPCs.cpp
 *  PURPOSE:     Water remote procedure calls
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CWaterRPCs.h"

void CWaterRPCs::LoadFunctions()
{
    AddHandler(SET_WORLD_WATER_LEVEL, SetWorldWaterLevel, "SetWorldWaterLevel");
    AddHandler(RESET_WORLD_WATER_LEVEL, ResetWorldWaterLevel, "ResetWorldWaterLevel");
    AddHandler(SET_ELEMENT_WATER_LEVEL, SetElementWaterLevel, "SetElementWaterLevel");
    AddHandler(SET_ALL_ELEMENT_WATER_LEVEL, SetAllElementWaterLevel, "SetAllElementWaterLevel");
    AddHandler(SET_WATER_VERTEX_POSITION, SetWaterVertexPosition, "SetWaterVertexPosition");
    AddHandler(SET_WATER_COLOR, SetWaterColor, "SetWaterColor");
    AddHandler(RESET_WATER_COLOR, ResetWaterColor, "ResetWaterColor");
}

void CWaterRPCs::SetWorldWaterLevel(NetBitStreamInterface& bitStream)
{
    float fLevel;
    bool  bIncludeWorldNonSeaLevel = true;
    bool  bIncludeWorldSeaLevel = true;
    bool  bIncludeOutsideWorldLevel = false;

    if (bitStream.Read(fLevel) && bitStream.ReadBit(bIncludeWorldNonSeaLevel))
    {
        if (bitStream.Can(eBitStreamVersion::SetWaterLevel_ChangeOutsideWorldLevel))
        {
            bitStream.ReadBit(bIncludeWorldSeaLevel);
            bitStream.ReadBit(bIncludeOutsideWorldLevel);
        }
        m_pWaterManager->SetWorldWaterLevel(fLevel, nullptr, bIncludeWorldNonSeaLevel, bIncludeWorldSeaLevel, bIncludeOutsideWorldLevel);
    }
}

void CWaterRPCs::ResetWorldWaterLevel(NetBitStreamInterface& bitStream)
{
    m_pWaterManager->ResetWorldWaterLevel();
}

void CWaterRPCs::SetElementWaterLevel(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float fLevel;

    if (bitStream.Read(fLevel))
    {
        // (water, level)
        CClientWater* pWater = m_pWaterManager->Get(pSource->GetID());
        if (pWater)
        {
            pWater->SetLevel(fLevel);
        }
    }
}

void CWaterRPCs::SetAllElementWaterLevel(NetBitStreamInterface& bitStream)
{
    float fLevel;

    if (bitStream.Read(fLevel))
    {
        m_pWaterManager->SetAllElementWaterLevel(fLevel, NULL);
    }
}

void CWaterRPCs::SetWaterVertexPosition(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    unsigned char ucVertexID;
    short         sX;
    short         sY;
    float         fZ;

    if (bitStream.Read(ucVertexID) && bitStream.Read(sX) && bitStream.Read(sY) && bitStream.Read(fZ))
    {
        CClientWater* pWater = m_pWaterManager->Get(pSource->GetID());
        if (pWater && ucVertexID < pWater->GetNumVertices())
        {
            CVector vecPosition(sX, sY, fZ);
            pWater->SetVertexPosition(ucVertexID, vecPosition);
        }
    }
}

void CWaterRPCs::SetWaterColor(NetBitStreamInterface& bitStream)
{
    unsigned char ucRed, ucGreen, ucBlue, ucAlpha;

    if (bitStream.Read(ucRed) && bitStream.Read(ucGreen) && bitStream.Read(ucBlue) && bitStream.Read(ucAlpha))
    {
        CStaticFunctionDefinitions::SetWaterColor(ucRed, ucGreen, ucBlue, ucAlpha);
    }
}

void CWaterRPCs::ResetWaterColor(NetBitStreamInterface& bitStream)
{
    CStaticFunctionDefinitions::ResetWaterColor();
}
