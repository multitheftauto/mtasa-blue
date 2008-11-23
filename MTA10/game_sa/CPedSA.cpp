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

CPedSA::CPedSA (  )
{
	DEBUG_TRACE("CPedSA::CPedSA(  )");
    this->PedIntelligence = NULL;
    m_pPedInterface = NULL;
}

CPedSA::CPedSA( CPedSAInterface * pedInterface )
{
	DEBUG_TRACE("CPedSA::CPedSA( CPedSAInterface * pedInterface )");
    this->PedIntelligence = NULL;
	this->m_pInterface = pedInterface;
}

CPedSA::~CPedSA ( void )
{
    if ( this->PedIntelligence ) delete this->PedIntelligence;
}

// used to init weapons at the moment, called by CPlayerPedSA when its been contstructed
// can't use constructor as thats called before the interface pointer has been set the the aforementioned constructor
/**
 * \todo Reimplement weapons and PedIK for SA
 */
VOID CPedSA::Init()
{
	DEBUG_TRACE("VOID CPedSA::Init()");
	CPedSAInterface * pedInterface = GetPedInterface ();
    DWORD dwPedInteligence = 0;
    DWORD dwFunc = 0x411DE0;
    _asm
    {
        mov     ecx, pedInterface
        call    dwFunc
        mov     dwPedInteligence, eax
    }
    CPedIntelligenceSAInterface * pedIntelligenceInterface = (CPedIntelligenceSAInterface *)(dwPedInteligence);
	this->PedIntelligence = new CPedIntelligenceSA(pedIntelligenceInterface, this);
	
    for ( int i = 0; i < WEAPONSLOT_MAX; i++ )
		Weapons[i] = new CWeaponSA(&(pedInterface->Weapons[i]), this, (eWeaponSlot)i);
	//this->PedIK = new CPedIKSA(&(pedInterface->pedIK));
}


VOID CPedSA::SetModelIndex ( DWORD dwModelIndex )
{
	DEBUG_TRACE("VOID CPedSA::SetModelIndex ( DWORD dwModelIndex )");
	DWORD dwFunction = FUNC_SetModelIndex;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	dwModelIndex
		call	dwFunction
	}
}

bool CPedSA::IsInWater ( void )
{
	DEBUG_TRACE("BOOL CPedSA::IsInWater ()");
	CTask *pTask = this->PedIntelligence->GetTaskManager ()->GetTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
	return ( pTask && ( pTask->GetTaskType () == TASK_COMPLEX_IN_WATER ) );
}

VOID CPedSA::AttachPedToBike(CEntity * entity, CVector * vector, unsigned short sUnk, FLOAT fUnk, FLOAT fUnk2, eWeaponType weaponType)
{
	DEBUG_TRACE("VOID CPedSA::AttachPedToBike(CEntity * entity, CVector * vector, unsigned short sUnk, FLOAT fUnk, FLOAT fUnk2, eWeaponType weaponType)");

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
		push	weaponType
		push	fUnk2
		push	fUnk
		push	sUnk
		push	fZ
		push	fY
		push	fX
        mov     ecx, dwThis
		push	dwEntityInterface
		call	dwFunc
	}	
}

BOOL CPedSA::AddProjectile ( eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity )
{
    return ((CProjectileInfoSA*)pGame->GetProjectileInfo())->AddProjectile ( (CEntitySA*)this, eWeapon, vecOrigin, fForce, target, targetEntity );
}


VOID CPedSA::DetachPedFromEntity ( void )
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

VOID CPedSA::AttachPedToEntity(DWORD dwEntityInterface, CVector * vector, unsigned short sDirection, FLOAT fRotationLimit, eWeaponType weaponType, BOOL bChangeCamera)
{
    // sDirection and fRotationLimit only apply to first-person shooting (bChangeCamera)
	DEBUG_TRACE("VOID CPedSA::AttachPedToEntity(CVehicle * entity, CVector * vector, unsigned short sUnk, FLOAT fUnk, eWeaponType weaponType)");
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
		push	weaponType
		push	fRotationLimit
        movzx   ecx, sDirection
		push	ecx
		push	fZ
		push	fY
		push	fX
		push	dwEntityInterface
        mov     ecx, dwThis
		call	dwFunc
	}
	
	// Hack the CPed type(?) to whatever it was set to
	if ( !bChangeCamera )
		((CPedSAInterface*)GetInterface())->bPedType = bPedType;
}

BOOL CPedSA::CanSeeEntity(CEntity * entity, FLOAT fDistance)
{
	DEBUG_TRACE("BOOL CPedSA::CanSeeEntity(CEntity * entity, FLOAT fDistance)");

	DWORD dwFunc = FUNC_CanSeeEntity;
	bool bReturn = false;

	CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( entity );
	if ( pEntitySA )
	{
		DWORD dwEntityInterface = (DWORD)pEntitySA->GetInterface();
		_asm
		{
			push	fDistance
			push	dwEntityInterface
			call	dwFunc
			mov		bReturn, al
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
		    return ((CPoolsSA *)pGame->GetPools())->GetVehicle((CVehicleSAInterface *)vehicle);
    }
    return NULL;
}

VOID CPedSA::Respawn(CVector * position, bool bCameraCut)
{
    CPed * pLocalPlayer = pGame->GetPools()->GetPed ( 1 );

    DWORD oldProt, oldProt2;
    if ( !bCameraCut )
    {
         // DISABLE call to CCamera__RestoreWithJumpCut when respawning
        VirtualProtect((LPVOID)0x4422EA,20,PAGE_EXECUTE_READWRITE,&oldProt);		
        memset( (void*)0x4422EA, 0x90, 20 );
        VirtualProtect((LPVOID)0x4422EA,20,oldProt,&oldProt2);
    }

	DEBUG_TRACE("VOID CPedSA::Respawn(CVector * position)");
	FLOAT fX = position->fX;
	FLOAT fY = position->fY;
	FLOAT fZ = position->fZ;
	FLOAT fUnk = 1.0f; 
	DWORD dwFunc = FUNC_RestorePlayerStuffDuringResurrection;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		push	fUnk
		push	fZ
		push	fY
		push	fX
		push	dwThis
		call	dwFunc
		add		esp, 20
	}
	dwFunc = 0x441440; // CGameLogic::SortOutStreamingAndMemory
	fUnk = 10.0f;
	_asm
	{
		push	fUnk
		push	position
		call	dwFunc
		add		esp, 8
	}

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
        VirtualProtect((LPVOID)0x4422EA,20,PAGE_EXECUTE_READWRITE,&oldProt);		
        memcpy( (void*)0x4422EA, szCode, 20 );
        VirtualProtect((LPVOID)0x4422EA,20,oldProt,&oldProt2);
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

VOID CPedSA::SetIsStanding( bool bStanding )
{
	DWORD dwFunc = FUNC_SetIsStanding;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	bStanding
		call	dwFunc
	}
}

CPedIntelligence * CPedSA::GetPedIntelligence ( void )
{
	return PedIntelligence;
}

DWORD CPedSA::GetType ( void )
{
	return dwType;
}

VOID CPedSA::SetType ( DWORD dwType )
{
	this->dwType = dwType;
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

CWeapon * CPedSA::GiveWeapon ( eWeaponType weaponType, unsigned int uiAmmo )
{
    if ( weaponType != WEAPONTYPE_UNARMED )
    {
        CWeaponInfo* pInfo = pGame->GetWeaponInfo ( weaponType );
        if ( pInfo )
        {
            int iModel = pInfo->GetModel();
            if ( iModel )
            {
                CModelInfo * pWeaponModel = pGame->GetModelInfo ( iModel );
                if ( pWeaponModel )
                {
                    pWeaponModel->Request ( true, true );
                }
            }
            // If the weapon is satchels, load the detonator too
            if ( weaponType == WEAPONTYPE_REMOTE_SATCHEL_CHARGE )
            {
                int iModel = pGame->GetWeaponInfo ( WEAPONTYPE_DETONATOR )->GetModel();
                if ( iModel )
                {
                    CModelInfo * pWeaponModel = pGame->GetModelInfo ( iModel );
                    if ( pWeaponModel )
                    {
                        pWeaponModel->Request ( true, true );
                    }
                }
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

    return GetWeapon ( (eWeaponSlot) dwReturn );
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
        return Weapons[weaponSlot];
    }
    else
        return NULL;
}

void CPedSA::ClearWeapons ( void )
{
    // Remove all the weapons
    for ( unsigned int i = 0; i < WEAPONSLOT_MAX; i++ )
    {
        if ( Weapons [i] ) 
        {
            Weapons [i]->SetAmmoInClip ( 0 );
            Weapons [i]->SetAmmoTotal ( 0 );
            Weapons [i]->Remove ();            
        }
    }
}

FLOAT CPedSA::GetCurrentRotation()
{
	return GetPedInterface ()->fCurrentRotation;
}

FLOAT CPedSA::GetTargetRotation()
{
	return GetPedInterface ()->fTargetRotation;
}

VOID CPedSA::SetCurrentRotation(FLOAT fRotation)
{
	GetPedInterface ()->fCurrentRotation = fRotation;
	
//	char szDebug[255] = {'\0'};
//	sprintf(szDebug,"CurrentRotate Offset: %d", ((DWORD)&((CPedSAInterface *)this->GetInterface())->CurrentRotate) -  (DWORD)this->GetInterface());
//	OutputDebugString(szDebug);
}

VOID CPedSA::SetTargetRotation(FLOAT fRotation)
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
        CPedSAInterface * thisPed = (CPedSAInterface *)this->GetInterface();
     
        // set the new weapon slot
        thisPed->bCurrentWeaponSlot = weaponSlot;

        // is the player the local player?
        CPed * pPed = pGame->GetPools()->GetPed ( 1 );
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

VOID CPedSA::SetClothesTextureAndModel ( char * szTexture, char * szModel, int textureType )
{
	DWORD dwFunc = FUNC_CPedClothesDesc__SetTextureAndModel;
	//DWORD dwThis = (DWORD)this->GetInterface()->PlayerPedData.m_pClothes;
    DWORD dwThis = (DWORD)((CPedSAInterface *)this->GetInterface())->pPlayerData->m_pClothes;
	_asm
	{
		mov		ecx, dwThis
		push	textureType
		push	szModel
		push	szTexture
		call	dwFunc
	}
}

VOID CPedSA::RebuildPlayer ( void )
{
	DWORD dwFunc = FUNC_CClothes__RebuildPlayer;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		push	0
		push	dwThis
		call	dwFunc
		add		esp, 8
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
				pReturn = (CEntity*)(pPools->GetVehicle((CVehicleSAInterface *)pInterface));
				break;
            case ENTITY_TYPE_OBJECT:
                pReturn = (CEntity*)(pPools->GetObject ((CObjectSAInterface *)pInterface));
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
				pReturn = (CEntity*)(pPools->GetVehicle((CVehicleSAInterface *)pInterface));
				break;
            case ENTITY_TYPE_OBJECT:
                pReturn = (CEntity*)(pPools->GetObject ((CObjectSAInterface *)pInterface));
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
    GetPedInterface()->pedFlags.bStayInSamePlace = bStay;
}

/*
BOOL CPedSA::CanPedReturnToState (  )
{
	DWORD dwFunction = FUNC_CanPedReturnToState;
	DWORD dwThis = (DWORD)this->GetInterface();
	BOOL bReturn;
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
		movzx	eax, al
		mov		bReturn, eax
	}
	return bReturn;
}*/
/*
BOOL CPedSA::CanSeeEntity ( CEntity * entity, FLOAT fUnknown )
{
	DWORD dwFunction = FUNC_CanSeeEntity;
	DWORD dwThis = (DWORD)this->GetInterface();
	BOOL bReturn;
	_asm
	{
		mov		ecx, dwThis
		push	fUnknown
		push	entity
		call	dwFunction
		movzx	eax, al
		mov		bReturn, eax
	}
	return bReturn;
}


VOID CPedSA::ClearAimFlag (  )
{
	DWORD dwFunction = FUNC_ClearAimFlag;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::ClearAll (  )
{
	DWORD dwFunction = FUNC_ClearAll;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::ClearAttackByRemovingAnim (  )
{
	DWORD dwFunction = FUNC_ClearAttackByRemovingAnim;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::ClearInvestigateEvent (  )
{
	DWORD dwFunction = FUNC_ClearInvestigateEvent;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::ClearLookFlag (  )
{
	DWORD dwFunction = FUNC_ClearLookFlag;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::ClearObjective (  )
{
	DWORD dwFunction = FUNC_ClearObjective;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::ClearPointGunAt (  )
{
	DWORD dwFunction = FUNC_ClearPointGunAt;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::ClearWeapons (  )
{
	DWORD dwFunction = FUNC_ClearWeapons;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::CreateDeadPedMoney (  )
{
	DWORD dwFunction = FUNC_CreateDeadPedMoney;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::CreateDeadPedWeaponPickups (  )
{
	DWORD dwFunction = FUNC_CreateDeadPedWeaponPickups;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::GetLocalDirection ( CVector2D * vector )
{
	DWORD dwFunction = FUNC_GetLocalDirection;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	vector
		call	dwFunction
	}
}

CWeapon * CPedSA::GiveWeapon ( eWeaponType weapon, DWORD dwAmmo ) // returns weapon slot (check)
{
	DWORD dwFunction = FUNC_GiveWeapon;
	DWORD dwThis = (DWORD)this->GetInterface();
	DWORD dwReturn = 0;
	_asm
	{
		mov		ecx, dwThis
		push	1
		push	dwAmmo
		push	weapon
		call	dwFunction
		mov		dwReturn, eax
	}
	return this->GetWeapon((eWeaponSlot)dwReturn);
}

CWeapon * CPedSA::GetWeapon ( eWeaponSlot weaponSlot )
{
	// we make it appear that the weapon is non existant if its empty
	if(weaponSlot < MAX_WEAPONS && Weapons[weaponSlot]->GetType() != WEAPONTYPE_UNARMED)
		return Weapons[weaponSlot];
	else
		return NULL;
}
*/
/**
 * \todo Remove this function (or make it work by overloading in CPlayerPed with converter)
 */
/*
VOID CPedSA::SetCurrentWeapon ( eWeaponType weapon )
{
	DWORD dwFunction = FUNC_SetCurrentWeaponFromID;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	weapon
		call	dwFunction	
	}
}
*/
/**
 * This is overloaded by CPlayerPed
 */
/*
VOID CPedSA::SetCurrentWeapon ( eWeaponSlot slot )
{
	DWORD dwFunction = FUNC_SetCurrentWeaponFromSlot;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	slot
		call	dwFunction
	}
}

CWeapon * CPedSA::GetCurrentWeapon (  )
{
	return this->Weapons[((CPedSAInterface *)this->GetInterface())->CurrentWeaponSlot];
}

BOOL CPedSA::IsPedInControl (  )
{
	DWORD dwFunction = FUNC_IsPedInControl;
	DWORD dwThis = (DWORD)this->GetInterface();
	BOOL bReturn = 0;
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
		movzx	eax, al
		mov		bReturn, eax
	}
	return bReturn;
}

BOOL CPedSA::IsPedShootable (  )
{
	DWORD dwFunction = FUNC_IsPedShootable;
	DWORD dwThis = (DWORD)this->GetInterface();
	BOOL bReturn = 0;
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
		movzx	eax, al
		mov		bReturn, eax
	}
	return bReturn;
}

VOID CPedSA::SetAttackTimer ( DWORD dwTimer )
{
	DWORD dwFunction = FUNC_SetAttackTimer;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	dwTimer
		call	dwFunction
	}
}

VOID CPedSA::SetDead (  )
{
	DWORD dwFunction = FUNC_SetDead;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::SetDie ( DWORD AnimationID, FLOAT fUnknown1, FLOAT fUnknown2 )
{
	DWORD dwFunction = FUNC_SetDie;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	fUnknown2
		push	fUnknown1
		push	AnimationID
		call	dwFunction
	}
}

VOID CPedSA::SetEvasiveDive ( CPhysical * avoid, BYTE bUnknown )
{
	DWORD dwFunction = FUNC_SetEvasiveDive;
	DWORD dwThis = (DWORD)this->GetInterface();
	DWORD dwUnk = bUnknown;
	_asm
	{
		mov		ecx, dwThis
		push	dwUnk
		push	avoid
		call	dwFunction
	}
}

VOID CPedSA::SetEvasiveStep ( CPhysical * avoid, BYTE bUnknown )
{
	DWORD dwFunction = FUNC_SetEvasiveStep;
	DWORD dwThis = (DWORD)this->GetInterface();
	DWORD dwUnk = bUnknown;
	_asm
	{
		mov		ecx, dwThis
		push	dwUnk
		push	avoid
		call	dwFunction
	}
}

VOID CPedSA::SetFall ( DWORD dwUnknown, DWORD AnimationID, BYTE bUnknown )
{
	DWORD dwFunction = FUNC_SetFall;
	DWORD dwThis = (DWORD)this->GetInterface();
	DWORD dwUnk = bUnknown;
	_asm
	{
		mov		ecx, dwThis
		push	dwUnk
		push	AnimationID
		push	dwUnknown
		call	dwFunction
	}
}

VOID CPedSA::SetFlee ( CVector2D * vecPoint, DWORD dwDuration )
{
	DWORD dwFunction = FUNC_SetFlee;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	dwDuration
		push	vecPoint
		call	dwFunction
	}
}

VOID CPedSA::SetIdle (  )
{
	DWORD dwFunction = FUNC_SetIdle;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::SetLeader ( CPed * leader )
{
	DWORD dwFunction = FUNC_SetLeader;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	leader
		call	dwFunction
	}
}

VOID CPedSA::SetLookFlag ( CEntity * lookat, BOOL bIgnoreCurrentLook ) // confirm BOOL
{
	DWORD dwFunction = FUNC_SetLookFlag;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	bIgnoreCurrentLook
		push	lookat
		call	dwFunction
	}
}

VOID CPedSA::SetLookTimer ( DWORD dwLookTime )
{
	DWORD dwFunction = FUNC_SetLookTimer;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	dwLookTime
		call	dwFunction
	}
}

VOID CPedSA::SetMoveState ( eMoveState movestate )
{
	DWORD dwFunction = FUNC_SetMoveState;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	movestate
		call	dwFunction
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

VOID CPedSA::GetObjectiveVector ( CVector * vector )
{
	memcpy(vector,&(((CPedSAInterface *)this->GetInterface())->ObjectiveVector),sizeof(CVector));
}
*/
/*	CPedSAInterface * pedInterface = ((CPedSAInterface *)this->GetInterface());
	pedInterface->Objective = objective;
	CEntitySA * entityVC = static_cast<CEntitySA *>(entity);
	CEntitySAInterface * entityInterface = entityVC->GetInterface();
	pedInterface->ObjectiveEntity = entityInterface;*/
/*
VOID CPedSA::SetObjective ( eObjective  objective, CVehicle * vehicle )
{
	char szDebug[255] = {'\0'};
	sprintf(szDebug, "Objective: %d, Vehicle: %d", objective, vehicle);
	OutputDebugString(szDebug);
	DWORD dwFunction = FUNC_SetObjective_ENTITY;
	DWORD dwThis = (DWORD)this->GetInterface();
	DWORD dwEntity = (DWORD)((CVehicleSA *)vehicle)->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	dwEntity
		push	objective
		call	dwFunction
	}
}

VOID CPedSA::SetObjective ( eObjective  objective, CPed * ped )
{
	DWORD dwFunction = FUNC_SetObjective_ENTITY;
	DWORD dwThis = (DWORD)this->GetInterface();
	DWORD dwEntity = (DWORD)((CPedSA *)ped)->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	dwEntity
		push	objective
		call	dwFunction
	}
}

VOID CPedSA::SetObjective ( eObjective  objective )
{
	DWORD dwFunction = FUNC_SetObjective;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	objective
		call	dwFunction
	}
}

VOID CPedSA::SetObjective ( eObjective  objective, CVector * vecPoint )
{
	DWORD dwFunction = FUNC_SetObjective_VECTOR;
	DWORD dwThis = (DWORD)this->GetInterface();
	FLOAT fx, fy, fz;
	fx = ((CVector *)vecPoint)->fX;
	fy = ((CVector *)vecPoint)->fY;
	fz = ((CVector *)vecPoint)->fZ;
	_asm
	{
		mov		ecx, dwThis
		push	fz
		push	fy
		push	fx
		push	objective
		call	dwFunction
	}
}

VOID CPedSA::SetSeekCar ( CVehicle * vehicle )
{
	DWORD dwFunction = FUNC_SetSeekCar;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	vehicle
		call	dwFunction
	}
}

VOID CPedSA::SetShootTimer ( DWORD dwTime )
{
	DWORD dwFunction = FUNC_SetShootTimer;
	DWORD dwThis = (DWORD)this;
	_asm
	{
		mov		ecx, dwThis
		push	dwTime
		call	dwFunction
	}
}

VOID CPedSA::SetSolicit (  )
{
	DWORD dwFunction = FUNC_SetSolicit;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::SetStoredState (  )
{
	DWORD dwFunction = FUNC_SetStoredState;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::RestorePreviousState (  )
{
	DWORD dwFunction = FUNC_RestorePreviousState;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunction
	}
}

VOID CPedSA::SetWaitState ( eWaitState waitstate )
{
	DWORD dwFunction = FUNC_SetWaitState;
	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	waitstate
		call	dwFunction
	}
}

VOID CPedSA::Teleport ( CVector * vecPoint )
{
	DWORD dwFunction = FUNC_Teleport;
	DWORD dwThis = (DWORD)this->GetInterface();
	FLOAT fx, fy, fz;
	fx = ((CVector *)vecPoint)->fX;
	fy = ((CVector *)vecPoint)->fY;
	fz = ((CVector *)vecPoint)->fZ;
	_asm
	{
		mov		ecx, dwThis
		push	fz
		push	fy
		push	fx
		call	dwFunction
	}
}

VOID CPedSA::WarpPedIntoCar ( CVehicle * vehicle )
{*/
	/*DWORD dwFunction = FUNC_WarpPedIntoCar;
	DWORD dwThis = (DWORD)this->GetInterface();
	DWORD dwVehicle = (DWORD)((CVehicleSA *)vehicle)->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		push	dwVehicle
		call	dwFunction
	}*/



/*	DWORD dwFunc = 0x650280;	// CCarEnterExit::SetPedInCarDirect
	DWORD vehicleInterface = (DWORD)((CVehicleSA *)vehicle)->GetInterface();
	DWORD pedInterface = (DWORD)this->GetInterface();
	_asm
	{
		push	1
		push	0
		push	vehicleInterface
		push	pedInterface
		call	dwFunc
		add		esp, 16
	}*/
/*
	// CODE BELOW IS SA but crashes for non player CPlayerPeds without CPlayerInfos (multiplayer hook required)
	DWORD dwFunc = 0x6470E0; // CTaskSimpleCarSetPedInAsDriver
	DWORD dwVehicle = (DWORD)((CVehicleSA *)vehicle)->GetInterface();
	DWORD dwTask[50];
	memset(dwTask, 0, 50);
	_asm
	{
		pushad
		lea		ecx, dwTask
		push	0			// seat
		push	dwVehicle	// vehicle
		call	dwFunc
		popad
	}
	
	dwFunc = 0x64B950; // CTaskSimpleCarSetPedInAsDriver::ProcessPed
	DWORD dwPed = (DWORD)this->GetInterface();
	_asm
	{
		lea		ecx, dwTask
		push	dwPed	// ped
		call	dwFunc
	}
}

FLOAT CPedSA::GetHealth ( )
{
	CPedSAInterface * ped = (CPedSAInterface *)this->GetInterface();
	return ped->Health;
}

VOID CPedSA::SetHealth ( FLOAT fHealth )
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

BOOL CPedSA::IsInVehicle()
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
VOID CPedSA::SetAsActivePed()
{
	*(DWORD *)VAR_LocalPlayer = (DWORD)this->GetInterface();
}

VOID CPedSA::SetPedState(ePedState PedState)
{
	((CPedSAInterface *)this->GetInterface())->PedState = (ePedStateVC)PedState;
}

ePedState CPedSA::GetPedState()
{
	return (ePedState)((CPedSAInterface *)this->GetInterface())->PedState;
}*/