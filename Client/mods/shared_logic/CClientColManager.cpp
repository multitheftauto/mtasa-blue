/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColManager.cpp
*  PURPOSE:     Collision entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using std::vector;

CClientColManager::~CClientColManager ( void )
{
    DeleteAll ();
}


void CClientColManager::DoPulse ( void )
{
    // Don't use iterators here as the list may get modified
    for ( uint i = 0 ; i < m_List.size () ; i++ )
        m_List[i]->DoPulse ();
}


void CClientColManager::DoHitDetection ( const CVector& vecNowPosition, float fRadius, CClientEntity* pEntity, CClientColShape * pJustThis, bool bChildren )
{
    // Skip if disconnecting
    if ( g_pClientGame->IsBeingDeleted () )
        return;

    if ( pJustThis )
        DoHitDetectionForColShape ( pJustThis );
    else
        DoHitDetectionForEntity ( vecNowPosition, 0.0f, pEntity );
}


//
// Handle the changing state of collision between one colshape and any entity
//
void CClientColManager::DoHitDetectionForColShape ( CClientColShape* pShape )
{
    // Ensure colshape is enabled and not being deleted
    if ( pShape->IsBeingDeleted () || !pShape->IsEnabled () )
        return;

    std::map < CClientEntity*, int > entityList;

    // Get all entities within the sphere
    CSphere querySphere = pShape->GetWorldBoundingSphere ();
    CClientEntityResult result;
    GetClientSpatialDatabase()->SphereQuery ( result, querySphere );
 
    // Extract relevant types
    for ( CClientEntityResult::const_iterator it = result.begin () ; it != result.end (); ++it )
    {
        CClientEntity* pEntity = *it;
        switch ( pEntity->GetType () )
        {
            case CCLIENTRADARMARKER:
            case CCLIENTRADARAREA:
            case CCLIENTTEAM:
            case CCLIENTGUI:
            case CCLIENTCOLSHAPE:
            case CCLIENTDUMMY:
            case SCRIPTFILE:
            case CCLIENTDFF:
            case CCLIENTCOL:
            case CCLIENTTXD:
            case CCLIENTSOUND:
                break;
            default:
                if ( pEntity->GetParent () )
                    entityList[ pEntity ] = 1;
        }
    }

    // Add existing colliders, so they can be disconnected if required
    for ( CFastList < CClientEntity* > ::const_iterator it = pShape->CollidersBegin () ; it != pShape->CollidersEnd (); ++it )
    {
       entityList[ *it ] = 1;
    }

    // Test each entity against the colshape
    for ( std::map < CClientEntity*, int > ::const_iterator it = entityList.begin () ; it != entityList.end (); ++it )
    {
        CClientEntity* pEntity = it->first;
        CVector vecPosition;
        pEntity->GetPosition ( vecPosition );

        // Collided?
        bool bHit = pShape->DoHitDetection ( vecPosition, 0.0f );
        HandleHitDetectionResult ( bHit, pShape, pEntity );
    }
}


//
// Handle the changing state of collision between one entity and any colshape
//
void CClientColManager::DoHitDetectionForEntity ( const CVector& vecNowPosition, float fRadius, CClientEntity* pEntity )
{
    std::map < CClientColShape*, int > shortList;

    // Get all entities within the sphere
    CClientEntityResult queryResult;
    GetClientSpatialDatabase()->SphereQuery ( queryResult, CSphere ( vecNowPosition, fRadius ) );

    // Extract colshapes
    for ( CClientEntityResult ::const_iterator it = queryResult.begin () ; it != queryResult.end (); ++it )
        if ( (*it)->GetType () == CCLIENTCOLSHAPE )
            shortList[ (CClientColShape*)*it ] = 1;

    // Add existing collisions, so they can be disconnected if required
    for ( CFastList < CClientColShape* > ::const_iterator it = pEntity->CollisionsBegin () ; it != pEntity->CollisionsEnd (); ++it )
        shortList[ *it ] = 1;

    // Test each colshape against the entity
    for ( std::map < CClientColShape*, int > ::const_iterator it = shortList.begin () ; it != shortList.end (); ++it )
    {
        CClientColShape* pShape = it->first;

        // Enabled and not being deleted?
        if ( !pShape->IsBeingDeleted () && pShape->IsEnabled () )
        {
            // Collided?
            bool bHit = pShape->DoHitDetection ( vecNowPosition, fRadius );
            HandleHitDetectionResult ( bHit, pShape, pEntity );
        }
    }
}


//
// Handle the changing state of collision between one colshape and one entity
//
void CClientColManager::HandleHitDetectionResult ( bool bHit, CClientColShape* pShape, CClientEntity* pEntity )
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
                pShape->CallEvent ( "onClientColShapeHit", Arguments, true );

                CLuaArguments Arguments2;
                Arguments2.PushElement ( pShape );
                Arguments2.PushBoolean ( ( pShape->GetDimension () == pEntity->GetDimension () ) );
                pEntity->CallEvent ( "onClientElementColShapeHit", Arguments2, true );
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
            pShape->CallEvent ( "onClientColShapeLeave", Arguments, true );

            CLuaArguments Arguments2;
            Arguments2.PushElement ( pShape );
            Arguments2.PushBoolean ( ( pShape->GetDimension () == pEntity->GetDimension () ) );
            pEntity->CallEvent ( "onClientElementColShapeLeave", Arguments2, true );

            pShape->CallLeaveCallback ( *pEntity );
        }
    }
}


bool CClientColManager::Exists ( CClientColShape* pShape )
{
    // Return true if it exists
    return ListContains ( m_List, pShape );
}


void CClientColManager::DeleteAll ( void )
{
    // Delete all of them
    vector < CClientColShape * > cloneList = m_List;
    vector < CClientColShape* > ::const_iterator iter = cloneList.begin ();
    for ( ; iter != cloneList.end (); ++iter )
    {
        delete *iter;
    }
    m_List.clear ();
}


void CClientColManager::RemoveFromList ( CClientColShape* pShape )
{
    ListRemove ( m_List, pShape );
}
