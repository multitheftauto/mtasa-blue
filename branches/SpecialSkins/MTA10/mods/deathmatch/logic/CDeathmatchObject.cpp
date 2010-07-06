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

CDeathmatchObject::CDeathmatchObject ( CClientManager* pManager, CMovingObjectsManager* pMovingObjectsManager, ElementID ID, unsigned short usModel ) : CClientObject ( pManager, ID, usModel )
{
    m_pMovingObjectsManager = pMovingObjectsManager;
    m_ulStartTime = 0;
    m_ulTargetTime = 0;
}


CDeathmatchObject::~CDeathmatchObject ( void )
{
    m_pMovingObjectsManager->Remove ( this );
}


void CDeathmatchObject::StartMovement ( const CVector& vecTargetPosition, const CVector& vecTargetRotation, unsigned long ulTime )
{
    // More than 0 ms to move?
    if ( ulTime > 0 )
    {
        // Set our data
        m_ulStartTime = CClientTime::GetTime ();
        m_ulTargetTime = m_ulStartTime + ulTime;
        GetPosition ( m_vecStartPosition );
        GetRotationRadians ( m_vecStartRotation );
        m_vecTargetPosition = vecTargetPosition;
        m_vecTargetRotation = vecTargetRotation;

        // Get the start rotation between 0 and 2*pi
        if ( m_vecStartRotation.fX < 0 )
            m_vecStartRotation.fX += 2.0f * PI;
        else if ( m_vecStartRotation.fX >= 2*PI )
            m_vecStartRotation.fX -= 2.0f * PI;

        if ( m_vecStartRotation.fY < 0 )
            m_vecStartRotation.fY += 2.0f * PI;
        else if ( m_vecStartRotation.fY >= 2*PI )
            m_vecStartRotation.fY -= 2.0f * PI;

        if ( m_vecStartRotation.fZ < 0 )
            m_vecStartRotation.fZ += 2.0f * PI;
        else if ( m_vecStartRotation.fZ >= 2*PI )
            m_vecStartRotation.fZ -= 2.0f * PI;

        // Add us to the moving object's manager
        m_pMovingObjectsManager->Add ( this );
    }
    else
    {
        SetPosition ( vecTargetPosition );
        SetRotationRadians ( vecTargetRotation );
    }
}


void CDeathmatchObject::StopMovement ( void )
{
    // Prevent it from moving in the future
    m_ulStartTime = 0;
    m_ulTargetTime = 0;
}


void CDeathmatchObject::FinishMovement ( void )
{
    // Prevent it from moving in the future
    m_ulStartTime = 0;
    m_ulTargetTime = 0;

    // Set the target position/rotation (ensures accuracy)
    SetPosition ( m_vecTargetPosition );
    SetRotationRadians ( m_vecStartRotation + m_vecTargetRotation );
}


void CDeathmatchObject::UpdateMovement ( void )
{
    // Got a start and end time?
    if ( m_ulStartTime != 0 )
    {
        // We're past our end time?
        unsigned long ulCurrentTime = CClientTime::GetTime ();
        if ( ulCurrentTime < m_ulTargetTime )
        {
            // Grab the movement duration and the time passed as floats
            float fDuration = static_cast < float > ( m_ulTargetTime - m_ulStartTime );
            float fTimePassed = static_cast < float > ( ulCurrentTime - m_ulStartTime );

            // Grab the delta position and rotation
            CVector vecDeltaPosition = m_vecTargetPosition - m_vecStartPosition;
            CVector vecRotation = m_vecTargetRotation;

            // Divide it on the duration and multiply with the time passed
            vecDeltaPosition /= fDuration;
            vecDeltaPosition *= fTimePassed;
            vecRotation /= fDuration;
            vecRotation *= fTimePassed;

            CVector vecPosition = m_vecStartPosition + vecDeltaPosition;

            // Plus the position with our new interpolated delta position and set it
            SetOrientation ( vecPosition, m_vecStartRotation + vecRotation );
        }
        else
        {
            FinishMovement ();
        }
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
    for ( ; iter != m_Contacts.end () ; iter++ )
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
    for ( ; itera != m_AttachedEntities.end () ; itera++ )
    {
        CClientEntity* pEntity = *itera;
        if ( IS_OBJECT ( pEntity ) )
        {
            CDeathmatchObject* pObject = static_cast < CDeathmatchObject* > ( pEntity );
            pObject->UpdateContacting( vecCenterOfRotation, vecFrameTranslation, vecFrameRotation );
        }
    }
}








