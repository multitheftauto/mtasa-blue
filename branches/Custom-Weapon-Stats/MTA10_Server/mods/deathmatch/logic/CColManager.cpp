/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CColManager.cpp
*  PURPOSE:     Collision entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CColManager::CColManager ( void )
{
    m_bCanRemoveFromList = true;
    m_bIteratingList = false;
}


CColManager::~CColManager ( void )
{
    DeleteAll ();
    TakeOutTheTrash ();
}


void CColManager::DoHitDetection ( const CVector& vecLastPosition, const CVector& vecNowPosition, float fRadius, CElement* pEntity, CColShape * pJustThis, bool bChildren )
{
    if ( pJustThis )
        DoHitDetectionForColShape ( pJustThis );
    else
        DoHitDetectionForEntity ( vecNowPosition, 0.0f, pEntity );
}


//
// Handle the changing state of collision between one colshape and any entity
//
void CColManager::DoHitDetectionForColShape ( CColShape* pShape )
{
    // Ensure colshape is enabled and not being deleted
    if ( pShape->IsBeingDeleted () || !pShape->IsEnabled () )
        return;

    std::map < CElement*, int > entityList;

    // Get all entities within the sphere
    CSphere querySphere = pShape->GetWorldBoundingSphere ();
    CElementResult result;
    GetSpatialDatabase()->SphereQuery ( result, querySphere );
 
    // Extract relevant types
    for ( CElementResult::const_iterator it = result.begin () ; it != result.end (); ++it )
    {
        CElement* pEntity = *it;
        switch ( pEntity->GetType () )
        {
            case CElement::COLSHAPE:
            case CElement::SCRIPTFILE:
            case CElement::RADAR_AREA:
            case CElement::CONSOLE:
            case CElement::TEAM:
            case CElement::BLIP:
            case CElement::DUMMY:
                break;
            default:
                if ( pEntity->GetParentEntity () )
                    entityList[ pEntity ] = 1;
        }
    }

    // Add existing colliders, so they can be disconnected if required
    for ( list < CElement* > ::const_iterator it = pShape->CollidersBegin () ; it != pShape->CollidersEnd (); ++it )
    {
       entityList[ *it ] = 1;
    }

    // Test each entity against the colshape
    for ( std::map < CElement*, int > ::const_iterator it = entityList.begin () ; it != entityList.end (); ++it )
    {
        CElement* pEntity = it->first;
        CVector vecPosition =
        pEntity->GetPosition ();

        // Collided?
        bool bHit = pShape->DoHitDetection ( vecPosition, vecPosition, 0.0f );
        HandleHitDetectionResult ( bHit, pShape, pEntity );
    }
}


//
// Handle the changing state of collision between one entity and any colshape
//
void CColManager::DoHitDetectionForEntity ( const CVector& vecNowPosition, float fRadius, CElement* pEntity )
{
    std::map < CColShape*, int > shortList;

    // Get all entities within the sphere
    CElementResult queryResult;
    GetSpatialDatabase()->SphereQuery ( queryResult, CSphere ( vecNowPosition, fRadius ) );

    // Extract colshapes
    for ( CElementResult ::const_iterator it = queryResult.begin () ; it != queryResult.end (); ++it )
        if ( (*it)->GetType () == CElement::COLSHAPE )
            shortList[ (CColShape*)*it ] = 1;

    // Add existing collisions, so they can be disconnected if required
    for ( list < CColShape* > ::const_iterator it = pEntity->CollisionsBegin () ; it != pEntity->CollisionsEnd (); ++it )
        shortList[ *it ] = 1;

    // Test each colshape against the entity
    for ( std::map < CColShape*, int > ::const_iterator it = shortList.begin () ; it != shortList.end (); ++it )
    {
        CColShape* pShape = it->first;

        // Enabled and not being deleted?
        if ( !pShape->IsBeingDeleted () && pShape->IsEnabled () )
        {
            // Collided?
            bool bHit = pShape->DoHitDetection ( vecNowPosition, vecNowPosition, fRadius );
            HandleHitDetectionResult ( bHit, pShape, pEntity );
        }
    }
}


//
// Handle the changing state of collision between one colshape and one entity
//
void CColManager::HandleHitDetectionResult ( bool bHit, CColShape* pShape, CElement* pEntity )
{
    if ( bHit )
    {              
        // If they havn't collided yet
        if ( !pEntity->CollisionExists ( pShape ) )
        {    
            // Add the collision and the collider
            pShape->AddCollider ( pEntity );
            pEntity->AddCollision ( pShape );

            // Can we call the event?
            if ( pShape->GetAutoCallEvent () )
            {
                // Call the event
                CLuaArguments Arguments;
                Arguments.PushElement ( pEntity );
                Arguments.PushBoolean ( ( pShape->GetDimension () == pEntity->GetDimension () ) );
                pShape->CallEvent ( "onColShapeHit", Arguments );

                CLuaArguments Arguments2;
                Arguments2.PushElement ( pShape );
                Arguments2.PushBoolean ( ( pShape->GetDimension () == pEntity->GetDimension () ) );
                pEntity->CallEvent ( "onElementColShapeHit", Arguments2 );
            }

            // Run whatever callback the collision item might have attached
            pShape->CallHitCallback ( *pEntity );
        }
    }
    else
    {
        // If they collided before
        if ( pEntity->CollisionExists ( pShape ) )
        {
            // Remove the collision and the collider
            pShape->RemoveCollider ( pEntity );
            pEntity->RemoveCollision ( pShape );

            // Call the event
            CLuaArguments Arguments;
            Arguments.PushElement ( pEntity );
            Arguments.PushBoolean ( ( pShape->GetDimension () == pEntity->GetDimension () ) );
            pShape->CallEvent ( "onColShapeLeave", Arguments );

            CLuaArguments Arguments2;
            Arguments2.PushElement ( pShape );
            Arguments2.PushBoolean ( ( pShape->GetDimension () == pEntity->GetDimension () ) );
            pEntity->CallEvent ( "onElementColShapeLeave", Arguments2 );

            pShape->CallLeaveCallback ( *pEntity );
        }
    }
}


bool CColManager::Exists ( CColShape* pShape )
{
    // Return true if it exists
    vector < CColShape* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pShape )
        {
            return true;
        }
    }

    return false;
}


void CColManager::DeleteAll ( void )
{
    // Delete all of them
    m_bCanRemoveFromList = false;
    vector < CColShape* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_bCanRemoveFromList = true;
    m_List.clear ();
}


void CColManager::RemoveFromList ( CColShape* pShape )
{
    if ( m_bCanRemoveFromList )
    {
        // Dont wanna remove it if we're going through the list
        if ( m_bIteratingList )
        {
            m_TrashCan.push_back ( pShape );
        }
        else
        {
            ListRemove ( m_List, pShape );
        }
    }
}


void CColManager::TakeOutTheTrash ( void )
{
    vector < CColShape* > ::const_iterator iter = m_TrashCan.begin ();
    for ( ; iter != m_TrashCan.end (); iter++ )
    {
        ListRemove ( m_List, *iter );
    }

    m_TrashCan.clear ();
}
