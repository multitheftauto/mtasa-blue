/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CRadarRPCs.cpp
*  PURPOSE:     Radar remote procedure calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CRadarRPCs.h"

void CRadarRPCs::LoadFunctions ( void )
{
    AddHandler ( DESTROY_ALL_RADAR_AREAS, DestroyAllRadarAreas, "DestroyAllRadarAreas" );
    AddHandler ( SET_RADAR_AREA_SIZE, SetRadarAreaSize, "SetRadarAreaSize" );
    AddHandler ( SET_RADAR_AREA_COLOR, SetRadarAreaColor, "SetRadarAreaColor" );
    AddHandler ( SET_RADAR_AREA_FLASHING, SetRadarAreaFlashing, "SetRadarAreaFlashing" );
}


void CRadarRPCs::DestroyAllRadarAreas ( NetBitStreamInterface& bitStream )
{
    // Delete all the radar areas
    m_pRadarAreaManager->DeleteAll ();
}


void CRadarRPCs::SetRadarAreaSize ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the radar area id and the size
    CVector2D vecSize;
    if ( bitStream.Read ( vecSize.fX ) &&
         bitStream.Read ( vecSize.fY ) )
    {
        // Grab the radar area
        CClientRadarArea* pArea = m_pRadarAreaManager->Get ( pSource->GetID () );
        if ( pArea )
        {
            // Set the new size
            pArea->SetSize ( vecSize );
        }
    }
}


void CRadarRPCs::SetRadarAreaColor ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the radar area id and the color
    SColor color;
    if ( bitStream.Read ( color.R ) &&
         bitStream.Read ( color.G ) &&
         bitStream.Read ( color.B ) &&
         bitStream.Read ( color.A ) )
    {
        // Grab the radar area
        CClientRadarArea* pArea = m_pRadarAreaManager->Get ( pSource->GetID () );
        if ( pArea )
        {
            // Set the new color
            pArea->SetColor ( color );
        }
    }
}


void CRadarRPCs::SetRadarAreaFlashing ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the radar area id and the flashing status
    bool bFlashing;
    if ( bitStream.ReadBit ( bFlashing ) )
    {
        // Grab the radar area
        CClientRadarArea* pArea = m_pRadarAreaManager->Get ( pSource->GetID () );
        if ( pArea )
        {
            // Set the new flashing status
            pArea->SetFlashing ( bFlashing );
        }
    }
}