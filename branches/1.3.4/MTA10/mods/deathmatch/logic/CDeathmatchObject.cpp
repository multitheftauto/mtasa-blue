/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDeathmatchObject.cpp
*  PURPOSE:     Object element interface
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

extern CClientGame * g_pClientGame;

#ifdef WITH_OBJECT_SYNC
CDeathmatchObject::CDeathmatchObject ( CClientManager* pManager, CMovingObjectsManager* pMovingObjectsManager, CObjectSync* pObjectSync, ElementID ID, unsigned short usModel ) : ClassInit ( this ), CClientObject ( pManager, ID, usModel )
{
    m_pMovingObjectsManager = pMovingObjectsManager;
    m_pObjectSync = pObjectSync;
    m_pMoveAnimation = NULL;
}


CDeathmatchObject::~CDeathmatchObject ( void )
{
    _StopMovement ( true );

    if ( m_pObjectSync )
    {
        m_pObjectSync->RemoveObject ( this );
    }
}

#else

CDeathmatchObject::CDeathmatchObject ( CClientManager* pManager, CMovingObjectsManager* pMovingObjectsManager, ElementID ID, unsigned short usModel, bool bLowLod ) : ClassInit ( this ), CClientObject ( pManager, ID, usModel, bLowLod )
{
    m_pMovingObjectsManager = pMovingObjectsManager;
    m_pMoveAnimation = NULL;
}


CDeathmatchObject::~CDeathmatchObject ( void )
{
    _StopMovement ( true );
}
#endif



void CDeathmatchObject::StartMovement ( const CPositionRotationAnimation& a_rMoveAnimation )
{
    if ( m_pMoveAnimation != NULL )
    {
        _StopMovement ( true );
    }

    if ( a_rMoveAnimation.IsRunning () )
    {
        // Add us to the moving object's manager
        m_pMovingObjectsManager->Add ( this );
        m_pMoveAnimation = new CPositionRotationAnimation ( a_rMoveAnimation );
    }
    else
    {
        SPositionRotation positionRotation;
        a_rMoveAnimation.GetFinalValue ( positionRotation );
        SetOrientation ( positionRotation.m_vecPosition, positionRotation.m_vecRotation );
    }
}


void CDeathmatchObject::StopMovement ( void )
{
   _StopMovement ( true );
}

void CDeathmatchObject::_StopMovement ( bool a_bUnregister )
{
    if ( m_pMoveAnimation != NULL )
    {
        if ( a_bUnregister )
        {
            m_pMovingObjectsManager->Remove ( this );
        }
        delete m_pMoveAnimation;
        m_pMoveAnimation = NULL;
    }
}

void CDeathmatchObject::UpdateMovement ( void )
{
    SPositionRotation positionRotation;
    bool bStillRunning = m_pMoveAnimation->GetValue ( positionRotation );
   
    SetOrientation ( positionRotation.m_vecPosition, positionRotation.m_vecRotation );
    
    if ( !bStillRunning )
    {
        _StopMovement ( false ); //We don't unregister ourselves here since CDeathmatchObject::UpdateMovement is called from an iteration in CMovingObjectsManager::DoPulse
        // and we are automatically removed from the list after CDeathmatchObject::UpdateMovement if we are finished
    }
}


//
// SetPosition with update of contacting peds
//
void CDeathmatchObject::SetPosition ( const CVector& vecPosition )
{
    CVector vecPreviousPosition = m_vecPosition;
    CVector vecPreviousRotation = m_vecRotation;

    // Call original
    CClientObject::SetPosition( vecPosition );

    // Update our contact players
    UpdateContactingBegin( vecPreviousPosition, vecPreviousRotation );
}


//
// SetRotationRadians with update of contacting peds
//
void CDeathmatchObject::SetRotationRadians ( const CVector& vecRotationRadians )
{
    CVector vecPreviousPosition = m_vecPosition;
    CVector vecPreviousRotation = m_vecRotation;

    // Call original
    CClientObject::SetRotationRadians( vecRotationRadians );

    // Update our contact players
    UpdateContactingBegin( vecPreviousPosition, vecPreviousRotation );
}


//
// SetOrientation with update of contacting peds
//
void CDeathmatchObject::SetOrientation ( const CVector& vecPosition, const CVector& vecRotationRadians )
{
    CVector vecPreviousPosition = m_vecPosition;
    CVector vecPreviousRotation = m_vecRotation;

    // Call original
    CClientObject::SetOrientation( vecPosition, vecRotationRadians );

    // Update our contact players
    UpdateContactingBegin( vecPreviousPosition, vecPreviousRotation );
}


//
// Prepare vars for UpdateContacting
//
void CDeathmatchObject::UpdateContactingBegin ( const CVector& vecPreviousPosition, const CVector& vecPreviousRotation )
{
    // Calc frame delta
    CVector vecFrameRotation    = m_vecRotation - vecPreviousRotation;
    CVector vecFrameTranslation = m_vecPosition - vecPreviousPosition;
    CVector vecCenterOfRotation = m_vecPosition;

    // Rotate frame rotation x/y by absolute rotation z, for some reason
    RotateVector ( vecFrameRotation, CVector(0, 0, -vecPreviousRotation.fZ ) );

    UpdateContacting ( vecCenterOfRotation, vecFrameTranslation, vecFrameRotation );
}


//
// Apply object movement to contacting peds
//
void CDeathmatchObject::UpdateContacting ( const CVector& vecCenterOfRotation, const CVector& vecFrameTranslation, const CVector& vecFrameRotation )
{
    bool bHasRotation    = ( vecFrameRotation.fX != 0 || vecFrameRotation.fY != 0 || vecFrameRotation.fZ != 0 );
    bool bHasTranslation = ( vecFrameTranslation.fX != 0 || vecFrameTranslation.fY != 0 || vecFrameTranslation.fZ != 0 );

    // Early out if no orientation change here
    if ( !bHasRotation && !bHasTranslation )
        return;

    // Step through each contacting ped
    list < CClientPed * > ::iterator iter = m_Contacts.begin ();
    for ( ; iter != m_Contacts.end () ; ++iter )
    {
        CClientPed* pPed = *iter;

        // Get ped start position
        CVector vecPlayerPosition;
        pPed->GetPosition ( vecPlayerPosition );

        // Apply rotation effect on position
        if ( bHasRotation )
        {
            vecPlayerPosition -= vecCenterOfRotation;
            RotateVector ( vecPlayerPosition, CVector(-vecFrameRotation.fX, -vecFrameRotation.fY, -vecFrameRotation.fZ ) );
            vecPlayerPosition += vecCenterOfRotation;
        }

        // Apply translation
        vecPlayerPosition += vecFrameTranslation;
        pPed->SetPosition ( vecPlayerPosition );

        // Also change ped facing direction
        if ( vecFrameRotation.fZ != 0 )
        {
            float fRotationZ = pPed->GetCurrentRotation ();
            pPed->SetCurrentRotation ( fRotationZ + vecFrameRotation.fZ );
        }
    }

    // Look in attached objects for more ped contacts
    list < CClientEntity * > ::iterator itera = m_AttachedEntities.begin ();
    for ( ; itera != m_AttachedEntities.end () ; ++itera )
    {
        CClientEntity* pEntity = *itera;
        if ( IS_OBJECT ( pEntity ) )
        {
            CDeathmatchObject* pObject = static_cast < CDeathmatchObject* > ( pEntity );
            pObject->UpdateContacting( vecCenterOfRotation, vecFrameTranslation, vecFrameRotation );
        }
    }
}








