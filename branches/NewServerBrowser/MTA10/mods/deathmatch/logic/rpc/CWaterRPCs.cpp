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
    AddHandler ( SET_WATER_LEVEL, SetGTAWaterLevel, "SetWaterLevel" );
    AddHandler ( SET_WATER_LEVEL, SetWaterElementLevel, "SetWaterLevel" );
    AddHandler ( SET_WATER_VERTEX_POSITION, SetWaterVertexPosition, "SetWaterVertexPosition" );
    AddHandler ( SET_WATER_COLOR, SetWaterColor, "SetWaterColor" );
    AddHandler ( RESET_WATER_COLOR, ResetWaterColor, "ResetWaterColor" );
}

void CWaterRPCs::SetGTAWaterLevel ( NetBitStreamInterface& bitStream )
{
    float fLevel;
    bool bUsePosition;

    if ( bitStream.Read ( fLevel ) && bitStream.ReadBit ( bUsePosition ) )
    {
        if ( bUsePosition == true )
        {
            // (x, y, z, level)
            short sX, sY;
            CVector vecPos;
            if ( bitStream.Read ( sX ) && bitStream.Read ( sY ) && bitStream.Read ( vecPos.fZ ) )
            {
                vecPos.fX = sX;
                vecPos.fY = sY;
                m_pWaterManager->SetWaterLevel ( &vecPos, fLevel );
            }
        }
        else
        {
            // (level)
            m_pWaterManager->SetWaterLevel ( (CVector *)NULL, fLevel );
        }
    }
}

void CWaterRPCs::SetWaterElementLevel ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    float fLevel;

    if ( bitStream.Read ( fLevel ) )
    {
        // (water, level)
        CClientWater* pWater = m_pWaterManager->Get ( pSource->GetID () );
        if ( pWater )
        {
            CVector vecVertexPos;
            for ( int i = 0; i < pWater->GetNumVertices (); i++ )
            {
                pWater->GetVertexPosition ( i, vecVertexPos );
                vecVertexPos.fZ = fLevel;
                pWater->SetVertexPosition ( i, vecVertexPos );
            }
        }
    }
}

void CWaterRPCs::SetWaterVertexPosition ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned char ucVertexID;
    short sX;
    short sY;
    float fZ;

    if ( bitStream.Read ( ucVertexID ) &&
         bitStream.Read ( sX ) &&
         bitStream.Read ( sY ) &&
         bitStream.Read ( fZ ) )
    {
        CClientWater* pWater = m_pWaterManager->Get ( pSource->GetID () );
        if ( pWater && ucVertexID < pWater->GetNumVertices () )
        {
            CVector vecPosition ( sX, sY, fZ );
            pWater->SetVertexPosition ( ucVertexID, vecPosition );
        }
    }
}

void CWaterRPCs::SetWaterColor ( NetBitStreamInterface& bitStream )
{
    unsigned char ucRed, ucGreen, ucBlue, ucAlpha;

    if ( bitStream.Read ( ucRed ) &&
         bitStream.Read ( ucGreen ) &&
         bitStream.Read ( ucBlue ) &&
         bitStream.Read ( ucAlpha ) )
    {
        CStaticFunctionDefinitions::SetWaterColor ( ucRed, ucGreen, ucBlue, ucAlpha );
    }
}

void CWaterRPCs::ResetWaterColor ( NetBitStreamInterface& bitStream )
{
    CStaticFunctionDefinitions::ResetWaterColor ( );
}
