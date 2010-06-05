/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CObjectRPCs.cpp
*  PURPOSE:     Object remote procedure calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CObjectRPCs.h"

void CObjectRPCs::LoadFunctions ( void )
{
    AddHandler ( DESTROY_ALL_OBJECTS, DestroyAllObjects, "DestroyAllObjects" );
    AddHandler ( SET_OBJECT_ROTATION, SetObjectRotation, "SetObjectRotation" );
    AddHandler ( MOVE_OBJECT, MoveObject, "MoveObject" );
    AddHandler ( STOP_OBJECT, StopObject, "StopObject" );
}


void CObjectRPCs::DestroyAllObjects ( NetBitStreamInterface& bitStream )
{
    m_pObjectManager->DeleteAll ();
}


void CObjectRPCs::SetObjectRotation ( NetBitStreamInterface& bitStream )
{
    // Read out the object ID
    ElementID ID;
    if ( bitStream.Read ( ID ) )
    {
        // Grab the object
        CDeathmatchObject* pObject = static_cast < CDeathmatchObject* > ( m_pObjectManager->Get ( ID ) );
        if ( pObject )
        {
            // Read out the new rotation
            CVector vecRotation;
            if ( bitStream.Read ( vecRotation.fX ) &&
                 bitStream.Read ( vecRotation.fY ) &&
                 bitStream.Read ( vecRotation.fZ ) )
            {
                // Set the new rotation
                pObject->SetRotationRadians ( vecRotation );

                // Update our target rotation
                pObject->SetTargetRotation ( vecRotation );
            }
        }
    }
}


void CObjectRPCs::MoveObject ( NetBitStreamInterface& bitStream )
{
    // Read out the object ID
    ElementID ID;
    if ( bitStream.Read ( ID ) )
    {
        // Grab the object
        CDeathmatchObject* pObject = static_cast < CDeathmatchObject* > ( m_pObjectManager->Get ( ID ) );
        if ( pObject )
        {
            // Read out the new time, position and rotation
            unsigned long ulTime;
            CVector vecSourcePosition;
            CVector vecSourceRotation;
            CVector vecTargetPosition;
            CVector vecTargetRotation;
            if ( bitStream.Read ( ulTime ) &&
                 bitStream.Read ( vecSourcePosition.fX ) &&
                 bitStream.Read ( vecSourcePosition.fY ) &&
                 bitStream.Read ( vecSourcePosition.fZ ) &&
                 bitStream.Read ( vecSourceRotation.fX ) &&
                 bitStream.Read ( vecSourceRotation.fY ) &&
                 bitStream.Read ( vecSourceRotation.fZ ) &&
                 bitStream.Read ( vecTargetPosition.fX ) &&
                 bitStream.Read ( vecTargetPosition.fY ) &&
                 bitStream.Read ( vecTargetPosition.fZ ) &&
                 bitStream.Read ( vecTargetRotation.fX ) &&
                 bitStream.Read ( vecTargetRotation.fY ) &&
                 bitStream.Read ( vecTargetRotation.fZ ) )
            {
                // Set it to the source position, then make it move towards that position
                pObject->SetOrientation ( vecSourcePosition, vecSourceRotation );
                pObject->StartMovement ( vecTargetPosition, vecTargetRotation, ulTime );
            }
        }
    }
}


void CObjectRPCs::StopObject ( NetBitStreamInterface& bitStream )
{
    // Read out the object ID
    ElementID ID;
    if ( bitStream.Read ( ID ) )
    {
        // Grab the object
        CDeathmatchObject* pObject = static_cast < CDeathmatchObject* > ( m_pObjectManager->Get ( ID ) );
        if ( pObject )
        {
            // Read out the position and rotation
            CVector vecSourcePosition;
            CVector vecSourceRotation;
            if ( bitStream.Read ( vecSourcePosition.fX ) &&
                 bitStream.Read ( vecSourcePosition.fY ) &&
                 bitStream.Read ( vecSourcePosition.fZ ) &&
                 bitStream.Read ( vecSourceRotation.fX ) &&
                 bitStream.Read ( vecSourceRotation.fY ) &&
                 bitStream.Read ( vecSourceRotation.fZ ) )
            {
                // Stop the movement
                pObject->StopMovement ();
                // Set it to the source position and rotation
                pObject->SetOrientation ( vecSourcePosition, vecSourceRotation );
            }
        }
    }
}