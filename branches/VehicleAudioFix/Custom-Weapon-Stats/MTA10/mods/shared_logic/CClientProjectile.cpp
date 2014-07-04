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
    // Make sure we're destroyed
    Destroy ();

    // If our creator is getting destroyed, this should be null
    if ( m_pCreator )
    {
        switch ( m_pCreator->GetType () )
        {
            case CCLIENTPLAYER:
            case CCLIENTPED:
                static_cast < CClientPed * > ( m_pCreator )->RemoveProjectile ( this );
                break;
            case CCLIENTVEHICLE:
                static_cast < CClientVehicle * > ( m_pCreator )->RemoveProjectile ( this );
                break;
            default: break;
        }
    }

    if ( m_pvecOrigin ) delete m_pvecOrigin;
    if ( m_pvecTarget ) delete m_pvecTarget;

    if ( m_pInitiateData ) delete m_pInitiateData;

    Unlink ();

    CClientEntityRefManager::RemoveEntityRefs ( 0, &m_pCreator, &m_pTarget, NULL );
}


void CClientProjectile::Unlink ( void )
{
    // Are we still linked? (this bool will be set to false when our manager is being destroyed)
    if ( m_bLinked )
    {
        m_pProjectileManager->RemoveFromList ( this ); 
        m_bLinked = false;
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
            if ( m_pInitiateData->pvecRotation ) SetRotation ( *m_pInitiateData->pvecRotation );
            if ( m_pInitiateData->pvecVelocity ) SetVelocity ( *m_pInitiateData->pvecVelocity );
            if ( m_pInitiateData->usModel ) SetModel ( m_pInitiateData->usModel );
        }

        // Let our manager know we've been initiated
        m_pProjectileManager->OnInitiate ( this );             
        m_bInitiate = false;
    }

    // Update our position/rotation if we're attached
    DoAttaching ();
}


void CClientProjectile::Initiate ( CVector * pvecPosition, CVector * pvecRotation, CVector * pvecVelocity, unsigned short usModel )
{
#ifdef MTA_DEBUG
    if ( m_pInitiateData ) _asm int 3
#endif

    // Store our initiation data
    m_pInitiateData = new CProjectileInitiateData;
    if ( pvecPosition ) m_pInitiateData->pvecPosition = new CVector ( *pvecPosition );
    else m_pInitiateData->pvecPosition = NULL;
    if ( pvecRotation ) m_pInitiateData->pvecRotation = new CVector ( *pvecRotation );
    else m_pInitiateData->pvecRotation = NULL;
    if ( pvecVelocity ) m_pInitiateData->pvecVelocity = new CVector ( *pvecVelocity );
    else m_pInitiateData->pvecVelocity = NULL;
    m_pInitiateData->usModel = usModel;
}


void CClientProjectile::Destroy ( void )
{
    if ( m_pProjectile )
    {
        m_pProjectile->Destroy ();
        m_pProjectile = NULL;
    }
}


bool CClientProjectile::IsActive ( void )
{
    // Ensure airbomb is cleaned up
    if ( m_weaponType == WEAPONTYPE_FREEFALL_BOMB && GetTickCount64_ () - m_llCreationTime > AIRBOMB_MAX_LIFETIME )
        return false;
    return ( m_pProjectile && m_pProjectileInfo->IsActive () );
}


bool CClientProjectile::GetMatrix ( CMatrix & matrix )
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

    m_pProjectile->SetMatrix ( &matrix );
    return true;
}


void CClientProjectile::GetPosition ( CVector & vecPosition ) const
{
    vecPosition = *m_pProjectile->GetPosition ();
}


void CClientProjectile::SetPosition ( const CVector & vecPosition )
{
    m_pProjectile->SetPosition ( const_cast < CVector* > ( &vecPosition ) );
}


void CClientProjectile::GetRotation ( CVector & vecRotation )
{
    CMatrix matrix;
    GetMatrix ( matrix );
    g_pMultiplayer->ConvertMatrixToEulerAngles ( matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
}

void CClientProjectile::GetRotationDegrees ( CVector & vecRotation )
{
    GetRotation ( vecRotation );
    ConvertRadiansToDegrees ( vecRotation );
}


void CClientProjectile::SetRotation ( CVector & vecRotation )
{
    CMatrix matrix;
    GetPosition ( matrix.vPos );
    g_pMultiplayer->ConvertEulerAnglesToMatrix ( matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    SetMatrix ( matrix );
}

void CClientProjectile::SetRotationDegrees ( CVector & vecRotation )
{
    ConvertDegreesToRadians ( vecRotation );
    SetRotation ( vecRotation );
}


void CClientProjectile::GetVelocity ( CVector & vecVelocity )
{
    m_pProjectile->GetMoveSpeed ( &vecVelocity );
}


void CClientProjectile::SetVelocity ( CVector & vecVelocity )
{
    m_pProjectile->SetMoveSpeed ( &vecVelocity );
}


void CClientProjectile::SetModel ( unsigned short usModel )
{
    m_pProjectile->SetModelIndex ( usModel );
}