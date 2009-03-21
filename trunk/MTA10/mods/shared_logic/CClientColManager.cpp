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

CClientColManager::~CClientColManager ( void )
{
    DeleteAll ();
}


void CClientColManager::DoHitDetection ( const CVector& vecNowPosition, float fRadius, CClientEntity* pEntity )
{
    CClientColShape* pShape = NULL;
    // Call the hit detection event on all our shapes
    list < CClientColShape * > cloneList = m_List;
    list < CClientColShape* > ::const_iterator iter = cloneList.begin ();
    for ( ; iter != cloneList.end (); iter++ )
    {
        pShape = *iter;

        // Enabled and not being deleted?
        if ( !pShape->IsBeingDeleted () && pShape->IsEnabled () )
        {
            // Collided?
            if ( pShape->DoHitDetection ( vecNowPosition, fRadius ) )
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
    }
}


bool CClientColManager::Exists ( CClientColShape* pShape )
{
    // Return true if it exists
    list < CClientColShape * > cloneList = m_List;
    list < CClientColShape* > ::const_iterator iter = cloneList.begin ();
    for ( ; iter != cloneList.end (); iter++ )
    {
        if ( *iter == pShape )
        {
            return true;
        }
    }

    return false;
}


void CClientColManager::DeleteAll ( void )
{
    // Delete all of them
    list < CClientColShape * > cloneList = m_List;
    list < CClientColShape* > ::const_iterator iter = cloneList.begin ();
    for ( ; iter != cloneList.end (); iter++ )
    {
        delete *iter;
    }
    m_List.clear ();
}


void CClientColManager::RemoveFromList ( CClientColShape* pShape )
{
    m_List.remove ( pShape );
}
