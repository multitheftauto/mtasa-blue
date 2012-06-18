/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWeapon.cpp
*  PURPOSE:     Weapon entity class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#include "StdInc.h"

CClientWeapon::CClientWeapon ( CClientManager * pManager, ElementID ID, eWeaponType type ) : CClientObject ( pManager, ID, CClientPickupManager::GetWeaponModel ( type ), false )
{
    m_pManager = pManager;
    m_pManager->GetWeaponManager ()->AddToList ( this );

    m_Type = type;
    m_State = WEAPONSTATE_READY;
    m_pWeapon = NULL;
    m_pWeaponInfo = g_pGame->GetWeaponInfo ( m_Type );
    m_bHasTargetDirection = false;
    SetStatic ( true );
    Create ();
    m_pMarker = new CClientMarker ( pManager, INVALID_ELEMENT_ID, 4 );
    m_pMarker->SetColor( SColor( 0xFF00FF00 ) );
    m_pMarker->SetSize ( 0.5f );
    m_pMarker2 = new CClientMarker ( pManager, INVALID_ELEMENT_ID, 4 );
    m_pMarker2->SetColor( SColor( 0xFFFF0000 ) );
    m_pMarker2->SetSize ( 0.5f );
}


CClientWeapon::~CClientWeapon ( void )
{
    m_pManager->GetWeaponManager ()->RemoveFromList ( this );

    Destroy ();
    delete m_pMarker;
    delete m_pMarker2;
}


void CClientWeapon::DoPulse ( void )
{
    if ( m_bHasTargetDirection )
    {
        CVector vecDirection = m_vecLastDirection;
        CVector vecOffset = m_vecTargetDirection - vecDirection;
        vecOffset /= CVector ( 10, 10, 10 );
        vecDirection += vecOffset;
        vecDirection.Normalize ();
        m_vecLastDirection = vecDirection;
        SetDirection ( vecDirection );
    }
    if ( m_State == WEAPONSTATE_FIRING ) Fire ();
}


void CClientWeapon::Create ( void )
{
    CClientObject::Create ();

    if ( !m_pWeapon )
    {
        m_pWeapon = g_pGame->CreateWeapon ();
        CPlayerPed * pPed = m_pManager->GetPlayerManager ()->GetLocalPlayer ()->GetGamePlayer ();
        m_pWeapon->Initialize ( m_Type, 9999, NULL );
    }
}


void CClientWeapon::Destroy ( void )
{
    CClientObject::Destroy ();

    if ( m_pWeapon )
    {
        m_pWeapon->Destroy ();
        m_pWeapon = NULL;
    }
}


void CClientWeapon::Fire ( void )
{
    if ( !m_pWeapon || !m_pObject ) return;
    switch ( m_Type )
    {
        case WEAPONTYPE_PISTOL:
        case WEAPONTYPE_PISTOL_SILENCED:
        case WEAPONTYPE_DESERT_EAGLE:
        case WEAPONTYPE_SHOTGUN: 
        case WEAPONTYPE_SAWNOFF_SHOTGUN:
        case WEAPONTYPE_SPAS12_SHOTGUN:
        case WEAPONTYPE_MICRO_UZI:
        case WEAPONTYPE_MP5:
        case WEAPONTYPE_AK47:
        case WEAPONTYPE_M4:
        case WEAPONTYPE_TEC9:
        case WEAPONTYPE_COUNTRYRIFLE:
        case WEAPONTYPE_SNIPERRIFLE:
        case WEAPONTYPE_MINIGUN:
        {
            CVector vecOrigin, vecRotation;
            GetPosition ( vecOrigin );
            if ( m_pAttachedToEntity )
            {

                GetRotationRadians ( vecRotation );
                // Fixes vehicle rotation.... god.
                vecRotation.fX = ConvertDegreesToRadians( 360.0f - ConvertRadiansToDegrees ( vecRotation.fX ) );
                vecRotation.fY = ConvertDegreesToRadians( 360.0f - ConvertRadiansToDegrees ( vecRotation.fY ) );
                vecRotation.fZ = ConvertDegreesToRadians( 360.0f - ConvertRadiansToDegrees ( vecRotation.fZ ) );
                vecRotation = -vecRotation;
            }
            else
            {
                GetRotationRadians ( vecRotation );

                vecRotation = -vecRotation;
            }
            

            CVector vecFireOffset = *m_pWeaponInfo->GetFireOffset ();
            RotateVector ( vecFireOffset, vecRotation );
            vecOrigin += vecFireOffset;

            float fDistance = m_pWeaponInfo->GetWeaponRange ();
            CVector vecDirection ( 1, 0, 0 );
            vecDirection *= fDistance;
            RotateVector ( vecDirection, vecRotation );
            CVector vecTarget = vecOrigin + vecDirection;
            
            m_pMarker->SetPosition ( vecTarget );
            FireInstantHit ( vecOrigin, vecTarget );
            break;
        }
        default: break;
    }
}


void CClientWeapon::FireInstantHit ( CVector & vecOrigin, CVector & vecTarget )
{
    CVector vecDirection = vecTarget - vecOrigin;
    vecDirection.Normalize ();
    
    g_pGame->GetPointLights ()->AddLight ( PLTYPE_POINTLIGHT, vecOrigin, CVector (), 3.0f, 0.22f, 0.25f, 0, 0, 0, 0 );
    
    if ( GetAttachedTo () ) g_pGame->GetFx ()->TriggerGunshot ( NULL, vecOrigin, vecDirection, false );
    else g_pGame->GetFx ()->TriggerGunshot ( NULL, vecOrigin, vecDirection, true );

    m_pWeapon->AddGunshell ( m_pObject, &vecOrigin, &CVector2D ( 0, -1 ), 0.45f );
    g_pGame->GetAudioEngine ()->ReportWeaponEvent ( WEAPON_EVENT_FIRE, m_Type, m_pObject );

    CClientEntity * pAttachedTo = GetAttachedTo ();    
    // Crashes with vehicle mounted epicness.
    //if ( pAttachedTo ) pAttachedTo->WorldIgnore ( true );

    CEntity * pColEntity = NULL;
    CColPoint * pColPoint = NULL;
    SLineOfSightFlags flags;
    flags.bShootThroughStuff = false;
    flags.bIgnoreSomeObjectsForCamera = false;
    flags.bSeeThroughStuff = true;
    flags.bCheckDummies = true;
    flags.bCheckObjects = true;
    flags.bCheckPeds = true;
    flags.bCheckVehicles = true;
    flags.bCheckBuildings = true;
    flags.bCheckCarTires = true;
    SLineOfSightBuildingResult pBuildingResult;
    if ( g_pGame->GetWorld ()->ProcessLineOfSight ( &vecOrigin, &vecTarget, &pColPoint, &pColEntity, flags, &pBuildingResult ) )
    {
        vecTarget = *pColPoint->GetPosition ();
    }

    //if ( pAttachedTo ) pAttachedTo->WorldIgnore ( false );

    CVector vecCollision;
    if ( g_pGame->GetWaterManager ()->TestLineAgainstWater ( vecOrigin, vecTarget, &vecCollision ) )
    {
        g_pGame->GetFx ()->TriggerBulletSplash ( vecCollision );
        g_pGame->GetAudioEngine ()->ReportBulletHit ( NULL, SURFACE_TYPE_WATER_SHALLOW, &vecCollision, 0.0f );
    }    
    m_pMarker2->SetPosition ( vecTarget );

    m_pWeapon->DoBulletImpact ( m_pObject, pColEntity, &vecOrigin, &vecTarget, pColPoint, 1 );
    if ( pColEntity && pColEntity->GetEntityType () == ENTITY_TYPE_PED )
    {
        ePedPieceTypes hitZone = ( ePedPieceTypes ) pColPoint->GetPieceTypeB ();
        short sDamage = m_pWeaponInfo->GetDamagePerHit ();
        m_pWeapon->GenerateDamageEvent ( dynamic_cast < CPed * > ( pColEntity ), m_pObject, m_Type, sDamage, hitZone, 0 );
    }

    pColPoint->Destroy ();
}


void CClientWeapon::GetDirection ( CVector & vecDirection )
{
    CVector vecRotation;
    GetRotationRadians ( vecRotation );

    vecDirection = CVector ( 1, 0, 0 );
    RotateVector ( vecDirection, vecRotation );
}


void CClientWeapon::SetDirection ( CVector & vecDirection )
{
    CVector vecRotation;
    GetRotationRadians ( vecRotation );

    vecRotation.fZ = atan2 ( vecDirection.fY, vecDirection.fX );
    CVector2D vecTemp ( vecDirection.fX, vecDirection.fY );
    float length = vecTemp.Length ();
    vecTemp = CVector2D ( length, vecDirection.fZ );
    vecTemp.Normalize ();
    vecRotation.fY = atan2 ( vecTemp.fX, vecTemp.fY ) - ConvertDegreesToRadians ( 90 );
    
    SetRotationRadians ( vecRotation );
}


void CClientWeapon::SetTargetDirection ( CVector & vecDirection )
{
    m_vecTargetDirection = vecDirection;
    m_bHasTargetDirection = true;
}


void CClientWeapon::LookAt ( CVector & vecPosition, bool bInterpolate )
{
    CVector vecCurrentPosition;
    GetPosition ( vecCurrentPosition );
    CVector vecDirection = vecPosition - vecCurrentPosition;
    vecDirection.Normalize ();

    if ( bInterpolate ) SetTargetDirection ( vecDirection );
    else SetDirection ( vecDirection );
}