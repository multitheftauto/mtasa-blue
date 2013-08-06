/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedSA.cpp
*  PURPOSE:     Ped entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA* pGame;

CPedSA::CPedSA (  ) :
    m_pPedIntelligence ( NULL ),
    m_pPedInterface ( NULL ),
    m_pPedSound ( NULL ),
    m_iCustomMoveAnim( 0 )
{
    DEBUG_TRACE("CPedSA::CPedSA(  )");

    MemSetFast ( this->m_pWeapons, 0, sizeof ( CWeaponSA* ) * WEAPONSLOT_MAX );
}

CPedSA::CPedSA( CPedSAInterface * pPedInterface ) :
    m_pPedIntelligence ( NULL ),
    m_pPedInterface ( pPedInterface ),
    m_pPedSound ( NULL ),
    m_iCustomMoveAnim( 0 )
{
    DEBUG_TRACE("CPedSA::CPedSA( CPedSAInterface * pedInterface )");

    MemSetFast ( this->m_pWeapons, 0, sizeof ( CWeaponSA* ) * WEAPONSLOT_MAX );
}

VOID CPedSA::SetInterface( CEntitySAInterface * intInterface )
{
    m_pInterface = intInterface;
}

CPedSA::~CPedSA ( void )
{
    if ( m_pPedIntelligence ) delete m_pPedIntelligence;
    if ( m_pPedSound ) delete m_pPedSound;

    for ( int i = 0; i < WEAPONSLOT_MAX; i++ )
    {
        if ( this->m_pWeapons[i] )
            delete this->m_pWeapons[i];
    }

    // Make sure this ped is not refed in the flame shot info array
    CFlameShotInfo* pInfo = (CFlameShotInfo*)ARRAY_CFlameShotInfo;
    for ( uint i = 0 ; i < MAX_FLAME_SHOT_INFOS ; i++ )
    {
        if ( pInfo->pInstigator == m_pInterface )
        {
            pInfo->pInstigator = NULL;
            pInfo->ucFlag1 = 0;
        }
        pInfo++;
    }
}

// used to init weapons at the moment, called by CPlayerPedSA when its been constructed
// can't use constructor as thats called before the interface pointer has been set the the aforementioned constructor
/**
 * \todo Reimplement weapons and PedIK for SA
 */

void CPedSA::Init()
{
    DEBUG_TRACE("void CPedSA::Init()");
    CPedSAInterface * pedInterface = GetPedInterface ();
    
    DWORD dwPedIntelligence = 0;
    DWORD dwFunc = 0x411DE0;
    DWORD dwInterface = (DWORD)pedInterface;
    _asm
    {
        mov     ecx, dwInterface
        call    dwFunc
        mov     dwPedIntelligence, eax
    }
    CPedIntelligenceSAInterface * m_pPedIntelligenceInterface = (CPedIntelligenceSAInterface *)(dwPedIntelligence);
    this->m_pPedIntelligence = new CPedIntelligenceSA(m_pPedIntelligenceInterface, this);
    this->m_pPedSound = new CPedSoundSA ( &pedInterface->pedSound );

    for ( int i = 0; i < WEAPONSLOT_MAX; i++ )
        this->m_pWeapons[i] = new CWeaponSA(&(pedInterface->Weapons[i]), this, (eWeaponSlot)i);

    //this->m_pPedIK = new Cm_pPedIKSA(&(pedInterface->m_pPedIK));
}


void CPedSA::SetModelIndex ( DWORD dwModelIndex )
{
    DEBUG_TRACE("void CPedSA::SetModelIndex ( DWORD dwModelIndex )");
    DWORD dwFunction = FUNC_SetModelIndex;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    dwModelIndex
        call    dwFunction
    }

    // Also set the voice gender
    CPedModelInfoSAInterface* pModelInfo = (CPedModelInfoSAInterface *)pGame->GetModelInfo (
        dwModelIndex )->GetInterface ();
    if ( pModelInfo )
    {
        DWORD dwType = pModelInfo->pedType;
        GetPedInterface ()->pedSound.m_bIsFemale = ( dwType == 5 || dwType == 22 );
    }
}

// Hacky thing done for the local player when changing model
void CPedSA::RemoveGeometryRef ( void )
{
    RpClump* pClump = (RpClump*)GetInterface ()->m_pRwObject;
    RpAtomic* pAtomic = (RpAtomic*)( ( pClump->atomics.root.next ) - 0x8 );
    RpGeometry* pGeometry = pAtomic->geometry;
    if ( pGeometry->refs > 1 )
        pGeometry->refs--;
}


bool CPedSA::IsInWater ( void )
{
    DEBUG_TRACE("bool CPedSA::IsInWater ()");
    CTask *pTask = this->m_pPedIntelligence->GetTaskManager ()->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
    return ( pTask && ( pTask->GetTaskType () == TASK_COMPLEX_IN_WATER ) );
}

void CPedSA::AttachPedToBike(CEntity * entity, CVector * vector, unsigned short sUnk, FLOAT fUnk, FLOAT fUnk2, eWeaponType weaponType)
{
    DEBUG_TRACE("void CPedSA::AttachPedToBike(CEntity * entity, CVector * vector, unsigned short sUnk, FLOAT fUnk, FLOAT fUnk2, eWeaponType weaponType)");

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( entity );
    if ( !pEntitySA ) return;

    DWORD dwEntityInterface = (DWORD)pEntitySA->GetInterface();
    DWORD dwFunc = FUNC_AttachPedToBike;
    FLOAT fX = vector->fX;
    FLOAT fY = vector->fY;
    FLOAT fZ = vector->fZ;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        push    weaponType
        push    fUnk2
        push    fUnk
        push    sUnk
        push    fZ
        push    fY
        push    fX
        mov     ecx, dwThis
        push    dwEntityInterface
        call    dwFunc
    }   
}

bool CPedSA::AddProjectile ( eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity )
{
    return ((CProjectileInfoSA*)pGame->GetProjectileInfo())->AddProjectile ( (CEntitySA*)this, eWeapon, vecOrigin, fForce, target, targetEntity );
}


void CPedSA::DetachPedFromEntity ( void )
{
    DWORD dwFunc = FUNC_DetachPedFromEntity;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

bool CPedSA::InternalAttachEntityToEntity(DWORD dwEntityInterface, const CVector * vecPosition, const CVector * vecRotation)
{
    AttachPedToEntity ( dwEntityInterface, const_cast < CVector * > ( vecPosition ), 0, 0.0f, WEAPONTYPE_UNARMED, FALSE );
    return true;
}

void CPedSA::AttachPedToEntity(DWORD dwEntityInterface, CVector * vector, unsigned short sDirection, FLOAT fRotationLimit, eWeaponType weaponType, bool bChangeCamera)
{
    // sDirection and fRotationLimit only apply to first-person shooting (bChangeCamera)
    DEBUG_TRACE("void CPedSA::AttachPedToEntity(CVehicle * entity, CVector * vector, unsigned short sUnk, FLOAT fUnk, eWeaponType weaponType)");
    DWORD dwFunc = FUNC_AttachPedToEntity;
    DWORD dwThis = (DWORD)this->GetInterface();
    FLOAT fX = vector->fX;
    FLOAT fY = vector->fY;
    FLOAT fZ = vector->fZ;
    BYTE bPedType = ((CPedSAInterface*)GetInterface())->bPedType;

    // Hack the CPed type(?) to non-player so the camera doesn't get changed
    if ( !bChangeCamera )
        ((CPedSAInterface*)GetInterface())->bPedType = 2;

    _asm
    {
        push    weaponType
        push    fRotationLimit
        movzx   ecx, sDirection
        push    ecx
        push    fZ
        push    fY
        push    fX
        push    dwEntityInterface
        mov     ecx, dwThis
        call    dwFunc
    }
    
    // Hack the CPed type(?) to whatever it was set to
    if ( !bChangeCamera )
        ((CPedSAInterface*)GetInterface())->bPedType = bPedType;
}

bool CPedSA::CanSeeEntity(CEntity * entity, FLOAT fDistance)
{
    DEBUG_TRACE("bool CPedSA::CanSeeEntity(CEntity * entity, FLOAT fDistance)");

    DWORD dwFunc = FUNC_CanSeeEntity;
    bool bReturn = false;

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( entity );
    if ( pEntitySA )
    {
        DWORD dwEntityInterface = (DWORD)pEntitySA->GetInterface();
        _asm
        {
            push    fDistance
            push    dwEntityInterface
            call    dwFunc
            mov     bReturn, al
        }
    }

    return bReturn;
}

CVehicle * CPedSA::GetVehicle()
{
    DEBUG_TRACE("CVehicle * CPedSA::GetVehicle()");
    if ( ((CPedSAInterface *)this->GetInterface())->pedFlags.bInVehicle )
    {
        CVehicleSAInterface * vehicle = (CVehicleSAInterface *)(((CPedSAInterface *)this->GetInterface())->CurrentObjective);
        if(vehicle)
            return ((CPoolsSA *)pGame->GetPools())->GetVehicle((DWORD *)vehicle);
    }
    return NULL;
}

void CPedSA::Respawn(CVector * position, bool bCameraCut)
{
    CPed * pLocalPlayer = pGame->GetPools()->GetPedFromRef ( (DWORD)1 );

    if ( !bCameraCut )
    {
         // DISABLE call to CCamera__RestoreWithJumpCut when respawning
        MemSet ( (void*)0x4422EA, 0x90, 20 );
    }

    DEBUG_TRACE("void CPedSA::Respawn(CVector * position)");
    FLOAT fX = position->fX;
    FLOAT fY = position->fY;
    FLOAT fZ = position->fZ;
    FLOAT fUnk = 1.0f; 
    DWORD dwFunc = FUNC_RestorePlayerStuffDuringResurrection;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        push    fUnk
        push    fZ
        push    fY
        push    fX
        push    dwThis
        call    dwFunc
        add     esp, 20
    }
#if 0   // Removed to see if it reduces crashes
    dwFunc = 0x441440; // CGameLogic::SortOutStreamingAndMemory
    fUnk = 10.0f;
    _asm
    {
        push    fUnk
        push    position
        call    dwFunc
        add     esp, 8
    }
#endif
    dwFunc = FUNC_RemoveGogglesModel;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    if ( !bCameraCut )
    {
        // B9 28 F0 B6 00 E8 4C 9A 0C 00 B9 28 F0 B6 00 E8 B2 97 0C 00
        unsigned char szCode[] = {0xB9, 0x28, 0xF0, 0xB6, 0x00, 0xE8, 0x4C, 0x9A, 0x0C, 0x00, 0xB9, 0x28, 0xF0, 0xB6, 0x00, 0xE8, 0xB2, 0x97, 0x0C, 0x00} ;
        // RE-ENABLE call to CCamera__RestoreWithJumpCut when respawning
        MemCpy ( (void*)0x4422EA, szCode, 20 );
    }
    //OutputDebugString ( "Respawn!!!!" );
}

FLOAT CPedSA::GetHealth()
{
    return GetPedInterface ()->fHealth;
}

void CPedSA::SetHealth ( float fHealth )
{
    GetPedInterface ()->fHealth = fHealth;
}

float CPedSA::GetArmor ( void )
{
    return GetPedInterface ()->fArmor;
}

void CPedSA::SetArmor ( float fArmor )
{
    GetPedInterface ()->fArmor = fArmor;
}

float CPedSA::GetOxygenLevel ( void )
{
    return GetPedInterface ()->pPlayerData->m_fBreath;
}

void CPedSA::SetOxygenLevel ( float fOxygen )
{
    GetPedInterface ()->pPlayerData->m_fBreath = fOxygen;
}

void CPedSA::SetIsStanding( bool bStanding )
{
    DWORD dwFunc = FUNC_SetIsStanding;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    bStanding
        call    dwFunc
    }
}

DWORD CPedSA::GetType ( void )
{
    return m_dwType;
}

void CPedSA::SetType ( DWORD m_dwType )
{
    this->m_dwType = m_dwType;
}

DWORD * CPedSA::GetMemoryValue ( DWORD dwOffset )
{
    if ( dwOffset <= SIZEOF_CPLAYERPED )
        return (DWORD *)((DWORD)(this->GetInterface()) + dwOffset);
    else
        return NULL;
}

void CPedSA::RemoveWeaponModel ( int iModel )
{
    DWORD dwFunc = FUNC_RemoveWeaponModel;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iModel
        call    dwFunc
    }
}

void CPedSA::ClearWeapon ( eWeaponType weaponType )
{
    DWORD dwFunc = FUNC_ClearWeapon;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    weaponType
        call    dwFunc
    }
}

CWeapon * CPedSA::GiveWeapon ( eWeaponType weaponType, unsigned int uiAmmo, eWeaponSkill skill )
{
    if ( weaponType != WEAPONTYPE_UNARMED )
    {
        CWeaponInfo* pInfo = pGame->GetWeaponInfo ( weaponType, skill );
        if ( pInfo )
        {
            int iModel = pInfo->GetModel();

            if ( iModel )
            {
                CModelInfo * pWeaponModel = pGame->GetModelInfo ( iModel );
                if ( pWeaponModel )
                {
                    pWeaponModel->Request ( BLOCKING, "CPedSA::GiveWeapon" );
                    pWeaponModel->MakeCustomModel ();
                }
            }
            // If the weapon is satchels, load the detonator too
            if ( weaponType == WEAPONTYPE_REMOTE_SATCHEL_CHARGE )
            {
                /*int iModel = pGame->GetWeaponInfo ( WEAPONTYPE_DETONATOR )->GetModel();
                if ( iModel )
                {
                    CModelInfo * pWeaponModel = pGame->GetModelInfo ( iModel );
                    if ( pWeaponModel )
                    {
                        pWeaponModel->Request ( true, true );
                    }
                }*/
                // Load the weapon and give it properly so getPedWeapon shows the weapon is there.
                GiveWeapon( WEAPONTYPE_DETONATOR, 1, WEAPONSKILL_STD );
            }
        }
    }

    DWORD dwReturn = 0;
    DWORD dwFunc = FUNC_GiveWeapon;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    1
        push    uiAmmo
        push    weaponType
        call    dwFunc
        mov     dwReturn, eax
    }

    CWeapon* pWeapon = GetWeapon ( (eWeaponSlot)dwReturn );

    return pWeapon;
}

CWeapon * CPedSA::GetWeapon ( eWeaponType weaponType )
{
    if ( weaponType < WEAPONTYPE_LAST_WEAPONTYPE )
    {
        CWeapon* pWeapon = GetWeapon ( pGame->GetWeaponInfo ( weaponType )->GetSlot () );
        if ( pWeapon->GetType () == weaponType )
            return pWeapon;
    }
    return NULL;
}

CWeapon * CPedSA::GetWeapon ( eWeaponSlot weaponSlot )
{
    if ( weaponSlot < WEAPONSLOT_MAX )
    {
        return m_pWeapons[weaponSlot];
    }
    else
        return NULL;
}

void CPedSA::ClearWeapons ( void )
{
    // Remove all the weapons
    for ( unsigned int i = 0; i < WEAPONSLOT_MAX; i++ )
    {
        if ( m_pWeapons [i] ) 
        {
            m_pWeapons [i]->SetAmmoInClip ( 0 );
            m_pWeapons [i]->SetAmmoTotal ( 0 );
            m_pWeapons [i]->Remove ();            
        }
    }
}

void CPedSA::RestoreLastGoodPhysicsState ( void )
{
    CPhysicalSA::RestoreLastGoodPhysicsState ();
    SetCurrentRotation ( 0 );
    SetTargetRotation ( 0 );
}

FLOAT CPedSA::GetCurrentRotation()
{
    return GetPedInterface ()->fCurrentRotation;
}

FLOAT CPedSA::GetTargetRotation()
{
    return GetPedInterface ()->fTargetRotation;
}

void CPedSA::SetCurrentRotation(FLOAT fRotation)
{
    GetPedInterface ()->fCurrentRotation = fRotation;
    
//  char szDebug[255] = {'\0'};
//  sprintf(szDebug,"CurrentRotate Offset: %d", ((DWORD)&((CPedSAInterface *)this->GetInterface())->CurrentRotate) -  (DWORD)this->GetInterface());
//  OutputDebugString(szDebug);
}

void CPedSA::SetTargetRotation(FLOAT fRotation)
{
    GetPedInterface ()->fTargetRotation = fRotation;
}

eWeaponSlot CPedSA::GetCurrentWeaponSlot ()
{
    return (eWeaponSlot) GetPedInterface ()->bCurrentWeaponSlot;
}

void CPedSA::SetCurrentWeaponSlot ( eWeaponSlot weaponSlot )
{
    if ( weaponSlot < WEAPONSLOT_MAX )
    {
        eWeaponSlot currentSlot = GetCurrentWeaponSlot ();
        if ( weaponSlot != GetCurrentWeaponSlot () )
        {
            CWeapon * pWeapon = GetWeapon ( currentSlot );
            if ( pWeapon ) RemoveWeaponModel ( pWeapon->GetInfo ( WEAPONSKILL_STD )->GetModel () );

            CPedSAInterface * thisPed = (CPedSAInterface *)this->GetInterface();
         
            // set the new weapon slot
            thisPed->bCurrentWeaponSlot = weaponSlot;

            // is the player the local player?
            CPed * pPed = pGame->GetPools()->GetPedFromRef ( (DWORD)1 );
            //if ( pPed == this && thisPed->pPlayerInfo )
            //{
            
            DWORD dwThis = (DWORD)this->GetInterface();
            if ( pPed == this )
            {
               ((CPlayerInfoSA *)pGame->GetPlayerInfo())->GetInterface()->PlayerPedData.m_nChosenWeapon = weaponSlot;

                DWORD dwFunc = FUNC_MakeChangesForNewWeapon_Slot;
                _asm
                {
                    mov     ecx, dwThis
                    push    weaponSlot
                    call    dwFunc
                }
            }
            else
            {
                DWORD dwFunc = FUNC_SetCurrentWeapon;
                _asm
                {
                    mov     ecx, dwThis
                    push    weaponSlot
                    call    dwFunc
                }
            }
        }
    }
}

CVector g_vecBonePosition;
int g_iBoneCallType = 0;

CVector * CPedSA::GetBonePosition ( eBone bone, CVector * vecPosition )
{
    DWORD dwFunc = FUNC_GetBonePosition;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        push    1
        push    bone
        push    vecPosition
        mov     ecx, dwThis
        call    dwFunc
    }

    // Clamp to a sane range as this function can occasionally return massive values,
    // which causes ProcessLineOfSight to effectively freeze
    if ( !IsValidPosition( *vecPosition ) )
        *vecPosition = *GetPosition();

    g_vecBonePosition = *vecPosition;
    g_iBoneCallType = 1;
    return vecPosition;
}

CVector * CPedSA::GetTransformedBonePosition ( eBone bone, CVector * vecPosition )
{
    DWORD dwFunc = FUNC_GetTransformedBonePosition;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        push    1
        push    bone
        push    vecPosition
        mov     ecx, dwThis
        call    dwFunc
    }

    // Clamp to a sane range as this function can occasionally return massive values,
    // which causes ProcessLineOfSight to effectively freeze
    if ( !IsValidPosition( *vecPosition ) )
        *vecPosition = *GetPosition();

    g_vecBonePosition = *vecPosition;
    g_iBoneCallType = 2;
    return vecPosition;
}

bool CPedSA::IsDucking ( void )
{
    return ((CPedSAInterface*)this->GetInterface())->pedFlags.bIsDucking;
}

void CPedSA::SetDucking ( bool bDuck )
{
    ((CPedSAInterface*)this->GetInterface())->pedFlags.bIsDucking = bDuck;
}

int CPedSA::GetCantBeKnockedOffBike ( void )
{
    return ((CPedSAInterface*)this->GetInterface())->pedFlags.CantBeKnockedOffBike;
}

void CPedSA::SetCantBeKnockedOffBike ( int iCantBeKnockedOffBike )
{
    ((CPedSAInterface*)this->GetInterface())->pedFlags.CantBeKnockedOffBike = iCantBeKnockedOffBike;
}

void CPedSA::QuitEnteringCar ( CVehicle * vehicle, int iSeat, bool bUnknown )
{
    CVehicleSA* pVehicleSA = dynamic_cast < CVehicleSA* > ( vehicle );
    if ( !pVehicleSA ) return;

    DWORD dwFunc = FUNC_QuitEnteringCar;
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwVehicle = (DWORD)pVehicleSA->GetInterface();
    _asm
    {
        push    bUnknown
        push    iSeat
        push    dwVehicle
        push    dwThis
        call    dwFunc
        add     esp, 16
    }
}

bool CPedSA::IsWearingGoggles ( void )
{
    DWORD dwFunc = FUNC_IsWearingGoggles;
    DWORD dwThis = (DWORD)this->GetInterface();
    bool bReturn = false;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CPedSA::SetGogglesState ( bool bIsWearingThem )
{
    DWORD dwFunc = FUNC_TakeOffGoggles;
    if ( bIsWearingThem )
        dwFunc = FUNC_PutOnGoggles;

    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CPedSA::SetClothesTextureAndModel ( const char* szTexture, const char* szModel, int textureType )
{
    DWORD dwFunc = FUNC_CPedClothesDesc__SetTextureAndModel;
    //DWORD dwThis = (DWORD)this->GetInterface()->PlayerPedData.m_pClothes;
    DWORD dwThis = (DWORD)((CPedSAInterface *)this->GetInterface())->pPlayerData->m_pClothes;
    _asm
    {
        mov     ecx, dwThis
        push    textureType
        push    szModel
        push    szTexture
        call    dwFunc
    }
}

void CPedSA::RebuildPlayer ( void )
{
    DWORD dwFunc = FUNC_CClothes__RebuildPlayer;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        push    0
        push    dwThis
        call    dwFunc
        add     esp, 8
    }
}


eFightingStyle CPedSA::GetFightingStyle ( void )
{
    return ( eFightingStyle ) ((CPedSAInterface *)this->GetInterface())->bFightingStyle;
}


void CPedSA::SetFightingStyle ( eFightingStyle style, BYTE bStyleExtra )
{
    BYTE bStyle = ( BYTE ) style;
    BYTE* pFightingStyle = &((CPedSAInterface *)this->GetInterface())->bFightingStyle;
    BYTE* pFightingStyleExtra = &((CPedSAInterface *)this->GetInterface())->bFightingStyleExtra;
    if ( bStyle != *pFightingStyle )
    {
        *pFightingStyle = bStyle;
        if ( bStyleExtra > 0 && bStyleExtra <= 6 )
        {
            bStyleExtra--;
            BYTE bTemp = *pFightingStyleExtra;
            switch ( bStyleExtra )
            {
                case 0:
                    break;
                case 1:
                    bTemp |= 1;
                    break;
                case 2:
                    bTemp |= 2;
                    break;
                case 3:
                    bTemp |= 4;
                    break;
                case 4:
                    bTemp |= 8;
                    break;
                case 5:
                    bTemp |= 16;
                    break;
            }
            *pFightingStyleExtra = bTemp;
        }
    }
}


CEntity* CPedSA::GetContactEntity ( void )
{
    CEntitySAInterface* pInterface = ((CPedSAInterface *)this->GetInterface())->pContactEntity;
    CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
    CEntity * pReturn = NULL;

    if ( pPools && pInterface )
    {
        switch ( pInterface->nType )
        {
            case ENTITY_TYPE_VEHICLE:
                pReturn = (CEntity*)(pPools->GetVehicle((DWORD *)pInterface));
                break;
            case ENTITY_TYPE_OBJECT:
                pReturn = (CEntity*)(pPools->GetObject ((DWORD *)pInterface));
                break;
            default:
                break;
        }
    }
    return pReturn;
}

unsigned char CPedSA::GetRunState ( void )
{
    return * ( unsigned char * ) ( ( (DWORD)(this->GetInterface()) + 1332 ) );
}


CEntity* CPedSA::GetTargetedEntity ( void )
{
    CEntitySAInterface* pInterface = ((CPedSAInterface *)this->GetInterface())->pTargetedEntity;
    CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
    CEntity * pReturn = NULL;

    if ( pPools && pInterface )
    {
        switch ( pInterface->nType )
        {
            case ENTITY_TYPE_PED:
                pReturn = (CEntity*)(pPools->GetPed((DWORD *)pInterface));
                break;
            case ENTITY_TYPE_VEHICLE:
                pReturn = (CEntity*)(pPools->GetVehicle((DWORD *)pInterface));
                break;
            case ENTITY_TYPE_OBJECT:
                pReturn = (CEntity*)(pPools->GetObject ((DWORD *)pInterface));
                break;
            default:
                break;
        }
    }
    return pReturn;
}


void CPedSA::SetTargetedEntity ( CEntity* pEntity )
{
    CEntitySAInterface* pInterface = NULL;
    if ( pEntity )
    {
        CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );
        if ( pEntitySA )
            pInterface = pEntitySA->GetInterface ();
    }

    ((CPedSAInterface *)this->GetInterface())->pTargetedEntity = pInterface;
}


bool CPedSA::GetCanBeShotInVehicle ( void )
{
    return GetPedInterface ()->pedFlags.bCanBeShotInVehicle;
}


bool CPedSA::GetTestForShotInVehicle ( void )
{
    return GetPedInterface ()->pedFlags.bTestForShotInVehicle;
}


void CPedSA::SetCanBeShotInVehicle ( bool bShot )
{
    GetPedInterface ()->pedFlags.bCanBeShotInVehicle = bShot;
}


void CPedSA::SetTestForShotInVehicle ( bool bTest )
{
    GetPedInterface ()->pedFlags.bTestForShotInVehicle = bTest;
}


void CPedSA::RemoveBodyPart ( int i, char c )
{
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwFunc = FUNC_CPed_RemoveBodyPart;
    _asm
    {
        mov     ecx, dwThis
        push    c
        push    i
        call    dwFunc
    }
}

void CPedSA::SetFootBlood ( unsigned int uiFootBlood )
{
    DWORD dwThis = (DWORD)this->GetInterface();
    // Check if the ped is to have foot blood
    if (uiFootBlood > 0)
    {
        // Make sure the foot blood flag is activated
        MemOrFast < unsigned short > ( dwThis + 0x46F, 16 );
    }
    else if (*(unsigned short*)(dwThis + 0x46F) & 16)
    {
        // If the foot blood flag is activated, deactivate it
        MemSubFast < unsigned short > ( dwThis + 0x46F, 16 );
    }
    // Set the amount of foot blood
    MemPutFast < unsigned int > ( dwThis + 0x750, uiFootBlood );
}

unsigned int CPedSA::GetFootBlood ( void )
{
    DWORD dwThis = (DWORD)this->GetInterface();
    // Check if the ped has the foot blood flag
    if (*(unsigned short*)(dwThis + 0x46F) & 16)
    {
        // If the foot blood flag is activated, return the amount of foot blood
        return *(unsigned int*)(dwThis + 0x750);
    }
    // Otherwise, return zero as there is no foot blood
    return 0;
}

bool CPedSA::IsOnFire ( void )
{
    if ( GetPedInterface()->pFireOnPed != NULL )
        return true;
    return false;
}

void CPedSA::SetOnFire ( bool bOnFire )
{
    CPedSAInterface* pInterface = GetPedInterface();

    if ( bOnFire )
    {
        // If we are already on fire, don't apply a new fire
        if ( pInterface->pFireOnPed == NULL )
        {
            CFireManagerSA* pFireManager = static_cast < CFireManagerSA * > ( pGame->GetFireManager() );
            CFire* pFire = pFireManager->StartFire ( this, NULL, (float)DEFAULT_FIRE_PARTICLE_SIZE );

            if ( pFire )
            {
                // Start the fire
                pFire->SetTarget( this );
                pFire->Ignite();
                pFire->SetStrength( 1.0f );
                // Attach the fire only to the player, do not let it
                // create child fires when moving.
                pFire->SetNumGenerationsAllowed( 0 );
                pInterface->pFireOnPed = pFire->GetInterface();
            }
        }
    }
    else
    {
        // Make sure that we have some attached fire
        if ( pInterface->pFireOnPed != NULL )
        {
            CFireManagerSA* pFireManager = static_cast < CFireManagerSA * > ( pGame->GetFireManager() );
            CFire* pFire = pFireManager->GetFire( static_cast < CFireSAInterface * > ( pInterface->pFireOnPed ) );

            if ( pFire )
            {
                pFire->Extinguish();
            }
        }
    }
}


void CPedSA::SetStayInSamePlace ( bool bStay )
{
    GetPedInterface ()->pedFlags.bStayInSamePlace = bStay;
}

void CPedSA::GetVoice ( short* psVoiceType, short* psVoiceID )
{
    if ( psVoiceType )
        *psVoiceType = m_pPedSound->GetVoiceTypeID ();
    if ( psVoiceID )
        *psVoiceID = m_pPedSound->GetVoiceID ();
}

void CPedSA::GetVoice ( const char** pszVoiceType, const char** pszVoice )
{
    short sVoiceType, sVoiceID;
    GetVoice ( &sVoiceType, &sVoiceID );
    if ( pszVoiceType )
        *pszVoiceType = CPedSoundSA::GetVoiceTypeNameFromID ( sVoiceType );
    if ( pszVoice )
        *pszVoice = CPedSoundSA::GetVoiceNameFromID ( sVoiceType, sVoiceID );
}

void CPedSA::SetVoice ( short sVoiceType, short sVoiceID )
{
    m_pPedSound->SetVoiceTypeID ( sVoiceType );
    m_pPedSound->SetVoiceID ( sVoiceID );
}

void CPedSA::SetVoice ( const char* szVoiceType, const char* szVoice )
{
    short sVoiceType = CPedSoundSA::GetVoiceTypeIDFromName ( szVoiceType );
    if ( sVoiceType < 0 )
        return;
    short sVoiceID = CPedSoundSA::GetVoiceIDFromName ( sVoiceType, szVoice );
    if ( sVoiceID < 0 )
        return;
    SetVoice ( sVoiceType, sVoiceID );
}

// GetCurrentWeaponStat will only work if the game ped context is currently set to this ped
CWeaponStat* CPedSA::GetCurrentWeaponStat ( void )
{
    if ( pGame->GetPedContext () != this )
    {
        OutputDebugLine ( "WARNING: GetCurrentWeaponStat ped context mismatch" );
        return NULL;
    }

    CWeapon* pWeapon = GetWeapon ( GetCurrentWeaponSlot () );

    if ( !pWeapon )
        return NULL;

    eWeaponType eWeapon = pWeapon->GetType ();
    ushort usStat = pGame->GetStats ()->GetSkillStatIndex ( eWeapon );
    float fSkill = pGame->GetStats()->GetStatValue ( usStat );
    CWeaponStat* pWeaponStat = pGame->GetWeaponStatManager ()->GetWeaponStatsFromSkillLevel ( eWeapon, fSkill );
    return pWeaponStat;
}

float CPedSA::GetCurrentWeaponRange ( void )
{
    CWeaponStat* pWeaponStat = GetCurrentWeaponStat ();
    if ( !pWeaponStat )
        return 1;

    return pWeaponStat->GetWeaponRange ();
}

void CPedSA::AddWeaponAudioEvent ( EPedWeaponAudioEventType audioEventType )
{
    DWORD dwFunc = FUNC_CAEPedWeaponAudioEntity__AddAudioEvent;
    CPedWeaponAudioEntitySAInterface* pThis = &GetPedInterface ()->weaponAudioEntity;
    _asm
    {
        mov     ecx, pThis
        push    audioEventType
        call    dwFunc
    }
}

int CPedSA::GetCustomMoveAnim( void )
{
    return m_iCustomMoveAnim;
}

/*
bool CPedSA::CanPedReturnToState (  )
{
    DWORD dwFunction = FUNC_CanPedReturnToState;
    DWORD dwThis = (DWORD)this->GetInterface();
    bool bReturn;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
        movzx   eax, al
        mov     bReturn, eax
    }
    return bReturn;
}*/
/*
bool CPedSA::CanSeeEntity ( CEntity * entity, FLOAT fUnknown )
{
    DWORD dwFunction = FUNC_CanSeeEntity;
    DWORD dwThis = (DWORD)this->GetInterface();
    bool bReturn;
    _asm
    {
        mov     ecx, dwThis
        push    fUnknown
        push    entity
        call    dwFunction
        movzx   eax, al
        mov     bReturn, eax
    }
    return bReturn;
}


void CPedSA::ClearAimFlag (  )
{
    DWORD dwFunction = FUNC_ClearAimFlag;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::ClearAll (  )
{
    DWORD dwFunction = FUNC_ClearAll;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::ClearAttackByRemovingAnim (  )
{
    DWORD dwFunction = FUNC_ClearAttackByRemovingAnim;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::ClearInvestigateEvent (  )
{
    DWORD dwFunction = FUNC_ClearInvestigateEvent;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::ClearLookFlag (  )
{
    DWORD dwFunction = FUNC_ClearLookFlag;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::ClearObjective (  )
{
    DWORD dwFunction = FUNC_ClearObjective;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::ClearPointGunAt (  )
{
    DWORD dwFunction = FUNC_ClearPointGunAt;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::Clearm_pWeapons (  )
{
    DWORD dwFunction = FUNC_Clearm_pWeapons;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::CreateDeadPedMoney (  )
{
    DWORD dwFunction = FUNC_CreateDeadPedMoney;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::CreateDeadPedWeaponPickups (  )
{
    DWORD dwFunction = FUNC_CreateDeadPedWeaponPickups;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::GetLocalDirection ( CVector2D * vector )
{
    DWORD dwFunction = FUNC_GetLocalDirection;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    vector
        call    dwFunction
    }
}

CWeapon * CPedSA::GiveWeapon ( eWeaponType weapon, DWORD dwAmmo ) // returns weapon slot (check)
{
    DWORD dwFunction = FUNC_GiveWeapon;
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        push    1
        push    dwAmmo
        push    weapon
        call    dwFunction
        mov     dwReturn, eax
    }
    return this->GetWeapon((eWeaponSlot)dwReturn);
}

CWeapon * CPedSA::GetWeapon ( eWeaponSlot weaponSlot )
{
    // we make it appear that the weapon is non existant if its empty
    if(weaponSlot < MAX_WEAPONS && m_pWeapons[weaponSlot]->GetType() != WEAPONTYPE_UNARMED)
        return m_pWeapons[weaponSlot];
    else
        return NULL;
}
*/
/**
 * \todo Remove this function (or make it work by overloading in CPlayerPed with converter)
 */
/*
void CPedSA::SetCurrentWeapon ( eWeaponType weapon )
{
    DWORD dwFunction = FUNC_SetCurrentWeaponFromID;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    weapon
        call    dwFunction  
    }
}
*/
/**
 * This is overloaded by CPlayerPed
 */
/*
void CPedSA::SetCurrentWeapon ( eWeaponSlot slot )
{
    DWORD dwFunction = FUNC_SetCurrentWeaponFromSlot;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    slot
        call    dwFunction
    }
}

CWeapon * CPedSA::GetCurrentWeapon (  )
{
    return this->m_pWeapons[((CPedSAInterface *)this->GetInterface())->CurrentWeaponSlot];
}

bool CPedSA::IsPedInControl (  )
{
    DWORD dwFunction = FUNC_IsPedInControl;
    DWORD dwThis = (DWORD)this->GetInterface();
    bool bReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
        movzx   eax, al
        mov     bReturn, eax
    }
    return bReturn;
}

bool CPedSA::IsPedShootable (  )
{
    DWORD dwFunction = FUNC_IsPedShootable;
    DWORD dwThis = (DWORD)this->GetInterface();
    bool bReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
        movzx   eax, al
        mov     bReturn, eax
    }
    return bReturn;
}

void CPedSA::SetAttackTimer ( DWORD dwTimer )
{
    DWORD dwFunction = FUNC_SetAttackTimer;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    dwTimer
        call    dwFunction
    }
}

void CPedSA::SetDead (  )
{
    DWORD dwFunction = FUNC_SetDead;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::SetDie ( DWORD AnimationID, FLOAT fUnknown1, FLOAT fUnknown2 )
{
    DWORD dwFunction = FUNC_SetDie;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    fUnknown2
        push    fUnknown1
        push    AnimationID
        call    dwFunction
    }
}

void CPedSA::SetEvasiveDive ( CPhysical * avoid, BYTE bUnknown )
{
    DWORD dwFunction = FUNC_SetEvasiveDive;
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwUnk = bUnknown;
    _asm
    {
        mov     ecx, dwThis
        push    dwUnk
        push    avoid
        call    dwFunction
    }
}

void CPedSA::SetEvasiveStep ( CPhysical * avoid, BYTE bUnknown )
{
    DWORD dwFunction = FUNC_SetEvasiveStep;
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwUnk = bUnknown;
    _asm
    {
        mov     ecx, dwThis
        push    dwUnk
        push    avoid
        call    dwFunction
    }
}

void CPedSA::SetFall ( DWORD dwUnknown, DWORD AnimationID, BYTE bUnknown )
{
    DWORD dwFunction = FUNC_SetFall;
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwUnk = bUnknown;
    _asm
    {
        mov     ecx, dwThis
        push    dwUnk
        push    AnimationID
        push    dwUnknown
        call    dwFunction
    }
}

void CPedSA::SetFlee ( CVector2D * vecPoint, DWORD dwDuration )
{
    DWORD dwFunction = FUNC_SetFlee;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    dwDuration
        push    vecPoint
        call    dwFunction
    }
}

void CPedSA::SetIdle (  )
{
    DWORD dwFunction = FUNC_SetIdle;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::SetLeader ( CPed * leader )
{
    DWORD dwFunction = FUNC_SetLeader;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    leader
        call    dwFunction
    }
}

void CPedSA::SetLookFlag ( CEntity * lookat, bool bIgnoreCurrentLook ) // confirm bool
{
    DWORD dwFunction = FUNC_SetLookFlag;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    bIgnoreCurrentLook
        push    lookat
        call    dwFunction
    }
}

void CPedSA::SetLookTimer ( DWORD dwLookTime )
{
    DWORD dwFunction = FUNC_SetLookTimer;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    dwLookTime
        call    dwFunction
    }
}

void CPedSA::SetMoveState ( eMoveState movestate )
{
    DWORD dwFunction = FUNC_SetMoveState;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    movestate
        call    dwFunction
    }
}

eObjective CPedSA::GetObjective (  )
{
    return ((CPedSAInterface *)this->GetInterface())->Objective;
}
*/
/**
 * Return a CVehicle or CPed for the entity thats this ped's objective
 * \todo add CObjects
 */
/*
CEntity * CPedSA::GetObjectiveEntity (  )
{
    CEntitySAInterface * entityInterface = ((CPedSAInterface *)this->GetInterface())->ObjectiveEntity;
    if(entityInterface)
    {
        CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
        if(pPools)
        {
            CEntitySA * entity = new CEntitySA();
            if(entity)
            {
                entity->SetInterface(entityInterface);
                eEntityType EntityType = entity->GetEntityType();
                delete entity;
                switch(EntityType)
                {
                case ENTITY_TYPE_PED:
                    return pPools->GetPed(pPools->GetPedRef ((DWORD)entityInterface));
                case ENTITY_TYPE_VEHICLE:
                    return pPools->GetVehicle((CVehicle *)entityInterface);
                }
            }
        }
    }
    return NULL;    
}

void CPedSA::GetObjectiveVector ( CVector * vector )
{
    MemCpyFast (vector,&(((CPedSAInterface *)this->GetInterface())->ObjectiveVector),sizeof(CVector));
}
*/
/*  CPedSAInterface * pedInterface = ((CPedSAInterface *)this->GetInterface());
    pedInterface->Objective = objective;
    CEntitySA * entityVC = static_cast<CEntitySA *>(entity);
    CEntitySAInterface * entityInterface = entityVC->GetInterface();
    pedInterface->ObjectiveEntity = entityInterface;*/
/*
void CPedSA::SetObjective ( eObjective  objective, CVehicle * vehicle )
{
    char szDebug[255] = {'\0'};
    sprintf(szDebug, "Objective: %d, Vehicle: %d", objective, vehicle);
    OutputDebugString(szDebug);
    DWORD dwFunction = FUNC_SetObjective_ENTITY;
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwEntity = (DWORD)((CVehicleSA *)vehicle)->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    dwEntity
        push    objective
        call    dwFunction
    }
}

void CPedSA::SetObjective ( eObjective  objective, CPed * ped )
{
    DWORD dwFunction = FUNC_SetObjective_ENTITY;
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwEntity = (DWORD)((CPedSA *)ped)->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    dwEntity
        push    objective
        call    dwFunction
    }
}

void CPedSA::SetObjective ( eObjective  objective )
{
    DWORD dwFunction = FUNC_SetObjective;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    objective
        call    dwFunction
    }
}

void CPedSA::SetObjective ( eObjective  objective, CVector * vecPoint )
{
    DWORD dwFunction = FUNC_SetObjective_VECTOR;
    DWORD dwThis = (DWORD)this->GetInterface();
    FLOAT fx, fy, fz;
    fx = ((CVector *)vecPoint)->fX;
    fy = ((CVector *)vecPoint)->fY;
    fz = ((CVector *)vecPoint)->fZ;
    _asm
    {
        mov     ecx, dwThis
        push    fz
        push    fy
        push    fx
        push    objective
        call    dwFunction
    }
}

void CPedSA::SetSeekCar ( CVehicle * vehicle )
{
    DWORD dwFunction = FUNC_SetSeekCar;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    vehicle
        call    dwFunction
    }
}

void CPedSA::SetShootTimer ( DWORD dwTime )
{
    DWORD dwFunction = FUNC_SetShootTimer;
    DWORD dwThis = (DWORD)this;
    _asm
    {
        mov     ecx, dwThis
        push    dwTime
        call    dwFunction
    }
}

void CPedSA::SetSolicit (  )
{
    DWORD dwFunction = FUNC_SetSolicit;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::SetStoredState (  )
{
    DWORD dwFunction = FUNC_SetStoredState;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::RestorePreviousState (  )
{
    DWORD dwFunction = FUNC_RestorePreviousState;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunction
    }
}

void CPedSA::SetWaitState ( eWaitState waitstate )
{
    DWORD dwFunction = FUNC_SetWaitState;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    waitstate
        call    dwFunction
    }
}

void CPedSA::Teleport ( CVector * vecPoint )
{
    DWORD dwFunction = FUNC_Teleport;
    DWORD dwThis = (DWORD)this->GetInterface();
    FLOAT fx, fy, fz;
    fx = ((CVector *)vecPoint)->fX;
    fy = ((CVector *)vecPoint)->fY;
    fz = ((CVector *)vecPoint)->fZ;
    _asm
    {
        mov     ecx, dwThis
        push    fz
        push    fy
        push    fx
        call    dwFunction
    }
}

void CPedSA::WarpPedIntoCar ( CVehicle * vehicle )
{*/
    /*DWORD dwFunction = FUNC_WarpPedIntoCar;
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwVehicle = (DWORD)((CVehicleSA *)vehicle)->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    dwVehicle
        call    dwFunction
    }*/



/*  DWORD dwFunc = 0x650280;    // CCarEnterExit::SetPedInCarDirect
    DWORD vehicleInterface = (DWORD)((CVehicleSA *)vehicle)->GetInterface();
    DWORD pedInterface = (DWORD)this->GetInterface();
    _asm
    {
        push    1
        push    0
        push    vehicleInterface
        push    pedInterface
        call    dwFunc
        add     esp, 16
    }*/
/*
    // CODE BELOW IS SA but crashes for non player CPlayerPeds without CPlayerInfos (multiplayer hook required)
    DWORD dwFunc = 0x6470E0; // CTaskSimpleCarSetPedInAsDriver
    DWORD dwVehicle = (DWORD)((CVehicleSA *)vehicle)->GetInterface();
    DWORD dwTask[50];
    MemSetFast (dwTask, 0, 50);
    _asm
    {
        pushad
        lea     ecx, dwTask
        push    0           // seat
        push    dwVehicle   // vehicle
        call    dwFunc
        popad
    }
    
    dwFunc = 0x64B950; // CTaskSimpleCarSetPedInAsDriver::ProcessPed
    DWORD dwPed = (DWORD)this->GetInterface();
    _asm
    {
        lea     ecx, dwTask
        push    dwPed   // ped
        call    dwFunc
    }
}

FLOAT CPedSA::GetHealth ( )
{
    CPedSAInterface * ped = (CPedSAInterface *)this->GetInterface();
    return ped->Health;
}

void CPedSA::SetHealth ( FLOAT fHealth )
{
    CPedSAInterface * ped = (CPedSAInterface *)this->GetInterface();
    ped->Health = fHealth;
}

CVehicle * CPedSA::GetCurrentVehicle (  )
{
    CPedSAInterface * ped = (CPedSAInterface *)this->GetInterface();
    if(this->IsInVehicle())
    {
        CPoolsSA * pools = (CPoolsSA *)pGame->GetPools();
        CVehicleSAInterface * vehicleInterface = ped->pMyVehicle;

        if(vehicleInterface)
        {
            CVehicle * vehicle = pools->GetVehicle((CVehicle *)vehicleInterface);

            if(vehicle && vehicle->IsBeingDriven())
                return vehicle;
        }
    }
    return NULL;
}

bool CPedSA::IsInVehicle()
{
    CPedSAInterface * ped = (CPedSAInterface *)this->GetInterface();
    if(ped->InVehicle)
        return TRUE;
    else
        return FALSE;
}

*/

/**
 * Set this ped as the ped thats the player, used generally for keysync
 */
/*
void CPedSA::SetAsActivePed()
{
    MemPut < DWORD > ( VAR_LocalPlayer, (DWORD)this->GetInterface() );
}

void CPedSA::SetPedState(ePedState PedState)
{
    ((CPedSAInterface *)this->GetInterface())->PedState = (ePedStateVC)PedState;
}

ePedState CPedSA::GetPedState()
{
    return (ePedState)((CPedSAInterface *)this->GetInterface())->PedState;
}*/
