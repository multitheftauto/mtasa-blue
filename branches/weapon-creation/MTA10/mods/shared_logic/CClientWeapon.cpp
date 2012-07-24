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

    SetTypeName ( "weapon" );

    SetStatic ( true );
    Create ();
    m_pMarker = new CClientMarker ( pManager, INVALID_ELEMENT_ID, 4 );
    m_pMarker->SetColor( SColor( 0xFF00FF00 ) );
    m_pMarker->SetSize ( 0.5f );
    m_pMarker2 = new CClientMarker ( pManager, INVALID_ELEMENT_ID, 4 );
    m_pMarker2->SetColor( SColor( 0xFFFF0000 ) );
    m_pMarker2->SetSize ( 0.5f );
    m_sDamage = m_pWeaponInfo->GetDamagePerHit ( );
    m_pWeaponStat = g_pGame->CreateWeaponStat ( type, WEAPONSKILL_STD );
    ResetWeaponTarget ( );
    // Setup weapon config.
    m_weaponConfig.bDisableWeaponModel = false;
    m_weaponConfig.bShootIfTargetBlocked = true;
    m_weaponConfig.bShootIfTargetOutOfRange = false;
    m_weaponConfig.flags.bShootThroughStuff = false;
    m_weaponConfig.flags.bIgnoreSomeObjectsForCamera = false;
    m_weaponConfig.flags.bSeeThroughStuff = false;
    m_weaponConfig.flags.bCheckDummies = true;
    m_weaponConfig.flags.bCheckObjects = true;
    m_weaponConfig.flags.bCheckPeds = true;
    m_weaponConfig.flags.bCheckVehicles = true;
    m_weaponConfig.flags.bCheckBuildings = true;
    m_weaponConfig.flags.bCheckCarTires = true;
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
    /*if ( m_bHasTargetDirection )
    {
        CVector vecDirection = m_vecLastDirection;
        CVector vecOffset = m_vecTargetDirection - vecDirection;
        vecOffset /= CVector ( 10, 10, 10 );
        vecDirection += vecOffset;
        vecDirection.Normalize ();
        m_vecLastDirection = vecDirection;
        SetDirection ( vecDirection );
    }*/

    if ( m_State == WEAPONSTATE_FIRING ) Fire ();
}


void CClientWeapon::Create ( void )
{
    CClientObject::Create ();
    if ( m_weaponConfig.bDisableWeaponModel )
    {
        SetVisible ( false );
    }
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
        /*case WEAPONTYPE_SHOTGUN: 
        case WEAPONTYPE_SAWNOFF_SHOTGUN:
        case WEAPONTYPE_SPAS12_SHOTGUN:*/
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

            }
            else
            {
                GetRotationRadians ( vecRotation );

                vecRotation = -vecRotation;
            }
            CVector vecTarget;
            float fDistance = m_pWeaponInfo->GetWeaponRange ();
            if ( m_targetType == TARGET_TYPE_ENTITY )
            {
                if ( m_pTarget )
                {
                    if ( m_pTarget->GetType ( ) == CCLIENTPED || m_pTarget->GetType ( ) == CCLIENTPLAYER )
                    {
                        CClientPed * pPed = (CClientPed *) m_pTarget;
                        pPed->GetBonePosition( m_targetBone, vecTarget );
                    }
                    else
                    {
                        m_pTarget->GetPosition( vecTarget );
                    }
                    if ( m_weaponConfig.bShootIfTargetOutOfRange == false && (vecOrigin - vecTarget).Length() >= fDistance )
                    {
                        return;
                    }
                }
                else
                {
                    ResetWeaponTarget ( );
                }
            }
            else if ( m_targetType == TARGET_TYPE_VECTOR )
            {
                vecTarget = m_vecTarget;
                if ( m_weaponConfig.bShootIfTargetOutOfRange == false && (vecOrigin - vecTarget).Length() >= fDistance )
                {
                    return;
                }
            }
            else
            {
                CVector vecFireOffset = *m_pWeaponInfo->GetFireOffset ();
                RotateVector ( vecFireOffset, vecRotation );
                vecOrigin += vecFireOffset;

                CVector vecDirection ( 1, 0, 0 );
                vecDirection *= fDistance;
                RotateVector ( vecDirection, vecRotation );
                vecTarget = vecOrigin + vecDirection;
            }


            // Save
            short sDamage = m_pWeaponInfo->GetDamagePerHit ( );
            float fAccuracy = m_pWeaponInfo->GetAccuracy ( );
            float fTargetRange = m_pWeaponInfo->GetTargetRange ( );
            float fRange = m_pWeaponInfo->GetWeaponRange ( );

            // Set new
            m_pWeaponInfo->SetDamagePerHit ( m_pWeaponStat->GetDamagePerHit ( ) );
            m_pWeaponInfo->SetAccuracy ( m_pWeaponStat->GetAccuracy ( ) );
            m_pWeaponInfo->SetTargetRange ( m_pWeaponStat->GetTargetRange ( ) );
            m_pWeaponInfo->SetWeaponRange ( m_pWeaponStat->GetWeaponRange ( ) );

            // Process
            m_pMarker->SetPosition ( vecTarget );
            FireInstantHit ( vecOrigin, vecTarget );

            // Restore
            m_pWeaponInfo->SetDamagePerHit ( sDamage );
            m_pWeaponInfo->SetAccuracy ( fAccuracy );
            m_pWeaponInfo->SetTargetRange ( fTargetRange );
            m_pWeaponInfo->SetWeaponRange ( fRange );

            break;
        }
        default: break;
    }
}


void CClientWeapon::FireInstantHit ( CVector & vecOrigin, CVector & vecTarget )
{
    CVector vecDirection = vecTarget - vecOrigin;
    vecDirection.Normalize ();
    CClientEntity * pAttachedTo = GetAttachedTo ();    
    // Crashes with vehicle mounted epicness.
    //if ( pAttachedTo ) pAttachedTo->WorldIgnore ( true );

    CEntity * pColEntity = NULL;
    CColPoint * pColPoint = NULL;
    SLineOfSightBuildingResult pBuildingResult;
    CEntitySAInterface * pEntity = NULL;
    
    if ( m_pWeapon->ProcessLineOfSight ( &vecOrigin, &vecTarget, &pColPoint, &pColEntity, m_weaponConfig.flags, &pBuildingResult, m_Type, &pEntity ) )
    {
        vecTarget = pColPoint->GetPosition ();
    }

    if ( ( m_pTarget != NULL && m_pTarget->GetGameEntity ( ) != NULL && m_pTarget->GetGameEntity()->GetInterface ( ) == pEntity ) && m_weaponConfig.bShootIfTargetBlocked == false )
    {
        return;
    }
    g_pGame->GetPointLights ()->AddLight ( PLTYPE_POINTLIGHT, vecOrigin, CVector (), 3.0f, 0.22f, 0.25f, 0, 0, 0, 0 );

    if ( GetAttachedTo () ) g_pGame->GetFx ()->TriggerGunshot ( NULL, vecOrigin, vecDirection, false );
    else g_pGame->GetFx ()->TriggerGunshot ( NULL, vecOrigin, vecDirection, true );

    m_pWeapon->AddGunshell ( m_pObject, &vecOrigin, &CVector2D ( 0, -1 ), 0.45f );
    g_pGame->GetAudioEngine ()->ReportWeaponEvent ( WEAPON_EVENT_FIRE, m_Type, m_pObject );


    //if ( pAttachedTo ) pAttachedTo->WorldIgnore ( false );

    CVector vecCollision;
    if ( g_pGame->GetWaterManager ()->TestLineAgainstWater ( vecOrigin, vecTarget, &vecCollision ) )
    {
        g_pGame->GetFx ()->TriggerBulletSplash ( vecCollision );
        g_pGame->GetAudioEngine ()->ReportBulletHit ( NULL, SURFACE_TYPE_WATER_SHALLOW, &vecCollision, 0.0f );
    }    
    m_pMarker2->SetPosition ( vecTarget );
    
    m_pWeapon->DoBulletImpact ( m_pObject, pEntity, &vecOrigin, &vecTarget, pColPoint, 0 );
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

void CClientWeapon::SetWeaponTarget ( CClientEntity * pTarget, eBone boneTarget )
{
    m_pTarget = pTarget; 
    m_targetType = TARGET_TYPE_ENTITY; 
    m_targetBone = boneTarget; 
}

void CClientWeapon::SetWeaponTarget ( CVector vecTarget )
{
    m_vecTarget = vecTarget; 
    m_targetType = TARGET_TYPE_VECTOR;
}

void CClientWeapon::ResetWeaponTarget ( void )
{
    m_pTarget = NULL;
    m_vecTarget = CVector ( 0, 0, 0 );
    m_targetType = TARGET_TYPE_NONE;
    m_targetBone = BONE_PELVIS;
}
