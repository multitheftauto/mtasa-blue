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

CClientWeapon::CClientWeapon ( CClientManager * pManager, ElementID ID, eWeaponType type )
    : ClassInit ( this )
    , CClientObject ( pManager, ID, CClientPickupManager::GetWeaponModel ( type ), false )
{
    // Ensure m_pTarget and m_pOwner get NULLed when they are destroyed
    CClientEntityRefManager::AddEntityRefs ( ENTITY_REF_DEBUG ( this, "CClientWeapon" ), &m_pTarget, &m_pOwner, NULL );

    m_pManager = pManager;
    m_pManager->GetWeaponManager ()->AddToList ( this );

    m_Type = type;
    m_State = WEAPONSTATE_READY;
    m_pWeapon = NULL;
    m_pWeaponInfo = g_pGame->GetWeaponInfo ( m_Type );
    m_bHasTargetDirection = false;
    m_pOwner = NULL;

    SetTypeName ( "weapon" );

    SetStatic ( true );
    Create ();
#ifdef MARKER_DEBUG
    m_pMarker = new CClientMarker ( pManager, INVALID_ELEMENT_ID, 4 );
    m_pMarker->SetColor( SColor( 0xFF00FF00 ) );
    m_pMarker->SetSize ( 0.5f );
    m_pMarker2 = new CClientMarker ( pManager, INVALID_ELEMENT_ID, 4 );
    m_pMarker2->SetColor( SColor( 0xFFFF0000 ) );
    m_pMarker2->SetSize ( 0.5f );
#endif
    m_sDamage = m_pWeaponInfo->GetDamagePerHit ( );
    m_pWeaponStat = g_pGame->CreateWeaponStat ( type, WEAPONSKILL_STD );
    ResetWeaponTarget ( );
    // Setup weapon config.
    m_weaponConfig.bDisableWeaponModel = false;
    m_weaponConfig.bShootIfTargetBlocked = false;
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

    m_itargetWheel = MAX_WHEELS + 1;
    m_nAmmoInClip = m_pWeaponStat->GetMaximumClipAmmo();
    m_ucCounter = 0;
    m_nAmmoTotal = 9999;
    m_iWeaponFireRate = GetWeaponFireTime ( m_pWeaponStat );
}


CClientWeapon::~CClientWeapon ( void )
{
    m_pManager->GetWeaponManager ()->RemoveFromList ( this );

    Destroy ();
#ifdef MARKER_DEBUG
    delete m_pMarker;
    delete m_pMarker2;
#endif

    CClientEntityRefManager::RemoveEntityRefs ( 0, &m_pTarget, &m_pOwner, NULL );
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
    if ( m_targetType != TARGET_TYPE_FIXED )
    {
        CVector vecTargetPos;
        if ( m_targetType == TARGET_TYPE_ENTITY )
        {
            if ( m_pTarget )
            {
                if ( m_pTarget->GetType ( ) == CCLIENTPED || m_pTarget->GetType ( ) == CCLIENTPLAYER )
                {
                    CClientPed * pPed = (CClientPed *) m_pTarget;
                    pPed->GetBonePosition( m_targetBone, vecTargetPos );
                }
                else
                {
                    m_pTarget->GetPosition( vecTargetPos );
                }
            }
            else
            {
                ResetWeaponTarget ( );
            }
        }
        else if ( m_targetType == TARGET_TYPE_VECTOR )
        {
            vecTargetPos = m_vecTarget;
        }

        if ( !m_pAttachedToEntity )
        {
            // Calculate direction to target
            CVector vecPosition;
            GetPosition ( vecPosition );
            CVector vecDirection = vecTargetPos - vecPosition;

            // Convert direction to rotation
            CVector vecRotation = vecDirection.ToRotation ();
            SetRotationRadians ( vecRotation );
        }
        else
        {
            // Transform target position into local (AttachedToEntity) space
            CMatrix attachedToMatrix;
            m_pAttachedToEntity->GetMatrix ( attachedToMatrix );
            CVector vecLocalTargetPos = attachedToMatrix.Inverse ().TransformVector ( vecTargetPos );

            // Calculate local direction
            CVector vecDirection = vecLocalTargetPos - m_vecAttachedPosition;

            // Convert local direction to local rotation
            CVector vecRotation = vecDirection.ToRotation ();

            // Apply local rotation 
            SetAttachedOffsets ( m_vecAttachedPosition, vecRotation );
        }
    }


    if ( m_nAmmoInClip <= 0 )
    {
        if ( m_weaponConfig.bInstantReload == false )
        {
            if ( m_nAmmoTotal >= m_pWeaponStat->GetMaximumClipAmmo() && m_State != WEAPONSTATE_RELOADING )
            {
                m_PreviousState = m_State;
                m_State = WEAPONSTATE_RELOADING;
                m_reloadTimer.Reset();
            }
            else if ( m_State == WEAPONSTATE_RELOADING && m_reloadTimer.Get() >= m_pWeapon->GetWeaponReloadTime ( m_pWeaponStat ) )
            {
                m_State = m_PreviousState;
                m_nAmmoInClip = m_pWeaponStat->GetMaximumClipAmmo();
                m_nAmmoTotal -= m_pWeaponStat->GetMaximumClipAmmo();
            }
            else
                return;
        }
        else
        {
            if ( m_nAmmoTotal >= m_pWeaponStat->GetMaximumClipAmmo() && m_State != WEAPONSTATE_RELOADING )
            {
                m_nAmmoInClip = m_pWeaponStat->GetMaximumClipAmmo();
                m_nAmmoTotal -= m_pWeaponStat->GetMaximumClipAmmo();
            }
        }
    }
    if ( m_nAmmoInClip > 0 )
    {
        if ( m_State == WEAPONSTATE_FIRING && m_fireTimer.Get() >= m_iWeaponFireRate ) 
        {
            Fire ();
        }
    }
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
        //g_pGame->GetAudioEngine ()->ReportWeaponEvent ( WEAPON_EVENT_RELOAD, m_Type, m_pObject );
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
#ifdef SHOTGUN_TEST
        case WEAPONTYPE_SHOTGUN: 
        case WEAPONTYPE_SAWNOFF_SHOTGUN:
        case WEAPONTYPE_SPAS12_SHOTGUN:
#endif
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
                        if ( m_pTarget->GetType() == CCLIENTVEHICLE )
                        {
                            if ( m_itargetWheel <= MAX_WHEELS )
                            {
                                CClientVehicle * pTarget = (CClientVehicle*)m_pTarget;
                                vecTarget = pTarget->GetGameVehicle()->GetWheelPosition ( (eWheels)m_itargetWheel );
                            }
                            else
                                m_pTarget->GetPosition( vecTarget );
                        }
                        else
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
#ifndef SHOTGUN_TEST
                vecOrigin += vecFireOffset;
#endif
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

            // Begin our lag compensation
            CPlayerPed* pOwnerPed = NULL;
            if ( m_pOwner && !m_pTarget && g_pClientGame->GetPlayerManager()->Exists(m_pOwner) ) // No need for compensation if we're hitting a target directly
            {
                pOwnerPed = m_pOwner->GetGamePlayer();
                //if ( g_pGame->m_pPreWeaponFireHandler && pOwnerPed )
                    //g_pGame->m_pPreWeaponFireHandler ( pOwnerPed, false );
            }
#ifdef SHOTGUN_TEST
            CVector vecTemp;
            CVector vecFireOffset = *m_pWeaponInfo->GetFireOffset ();
            RotateVector ( vecFireOffset, vecRotation );
            vecTemp = vecFireOffset;
            vecTemp += vecOrigin;
#ifdef MARKER_DEBUG
            // Process
            m_pMarker->SetPosition ( vecOrigin );
#endif
            CVector vecTemp2;
            GetRotationDegrees(vecTemp2);
            vecTemp2.fZ -= 84.6f;
            SetRotationDegrees(vecTemp2);
            FireInstantHit ( vecOrigin, vecTarget-vecOrigin, vecTemp );
            vecTemp2.fZ += 84.6f;
            SetRotationDegrees(vecTemp2);
#else
            FireInstantHit ( vecOrigin, vecTarget );
#endif
            // Restore
            m_pWeaponInfo->SetDamagePerHit ( sDamage );
            m_pWeaponInfo->SetAccuracy ( fAccuracy );
            m_pWeaponInfo->SetTargetRange ( fTargetRange );
            m_pWeaponInfo->SetWeaponRange ( fRange );

            // End our lag compensation
            //if ( g_pGame->m_pPostWeaponFireHandler && pOwnerPed )
                //g_pGame->m_pPostWeaponFireHandler ();

            m_nAmmoInClip--;
            m_fireTimer.Reset();
            break;
        }
        default: break;
    }
}

#ifdef SHOTGUN_TEST
void CClientWeapon::FireInstantHit ( CVector & vecOrigin, CVector & vecTarget, CVector & vecRotation )
#else
void CClientWeapon::FireInstantHit ( CVector & vecOrigin, CVector & vecTarget )
#endif
{
    CVector vecDirection = vecTarget - vecOrigin;
    vecDirection.Normalize ();
    CClientEntity * pAttachedTo = GetAttachedTo ();    
    
    CEntity * pColEntity = NULL;
    CColPoint * pColPoint = NULL;
    SLineOfSightBuildingResult pBuildingResult;
    CEntitySAInterface * pEntity = NULL;

    if ( m_Type != WEAPONTYPE_SHOTGUN )
    {
        //if ( pAttachedTo ) pAttachedTo->WorldIgnore ( true );
        if ( m_pWeapon->ProcessLineOfSight ( &vecOrigin, &vecTarget, &pColPoint, &pColEntity, m_weaponConfig.flags, &pBuildingResult, m_Type, &pEntity ) )
        {
            vecTarget = pColPoint->GetPosition ();
        }

        // Don't continue without a valid colpoint
        if ( !pColPoint )
            return;

        //if ( pAttachedTo ) pAttachedTo->WorldIgnore ( false );

        // return if shoot if target is blocked is false and we aren't pointing at our target
        if ( ( m_pTarget != NULL && m_pTarget->GetGameEntity ( ) != NULL && m_pTarget->GetGameEntity()->GetInterface ( ) != pEntity ) && m_weaponConfig.bShootIfTargetBlocked == false )
        {
            return;
        }

        // Execute our weapon fire event
        CClientEntity * pClientEntity = m_pManager->FindEntitySafe ( pColEntity );
        CLuaArguments Arguments;
        if ( pClientEntity )
            Arguments.PushElement ( pClientEntity );            // entity that got hit
        else
            Arguments.PushNil ( ); // Probably a building.
        Arguments.PushNumber ( pColPoint->GetPosition ( ).fX ); // pos x
        Arguments.PushNumber ( pColPoint->GetPosition ( ).fY ); // pos y
        Arguments.PushNumber ( pColPoint->GetPosition ( ).fZ ); // pos z
        Arguments.PushNumber ( pColPoint->GetNormal ( ).fX ); // Normal x
        Arguments.PushNumber ( pColPoint->GetNormal ( ).fY ); // Normal y
        Arguments.PushNumber ( pColPoint->GetNormal ( ).fZ ); // Normal z
        Arguments.PushNumber ( pColPoint->GetSurfaceTypeB ( ) ); // Surface type "B"
        Arguments.PushNumber ( pColPoint->GetLightingForTimeOfDay ( ) ); // Lighting
        Arguments.PushNumber ( pColPoint->GetPieceTypeB ( ) ); // Piece
        if ( !CallEvent ( "onClientWeaponFire", Arguments, true ) )
        {
            return;
        }

        DoGunShells ( vecOrigin, vecDirection );

        CVector vecCollision;
        if ( g_pGame->GetWaterManager ()->TestLineAgainstWater ( vecOrigin, vecTarget, &vecCollision ) )
        {
            g_pGame->GetFx ()->TriggerBulletSplash ( vecCollision );
            g_pGame->GetAudioEngine ()->ReportBulletHit ( NULL, SURFACE_TYPE_WATER_SHALLOW, &vecCollision, 0.0f );
        }    
#ifdef MARKER_DEBUG
        m_pMarker2->SetPosition ( vecTarget );
#endif
        m_pWeapon->DoBulletImpact ( m_pObject, pEntity, &vecOrigin, &vecTarget, pColPoint, 0 );
        if ( pColEntity && pColEntity->GetEntityType () == ENTITY_TYPE_PED )
        {
            ePedPieceTypes hitZone = ( ePedPieceTypes ) pColPoint->GetPieceTypeB ();
            short sDamage = m_pWeaponInfo->GetDamagePerHit ();
            m_pWeapon->GenerateDamageEvent ( dynamic_cast < CPed * > ( pColEntity ), m_pObject, m_Type, sDamage, hitZone, 0 );
        }
    }
#ifdef SHOTGUN_TEST
    else
    {
        //if ( pAttachedTo ) pAttachedTo->WorldIgnore ( true );
        //if ( pAttachedTo ) pAttachedTo->WorldIgnore ( false );
        // Fire instant hit is off by a few degrees
        FireShotgun ( m_pObject, vecOrigin, vecTarget, vecRotation );
    }
#endif
    if ( pColPoint )
        pColPoint->Destroy ();
}


void CClientWeapon::FireShotgun ( CEntity* pFiringEntity, const CVector& vecOrigin, const CVector& vecTarget, CVector & vecRotation )
{
    //m_pWeapon->FireBullet ( pFiringEntity, vecOrigin, vecTarget, false, vecRotation );
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
    vecRotation.fY = atan2 ( vecTemp.fX, vecTemp.fY )/* - ConvertDegreesToRadians ( 90 )*/;
    
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

void CClientWeapon::SetWeaponTarget ( CClientEntity * pTarget, int subTarget )
{
    m_pTarget = pTarget; 
    m_targetType = TARGET_TYPE_ENTITY; 
    if ( pTarget->GetType() == CCLIENTPED )
    {
        if ( subTarget == 255 )
            m_targetBone = eBone::BONE_PELVIS;
        else
            m_targetBone = (eBone)subTarget; 
    }
    if ( pTarget->GetType() == CCLIENTVEHICLE )
    {
        m_itargetWheel = subTarget; 
    }
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
    m_targetType = TARGET_TYPE_FIXED;
    m_targetBone = BONE_PELVIS;
}

bool CClientWeapon::SetFlags ( eWeaponFlags flag, bool bData )
{
    switch ( flag )
    {
    case WEAPONFLAGS_SHOOT_IF_OUT_OF_RANGE:
        m_weaponConfig.bShootIfTargetOutOfRange = bData;
        return true;
    case WEAPONFLAGS_SHOOT_IF_TARGET_BOCKED:
        m_weaponConfig.bShootIfTargetBlocked = bData;
        return true;
    case WEAPONFLAGS_DISABLE_MODEL:
        m_weaponConfig.bDisableWeaponModel = bData;
        if ( m_weaponConfig.bDisableWeaponModel )
        {
            SetVisible( false );
        }
        else
        {
            SetVisible( true );
        }
        return true;
    case WEAPONFLAGS_INSTANT_RELOAD:
        m_weaponConfig.bInstantReload = bData;
        return true;
    }
    return false;
}

bool CClientWeapon::SetFlags ( const SLineOfSightFlags flags )
{
    m_weaponConfig.flags = flags;
    return true;
}

bool CClientWeapon::GetFlags ( eWeaponFlags flag, bool &bData )
{

    switch ( flag )
    {
    case WEAPONFLAGS_SHOOT_IF_OUT_OF_RANGE:
        bData = m_weaponConfig.bShootIfTargetOutOfRange;
        return true;
    case WEAPONFLAGS_SHOOT_IF_TARGET_BOCKED:
        bData = m_weaponConfig.bShootIfTargetBlocked;
        return true;
    case WEAPONFLAGS_DISABLE_MODEL:
        bData = m_weaponConfig.bDisableWeaponModel;
        return true;
    case WEAPONFLAGS_INSTANT_RELOAD:
        bData = m_weaponConfig.bInstantReload;
        return true;
    }
    return false;
}

bool CClientWeapon::GetFlags ( SLineOfSightFlags &flags )
{
    flags = m_weaponConfig.flags;
    return true;
}

void CClientWeapon::DoGunShells ( CVector vecOrigin, CVector vecDirection )
{

    float fShellSize = 0.0f;

    switch( m_Type )
    {
        case(WEAPONTYPE_MINIGUN):
        case(WEAPONTYPE_M4):
        case(WEAPONTYPE_AK47):
        {
            if ( m_ucCounter == 0)
            {
                fShellSize = 0.25f;
                m_ucCounter = 1;
            }
            else
            {
                m_ucCounter = 0;
            }
            break;
        }
        break;
        case(WEAPONTYPE_MICRO_UZI):
        case(WEAPONTYPE_MP5):
        case(WEAPONTYPE_TEC9):
            fShellSize = 0.3f;
        break;

        case(WEAPONTYPE_SNIPERRIFLE):
        case(WEAPONTYPE_PISTOL):
        case(WEAPONTYPE_PISTOL_SILENCED):
        case(WEAPONTYPE_DESERT_EAGLE):
            fShellSize = 0.25f;
        break;

        case(WEAPONTYPE_SHOTGUN):
        case(WEAPONTYPE_SAWNOFF_SHOTGUN):
        case(WEAPONTYPE_SPAS12_SHOTGUN):
            fShellSize = 0.45f;
        break;
    }

    if(fShellSize > 0.0f)
    {
        g_pGame->GetPointLights ()->AddLight ( PLTYPE_POINTLIGHT, vecOrigin, CVector (), 3.0f, 0.22f, 0.25f, 0, 0, 0, 0 );

        if ( GetAttachedTo () ) g_pGame->GetFx ()->TriggerGunshot ( NULL, vecOrigin, vecDirection, true );
        else g_pGame->GetFx ()->TriggerGunshot ( NULL, vecOrigin, vecDirection, true );

        m_pWeapon->AddGunshell ( m_pObject, &vecOrigin, &CVector2D ( 0, -1 ), fShellSize );
        g_pGame->GetAudioEngine ()->ReportWeaponEvent ( WEAPON_EVENT_FIRE, m_Type, m_pObject );

    }
}

int CClientWeapon::GetWeaponFireTime ( CWeaponStat * pWeaponStat )
{
    float fWeaponFireTime = (pWeaponStat->GetWeaponAnimLoopStop() - pWeaponStat->GetWeaponAnimLoopStart()) * 1000.0f;
    return (int)fWeaponFireTime;
}

void CClientWeapon::SetWeaponFireTime ( int iFireTime )
{
    m_iWeaponFireRate = iFireTime;
}

int CClientWeapon::GetWeaponFireTime ( void )
{
    return m_iWeaponFireRate;
}

void CClientWeapon::ResetWeaponFireTime ( void )
{
    m_iWeaponFireRate = GetWeaponFireTime ( m_pWeaponStat );
}

eTargetType CClientWeapon::GetWeaponTargetType ( void )
{
    return m_targetType;
}

CVector CClientWeapon::GetWeaponVectorTarget ( void )
{
    return m_vecTarget;
}

CClientEntity * CClientWeapon::GetWeaponEntityTarget ( void )
{
    return m_pTarget;
}
