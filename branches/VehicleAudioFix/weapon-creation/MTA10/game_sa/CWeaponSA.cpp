/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeaponSA.cpp
*  PURPOSE:     Weapon class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CWeaponSA::CWeaponSA( CWeaponSAInterface * weaponInterface, CPed * ped, eWeaponSlot weaponSlot )
{
    DEBUG_TRACE("CWeaponSA::CWeaponSA( CWeaponSAInterface * weaponInterface, CPed * ped, DWORD dwSlot )");
    this->owner = ped;
    this->m_weaponSlot = weaponSlot;
    internalInterface = weaponInterface;    
}
void CWeaponSA::Destroy ( void )
{
    if ( !owner )
    {
        delete internalInterface;
        delete this;
    }
}

eWeaponType CWeaponSA::GetType(  )
{
    DEBUG_TRACE("eWeaponType CWeaponSA::GetType(  )");
    return this->internalInterface->m_eWeaponType;
};

VOID CWeaponSA::SetType( eWeaponType type )
{
    DEBUG_TRACE("VOID CWeaponSA::SetType( eWeaponType type )");
    this->internalInterface->m_eWeaponType = type;
}

eWeaponState CWeaponSA::GetState(  )
{
    DEBUG_TRACE("eWeaponState CWeaponSA::GetState(  )");
    return this->internalInterface->m_eState;
}

void CWeaponSA::SetState ( eWeaponState state )
{
    DEBUG_TRACE("void CWeaponSA::SetState ( eWeaponState state )");
    this->internalInterface->m_eState = state;
}

DWORD CWeaponSA::GetAmmoInClip(  )
{
    DEBUG_TRACE("DWORD CWeaponSA::GetAmmoInClip(  )");
    return this->internalInterface->m_nAmmoInClip;
}

VOID CWeaponSA::SetAmmoInClip( DWORD dwAmmoInClip )
{
    DEBUG_TRACE("VOID CWeaponSA::SetAmmoInClip( DWORD dwAmmoInClip )");
    this->internalInterface->m_nAmmoInClip = dwAmmoInClip;
}

DWORD CWeaponSA::GetAmmoTotal(  )
{
    DEBUG_TRACE("DWORD CWeaponSA::GetAmmoTotal(  )");
    return this->internalInterface->m_nAmmoTotal;
}

VOID CWeaponSA::SetAmmoTotal( DWORD dwAmmoTotal )
{
    DEBUG_TRACE("VOID CWeaponSA::SetAmmoTotal( DWORD dwAmmoTotal )");
    this->internalInterface->m_nAmmoTotal = dwAmmoTotal;
}

CPed * CWeaponSA::GetPed()
{
    DEBUG_TRACE("CPed * CWeaponSA::GetPed()");
    return (CPed *)owner;
}

eWeaponSlot CWeaponSA::GetSlot()
{
    DEBUG_TRACE("eWeaponSlot CWeaponSA::GetSlot()");
    return m_weaponSlot;
}

VOID CWeaponSA::SetAsCurrentWeapon()
{
    DEBUG_TRACE("VOID CWeaponSA::SetAsCurrentWeapon()");
    owner->SetCurrentWeaponSlot( m_weaponSlot );
}

void CWeaponSA::Remove ()
{
    DEBUG_TRACE("void CWeaponSA::Remove ()");
    DWORD dwFunc = FUNC_Shutdown;
    DWORD dwThis = (DWORD)this->internalInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    // If the removed weapon was the currently active weapon, switch to empty-handed
    if ( owner->GetCurrentWeaponSlot () == m_weaponSlot )
    {
        CWeaponInfo* pInfo = pGame->GetWeaponInfo ( this->internalInterface->m_eWeaponType );
        if ( pInfo )
        {
            int iModel = pInfo->GetModel();
            owner->RemoveWeaponModel ( iModel );
        }
        owner->SetCurrentWeaponSlot ( WEAPONSLOT_TYPE_UNARMED );
    }
}
void CWeaponSA::Initialize ( eWeaponType type, unsigned int uiAmmo, CPed * pPed )
{
    DWORD dwPedInterface = 0;
    if ( pPed ) dwPedInterface = ( DWORD ) pPed->GetInterface ();
    unsigned int uiType = ( unsigned int ) type;
    DWORD dwThis = ( DWORD ) internalInterface;
    DWORD dwFunc = FUNC_CWeapon_Initialize;
    _asm
    {
        mov     ecx, dwThis
        push    dwPedInterface
        push    uiAmmo
        push    uiType
        call    dwFunc
    }
}


void CWeaponSA::Update ( CPed * pPed )
{
    // Note: CWeapon::Update is called mainly to check for reload
    DWORD dwPedInterface = ( DWORD ) pPed->GetInterface ();
    DWORD dwThis = ( DWORD ) internalInterface;
    DWORD dwFunc = FUNC_CWeapon_Update;
    _asm
    {
        mov     ecx, dwThis
        push    dwPedInterface
        call    dwFunc
    }
}


bool CWeaponSA::Fire ( CEntity * pFiringEntity, CVector * pvecOrigin, CVector * pvecTarget, CEntity * pTargetEntity, CVector * pvec_1, CVector * pvec_2 )
{
    bool bReturn;
    DWORD dwFiringInterface = 0;
    if ( pFiringEntity ) dwFiringInterface = ( DWORD ) pFiringEntity->GetInterface ();
    DWORD dwTargetInterface = 0;
    if ( pTargetEntity ) dwTargetInterface = ( DWORD ) pTargetEntity->GetInterface ();
    DWORD dwThis = ( DWORD ) internalInterface;
    DWORD dwFunc = FUNC_CWeapon_Fire;
    _asm
    {
        mov     ecx, dwThis
        push    pvec_2
        push    pvec_1
        push    dwTargetInterface
        push    pvecTarget
        push    pvecOrigin
        push    dwFiringInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


void CWeaponSA::AddGunshell ( CEntity * pFiringEntity, CVector * pvecOrigin, CVector2D * pvecDirection, float fSize )
{
    DWORD dwEntityInterface = 0;
    if ( pFiringEntity ) dwEntityInterface = ( DWORD ) pFiringEntity->GetInterface ();
    DWORD dwThis = ( DWORD ) internalInterface;
    DWORD dwFunc = FUNC_CWeapon_AddGunshell;
    _asm
    {
        mov     ecx, dwThis
        push    fSize
        push    pvecDirection
        push    pvecOrigin
        push    dwEntityInterface
        call    dwFunc
    }
}


void CWeaponSA::DoBulletImpact ( CEntity * pFiringEntity, CEntitySAInterface * pEntityInterface, CVector * pvecOrigin, CVector * pvecTarget, CColPoint * pColPoint, int i_1 )
{
    DWORD dwEntityInterface = 0;
    if ( pFiringEntity ) dwEntityInterface = ( DWORD ) pFiringEntity->GetInterface ();
    DWORD dwEntityInterface_2 = (DWORD)pEntityInterface;
    DWORD dwColPointInterface = 0;
    if ( pColPoint ) dwColPointInterface = ( DWORD )  pColPoint->GetInterface ();
    DWORD dwThis = ( DWORD ) internalInterface;
    DWORD dwFunc = FUNC_CWeapon_DoBulletImpact;
    _asm
    {
        mov     ecx, dwThis
        push    i_1
        push    dwColPointInterface
        push    pvecTarget
        push    pvecOrigin
        push    dwEntityInterface_2
        push    dwEntityInterface
        call    dwFunc
    }
}


unsigned char CWeaponSA::GenerateDamageEvent ( CPed * pPed, CEntity * pResponsible, eWeaponType weaponType, int iDamagePerHit, ePedPieceTypes hitZone, int i_2 )
{
    unsigned int ucReturn;
    DWORD dwPedInterface = ( DWORD ) pPed->GetInterface ();
    DWORD dwResponsibleInterface = 0;
    if ( pResponsible ) dwResponsibleInterface = ( DWORD ) pResponsible->GetInterface ();
    DWORD dwFunc = FUNC_CWeapon_GenerateDamageEvent;
    _asm
    {
        push    i_2
        push    hitZone
        push    iDamagePerHit
        push    weaponType
        push    dwResponsibleInterface
        push    dwPedInterface
        call    dwFunc
        add     esp, 24
        mov     ucReturn, eax
    }
    return ( unsigned char ) ucReturn;
}

bool CWeaponSA::FireBullet ( CEntity* pFiringEntity, const CVector& vecOrigin, const CVector& vecTarget, bool bAddMuzzle, CVector & vecRotation )
{
    if ( !pFiringEntity )
    {
        return false;
    }

    switch ( GetType () )
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
            // Don't hit shooter
            //pGame->GetWorld ()->IgnoreEntity ( pFiringEntity );

            // Do pre shot lag compensation
            CPlayerPed* pFiringPlayerPed = dynamic_cast < CPlayerPed* > ( pFiringEntity );
            //if ( pGame->m_pPreWeaponFireHandler && pFiringPlayerPed )
            //    pGame->m_pPreWeaponFireHandler ( pFiringPlayerPed, false );

            // Get the gun muzzle position
            float fSkill = 999.f;
            CWeaponStat* pCurrentWeaponInfo = pGame->GetWeaponStatManager ( )->GetWeaponStatsFromSkillLevel ( GetType (), fSkill );
            CVector vecGunMuzzle;
            CVector vecOriginPos = vecOrigin;
            CVector vecShotVector;
            if ( bAddMuzzle )
            {
                //vecGunMuzzle = *pCurrentWeaponInfo->GetFireOffset ();
            }
            else
            {
                vecGunMuzzle = vecRotation;
            }
            if ( pFiringPlayerPed )
                pFiringPlayerPed->GetTransformedBonePosition ( BONE_RIGHTWRIST, &vecGunMuzzle );

            // Bullet trace
            FireInstantHit ( pFiringEntity, &vecOriginPos, &vecGunMuzzle, NULL, &vecTarget, &vecOrigin, false, true );

            // Do post shot lag compensation reset & script events
            //if ( pGame->m_pPostWeaponFireHandler && pFiringPlayerPed )
            //    pGame->m_pPostWeaponFireHandler ();

            //pGame->GetWorld ()->IgnoreEntity ( NULL );

            return true;
        }

    default:
        break;

    }
    return false;
}


bool CWeaponSA::FireInstantHit ( CEntity * pFiringEntity, const CVector* pvecOrigin, const CVector* pvecMuzzle, CEntity* pTargetEntity, const CVector* pvecTarget, const CVector* pvec, bool bCrossHairGun, bool bCreateGunFx )
{
    bool bReturn;
    DWORD dwEntityInterface = 0;
    if ( pFiringEntity ) dwEntityInterface = ( DWORD ) pFiringEntity->GetInterface ();
    DWORD dwTargetInterface = 0;
    if ( pTargetEntity ) dwTargetInterface = ( DWORD ) pTargetEntity->GetInterface ();
    DWORD dwThis = ( DWORD ) internalInterface;
    DWORD dwFunc = FUNC_CWeapon_FireInstantHit;
    _asm
    {
        mov     ecx, dwThis
        push    bCreateGunFx
        push    bCrossHairGun
        push    pvec
        push    pvecTarget
        push    dwTargetInterface
        push    pvecMuzzle
        push    pvecOrigin
        push    dwEntityInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CWeaponSA::ProcessLineOfSight ( const CVector * vecStart, const CVector * vecEnd, CColPoint ** colCollision, 
                                             CEntity ** CollisionEntity,
                                             const SLineOfSightFlags flags,
                                             SLineOfSightBuildingResult* pBuildingResult, 
                                             eWeaponType weaponType, 
                                             CEntitySAInterface ** pEntity )
{
    DWORD dwFunction = FUNC_CBirds_CheckForHit;
    _asm
    {
        push vecEnd
        push vecStart
        call dwFunction
    }
    dwFunction = FUNC_CShadows_CheckForHit;
    _asm
    {
        push vecEnd
        push vecStart
        call dwFunction
    }
    bool bReturn = g_pCore->GetGame()->GetWorld()->ProcessLineOfSight( vecStart, vecEnd, colCollision, CollisionEntity, flags, pBuildingResult );
    _asm
    {
        add esp, 10h
    }

    if ( bReturn )
    {
        if ( *CollisionEntity )
        {
            *pEntity = (*CollisionEntity)->GetInterface();
        }
        else
        {
            if ( pBuildingResult->bValid )
                *pEntity = pBuildingResult->pInterface;
        }
    }
    if ( *pEntity && (*pEntity)->nType == ENTITY_TYPE_VEHICLE )
    {
        CEntitySAInterface * pHitInterface = *pEntity;
        CColPointSAInterface * pColPointSAInterface = (*colCollision)->GetInterface();
        DWORD dwFunc = FUNC_CWeapon_CheckForShootingVehicleOccupant;
        _asm
        {
            push vecEnd
            push vecStart
            push weaponType
            push pColPointSAInterface
            lea eax, pHitInterface
            push eax
            call dwFunc
            add esp, 14h
        }
    }
    return  bReturn;
}

int CWeaponSA::GetWeaponReloadTime ( CWeaponStat * pWeaponStat )
{
    CWeaponStatSA * pWeaponStats = (CWeaponStatSA*)pWeaponStat;
    DWORD dwReturn = 0;
    DWORD dwFunction = FUNC_CWeaponInfo_GetWeaponReloadTime;
    DWORD dwInterface = (DWORD)pWeaponStats->GetInterface();
    _asm
    {
        mov ecx, dwInterface
        call dwFunction
        mov dwReturn, eax
    }
    return dwReturn;
}

int CWeaponSA::GetWeaponFireTime ( CWeaponStat * pWeaponStat )
{
    int iGlobalTimer = pGame->GetSystemTime();
    float fWeaponFireTime = (pWeaponStat->GetWeaponAnimLoopStop() - pWeaponStat->GetWeaponAnimLoopStart()) * 1000.0f;
    return (int)fWeaponFireTime;
}