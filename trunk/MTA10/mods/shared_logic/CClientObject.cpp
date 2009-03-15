/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientObject.cpp
*  PURPOSE:     Physical object entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

#include <StdInc.h>

#define MTA_BUILDINGS
#define CCLIENTOBJECT_MAX 250

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CClientObject::CClientObject ( CClientManager* pManager, ElementID ID, unsigned short usModel ) : CClientStreamElement ( pManager->GetObjectStreamer (), ID )
{
    // Init
    m_pManager = pManager;
    m_pObjectManager = pManager->GetObjectManager ();
    m_pModelRequester = pManager->GetModelRequestManager ();

    m_pObject = NULL;
    m_usModel = usModel;

    SetTypeName ( "object" );

    m_usModel = usModel;
    m_bIsVisible = true;
    m_bIsStatic = true;
    m_bUsesCollision = true;
    m_ucAlpha = 255;
    m_fScale = 1.0f;

    m_pModelInfo = g_pGame->GetModelInfo ( usModel );

    // Add this object to the list
    m_pObjectManager->AddToList ( this );
}


CClientObject::~CClientObject ( void )
{
    // Unrequest whatever we've requested or we'll crash in unlucky situations
    m_pModelRequester->Cancel ( this );  

    // Detach us from anything
	AttachTo ( NULL );

    // Destroy the object
    Destroy ();

    // Remove us from the list
    Unlink ();
}


void CClientObject::Unlink ( void )
{
    m_pObjectManager->RemoveFromList ( this );
    m_pObjectManager->m_Attached.remove ( this );
    m_pObjectManager->m_StreamedIn.remove ( this );
}


void CClientObject::GetPosition ( CVector & vecPosition ) const
{
    if ( m_pObject )
    {
        vecPosition = *m_pObject->GetPosition ();
    }
    else if ( m_pAttachedToEntity )
    {
        m_pAttachedToEntity->GetPosition ( vecPosition );
        vecPosition += m_vecAttachedPosition;
    }
    else
    {
        vecPosition = m_vecPosition;
    }
}


void CClientObject::SetPosition ( const CVector& vecPosition )
{   
    // Move the object
    if ( m_pObject )
    {
        CVector vecRot;
        GetRotationRadians ( vecRot );
        m_pObject->Teleport ( vecPosition.fX, vecPosition.fY, vecPosition.fZ );
#ifndef MTA_BUILDINGS
        m_pObject->ProcessCollision ();
#endif
        m_pObject->SetupLighting ();
        SetRotationRadians ( vecRot );
    }

    if ( m_vecPosition != vecPosition )
    {
        // Store the position in our datastruct
        m_vecPosition = vecPosition;

        // Update our streaming position
        UpdateStreamPosition ( vecPosition );
    }
}


void CClientObject::SetMatrix ( const CMatrix& Matrix )
{
	if ( m_pObject )
		m_pObject->SetMatrix ( const_cast < CMatrix* > ( &Matrix ) );
}


void CClientObject::GetRotationDegrees ( CVector& vecRotation ) const
{
    GetRotationRadians ( vecRotation );
    ConvertRadiansToDegrees ( vecRotation );
}


void CClientObject::GetRotationRadians ( CVector& vecRotation ) const
{
    if ( m_pObject && m_pAttachedToEntity )// Temp fix for static objects->
    {
        // We've been returning the rotation that got set last so far (::m_vecRotation)..
        //   but we need to get the real rotation for when the game moves the objects..
        //  (eg: physics/attaching), the code below returns wrong values, see #2732
        CMatrix matTemp;
        m_pObject->GetMatrix ( &matTemp );
        g_pMultiplayer->ConvertMatrixToEulerAngles ( matTemp, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    }
    else
    {
        vecRotation = m_vecRotation;
    }
}


void CClientObject::SetRotationDegrees ( const CVector & vecRotation )
{
    // Convert from degrees to radians
    CVector vecTemp;
    vecTemp.fX = vecRotation.fX * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fY = vecRotation.fY * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fZ = vecRotation.fZ * 3.1415926535897932384626433832795f / 180.0f;

    SetRotationRadians ( vecTemp );
}


void CClientObject::SetRotationRadians ( const CVector& vecRotation )
{
    if ( m_pObject )
    {
        m_pObject->SetOrientation ( vecRotation.fX, vecRotation.fY, vecRotation.fZ );
#ifndef MTA_BUILDINGS
        m_pObject->ProcessCollision ();
#endif
        m_pObject->SetupLighting ();
    }

    // Store it in our datastruct
    m_vecRotation = vecRotation;
}


void CClientObject::GetOrientation ( CVector& vecPosition, CVector& vecRotationRadians )
{
    GetPosition ( vecPosition );
    GetRotationRadians ( vecRotationRadians );
}


void CClientObject::SetOrientation ( const CVector& vecPosition, const CVector& vecRotationRadians )
{
    if ( m_vecPosition != vecPosition )
    {
        // Store the position in our datastruct
        m_vecPosition = vecPosition;

        // Update our streaming position
        UpdateStreamPosition ( vecPosition );
    }

    // Update our internal data
    m_vecPosition = vecPosition;
    m_vecRotation = vecRotationRadians;

    // Eventually move the object
    if ( m_pObject )
    {
        m_pObject->Teleport ( vecPosition.fX, vecPosition.fY, vecPosition.fZ );
        m_pObject->SetOrientation ( vecRotationRadians.fX, vecRotationRadians.fY, vecRotationRadians.fZ );
#ifndef MTA_BUILDINGS
        m_pObject->ProcessCollision ();
#endif
        m_pObject->SetupLighting ();
    }
}


void CClientObject::ModelRequestCallback ( CModelInfo* pModelInfo )
{
    // Create our object
    Create ();
}


float CClientObject::GetDistanceFromCentreOfMassToBaseOfModel ( void )
{
    if ( m_pObject )
    {
        return m_pObject->GetDistanceFromCentreOfMassToBaseOfModel ();
    }
    else
    {
        return 0;
    }
}


void CClientObject::SetVisible ( bool bVisible )
{
    if ( m_pObject )
    {
        m_pObject->SetVisible ( bVisible );
    }

    m_bIsVisible = bVisible;
}


void CClientObject::SetModel ( unsigned short usModel )
{
    // Valid model ID?
    if ( CClientObjectManager::IsValidModel ( usModel ) )
    {
        // Destroy current model
        Destroy ();

        // Set the new model ID and recreate the model
        m_usModel = usModel;
        m_pModelInfo = g_pGame->GetModelInfo ( usModel );

        // Request the new model so we can recreate when it's done
        if ( m_pModelRequester->Request ( usModel, this ) )
        {
            Create ();
        }
    }
}


void CClientObject::Render ( void )
{
    if ( m_pObject )
    {
        m_pObject->Render ();
    }
}


void CClientObject::SetStatic ( bool bStatic )
{
    // This will only change on the next creation (maybe recreate here, if streamed in)
    m_bIsStatic = bStatic;
}


void CClientObject::SetAlpha ( unsigned char ucAlpha )
{
    if ( m_pObject )
    {
        m_pObject->SetAlpha ( ucAlpha );
    }
    m_ucAlpha = ucAlpha;
}


void CClientObject::SetScale ( float fScale )
{
    if ( m_pObject )
    {
        m_pObject->SetScale ( fScale );
    }

    m_fScale = fScale;
}


void CClientObject::SetCollisionEnabled ( bool bCollisionEnabled )
{
    if ( m_pObject )
    {
        m_pObject->SetUsesCollision ( bCollisionEnabled );
    }

    m_bUsesCollision = bCollisionEnabled;
}


void CClientObject::StreamIn ( bool bInstantly )
{
    // We need to load now?
    if ( bInstantly )
    {
        // Request the model blocking
        if ( m_pModelRequester->RequestBlocking ( m_usModel ) )
        {
            // Create us
            Create ();
        }
        else NotifyUnableToCreate ();
    }
    else
    {
        // Request the model async
        if ( m_pModelRequester->Request ( m_usModel, this ) )
        {
            // Create us now if we already had it loaded
            Create ();
        }
        else NotifyUnableToCreate ();
    }
}


void CClientObject::StreamOut ( void )
{
    // Destroy the object.
    Destroy ();

    // Cancel anything we might've requested.
    m_pModelRequester->Cancel ( this );
}


void CClientObject::ReCreate ( void )
{
    if ( m_pObject )
    {
        Destroy ();
        Create ();
    }
}


void CClientObject::Create ( void )
{
    // Not already created an object?
    if ( !m_pObject )
    {
        // Check again that the limit isn't reached. We are required to do so because
        // we load async. The streamer isn't always aware of our limits.
        if ( !CClientObjectManager::IsObjectLimitReached () )
        {
            // Add a reference to the object
            m_pModelInfo->AddRef ( true );

            // Create the object
            m_pObject = g_pGame->GetPools ()->AddObject ( m_usModel );
            if ( m_pObject )
            {                
                // Put our pointer in its stored pointer
                m_pObject->SetStoredPointer ( this );

                // Set some flags first
                m_pObject->SetStaticWaitingForCollision ( m_bIsStatic );

                // Apply our data to the object
                m_pObject->Teleport ( m_vecPosition.fX, m_vecPosition.fY, m_vecPosition.fZ );
                m_pObject->SetOrientation ( m_vecRotation.fX, m_vecRotation.fY, m_vecRotation.fZ );
                #ifndef MTA_BUILDINGS
                m_pObject->ProcessCollision ();
                #endif
                m_pObject->SetupLighting ();

                if ( !m_bIsVisible ) SetVisible ( false );
                if ( !m_bUsesCollision ) SetCollisionEnabled ( false );
                if ( m_fScale != 1.0f ) SetScale ( m_fScale );
                m_pObject->SetAreaCode ( m_ucInterior );
                SetAlpha ( m_ucAlpha );

                // Reattach to an entity + any entities attached to this
                ReattachEntities ();

                // Validate this entity again
                m_pManager->RestoreEntity ( this );

                // Tell the streamer we've created this object
                NotifyCreate ();

                // Done
                return;
	        }
            else
            {
                // Remove our reference to the object again
                m_pModelInfo->RemoveRef ();
            }
        }

        // Tell the streamer we could not create it
        NotifyUnableToCreate ();
    }
}


void CClientObject::Destroy ( void )
{
    // If the object exists
    if ( m_pObject )
    {
        // Invalidate
        m_pManager->InvalidateEntity ( this );

        // Destroy the object
        g_pGame->GetPools ()->RemoveObject ( m_pObject );
        m_pObject = NULL;

        // Remove our reference to its model
        m_pModelInfo->RemoveRef ();

        NotifyDestroy ();
    }
}


void CClientObject::NotifyCreate ( void )
{
    m_pObjectManager->OnCreation ( this );
    CClientStreamElement::NotifyCreate ();
}


void CClientObject::NotifyDestroy ( void )
{
    m_pObjectManager->OnDestruction ( this );
}

void CClientObject::StreamedInPulse ( void )
{
    // Are we not a static object (allowed to move by physics)
    if ( !m_bIsStatic )
    {
        // Grab our actual position (as GTA moves it too)
        CVector vecPosition = *m_pObject->GetPosition ();

        // Has it moved without MTA knowing?
        if ( vecPosition != m_vecPosition )
        {
            m_vecPosition = vecPosition;

            // Update our streaming position
            UpdateStreamPosition ( m_vecPosition );
        }
    }
}


void CClientObject::AttachTo ( CClientEntity* pEntity )
{
	// Add/remove us to/from our managers attached list
    if ( m_pAttachedToEntity && !pEntity ) m_pObjectManager->m_Attached.remove ( this );
    else if ( !m_pAttachedToEntity && pEntity ) m_pObjectManager->m_Attached.push_back ( this );

    CClientEntity::AttachTo ( pEntity );
}


void CClientObject::GetMoveSpeed ( CVector& vecMoveSpeed ) const
{
    if ( m_pObject )
    {
        m_pObject->GetMoveSpeed ( &vecMoveSpeed );
    }
    else
    {
        vecMoveSpeed = m_vecMoveSpeed;
    }
}


void CClientObject::SetMoveSpeed ( const CVector& vecMoveSpeed )
{
    if ( m_pObject )
    {
        m_pObject->SetMoveSpeed ( const_cast < CVector* > ( &vecMoveSpeed ) );
    }
    m_vecMoveSpeed = vecMoveSpeed;
}