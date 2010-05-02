/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CObject.cpp
*  PURPOSE:     Object entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame * g_pGame;

CObject::CObject ( CElement* pParent, CXMLNode* pNode, CObjectManager* pObjectManager ) : CElement ( pParent, pNode )
{
    // Init
    m_iType = CElement::OBJECT;
    SetTypeName ( "object" );
    m_szName [0] = 0;
    m_szName [MAX_ELEMENT_NAME_LENGTH] = 0;

    m_pObjectManager = pObjectManager;
    m_usModel = 0xFFFF;
    m_moveData.bActive = false;
    m_ucAlpha = 255;

    // Add us to the manager's list
    pObjectManager->AddToList ( this );
}


CObject::CObject ( const CObject& Copy ) : CElement ( Copy.m_pParent, Copy.m_pXMLNode )
{
    // Init
    m_szName [0] = 0;
    m_szName [MAX_ELEMENT_NAME_LENGTH] = 0;

    m_pObjectManager = Copy.m_pObjectManager;
    m_usModel = Copy.m_usModel;
    m_vecPosition = Copy.m_vecPosition;
    m_vecRotation = Copy.m_vecRotation;
    // TODO: copy move data properly
    m_moveData.bActive = false;

    // Add us to the manager's list
    m_pObjectManager->AddToList ( this );
}


CObject::~CObject ( void )
{
    // Unlink us from manager
    Unlink ();
}


void CObject::Unlink ( void )
{
    // Remove us from the manager's list
    m_pObjectManager->RemoveFromList ( this );
    m_pObjectManager->m_Attached.remove ( this );
}


bool CObject::ReadSpecialData ( void )
{
    // Grab the "posX" data
    if ( !GetCustomDataFloat ( "posX", m_vecPosition.fX, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posX' attribute in <object> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posY" data
    if ( !GetCustomDataFloat ( "posY", m_vecPosition.fY, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posY' attribute in <object> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posZ" data
    if ( !GetCustomDataFloat ( "posZ", m_vecPosition.fZ, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posZ' attribute in <object> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "rotX", "rotY" and "rotZ" data
    GetCustomDataFloat ( "rotX", m_vecRotation.fX, true );
    GetCustomDataFloat ( "rotY", m_vecRotation.fY, true );
    GetCustomDataFloat ( "rotZ", m_vecRotation.fZ, true );
    // We store radians, but load degrees
    ConvertDegreesToRadians ( m_vecRotation );

    // Grab the "model" data
    int iTemp;
    if ( GetCustomDataInt ( "model", iTemp, true ) )
    {
        // Valid id?
        if ( CObjectManager::IsValidModel ( iTemp ) )
        {
            // Set the object id
            m_usModel = static_cast < unsigned short > ( iTemp );
        }
        else
        {
            CLogger::ErrorPrintf ( "Bad 'model' id specified in <object> (line %u)\n", m_uiLine );
            return false;
        }
    }
    else
    {
        CLogger::ErrorPrintf ( "Bad/missing 'model' attribute in <object> (line %u)\n", m_uiLine );
        return false;
    }

    if ( GetCustomDataInt ( "interior", iTemp, true ) )
        m_ucInterior = static_cast < unsigned char > ( iTemp );

    if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    // Success
    return true;
}


const CVector& CObject::GetPosition ( void )
{
    // Are we attached to something?
    if ( m_pAttachedTo ) GetAttachedPosition ( m_vecPosition );

    // Are we moving?
    else if ( IsMoving () )
    {
        // Calculate our current Position
        unsigned long ulCurrentTime = GetTime ();

        // Grab the difference between start and finish
        CVector vecJourney = m_moveData.vecStopPosition - m_moveData.vecStartPosition;
        
        // Grab the duration the object takes to move
        float fDuration = static_cast < float > ( m_moveData.ulTime );
        // Grab the time that has passed since we started moving
        float fTimePassed = static_cast < float > ( ulCurrentTime - m_moveData.ulTimeStart );

        // How far along our journey should we be?
        vecJourney /= fDuration;
        vecJourney *= fTimePassed;

        // Update our stored position
        m_vecPosition = m_moveData.vecStartPosition + vecJourney;
    }

    // Finally, return it
    return m_vecPosition;
}


void CObject::SetPosition ( const CVector& vecPosition )
{
    // If we're attached to something, dont change anything (attachment takes priority over moving here)
    if ( m_pAttachedTo ) return;

    // If we're moving, dont change anything
    if ( IsMoving () ) return;    

    // Different position?
    if ( m_vecPosition != vecPosition )
    {
        // Update our vectors
        m_vecLastPosition = m_vecPosition;
        m_vecPosition = vecPosition;
    }
}


void CObject::GetRotation ( CVector & vecRotation )
{
    vecRotation = m_vecRotation;

    // Are we attached to something?
    if ( m_pAttachedTo ) GetAttachedRotation ( vecRotation );

    // Are we moving?
    else if ( IsMoving () )
    {
        // Calculate our current rotation
        unsigned long ulCurrentTime = GetTime ();

        // Grab the difference between start and finish
        CVector vecJourney = m_moveData.vecStopRotation - m_moveData.vecStartRotation;
        
        // Grab the duration the object takes to move
        float fDuration = static_cast < float > ( m_moveData.ulTime );
        // Grab the time that has passed since we started moving
        float fTimePassed = static_cast < float > ( ulCurrentTime - m_moveData.ulTimeStart );

        // How far along our journey should we be?
        vecJourney /= fDuration;
        vecJourney *= fTimePassed;

        // Update our stored rotation
        vecRotation = m_vecRotation = m_moveData.vecStartRotation + vecJourney;
    }
}


void CObject::SetRotation ( const CVector& vecRotation )
{
    // If we're attached to something, dont change anything (attachment takes priority over moving here)
    if ( m_pAttachedTo ) return;

    // If we're moving, dont change anything
    if ( IsMoving () ) return;    

    // Different rotation?
    if ( m_vecRotation != vecRotation )
    {
        // Set the new rotation
        m_vecRotation = vecRotation;
    }
}


bool CObject::IsMoving ( void )
{
    // Are we currently moving?
    if ( m_moveData.bActive )
    {
        // Should we have stopped moving by now?
        if ( GetTime () >= m_moveData.ulTimeStop )
        {
            // Stop our movement
            StopMoving ();
        }
    }
    // Are we still moving after the above check?
    return ( m_moveData.bActive );
}


void CObject::Move ( const CVector& vecPosition, const CVector& vecRotation, unsigned long ulTime )
{
    // Are we already moving?
    if ( IsMoving () )
    {
        // Stop our current movement
        StopMoving ();
    }

    // If it's more than 0 milliseconds
    if ( ulTime > 0 )
    {
        // Setup our move data
        m_moveData.vecStartPosition = GetPosition ();
        m_moveData.vecStopPosition = vecPosition;
        GetRotation ( m_moveData.vecStartRotation );
        m_moveData.vecStopRotation = vecRotation;
        m_moveData.ulTime = ulTime;
        m_moveData.ulTimeStart = GetTime ();
        m_moveData.ulTimeStop = m_moveData.ulTimeStart + ulTime; 
        m_moveData.bActive = true;
    }
    // If we have a time of 0, move there now
    else
    {
        SetPosition ( vecPosition );
        CVector vecTemp;
        GetRotation ( vecTemp );
        SetRotation ( vecTemp + vecRotation );
    }
}


void CObject::StopMoving ( void )
{
    // Were we moving in the first place
    if ( m_moveData.bActive )
    {
        m_moveData.bActive = false;

        // Calculate our current Position
        unsigned long ulCurrentTime = GetTime ();

        if ( ulCurrentTime >= m_moveData.ulTimeStop )
        {
            m_vecPosition = m_moveData.vecStopPosition;
            m_vecRotation = m_moveData.vecStopRotation;
            return;
        }

        // Grab the difference between start and finish
        CVector vecJourney = m_moveData.vecStopPosition - m_moveData.vecStartPosition;
        
        // Grab the duration the object takes to move
        float fDuration = static_cast < float > ( m_moveData.ulTime );
        // Grab the time that has passed since we started moving
        float fTimePassed = static_cast < float > ( ulCurrentTime - m_moveData.ulTimeStart );

        // How far along our journey should we be?
        vecJourney /= fDuration;
        vecJourney *= fTimePassed;

        // Update our stored position
        m_vecPosition = m_moveData.vecStartPosition + vecJourney;     

        // Grab the difference between start and finish
        vecJourney = m_moveData.vecStopRotation - m_moveData.vecStartRotation;

        // How far along our journey should we be?
        vecJourney /= fDuration;
        vecJourney *= fTimePassed;

        // Update our stored rotation
        m_vecRotation = m_moveData.vecStartRotation + vecJourney;
    }
}


unsigned long CObject::GetMoveTimeLeft ( void )
{
    // Check that a movement is going on and that we aren't above the time its scheduled to finish
    if ( IsMoving () )
    {        
        unsigned long ulCurrentTime = GetTime ();
        return m_moveData.ulTimeStop - ulCurrentTime;
    }

    return 0;
}


void CObject::AttachTo ( CElement * pElement )
{
    CElement * pPreviousAttachedToElement = m_pAttachedTo;
    CElement::AttachTo ( pElement );

    if ( !pPreviousAttachedToElement && m_pAttachedTo )
    {
        // Add us to our managers attached list
        m_pObjectManager->m_Attached.push_back ( this );
    }
    else if ( pPreviousAttachedToElement && !m_pAttachedTo )
    {
        // Remove us from our managers attached list
        m_pObjectManager->m_Attached.remove ( this );
    }
}
