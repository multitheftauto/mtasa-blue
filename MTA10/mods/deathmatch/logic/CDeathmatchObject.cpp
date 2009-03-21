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

            CVector vecPreviousPosition, vecPosition;
            GetPosition ( vecPreviousPosition );
            vecPosition = m_vecStartPosition + vecDeltaPosition;

            // Plus the position with our new interpolated delta position and set it
            SetOrientation ( vecPosition, m_vecStartRotation + vecRotation );
        
            // Update our contact players
            CVector vecPlayerPosition;
            CClientPed * pPed = NULL;
            list < CClientPed * > ::iterator iter = m_Contacts.begin ();
            for ( ; iter != m_Contacts.end () ; iter++ )
            {
                pPed = *iter;
                pPed->GetPosition ( vecPlayerPosition );
                vecPlayerPosition = vecPosition + ( vecPlayerPosition - vecPreviousPosition );
                pPed->SetPosition ( vecPlayerPosition );
            }
        }
        else
        {
            FinishMovement ();
        }
    }
}
