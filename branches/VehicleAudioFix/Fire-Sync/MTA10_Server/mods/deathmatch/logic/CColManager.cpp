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

//
// This is now a mess due to backward compatabilty.
// ** Review before v1.1 release **
//

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



#ifdef SPATIAL_DATABASE_TESTS

struct CTestSet
{
    struct CTestResult
    {
        struct CEntityPair
        {
            CEntityPair ( CColShape* pInColShape, CElement* pInEntity )
                : pColShape(pInColShape)
                , pEntity(pInEntity)
            {}
            CColShape* pColShape;
            CElement* pEntity;
        };
        std::vector < CEntityPair > onList;
        std::vector < CEntityPair > offList;
    };
    CTestResult oldWay;
    CTestResult newWay;
    bool bCountOnly;
};

CTestSet testResult;

bool MyDataSortPredicate ( const CTestSet::CTestResult::CEntityPair& d1, const CTestSet::CTestResult::CEntityPair& d2 )
{
    return d1.pColShape == d2.pColShape ? d1.pEntity < d2.pEntity : d1.pColShape < d2.pColShape;
}

bool operator== ( const CTestSet::CTestResult::CEntityPair& d1, const CTestSet::CTestResult::CEntityPair& d2 )
{
    return d1.pColShape == d2.pColShape && d1.pEntity == d2.pEntity;
}

void RemoveSameItems( std::vector < CTestSet::CTestResult::CEntityPair >& listA ,std::vector < CTestSet::CTestResult::CEntityPair >& listB )
{
again:
    vector < CTestSet::CTestResult::CEntityPair > ::const_iterator itA = listA.begin ();
    for ( ; itA != listA.end (); itA++ )
    {
        vector < CTestSet::CTestResult::CEntityPair > ::const_iterator itB = listB.begin ();
        for ( ; itB != listB.end (); itB++ )
        {
            if ( *itA == *itB )
            {
                listA.erase ( itA );
                listB.erase ( itB );
                goto again;
            }
        }
    }
}

// Test version
void CColManager::DoHitDetection ( const CVector& vecLastPosition, const CVector& vecNowPosition, float fRadius, CElement* pEntity, CColShape * pJustThis, bool bChildren )
{
    // Prepare
    testResult = CTestSet ();
    testResult.bCountOnly = true;

    // Do old way
    DoHitDetectionOld ( vecLastPosition, vecNowPosition, fRadius, pEntity, pJustThis, bChildren );

    // Do new way
    if ( pJustThis )
        DoHitDetectionForColShape ( pJustThis );
    else
        DoHitDetectionForEntity ( vecNowPosition, 0.0f, pEntity );

    // Check test results

    // Order lists
    std::sort( testResult.oldWay.onList.begin(), testResult.oldWay.onList.end(), MyDataSortPredicate );
    std::sort( testResult.oldWay.offList.begin(), testResult.oldWay.offList.end(), MyDataSortPredicate );
    std::sort( testResult.newWay.onList.begin(), testResult.newWay.onList.end(), MyDataSortPredicate );
    std::sort( testResult.newWay.offList.begin(), testResult.newWay.offList.end(), MyDataSortPredicate );

    #ifdef SPATIAL_DATABASE_DEBUG_OUTPUTC
        // Report count
        if ( testResult.oldWay.onList.size () || testResult.newWay.onList.size () )
            OutputDebugLine( SString ( "onList  colshape:%d  newWay:%d  oldWay:%d", pJustThis!=NULL, testResult.newWay.onList.size (), testResult.oldWay.onList.size () ) );

        if ( testResult.oldWay.offList.size () || testResult.newWay.offList.size () )
            OutputDebugLine( SString ( "offList  colshape:%d  newWay:%d  oldWay:%d", pJustThis!=NULL, testResult.newWay.offList.size (), testResult.oldWay.offList.size () ) );

        // Report mismatch
        if ( testResult.oldWay.onList != testResult.newWay.onList )
            OutputDebugLine( SString ( "onList mismatch  colshape:%d  newWay:%d  oldWay:%d", pJustThis!=NULL, testResult.newWay.onList.size (), testResult.oldWay.onList.size () ) );

        if ( testResult.oldWay.offList != testResult.newWay.offList )
            OutputDebugLine( SString ( "offList mismatch  colshape:%d  newWay:%d  oldWay:%d", pJustThis!=NULL, testResult.newWay.offList.size (), testResult.oldWay.offList.size () ) );
    #endif

    // Diff
    RemoveSameItems( testResult.oldWay.onList, testResult.newWay.onList );
    RemoveSameItems( testResult.oldWay.offList, testResult.newWay.offList );

    // Output errors
    for ( vector < CTestSet::CTestResult::CEntityPair > ::const_iterator it = testResult.oldWay.onList.begin (); it != testResult.oldWay.onList.end (); it++ )
    {
        CColShape* pColShape = (*it).pColShape;
        CElement* pEntity = (*it).pEntity;
        CLogger::ErrorPrintf( "oldWay.onList has extra entity type: %d %s\n", pEntity->GetType (), pEntity->GetTypeName ().c_str () );
    }

    for ( vector < CTestSet::CTestResult::CEntityPair > ::const_iterator it = testResult.oldWay.offList.begin (); it != testResult.oldWay.offList.end (); it++ )
    {
        CColShape* pColShape = (*it).pColShape;
        CElement* pEntity = (*it).pEntity;
        CLogger::ErrorPrintf( "oldWay.offList has extra entity type: %d %s\n", pEntity->GetType (), pEntity->GetTypeName ().c_str () );
    }

    for ( vector < CTestSet::CTestResult::CEntityPair > ::const_iterator it = testResult.newWay.onList.begin (); it != testResult.newWay.onList.end (); it++ )
    {
        CColShape* pColShape = (*it).pColShape;
        CElement* pEntity = (*it).pEntity;
        CLogger::ErrorPrintf( "newWay.onList has extra entity type: %d %s\n", pEntity->GetType (), pEntity->GetTypeName ().c_str () );
    }

    for ( vector < CTestSet::CTestResult::CEntityPair > ::const_iterator it = testResult.newWay.offList.begin (); it != testResult.newWay.offList.end (); it++ )
    {
        CColShape* pColShape = (*it).pColShape;
        CElement* pEntity = (*it).pEntity;
        CLogger::ErrorPrintf( "newWay.offList has extra entity type: %d %s\n", pEntity->GetType (), pEntity->GetTypeName ().c_str () );
    }

    // Finally, use the old way to DoHitDetection
    testResult = CTestSet ();
    testResult.bCountOnly = false;
    DoHitDetectionOld ( vecLastPosition, vecNowPosition, fRadius, pEntity, pJustThis, bChildren );
}


void CColManager::DoHitDetectionOld ( const CVector& vecLastPosition, const CVector& vecNowPosition, float fRadius, CElement* pEntity, CColShape * pJustThis, bool bChildren )
{
    if ( bChildren )
    {
        list < CElement* > ::const_iterator iter = pEntity->IterBegin ();
        for ( ; iter != pEntity->IterEnd (); iter++ )
        {
            DoHitDetectionOld ( (*iter)->GetLastPosition(), (*iter)->GetPosition(), 0.0f, *iter, pJustThis, true );
        }
        if ( IS_COLSHAPE ( pEntity ) ||
             IS_FILE ( pEntity ) ||
             IS_HANDLING ( pEntity ) ||
             IS_RADAR_AREA ( pEntity ) ||
             IS_CONSOLE ( pEntity ) ||
             IS_TEAM ( pEntity ) ||
             IS_BLIP ( pEntity ) ||
             IS_DUMMY ( pEntity ) )
        {
            return;
        }
    }

    vector < CColShape * > cloneList;

    if ( !pJustThis )
    {
        // Call the hit detection event on all our shapes
        cloneList = m_List;
    }
    else
    {
        cloneList.push_back ( pJustThis );
    }

    m_bIteratingList = true;

    vector < CColShape* > ::const_iterator iter = cloneList.begin ();
    for ( ; iter != cloneList.end (); ++iter )
    {
        CColShape* pShape = *iter;

        // Not being deleted and enabled?
        if ( !pShape->IsBeingDeleted () && pShape->IsEnabled () )
        {
            // Collided?
            if ( pShape->DoHitDetection ( vecLastPosition, vecNowPosition, fRadius ) )
            {
                // If they havn't collided yet
                if ( !pEntity->CollisionExists ( pShape ) )
                {
                    testResult.oldWay.onList.push_back ( CTestSet::CTestResult::CEntityPair ( pShape, pEntity ) );

                    if ( !testResult.bCountOnly )
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
            }
            else
            {
                // If they collided before
                if ( pEntity->CollisionExists ( pShape ) )
                {
                    testResult.oldWay.offList.push_back ( CTestSet::CTestResult::CEntityPair ( pShape, pEntity ) );

                    if ( !testResult.bCountOnly )
                    {
                        // Remove the collision and the collider
                        pShape->RemoveCollider ( pEntity );
                        pEntity->RemoveCollision ( pShape );

                        // Can we call the event?
                        if ( pShape->GetAutoCallEvent () )
                        {
                            // Call the event
                            CLuaArguments Arguments;
                            Arguments.PushElement ( pEntity );
                            Arguments.PushBoolean ( ( pShape->GetDimension () == pEntity->GetDimension () ) );
                            pShape->CallEvent ( "onColShapeLeave", Arguments );

                            CLuaArguments Arguments2;
                            Arguments2.PushElement ( pShape );
                            Arguments2.PushBoolean ( ( pShape->GetDimension () == pEntity->GetDimension () ) );
                            pEntity->CallEvent ( "onElementColShapeLeave", Arguments2 );
                        }

                        // Run whatever callback the collision item might have attached
                        pShape->CallLeaveCallback ( *pEntity );
                    }
                }
            }
        }
    }
    m_bIteratingList = false;
    TakeOutTheTrash ();
}



#else  // SPATIAL_DATABASE_TESTS

void CColManager::DoHitDetection ( const CVector& vecLastPosition, const CVector& vecNowPosition, float fRadius, CElement* pEntity, CColShape * pJustThis, bool bChildren )
{
    if ( pJustThis )
        DoHitDetectionForColShape ( pJustThis );
    else
        DoHitDetectionForEntity ( vecNowPosition, 0.0f, pEntity );
}

#endif  // SPATIAL_DATABASE_TESTS


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
            case CElement::HANDLING:
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
  
#ifdef SPATIAL_DATABASE_TESTS
            testResult.newWay.onList.push_back ( CTestSet::CTestResult::CEntityPair ( pShape, pEntity ) );
            if ( !testResult.bCountOnly )
#endif
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
    }
    else
    {
        // If they collided before
        if ( pEntity->CollisionExists ( pShape ) )
        {

#ifdef SPATIAL_DATABASE_TESTS
            testResult.newWay.offList.push_back ( CTestSet::CTestResult::CEntityPair ( pShape, pEntity ) );
            if ( !testResult.bCountOnly )
#endif
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
