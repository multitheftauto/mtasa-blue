/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientProjectile.cpp
*  PURPOSE:     Projectile entity class
*  DEVELOPERS:  Jax <>
*               Ed Lyons <eai@opencoding.net>
*
*****************************************************************************/

#include <StdInc.h>

#define AIRBOMB_MAX_LIFETIME 60000

/* An instance of this class is created when GTA creates a projectile, it automatically
   destroys itself when GTA is finished with the projectile, this could/should eventually be
   used as a server created element and streamed.
*/
CClientProjectile::CClientProjectile ( class CClientManager* pManager, CProjectile* pProjectile, CProjectileInfo* pProjectileInfo, CClientEntity * pCreator, CClientEntity * pTarget, eWeaponType weaponType, CVector * pvecOrigin, CVector * pvecTarget, float fForce, bool bLocal ) : ClassInit ( this ), CClientEntity ( INVALID_ELEMENT_ID )
{
    CClientEntityRefManager::AddEntityRefs ( ENTITY_REF_DEBUG ( this, "CClientProjectile" ), &m_pCreator, &m_pTarget, NULL );

    m_pManager = pManager;
    m_pProjectileManager = pManager->GetProjectileManager ();
    m_pProjectile = pProjectile;
    m_pProjectileInfo = pProjectileInfo;

    SetTypeName ( "projectile" );

    m_pCreator = pCreator;
    m_pTarget = pTarget;
    m_weaponType = weaponType;
    if ( pvecOrigin ) m_pvecOrigin = new CVector ( *pvecOrigin );
    else m_pvecOrigin = NULL;
    if ( pvecTarget ) m_pvecTarget = new CVector ( *pvecTarget );
    else m_pvecTarget = NULL;
    m_fForce = fForce;
    m_bLocal = bLocal;
    m_llCreationTime = GetTickCount64_ ();

    m_pInitiateData = NULL;
    m_bInitiate = true;

    m_pProjectileManager->AddToList ( this );
    m_bLinked = true;

    if ( pCreator )
    {
        switch ( pCreator->GetType () )
        {
            case CCLIENTPLAYER:
            case CCLIENTPED:               
                static_cast < CClientPed * > ( pCreator )->AddProjectile ( this );
                break;
            case CCLIENTVEHICLE:
                static_cast < CClientVehicle * > ( pCreator )->AddProjectile ( this );
                break;
            default: break;
        }
    }
}


CClientProjectile::~CClientProjectile ( void )
{
    // If our creator is getting destroyed, this should be null
    if ( m_pCreator )
    {
        switch ( m_pCreator->GetType () )
        {
            case CCLIENTPLAYER:
            case CCLIENTPED:
                static_cast < CClientPed * > ( (CClientEntity*)m_pCreator )->RemoveProjectile ( this );
                break;
            case CCLIENTVEHICLE:
                static_cast < CClientVehicle * > ( (CClientEntity*)m_pCreator )->RemoveProjectile ( this );
                break;
            default: break;
        }
    }

    if ( m_pvecOrigin ) delete m_pvecOrigin;
    if ( m_pvecTarget ) delete m_pvecTarget;

    if ( m_pInitiateData ) delete m_pInitiateData;

    Unlink ();

    if ( m_pProjectile )
    {
        // Make sure we're destroyed
        delete m_pProjectile;

        m_pProjectile = NULL;
    }

    CClientEntityRefManager::RemoveEntityRefs ( 0, &m_pCreator, &m_pTarget, NULL );
}


void CClientProjectile::Unlink ( void )
{
    // Are we still linked? (this bool will be set to false when our manager is being destroyed)
    if ( m_bLinked )
    {
        m_pProjectileManager->RemoveFromList ( this ); 
        m_bLinked = false;
        if ( m_pProjectile )
        {
            // Make sure we're destroyed
            delete m_pProjectile;

            m_pProjectile = NULL;
        }
    }
}


void CClientProjectile::DoPulse ( void )
{
    // We use initiate data to set values on creation (as it doesn't exist until a frame after our projectile hook)
    if ( m_bInitiate )
    {
        if ( m_pInitiateData )
        {
            if ( m_pInitiateData->pvecPosition ) SetPosition ( *m_pInitiateData->pvecPosition );
            if ( m_pInitiateData->pvecRotation ) SetRotationRadians ( *m_pInitiateData->pvecRotation );
            if ( m_pInitiateData->pvecVelocity ) SetVelocity ( *m_pInitiateData->pvecVelocity );
            if ( m_pInitiateData->usModel ) SetModel ( m_pInitiateData->usModel );
        }

        // Handle net sync and script event
        g_pClientGame->ProjectileInitiateHandler ( this );

        m_bInitiate = false;
    }

    // Update our position/rotation if we're attached
    DoAttaching ();
}


void CClientProjectile::Initiate ( CVector& vecPosition, CVector& vecRotation, CVector& vecVelocity, unsigned short usModel )
{
#ifdef MTA_DEBUG
    if ( m_pInitiateData ) _asm int 3
#endif

    // Store our initiation data
    m_pInitiateData = new CProjectileInitiateData;
    m_pInitiateData->pvecPosition = new CVector ( vecPosition );

    if ( vecRotation != CVector ( 0, 0, 0 ) )
    {
        m_pInitiateData->pvecRotation = new CVector ( vecRotation );
    }
    else
    {
        m_pInitiateData->pvecRotation = NULL;
    }

    if ( vecVelocity != CVector(0,0,0) ) 
        m_pInitiateData->pvecVelocity = new CVector ( vecVelocity );
    else 
        m_pInitiateData->pvecVelocity = NULL;

    m_pInitiateData->usModel = usModel;
}


void CClientProjectile::Destroy ( bool bBlow )
{
    if ( m_pProjectile )
    {
        m_pProjectile->Destroy ( bBlow );
    }
}


bool CClientProjectile::IsActive ( void )
{
    // Ensure airbomb is cleaned up
    if ( m_weaponType == WEAPONTYPE_FREEFALL_BOMB && GetTickCount64_ () - m_llCreationTime > AIRBOMB_MAX_LIFETIME )
        return false;
    return ( m_pProjectile && m_pProjectileInfo->IsActive () );
}


bool CClientProjectile::GetMatrix ( CMatrix & matrix ) const
{
    if ( m_pProjectile )
    {
        if ( m_pProjectile->GetMatrix ( &matrix ) )
        {
            // Jax: If the creator is a ped, we need to invert X and Y on Direction and Was for CMultiplayer::ConvertMatrixToEulerAngles
            if ( m_pCreator && IS_PED ( m_pCreator ) )
            {
                matrix.vFront.fX = 0.0f - matrix.vFront.fX;
                matrix.vFront.fY = 0.0f - matrix.vFront.fY;
                matrix.vUp.fX = 0.0f - matrix.vUp.fX;
                matrix.vUp.fY = 0.0f - matrix.vUp.fY;
            }
            return true;
        }
    }
    return false;
}


bool CClientProjectile::SetMatrix ( const CMatrix & matrix_ )
{
    CMatrix matrix ( matrix_ );

    // Jax: If the creator is a ped, we need to invert X and Y on Direction and Was for CMultiplayer::ConvertEulerAnglesToMatrix
    if ( m_pCreator && IS_PED ( m_pCreator ) )
    {        
        matrix.vFront.fX = 0.0f - matrix.vFront.fX;
        matrix.vFront.fY = 0.0f - matrix.vFront.fY;
        matrix.vUp.fX = 0.0f - matrix.vUp.fX;
        matrix.vUp.fY = 0.0f - matrix.vUp.fY;
    }

    if ( m_pProjectile )
        m_pProjectile->SetMatrix ( &matrix );
    return true;
}


void CClientProjectile::GetPosition ( CVector & vecPosition ) const
{
    if ( m_pProjectile )
        vecPosition = *m_pProjectile->GetPosition ();
    else
        vecPosition = CVector();
}


void CClientProjectile::SetPosition ( const CVector & vecPosition )
{
    if ( m_pProjectile )
        m_pProjectile->SetPosition ( const_cast < CVector* > ( &vecPosition ) );
}


void CClientProjectile::GetRotationRadians ( CVector & vecRotation ) const
{
    CMatrix matrix;
    GetMatrix ( matrix );
    g_pMultiplayer->ConvertMatrixToEulerAngles ( matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
}

void CClientProjectile::GetRotationDegrees ( CVector & vecRotation ) const
{
    GetRotationRadians ( vecRotation );
    ConvertRadiansToDegrees ( vecRotation );
}


void CClientProjectile::SetRotationRadians ( const CVector & vecRotation )
{
    CMatrix matrix;
    GetPosition ( matrix.vPos );
    g_pMultiplayer->ConvertEulerAnglesToMatrix ( matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    SetMatrix ( matrix );
}

void CClientProjectile::SetRotationDegrees ( const CVector & vecRotation )
{
    CVector vecTemp = vecRotation;
    ConvertDegreesToRadians ( vecTemp );
    SetRotationRadians ( vecTemp );
}


void CClientProjectile::GetVelocity ( CVector & vecVelocity )
{
    if ( m_pProjectile )
        m_pProjectile->GetMoveSpeed ( &vecVelocity );
    else
        vecVelocity = CVector();
}


void CClientProjectile::SetVelocity ( CVector & vecVelocity )
{
    if ( m_pProjectile )
        m_pProjectile->SetMoveSpeed ( &vecVelocity );
}

unsigned short CClientProjectile::GetModel ( void )
{
    if ( m_pProjectile )
        return m_pProjectile->GetModelIndex ();
    return 0;
}

void CClientProjectile::SetModel ( unsigned short usModel )
{
    if ( m_pProjectile )
        m_pProjectile->SetModelIndex ( usModel );
}

void CClientProjectile::SetCounter ( DWORD dwCounter )
{
    if ( m_pProjectile )
        m_pProjectileInfo->SetCounter ( dwCounter );
}

DWORD CClientProjectile::GetCounter ( void )
{
    if ( m_pProjectile )
        return m_pProjectileInfo->GetCounter ( );
    return 0;
}