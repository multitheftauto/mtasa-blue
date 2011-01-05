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
#include "net/SyncStructures.h"

void CBlipRPCs::LoadFunctions ( void )
{
    AddHandler ( DESTROY_ALL_BLIPS, DestroyAllBlips, "DestroyAllBlips" );
    AddHandler ( SET_BLIP_ICON, SetBlipIcon, "SetBlipIcon" );
    AddHandler ( SET_BLIP_SIZE, SetBlipSize, "SetBlipSize" );
    AddHandler ( SET_BLIP_COLOR, SetBlipColor, "SetBlipColor" );
    AddHandler ( SET_BLIP_ORDERING, SetBlipOrdering, "SetBlipOrdering" );
    AddHandler ( SET_BLIP_VISIBLE_DISTANCE, SetBlipVisibleDistance, "SetBlipVisibleDistance" );
}


void CBlipRPCs::DestroyAllBlips ( NetBitStreamInterface& bitStream )
{
    m_pRadarMarkerManager->DeleteAll ();
}


void CBlipRPCs::SetBlipIcon ( NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the icon id
    ElementID ID;
    SBlipIconSync icon;
    if ( bitStream.ReadCompressed ( ID ) &&
         bitStream.Read ( &icon ) )
    {
        // Valid range?
        if ( icon.data.ucIcon <= RADAR_MARKER_LIMIT )
        {
            // Grab the blip
            CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( ID );
            if ( pMarker )
            {
                // Set the new icon
                pMarker->SetSprite ( icon.data.ucIcon );
            }
        }
    }
}


void CBlipRPCs::SetBlipSize ( NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the size
    ElementID ID;
    SBlipSizeSync size;
    if ( bitStream.ReadCompressed ( ID ) &&
         bitStream.Read ( &size ) )
    {
        // Grab the blip
        CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( ID );
        if ( pMarker )
        {
            // Set the new size
            pMarker->SetScale ( size.data.ucSize );
        }
    }
}


void CBlipRPCs::SetBlipColor ( NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the color
    ElementID ID;
    SColorSync color;
    if ( bitStream.ReadCompressed ( ID ) &&
         bitStream.Read ( &color ) )
    {
        // Grab the blip
        CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( ID );
        if ( pMarker )
        {
            // Set the new color
            pMarker->SetColor ( color );
        }
    }
}


void CBlipRPCs::SetBlipOrdering ( NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the ordering
    ElementID ID;
    short sOrdering;
    if ( bitStream.ReadCompressed ( ID ) &&
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


void CBlipRPCs::SetBlipVisibleDistance ( NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the distance
    ElementID ID;
    SFloatAsBitsSync<20> visibleDistance ( 0.0, 99999.0, true );
    if ( bitStream.ReadCompressed ( ID ) &&
         bitStream.Read ( &visibleDistance ) )
    {
        // Grab the blip
        CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( ID );
        if ( pMarker )
        {
            // Set the new visible distance
            pMarker->SetVisibleDistance ( visibleDistance.data.fValue );
        }
    }
}