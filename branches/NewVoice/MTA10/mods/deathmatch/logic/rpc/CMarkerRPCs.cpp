/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CMarkerRPCs.cpp
*  PURPOSE:     Marker remote procedure calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CMarkerRPCs.h"

void CMarkerRPCs::LoadFunctions ( void )
{
    AddHandler ( SET_MARKER_TYPE, SetMarkerType, "SetMarkerType" );
    AddHandler ( SET_MARKER_COLOR, SetMarkerColor, "SetMarkerColor" );
    AddHandler ( SET_MARKER_SIZE, SetMarkerSize, "SetMarkerSize" );
    AddHandler ( SET_MARKER_TARGET, SetMarkerTarget, "SetMarkerTarget" );
    AddHandler ( SET_MARKER_ICON, SetMarkerIcon, "SetMarkerIcon" );
}


void CMarkerRPCs::SetMarkerType ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the ID and the type
    unsigned char ucType;
    if ( bitStream.Read ( ucType ) )
    {
        // Grab the marker
        CClientMarker* pMarker = m_pMarkerManager->Get ( pSource->GetID () );
        if ( pMarker )
        {
            // Set the new type
            pMarker->SetMarkerType ( static_cast < CClientMarker::eMarkerType > ( ucType ) );
        }
    }
}


void CMarkerRPCs::SetMarkerColor ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the ID and the color
    SColor color;
    if ( bitStream.Read ( color.B ) &&
         bitStream.Read ( color.G ) &&
         bitStream.Read ( color.R ) &&
         bitStream.Read ( color.A ) )
    {
        // Grab the marker
        CClientMarker* pMarker = m_pMarkerManager->Get ( pSource->GetID () );
        if ( pMarker )
        {
            // Set the new position
            pMarker->SetColor ( color );
        }
    }
}


void CMarkerRPCs::SetMarkerSize ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the ID and the color
    float fSize;
    if ( bitStream.Read ( fSize ) )
    {
        // Grab the marker
        CClientMarker* pMarker = m_pMarkerManager->Get ( pSource->GetID () );
        if ( pMarker )
        {
            // Set the new position
            pMarker->SetSize ( fSize );
        }
    }
}


void CMarkerRPCs::SetMarkerTarget ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the marker id and if there's a new target
    unsigned char ucTemp;
    if ( bitStream.Read ( ucTemp ) )
    {
        // Grab the marker
        CClientMarker* pMarker = m_pMarkerManager->Get ( pSource->GetID () );
        if ( pMarker )
        {
            // Grab the checkpoint marker from it
            CClientCheckpoint* pCheckpoint = pMarker->GetCheckpoint ();
            if ( pCheckpoint )
            {
                // Handle this according to the temp char
                switch ( ucTemp )
                {
                    // No target?
                    case 0:
                    {
                        pCheckpoint->SetIcon ( CClientCheckpoint::ICON_NONE );
                        pCheckpoint->SetDirection ( CVector ( 1.0f, 0.0f, 0.0f ) );
                        pCheckpoint->SetHasTarget ( false );
                        pCheckpoint->ReCreateWithSameIdentifier();
                        break;
                    }

                    // Position target?
                    case 1:
                    {
                        // Read out the position
                        CVector vecPosition;
                        if ( bitStream.Read ( vecPosition.fX ) &&
                            bitStream.Read ( vecPosition.fY ) &&
                            bitStream.Read ( vecPosition.fZ ) )
                        {
                            // Set the icon to arrow and the target position
                            pCheckpoint->SetNextPosition ( vecPosition );
                            pCheckpoint->SetIcon ( CClientCheckpoint::ICON_ARROW );
                            pCheckpoint->SetHasTarget ( true );
                            pCheckpoint->SetTarget ( vecPosition );
                            pCheckpoint->ReCreateWithSameIdentifier();
                        }

                        break;
                    }
                }
            }
        }
    }
}


void CMarkerRPCs::SetMarkerIcon ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the marker id and icon
    unsigned char ucIcon;
    if ( bitStream.Read ( ucIcon ) )
    {
        // Grab the marker
        CClientMarker* pMarker = m_pMarkerManager->Get ( pSource->GetID () );
        if ( pMarker )
        {
            // Convert it to a checkpoint
            CClientCheckpoint* pCheckpoint = pMarker->GetCheckpoint ();
            if ( pCheckpoint )
            {
                pCheckpoint->SetIcon ( static_cast < unsigned int > ( ucIcon ) );
            }
        }
    }
}