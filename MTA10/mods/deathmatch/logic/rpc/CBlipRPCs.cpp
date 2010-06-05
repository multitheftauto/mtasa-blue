/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CBlipRPCs.cpp
*  PURPOSE:     Blip remote procedure calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CBlipRPCs.h"

void CBlipRPCs::LoadFunctions ( void )
{
    AddHandler ( DESTROY_ALL_BLIPS, DestroyAllBlips, "DestroyAllBlips" );
    AddHandler ( SET_BLIP_ICON, SetBlipIcon, "SetBlipIcon" );
    AddHandler ( SET_BLIP_SIZE, SetBlipSize, "SetBlipSize" );
    AddHandler ( SET_BLIP_COLOR, SetBlipColor, "SetBlipColor" );
    AddHandler ( SET_BLIP_ORDERING, SetBlipOrdering, "SetBlipOrdering" );
}


void CBlipRPCs::DestroyAllBlips ( NetBitStreamInterface& bitStream )
{
    m_pRadarMarkerManager->DeleteAll ();
}


void CBlipRPCs::SetBlipIcon ( NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the icon id
    ElementID ID;
    unsigned char ucIcon;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucIcon ) )
    {
        // Valid range?
        if ( ucIcon <= RADAR_MARKER_LIMIT )
        {
            // Grab the blip
            CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( ID );
            if ( pMarker )
            {
                // Set the new icon
                pMarker->SetSprite ( ucIcon );
            }
        }
    }
}


void CBlipRPCs::SetBlipSize ( NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the icon id
    ElementID ID;
    unsigned char ucSize;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucSize ) )
    {
        // Grab the blip
        CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( ID );
        if ( pMarker )
        {
            // Set the new size
            pMarker->SetScale ( ucSize );
        }
    }
}


void CBlipRPCs::SetBlipColor ( NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the color
    ElementID ID;
    unsigned char ucRed;
    unsigned char ucGreen;
    unsigned char ucBlue;
    unsigned char ucAlpha;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucRed ) &&
         bitStream.Read ( ucGreen ) &&
         bitStream.Read ( ucBlue ) &&
         bitStream.Read ( ucAlpha ) )
    {
        // Grab the blip
        CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( ID );
        if ( pMarker )
        {
            // Set the new color
            pMarker->SetColor ( ucRed, ucGreen, ucBlue, ucAlpha );
        }
    }
}


void CBlipRPCs::SetBlipOrdering ( NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the color
    ElementID ID;
    short sOrdering;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( sOrdering ) )
    {
        // Grab the blip
        CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( ID );
        if ( pMarker )
        {
            // Set the new color
            pMarker->SetOrdering ( sOrdering );
        }
    }
}