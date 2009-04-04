/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CElementRPCs.cpp
*  PURPOSE:     Element remote procedure calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CElementRPCs.h"

using std::list;

void CElementRPCs::LoadFunctions ( void )
{
    AddHandler ( SET_ELEMENT_PARENT, SetElementParent, "SetElementParent" );
    AddHandler ( SET_ELEMENT_DATA, SetElementData, "SetElementData" );
    AddHandler ( REMOVE_ELEMENT_DATA, RemoveElementData, "RemoveElementData" );
    AddHandler ( SET_ELEMENT_POSITION, SetElementPosition, "SetElementPosition" );
    AddHandler ( SET_ELEMENT_VELOCITY, SetElementVelocity, "SetElementVelocity" );
    AddHandler ( SET_ELEMENT_INTERIOR, SetElementInterior, "SetElementInterior" );
    AddHandler ( SET_ELEMENT_DIMENSION, SetElementDimension, "" );
    AddHandler ( ATTACH_ELEMENTS, AttachElements, "AttachElements" );
    AddHandler ( DETACH_ELEMENTS, DetachElements, "DetachElements" );
    AddHandler ( SET_ELEMENT_ALPHA, SetElementAlpha, "SetElementAlpha" );
	AddHandler ( SET_ELEMENT_NAME, SetElementName, "SetElementName" );
    AddHandler ( SET_ELEMENT_HEALTH, SetElementHealth, "SetElementHealth" );
    AddHandler ( SET_ELEMENT_MODEL, SetElementModel, "SetElementModel" );
}


void CElementRPCs::SetElementParent ( NetBitStreamInterface& bitStream )
{
    // Read out the entity id and parent id
    ElementID ID, ParentID;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ParentID ) )
    {
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        CClientEntity* pParent = CElementIDs::GetElement ( ParentID );
        if ( pEntity && pParent )
        {
            pEntity->SetParent ( pParent );
        }
        else
        {
            // TODO: raise an error
        }
    }
    else
    {
        // TODO: raise an error
    }
}


void CElementRPCs::SetElementData ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned short usNameLength;
    if ( bitStream.Read ( ID ) && bitStream.Read ( usNameLength ) )
    {
        char* szName = new char [ usNameLength + 1 ];
		szName [ usNameLength ] = NULL;

        CLuaArgument Argument;
        if ( bitStream.Read ( szName, usNameLength ) && Argument.ReadFromBitStream ( bitStream ) )
        {
            CClientEntity* pEntity = CElementIDs::GetElement ( ID );
            if ( pEntity )
            {
                pEntity->SetCustomData ( szName, Argument, NULL );
            }
        }            
        delete [] szName;
    }
}


void CElementRPCs::RemoveElementData ( NetBitStreamInterface& bitStream )
{
    // Read out the entity id and name length
    ElementID ID;
    unsigned short usNameLength;
    unsigned char ucRecursive;
    if ( bitStream.Read ( ID ) && bitStream.Read ( usNameLength ) )
    {
        // Allocate a buffer for the name
        char* szName = new char [ usNameLength + 1 ];
		szName [ usNameLength ] = NULL;

        // Read it out plus whether it's recursive or not
        if ( bitStream.Read ( szName, usNameLength ) &&
             bitStream.Read ( ucRecursive ) )
        {
            // Grab the entity
            CClientEntity* pEntity = CElementIDs::GetElement ( ID );
            if ( pEntity )
            {
                // Remove that name
                pEntity->DeleteCustomData ( szName, ucRecursive != 0 );
            }
        }

        // Delete the name buffer
        delete [] szName;
    }
}


void CElementRPCs::SetElementPosition ( NetBitStreamInterface& bitStream )
{
    // Read out the entity id and the position
    ElementID ID;
    CVector vecPosition;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( vecPosition.fX ) &&
         bitStream.Read ( vecPosition.fY ) &&
         bitStream.Read ( vecPosition.fZ ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        // Grab the entity
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity )
        {            
            // Update the sync context to the new one
            pEntity->SetSyncTimeContext ( ucTimeContext );

            // If it's a player, use Teleport
            if ( pEntity->GetType () == CCLIENTPLAYER )
            {
                CClientPlayer* pPlayer = static_cast < CClientPlayer* > ( pEntity );

                pPlayer->Teleport ( vecPosition );
                pPlayer->ResetInterpolation ();

                // If local player, reset return position (so we can't warp back if connection fails)
                if ( pPlayer->IsLocalPlayer () )
                {
                    m_pClientGame->GetNetAPI ()->ResetReturnPosition ();
                }
            }
            else if ( pEntity->GetType () == CCLIENTVEHICLE ) 
            {
                CClientVehicle* pVehicle = static_cast < CClientVehicle* > ( pEntity );
                pVehicle->RemoveTargetPosition ();
                pVehicle->SetPosition ( vecPosition );
            }
            else
            {
                // Set its position
                pEntity->SetPosition ( vecPosition );
            }
        }
    }
}


void CElementRPCs::SetElementVelocity ( NetBitStreamInterface& bitStream )
{
    // Read out the entity id and the speed
    ElementID ID;
    CVector vecVelocity;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( vecVelocity.fX ) &&
         bitStream.Read ( vecVelocity.fY ) &&
         bitStream.Read ( vecVelocity.fZ ) )
    {
        // Grab the entity
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity )
        {
            switch ( pEntity->GetType () )
            {
                case CCLIENTPED:
                case CCLIENTPLAYER:
                {
                    CClientPed* pPed = static_cast < CClientPed* > ( pEntity );

                    pPed->SetMoveSpeed ( vecVelocity );
                    pPed->ResetInterpolation ();

                    // If local player, reset return position (so we can't warp back if connection fails)
                    if ( pPed->IsLocalPlayer () )
                    {
                        m_pClientGame->GetNetAPI ()->ResetReturnPosition ();
                    }
                    break;
                }
                case CCLIENTVEHICLE:
                {
                    CClientVehicle* pVehicle = static_cast < CClientVehicle* > ( pEntity );                    
                    pVehicle->SetMoveSpeed ( vecVelocity );

                    break;
                }
                case CCLIENTOBJECT:
                {
                    CClientObject * pObject = static_cast < CClientObject * > ( pEntity );
                    pObject->SetMoveSpeed ( vecVelocity );
                    
                    break;
                }
            }
        }
    }
}


void CElementRPCs::SetElementInterior ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucInterior, ucSetPosition;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucInterior ) && bitStream.Read ( ucSetPosition ) )
    {
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity )
        {
            pEntity->SetInterior ( ucInterior );

            if ( ucSetPosition == 1 )
            {
                CVector vecPosition;
                if ( bitStream.Read ( vecPosition.fX ) &&
                     bitStream.Read ( vecPosition.fY ) &&
                     bitStream.Read ( vecPosition.fZ ) )
                {
                    pEntity->SetPosition ( vecPosition );
                }
            }
        }
    }
}


void CElementRPCs::SetElementDimension ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned short usDimension;
    if ( bitStream.Read ( ID ) && bitStream.Read ( usDimension ) )
    {
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity )
        {
            if ( pEntity->GetType () == CCLIENTTEAM )
            {
                CClientTeam* pTeam = static_cast < CClientTeam* > ( pEntity );
                list < CClientPlayer* > ::const_iterator iter = pTeam->IterBegin ();
                for ( ; iter != pTeam->IterEnd () ; iter++ )
                {
                    CClientPlayer* pPlayer = *iter;
                    if ( pPlayer->IsLocalPlayer () )
                    {
                        // Update all of our streamers/managers to the local player's dimension
                        m_pClientGame->SetAllDimensions ( usDimension );
                    }

                    pPlayer->SetDimension ( usDimension );
                }
            }
            else
            {
                if ( pEntity->GetType () == CCLIENTPLAYER )
                {
                    CClientPlayer* pPlayer = static_cast < CClientPlayer* > ( pEntity );
                    if ( pPlayer->IsLocalPlayer () )
                    {
                        // Update all of our streamers/managers to the local player's dimension
                        m_pClientGame->SetAllDimensions ( usDimension );
                    }         
                }
                
                pEntity->SetDimension ( usDimension );
            }
        }
    }
}


void CElementRPCs::AttachElements ( NetBitStreamInterface& bitStream )
{
    ElementID ID, usAttachedToID;
    CVector vecPosition, vecRotation;
    if ( bitStream.Read ( ID ) && bitStream.Read ( usAttachedToID ) &&
        bitStream.Read ( vecPosition.fX ) &&
        bitStream.Read ( vecPosition.fY ) &&
        bitStream.Read ( vecPosition.fZ ) &&
        bitStream.Read ( vecRotation.fX ) &&
        bitStream.Read ( vecRotation.fY ) &&
        bitStream.Read ( vecRotation.fZ ) )
    {
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        CClientEntity* pAttachedToEntity = CElementIDs::GetElement ( usAttachedToID );
        if ( pEntity && pAttachedToEntity )
        {
            pEntity->SetAttachedOffsets ( vecPosition, vecRotation );
            pEntity->AttachTo ( pAttachedToEntity );
        }
    }
}


void CElementRPCs::DetachElements ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity )
        {
            pEntity->SetSyncTimeContext ( ucTimeContext );
            pEntity->AttachTo ( NULL );

            CVector vecPosition;
            if ( bitStream.Read ( vecPosition.fX ) &&
                 bitStream.Read ( vecPosition.fY ) &&
                 bitStream.Read ( vecPosition.fZ ) )
            {
                pEntity->SetPosition ( vecPosition );
            }
        }
    }
}


void CElementRPCs::SetElementAlpha ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned char ucAlpha;
    if ( bitStream.Read ( ID ) && bitStream.Read ( ucAlpha ) )
    {
        CClientEntity* pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity )
        {
            switch ( pEntity->GetType () )
            {
                case CCLIENTPED:
                case CCLIENTPLAYER:
                {
                    CClientPed* pPed = static_cast < CClientPed* > ( pEntity );
                    pPed->SetAlpha ( ucAlpha );
                    break;
                }                
                case CCLIENTVEHICLE:
                {
                    CClientVehicle* pVehicle = static_cast < CClientVehicle* > ( pEntity );
                    pVehicle->SetAlpha ( ucAlpha );
                    break;
                }
                case CCLIENTOBJECT:
                {
                    CClientObject * pObject = static_cast < CClientObject* > ( pEntity );
                    pObject->SetAlpha ( ucAlpha );
                    break;
                }
                default: break;
            }
        }
    }
}


void CElementRPCs::SetElementName ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned short usNameLength;
    if ( bitStream.Read ( ID ) && bitStream.Read ( usNameLength ) )
    {
        char* szName = new char [ usNameLength + 1 ];
		szName [ usNameLength ] = 0;

        if ( bitStream.Read ( szName, usNameLength ) )
        {
            CClientEntity* pEntity = CElementIDs::GetElement ( ID );
            if ( pEntity )
            {
				pEntity->SetName ( szName );
            }
        }            
        delete [] szName;
    }
}


void CElementRPCs::SetElementHealth ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    float fHealth;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( fHealth ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        CClientEntity * pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity )
        {
            pEntity->SetSyncTimeContext ( ucTimeContext );

            switch ( pEntity->GetType () )
            {
                case CCLIENTPED:
                case CCLIENTPLAYER:
                {
                    CClientPed* pPed = static_cast < CClientPed * > ( pEntity );                    
                    pPed->SetHealth ( fHealth );
                    break;
                }

                case CCLIENTVEHICLE:
                {
                    CClientVehicle* pVehicle = static_cast < CClientVehicle * > ( pEntity );
                    pVehicle->SetHealth ( fHealth );
                    break;
                }
            }
        }
    }
}


void CElementRPCs::SetElementModel ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    unsigned short usModel;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( usModel ) )
    {
        CClientEntity * pEntity = CElementIDs::GetElement ( ID );
        if ( pEntity )
        {
            switch ( pEntity->GetType () )
            {
                case CCLIENTPED:
                case CCLIENTPLAYER:
                {
                    CClientPed* pPed = static_cast < CClientPed * > ( pEntity );
                    pPed->SetModel ( usModel );
                    break;
                }

                case CCLIENTVEHICLE:
                {
                    CClientVehicle* pVehicle = static_cast < CClientVehicle * > ( pEntity );
                    pVehicle->SetModelBlocking ( usModel );
                    break;
                }

                case CCLIENTOBJECT:
                {
                    CClientObject* pObject = static_cast < CClientObject * > ( pEntity );
                    pObject->SetModel ( usModel );
                    break;
                }
            }
        }
    }
}