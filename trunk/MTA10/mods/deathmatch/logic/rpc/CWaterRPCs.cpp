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
    AddHandler ( SET_WATER_LEVEL, SetWaterLevel, "SetWaterLevel" );
    AddHandler ( SET_WATER_VERTEX_POSITION, SetWaterVertexPosition, "SetWaterVertexPosition" );
}

void CWaterRPCs::SetWaterLevel ( NetBitStreamInterface& bitStream )
{
    unsigned short usResourceID;
    CResource* pResource;
    float fLevel;
    unsigned char ucWhat;

    if ( bitStream.Read ( usResourceID ) && bitStream.Read ( fLevel ) && bitStream.Read ( ucWhat ) )
    {
        pResource = g_pClientGame->GetResourceManager ()->GetResource ( usResourceID );
        if ( !pResource )
            return;

        switch ( ucWhat )
        {
            case 0:
            {
                // (x, y, z, level)
                short sX, sY;
                CVector vecPos;
                if ( bitStream.Read ( sX ) && bitStream.Read ( sY ) && bitStream.Read ( vecPos.fZ ) )
                {
                    vecPos.fX = sX;
                    vecPos.fY = sY;
                    m_pWaterManager->SetWaterLevel ( &vecPos, fLevel, pResource );
                }
                break;
            }
            case 1:
            {
                // (water, level)
                ElementID waterID;
                if ( bitStream.Read ( waterID ) )
                {
                    CClientWater* pWater = m_pWaterManager->Get ( waterID );
                    if ( pWater )
                    {
                        CVector vecVertexPos;
                        for ( int i = 0; i < pWater->GetNumVertices (); i++ )
                        {
                            pWater->GetVertexPosition ( i, vecVertexPos );
                            vecVertexPos.fZ = fLevel;
                            pWater->SetVertexPosition ( i, vecVertexPos, pResource );
                        }
                    }
                }
                break;
            }
            case 2:
            {
                // (level)
                m_pWaterManager->SetWaterLevel ( (CVector *)NULL, fLevel, pResource );
                break;
            }
        }
    }
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
