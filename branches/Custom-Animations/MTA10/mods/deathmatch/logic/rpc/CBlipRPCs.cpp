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


void CBlipRPCs::SetBlipIcon ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the icon id
    SIntegerSync < unsigned char, 6 > icon;
    if ( bitStream.Read ( &icon ) )
    {
        // Valid range?
        if ( icon <= RADAR_MARKER_LIMIT )
        {
            // Grab the blip
            CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( pSource->GetID() );
            if ( pMarker )
            {
                // Set the new icon
                pMarker->SetSprite ( icon );
            }
        }
    }
}


void CBlipRPCs::SetBlipSize ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the size
    SIntegerSync < unsigned char, 5 > size;
    if ( bitStream.Read ( &size ) )
    {
        // Grab the blip
        CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( pSource->GetID() );
        if ( pMarker )
        {
            // Set the new size
            pMarker->SetScale ( size );
        }
    }
}


void CBlipRPCs::SetBlipColor ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the color
    SColorSync color;
    if ( bitStream.Read ( &color ) )
    {
        // Grab the blip
        CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( pSource->GetID() );
        if ( pMarker )
        {
            // Set the new color
            pMarker->SetColor ( color );
        }
    }
}


void CBlipRPCs::SetBlipOrdering ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the ordering
    short sOrdering;
    if ( bitStream.ReadCompressed ( sOrdering ) )
    {
        // Grab the blip
        CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( pSource->GetID() );
        if ( pMarker )
        {
            // Set the new color
            pMarker->SetOrdering ( sOrdering );
        }
    }
}


void CBlipRPCs::SetBlipVisibleDistance ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the blip ID and the distance
    SIntegerSync < unsigned short, 14 > visibleDistance;
    if ( bitStream.Read ( &visibleDistance ) )
    {
        // Grab the blip
        CClientRadarMarker* pMarker = m_pRadarMarkerManager->Get ( pSource->GetID() );
        if ( pMarker )
        {
            // Set the new visible distance
            pMarker->SetVisibleDistance ( visibleDistance );
        }
    }
}