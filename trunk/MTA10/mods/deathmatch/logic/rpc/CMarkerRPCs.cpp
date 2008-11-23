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


void CMarkerRPCs::SetMarkerType ( NetBitStreamInterface& bitStream )
{
    // Read out the ID and the type
    ElementID ID;
    unsigned char ucType;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucType ) )
    {
        // Grab the marker
        CClientMarker* pMarker = m_pMarkerManager->Get ( ID );
        if ( pMarker )
        {
            // Set the new type
            pMarker->SetMarkerType ( static_cast < CClientMarker::eMarkerType > ( ucType ) );
        }
    }
}


void CMarkerRPCs::SetMarkerColor ( NetBitStreamInterface& bitStream )
{
    // Read out the ID and the color
    ElementID ID;
    unsigned long ulColor;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ulColor ) )
    {
        // Grab the marker
        CClientMarker* pMarker = m_pMarkerManager->Get ( ID );
        if ( pMarker )
        {
            // Set the new position
            pMarker->SetColor ( ulColor );
        }
    }
}


void CMarkerRPCs::SetMarkerSize ( NetBitStreamInterface& bitStream )
{
    // Read out the ID and the color
    ElementID ID;
    float fSize;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( fSize ) )
    {
        // Grab the marker
        CClientMarker* pMarker = m_pMarkerManager->Get ( ID );
        if ( pMarker )
        {
            // Set the new position
            pMarker->SetSize ( fSize );
        }
    }
}


void CMarkerRPCs::SetMarkerTarget ( NetBitStreamInterface& bitStream )
{
    // Read out the marker id and if there's a new target
    ElementID ID;
    unsigned char ucTemp;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucTemp ) )
    {
        // Grab the marker
        CClientMarker* pMarker = m_pMarkerManager->Get ( ID );
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


void CMarkerRPCs::SetMarkerIcon ( NetBitStreamInterface& bitStream )
{
    // Read out the marker id and icon
    ElementID ID;
    unsigned char ucIcon;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucIcon ) )
    {
        // Grab the marker
        CClientMarker* pMarker = m_pMarkerManager->Get ( ID );
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