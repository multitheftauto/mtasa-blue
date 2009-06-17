/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleSA.cpp
*  PURPOSE:     Vehicle base entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA* pGame;

CVehicleSA::CVehicleSA ()
    : m_ucAlpha ( 255 ), m_bIsDerailable ( true ), m_vecGravity ( 0.0f, 0.0f, -1.0f ), m_HeadLightColor ( COLOR_RGBA ( 255, 255, 255, 255 ) )
{
}

/**
 *\todo ASAP: Remove all the VC specific (SCM) function calls propperly
 */
CVehicleSA::CVehicleSA( eVehicleTypes dwModelID )
    : m_ucAlpha ( 255 ), m_bIsDerailable ( true ), m_vecGravity ( 0.0f, 0.0f, -1.0f ), m_HeadLightColor ( COLOR_RGBA ( 255, 255, 255, 255 ) )
{
	DEBUG_TRACE("CVehicleSA::CVehicleSA( eVehicleTypes dwModelID )");
	// for SA, we can just call the following function and it should just work:
	// CCarCtrl::CreateCarForScript(int,class CVector,unsigned char)
	//								ModelID, Position, IsMissionVehicle

    DWORD dwReturn = 0;

    // Is it a train?
	DWORD dwFunc = FUNC_CCarCtrlCreateCarForScript;
	_asm
	{
		push	0			// its a mission vehicle
		push	0
		push	0
		push	0			// spawn at 0,0,0
		push	dwModelID	
		call	dwFunc
		add		esp, 0x14
		mov		dwReturn, eax
	}

	m_pInterface = reinterpret_cast < CEntitySAInterface* > ( dwReturn );

	this->BeingDeleted = FALSE;

    m_pInterface->bStreamingDontDelete = true;
    m_pInterface->bDontStream = true;
	
    // store our CVehicleSA pointer in the vehicle's time of creation member (as it won't get modified later and as far as I know it isn't used for something important)
    GetVehicleInterface ()->m_pVehicle = this;

	// Unlock doors as they spawn randomly with locked doors
	LockDoors ( false );

	// Reset the car countss to 0 so that this vehicle doesn't affect the population vehicles
	for ( int i = 0; i < 5; i++ )
	{
		*(DWORD *)(VARS_CarCounts + i * sizeof(DWORD)) = 0;
	}

    // only applicable for CAutomobile based vehicles (i.e. not bikes or boats, but includes planes, helis etc)
    this->damageManager = new CDamageManagerSA ( m_pInterface, (CDamageManagerSAInterface *)((DWORD)this->GetInterface() + 1440));


    // Replace the handling interface with our own to prevent handlig.cfg cheats and allow custom handling stuff.
    // We don't use SA's array because we want one handling per vehicle type and also allow custom handlings
    // per car later.
    CHandlingEntry* pEntry = pGame->GetHandlingManager ()->GetHandlingData ( dwModelID );
    SetHandlingData ( pEntry );

    GetVehicleInterface ()->m_nVehicleFlags.bVehicleCanBeTargetted = true;

    this->internalID = pGame->GetPools ()->GetVehicleRef ( (DWORD *)this->GetVehicleInterface () );
}

CVehicleSA::CVehicleSA ( CVehicleSAInterface * vehicleInterface )
{
    m_pInterface = vehicleInterface;

    m_pInterface->bStreamingDontDelete = true;
    m_pInterface->bDontStream = true;
    this->BeingDeleted = FALSE;

    // Store our CVehicleSA pointer in the vehicle's time of creation member (as it won't get modified later and as far as I know it isn't used for something important)
    GetVehicleInterface ()->m_pVehicle = this;

    // Reset the car countss to 0 so that this vehicle doesn't affect the population vehicles
	for ( int i = 0; i < 5; i++ )
	{
		*(DWORD *)(VARS_CarCounts + i * sizeof(DWORD)) = 0;
	}

    // only applicable for CAutomobile based vehicles (i.e. not bikes, trains or boats, but includes planes, helis etc)
    this->damageManager = new CDamageManagerSA( m_pInterface, (CDamageManagerSAInterface *)((DWORD)this->GetInterface() + 1440));

    this->internalID = pGame->GetPools ()->GetVehicleRef ( (DWORD *)this->GetVehicleInterface () );

    m_bIsDerailable = true;
    m_ucAlpha = 255;
}

// DESTRUCTOR
CVehicleSA::~CVehicleSA()
{
	DEBUG_TRACE("CVehicleSA::~CVehicleSA()");
	if(!this->BeingDeleted)
	{
		if ( (DWORD)m_pInterface->vtbl != VTBL_CPlaceable )
		{
            if ( this->damageManager ) delete this->damageManager;
            
            DWORD dwThis = (DWORD) m_pInterface;
            DWORD dwFunc = 0x6D2460;
            _asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }

			CWorldSA * world = (CWorldSA *)pGame->GetWorld();
			world->Remove ( m_pInterface );
            world->RemoveReferencesToDeletedObject ( m_pInterface );

			
			dwFunc = m_pInterface->vtbl->SCALAR_DELETING_DESTRUCTOR; // we use the vtbl so we can be vehicle type independent
			_asm
			{
				mov		ecx, dwThis
				push	1			//delete too
				call	dwFunc
			}
		}
		this->BeingDeleted = true;
		((CPoolsSA *)pGame->GetPools())->RemoveVehicle((CVehicle *)this);
	}
}

VOID CVehicleSA::SetMoveSpeed ( CVector* vecMoveSpeed )
{
    DWORD dwFunc = FUNC_GetMoveSpeed;
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwReturn = 0;
    _asm
    {
        mov		ecx, dwThis
        call	dwFunc
        mov		dwReturn, eax
    }
    memcpy((void *)dwReturn, vecMoveSpeed, sizeof(CVector));

    // In case of train: calculate on-rail speed
    WORD wModelID = GetModelIndex();
    if ( wModelID == 537 || wModelID == 538 || wModelID == 569 || wModelID == 570 || wModelID == 590 || wModelID == 449 )
    {
        if ( !IsDerailed () )
        {
            CVehicleSAInterface* pInterf = GetVehicleInterface ();

            // Find the rail node we are on
            DWORD dwNumNodes = ((DWORD *)ARRAY_NumRailTrackNodes) [ pInterf->m_ucRailTrackID ];
            SRailNodeSA* pNode = ( (SRailNodeSA **) ARRAY_RailTrackNodePointers ) [ pInterf->m_ucRailTrackID ];
            SRailNodeSA* pNodesEnd = &pNode [ dwNumNodes ];
            while ( (float)pNode->sRailDistance / 3.0f <= pInterf->m_fTrainRailDistance && pNode < pNodesEnd )
            {
                pNode++;
            }
            if ( pNode >= pNodesEnd )
                return;
            // Get the direction vector between the nodes the train is between
            CVector vecNode1 ( (float)(pNode - 1)->sX / 8.0f, (float)(pNode - 1)->sY / 8.0f, (float)(pNode - 1)->sZ / 8.0f );
            CVector vecNode2 ( (float)pNode->sX / 8.0f, (float)pNode->sY / 8.0f, (float)pNode->sZ / 8.0f );
            CVector vecDirection = vecNode2 - vecNode1;
            vecDirection.Normalize ();
            // Set the speed
            pInterf->m_fTrainSpeed = vecDirection.DotProduct ( vecMoveSpeed );
        }
    }
}

CVehicleSAInterface * CVehicleSA::GetNextCarriageInTrain ( void )
{
    return (CVehicleSAInterface *)*(DWORD *)((DWORD)this->GetInterface() + 1492);
}

CVehicle * CVehicleSA::GetNextTrainCarriage ( void )
{
    CVehicleSAInterface * pVehicle = GetNextCarriageInTrain();
    if ( pVehicle )
        return pGame->GetPools()->GetVehicle ( (DWORD *)pVehicle );
    else
        return NULL;
}

bool CVehicleSA::AddProjectile ( eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity )
{
    return ((CProjectileInfoSA*)pGame->GetProjectileInfo())->AddProjectile ( (CEntitySA*)this, eWeapon, vecOrigin, fForce, target, targetEntity );
}

void CVehicleSA::SetNextTrainCarriage ( CVehicle * next )
{
    if ( next )
    {
        CVehicleSA * pNextVehicle = dynamic_cast < CVehicleSA* > ( next );
		if ( pNextVehicle )
		{
			*(DWORD *)((DWORD)this->GetInterface () + 1492) = (DWORD)pNextVehicle->GetInterface();
			if ( pNextVehicle->GetPreviousTrainCarriage () != this )
				pNextVehicle->SetPreviousTrainCarriage ( this );
		}
    }
    else
    {
        *(DWORD *)((DWORD)this->GetInterface() + 1492) = NULL;
    }
}


CVehicleSAInterface * CVehicleSA::GetPreviousCarriageInTrain ( void )
{
    return (CVehicleSAInterface *)*(DWORD *)((DWORD)this->GetInterface() + 1488);
}

void CVehicleSA::SetPreviousTrainCarriage ( CVehicle * previous )
{
    if ( previous )
    {
        CVehicleSA * pPreviousVehicle = dynamic_cast < CVehicleSA* > ( previous );
		if ( pPreviousVehicle )
		{
			*(DWORD *)((DWORD)this->GetInterface () + 1488) = (DWORD)pPreviousVehicle->GetInterface();
			if ( pPreviousVehicle->GetNextTrainCarriage () != this )
				pPreviousVehicle->SetNextTrainCarriage ( this );
		}
    }
    else
    {
        *(DWORD *)((DWORD)this->GetInterface() + 1488) = NULL;
    }
}


CVehicle * CVehicleSA::GetPreviousTrainCarriage ( void )
{
    CVehicleSAInterface * pVehicle = GetPreviousCarriageInTrain();
    if ( pVehicle )
        return pGame->GetPools()->GetVehicle ( (DWORD *)pVehicle );
    else
        return NULL;
}


bool CVehicleSA::IsDerailed ( void )
{
    CVehicleSAInterface* pInterface = GetVehicleInterface ();
    return pInterface->trainFlags.bIsDerailed;
}


void CVehicleSA::SetDerailed ( bool bDerailed )
{
    WORD wModelID = GetModelIndex();
    if ( wModelID == 537 || wModelID == 538 || wModelID == 569 || wModelID == 570 || wModelID == 590 || wModelID == 449 )
    {
        CVehicleSAInterface* pInterface = GetVehicleInterface ();
        DWORD dwThis = (DWORD)pInterface;

        if ( bDerailed )
        {
            pInterface->trainFlags.bIsDerailed = true;
            * ( DWORD * ) ( dwThis + 64 ) &= ( DWORD ) 0xFFFDFFFB;
        }
        else
        {
            pInterface->trainFlags.bIsDerailed = false;
            * ( DWORD * ) ( dwThis + 64 ) |= ( DWORD ) 0x20004;

            // Recalculate the on-rail distance from the start node (train position parameter, m_fTrainRailDistance)
            DWORD dwFunc = FUNC_CVehicle_RecalcOnRailDistance;
            _asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }

            // Reset the speed
            GetVehicleInterface ()->m_fTrainSpeed = 0.0f;
        }
    }   
}

float CVehicleSA::GetTrainSpeed ()
{
    return GetVehicleInterface ()->m_fTrainSpeed;
}


void CVehicleSA::SetTrainSpeed ( float fSpeed )
{
    GetVehicleInterface ()->m_fTrainSpeed = fSpeed;
}


bool CVehicleSA::GetTrainDirection ()
{
    return ( *( (BYTE *)GetInterface () + 1464 ) & 0x40 ) != 0;
}


void CVehicleSA::SetTrainDirection ( bool bDirection )
{
    if ( bDirection )
    {
        *( (BYTE *)GetInterface () + 1464 ) |= 0x40;
    }
    else
    {
        *( (BYTE *)GetInterface () + 1464 ) &= ~0x40;
    }
}

BYTE CVehicleSA::GetRailTrack ()
{
    return GetVehicleInterface ()->m_ucRailTrackID;
}

void CVehicleSA::SetRailTrack ( BYTE ucTrackID )
{
    if ( ucTrackID >= NUM_RAILTRACKS )
        return;

    CVehicleSAInterface* pInterf = GetVehicleInterface ();
    pInterf->m_ucRailTrackID = ucTrackID;
    if ( !IsDerailed () )
    {
        DWORD dwFunc = FUNC_CVehicle_RecalcOnRailDistance;
        _asm
        {
            mov ecx, pInterf
            call dwFunc
        }
    }
}

bool CVehicleSA::CanPedEnterCar ( void )
{
	DEBUG_TRACE("bool CVehicleSA::CanPedEnterCar ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_CanPedEnterCar;
	bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
		mov		bReturn, al
    }

	return bReturn;
}


bool CVehicleSA::CanPedJumpOutCar ( CPed* pPed )
{
	DEBUG_TRACE("bool CVehicleSA::CanPedJumpOutCar ( CPed* pPed )");

	bool bReturn = false;

	CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );

	if ( pPedSA )
	{
		DWORD dwThis = (DWORD) m_pInterface;
		CPedSAInterface* pPedInt = pPedSA->GetPedInterface();
		DWORD dwFunc = FUNC_CVehicle_CanPedJumpOutCar;

		_asm
		{
			mov     ecx, dwThis
			push    pPedInt
			call    dwFunc
			mov		bReturn, al
		}
	}

	return bReturn;
}

bool CVehicleSA::AreDoorsLocked ( void )
{
    return ( GetVehicleInterface ()->ul_doorstate == 2 || GetVehicleInterface ()->ul_doorstate == 5 || 
             GetVehicleInterface ()->ul_doorstate == 4 || GetVehicleInterface ()->ul_doorstate == 7 || 
             GetVehicleInterface ()->ul_doorstate == 3 );
}

void CVehicleSA::LockDoors ( bool bLocked )
{
    bool bAreDoorsLocked = AreDoorsLocked ();
    bool bAreDoorsUndamageable = AreDoorsUndamageable ();

    if ( bLocked && !bAreDoorsLocked )
    {
        if ( bAreDoorsUndamageable )
            GetVehicleInterface ()->ul_doorstate = 7;
        else
            GetVehicleInterface ()->ul_doorstate = 2;
    }
    else if ( !bLocked && bAreDoorsLocked )
    {
        if ( bAreDoorsUndamageable )
            GetVehicleInterface ()->ul_doorstate = 1;
        else
            GetVehicleInterface ()->ul_doorstate = 0;
    }
}

bool CVehicleSA::AreDoorsUndamageable ( void )
{
    return ( GetVehicleInterface ()->ul_doorstate == 1 ||
             GetVehicleInterface ()->ul_doorstate == 7 );
}

void CVehicleSA::SetDoorsUndamageable ( bool bUndamageable )
{
    bool bAreDoorsLocked = AreDoorsLocked ();
    bool bAreDoorsUndamageable = AreDoorsUndamageable ();

    if ( bUndamageable && !bAreDoorsUndamageable )
    {
        if ( bAreDoorsLocked )
            GetVehicleInterface ()->ul_doorstate = 7;
        else
            GetVehicleInterface ()->ul_doorstate = 1;
    }
    else if ( !bUndamageable && bAreDoorsUndamageable )
    {
        if ( bAreDoorsLocked )
            GetVehicleInterface ()->ul_doorstate = 2;
        else
            GetVehicleInterface ()->ul_doorstate = 0;
    }
}

void CVehicleSA::AddVehicleUpgrade ( DWORD dwModelID )
{	
	DEBUG_TRACE("void CVehicleSA::AddVehicleUpgrade ( DWORD dwModelID )");
    if ( dwModelID >= 1000 && dwModelID <= 1193 )
	{
		DWORD dwThis = (DWORD) m_pInterface;

		DWORD dwFunc = FUNC_CVehicle_AddVehicleUpgrade;
		_asm
		{
			mov     ecx, dwThis
			push    dwModelID
			call    dwFunc
		}
	}
}

void CVehicleSA::RemoveVehicleUpgrade ( DWORD dwModelID )
{
	DEBUG_TRACE("void CVehicleSA::RemoveVehicleUpgrade ( DWORD dwModelID )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_RemoveVehicleUpgrade;

    _asm
    {
        mov     ecx, dwThis
        push    dwModelID
        call    dwFunc
    }
}

bool CVehicleSA::CanPedLeanOut ( CPed* pPed )
{
	DEBUG_TRACE("bool CVehicleSA::CanPedLeanOut ( CPed* pPed )");

	bool bReturn = false;

	CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );

	if ( pPedSA )
	{
		DWORD dwThis = (DWORD) m_pInterface;
		CPedSAInterface* pPedInt = pPedSA->GetPedInterface();
		DWORD dwFunc = FUNC_CVehicle_CanPedLeanOut;

		_asm
		{
			mov     ecx, dwThis
			push    pPedInt
			call    dwFunc
			mov		bReturn, al
		}
	}

	return bReturn;

}


bool CVehicleSA::CanPedStepOutCar ( bool bUnknown )
{
	DEBUG_TRACE("bool CVehicleSA::CanPedStepOutCar ( bool bUnknown )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwUnknown = (DWORD) bUnknown;
    DWORD dwFunc = FUNC_CVehicle_CanPedStepOutCar;
	bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        push    dwUnknown
        call    dwFunc
        mov		bReturn, al
    }

	return bReturn;
}


bool CVehicleSA::CarHasRoof ( void )
{
	DEBUG_TRACE("bool CVehicleSA::CarHasRoof ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_CarHasRoof;
	bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov		bReturn, al
    }

	return bReturn;
}


void CVehicleSA::ExtinguishCarFire ( void )
{
	DEBUG_TRACE("void CVehicleSA::ExtinguishCarFire ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_ExtinguishCarFire;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


DWORD CVehicleSA::GetBaseVehicleType ( void )
{
	DEBUG_TRACE("DWORD CVehicleSA::GetBaseVehicleType ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_GetBaseVehicleType;
	DWORD dwReturn = 0;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
		mov		dwReturn, eax

    }

	return dwReturn;
}


void CVehicleSA::SetBodyDirtLevel ( float fDirtLevel )
{
	DEBUG_TRACE("void CVehicleSA::SetBodyDirtLevel ( float fDirtLevel )");
    GetVehicleInterface ()->nBodyDirtLevel = fDirtLevel;
}


float CVehicleSA::GetBodyDirtLevel ( void )
{
	DEBUG_TRACE("float CVehicleSA::GetBodyDirtLevel ( void )");
    return GetVehicleInterface ()->nBodyDirtLevel;
}


unsigned char CVehicleSA::GetCurrentGear ( void )
{
	DEBUG_TRACE("unsigned char CVehicleSA::GetCurrentGear ( void )");
    return GetVehicleInterface ()->m_nCurrentGear;
}


float CVehicleSA::GetGasPedal ( void )
{
	DEBUG_TRACE("float CVehicleSA::GetGasPedal ( void )");
    return GetVehicleInterface ()->m_fGasPedal;
}


float CVehicleSA::GetHeightAboveRoad ( void )
{
	DEBUG_TRACE("float CVehicleSA::GetHeightAboveRoad ( void )");
    DWORD dwThis = (DWORD) GetVehicleInterface ();
    DWORD dwFunc = FUNC_CVehicle_GetHeightAboveRoad;
    float fReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fReturn
    }

	return fReturn;
}


float CVehicleSA::GetSteerAngle ( void )
{
	DEBUG_TRACE("float CVehicleSA::GetSteerAngle ( void )");
    return GetVehicleInterface ()->m_fSteerAngle;
}


bool CVehicleSA::GetTowBarPos ( CVector* pVector )
{
	DEBUG_TRACE("bool CVehicleSA::GetTowBarPos ( CVector* pVector )");
    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(m_pInterface->vtbl);
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = vehicleVTBL->GetTowbarPos;
	bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        push    0
        push    1
        push    pVector
        call    dwFunc
    	mov		bReturn, al
    }

	return bReturn;
}


bool CVehicleSA::GetTowHitchPos ( CVector* pVector )
{
	DEBUG_TRACE("bool CVehicleSA::GetTowHitchPos ( CVector* pVector )");
    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(m_pInterface->vtbl);
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = vehicleVTBL->GetTowHitchPos;
	bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        push    0
        push    1
        push    pVector
        call    dwFunc
		mov		bReturn, al
    }

	return bReturn;
}


bool CVehicleSA::IsOnItsSide ( void )
{
	DEBUG_TRACE("bool CVehicleSA::IsOnItsSide ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsOnItsSide;
	bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
		mov		bReturn, al
    }
	return bReturn;
}


bool CVehicleSA::IsLawEnforcementVehicle ( void )
{
	DEBUG_TRACE("bool CVehicleSA::IsLawEnforcementVehicle ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsLawEnforcementVehicle;
	bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
		mov		bReturn, al
    }
	return bReturn;
}


bool CVehicleSA::IsPassenger ( CPed* pPed )
{
	DEBUG_TRACE("bool CVehicleSA::IsPassenger ( CPed* pPed )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsPassenger;
	bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        push    pPed
        call    dwFunc
		mov		bReturn, al
    }
	return bReturn;
}


bool CVehicleSA::IsSphereTouchingVehicle ( CVector * vecOrigin, float fRadius )
{
	DEBUG_TRACE("bool CVehicleSA::IsSphereTouchingVehicle ( CVector * vecOrigin, float fRadius )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsSphereTouchingVehicle;
	bool bReturn = false;

    _asm
    {
		push	eax

        mov     ecx, dwThis
		mov		eax, vecOrigin
		push	fRadius
        push    dword ptr [eax]
        push    dword ptr [eax + 4]
        push    dword ptr [eax + 8]
		call	dwFunc
		mov		bReturn, al
    }
	return bReturn;
}


bool CVehicleSA::IsUpsideDown ( void )
{
	DEBUG_TRACE("bool CVehicleSA::IsUpsideDown ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsUpsideDown;
	bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
		mov		bReturn, al
    }

	return bReturn;
}


void CVehicleSA::MakeDirty ( CColPoint* pPoint )
{
	DEBUG_TRACE("void CVehicleSA::MakeDirty ( CColPoint* pPoint )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_MakeDirty;

    _asm
    {
        mov     ecx, dwThis
        push    pPoint
        call    dwFunc
    }
}


void CVehicleSA::SetEngineOn ( bool bEngineOn )
{
	DEBUG_TRACE("void CVehicleSA::SetEngineOn ( bool bEngineOn )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwEngineOn = (DWORD) bEngineOn;
    DWORD dwFunc = FUNC_CVehicle_SetEngineOn;

    _asm
    {
        mov     ecx, dwThis
        push    dwEngineOn
        call    dwFunc
    }
}


CPed* CVehicleSA::GetDriver ( void )
{
	DEBUG_TRACE("CPed* CVehicleSA::GetDriver ( void )");
	CPoolsSA* pPools  = (CPoolsSA *)pGame->GetPools();

    CPedSAInterface* pDriver = GetVehicleInterface ()->pDriver;
	if ( pDriver )
		return pPools->GetPed( (DWORD*) pDriver );
	else
		return NULL;
}


CPed* CVehicleSA::GetPassenger ( unsigned char ucSlot )
{
    DEBUG_TRACE("CPed* CVehicleSA::GetPassenger ( unsigned char ucSlot )");
	CPoolsSA* pPools  = (CPoolsSA *)pGame->GetPools();

    if ( ucSlot < 8 )
    {
        CPedSAInterface* pPassenger = GetVehicleInterface ()->pPassengers [ucSlot];
	    if ( pPassenger )
		    return pPools->GetPed( (DWORD*) pPassenger );
    }

    return NULL;
}


bool CVehicleSA::IsBeingDriven()
{
	DEBUG_TRACE("bool CVehicleSA::IsBeingDriven()");
	CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
	if ( GetVehicleInterface ()->pDriver != NULL )
		return TRUE;
	else
		return FALSE;
}


/**
 * \todo Implement for other vehicle types too and check
 */
void CVehicleSA::PlaceBikeOnRoadProperly()
{
	DEBUG_TRACE("void CVehicleSA::PlaceBikeOnRoadProperly()");
	DWORD dwFunc = FUNC_Bike_PlaceOnRoadProperly;
	DWORD dwBike = (DWORD)this->GetInterface();

	_asm
	{
		mov		ecx, dwBike
		call	dwFunc
	}
}

void CVehicleSA::PlaceAutomobileOnRoadProperly()
{
	DEBUG_TRACE("void CVehicleSA::PlaceAutomobileOnRoadProperly()");
	DWORD dwFunc = FUNC_Automobile_PlaceOnRoadProperly;
	DWORD dwAutomobile = (DWORD)this->GetInterface();

	_asm
	{
		mov		ecx, dwAutomobile
		call	dwFunc
	}
}

void CVehicleSA::SetColor ( unsigned char color1, unsigned char color2, unsigned char color3, unsigned char color4 )
{
	((CVehicleSAInterface *)(this->GetInterface()))->m_colour1 = color1;
	((CVehicleSAInterface *)(this->GetInterface()))->m_colour2 = color2;
	((CVehicleSAInterface *)(this->GetInterface()))->m_colour3 = color3;
	((CVehicleSAInterface *)(this->GetInterface()))->m_colour3 = color4;
}

void CVehicleSA::GetColor ( unsigned char* color1, unsigned char* color2, unsigned char* color3, unsigned char* color4 )
{
	*color1 = ((CVehicleSAInterface *)(this->GetInterface()))->m_colour1;
	*color2 = ((CVehicleSAInterface *)(this->GetInterface()))->m_colour2;
	*color3 = ((CVehicleSAInterface *)(this->GetInterface()))->m_colour3;
	*color4 = ((CVehicleSAInterface *)(this->GetInterface()))->m_colour4;
}

// works with firetrucks & tanks
void CVehicleSA::GetTurretRotation ( float * fHorizontal, float * fVertical )
{
	DEBUG_TRACE("void * CVehicleSA::GetTurretRotation ( float * fHorizontal, float * fVertical )");
	// This is coded in asm because for some reason it was failing to compile
	// correctly with normal c++.
	DWORD vehicleInterface = (DWORD)this->GetInterface();
	float fHoriz = 0.0f;
	float fVert = 0.0f;
	_asm
	{
		mov		eax, vehicleInterface
		add		eax, 0x94C
		fld		[eax]
		fstp	fHoriz
		add		eax, 4
		fld		[eax]
		fstp	fVert
	}
	*fHorizontal = fHoriz;
	*fVertical = fVert;
}

void CVehicleSA::SetTurretRotation ( float fHorizontal, float fVertical )
{
	DEBUG_TRACE("void * CVehicleSA::SetTurretRotation ( float fHorizontal, float fVertical )");
	//*(float *)(this->GetInterface() + 2380) = fHorizontal;
	//*(float *)(this->GetInterface() + 2384) = fVertical;
	DWORD vehicleInterface = (DWORD)this->GetInterface();
	_asm
	{
		mov		eax, vehicleInterface
		add		eax, 0x94C
		fld		fHorizontal
		fstp	[eax]
		add		eax, 4
		fld		fVertical
		fstp	[eax]
	}
}

bool CVehicleSA::IsSirenOrAlarmActive ( )
{
    return ((CVehicleSAInterface *)this->GetInterface())->m_nVehicleFlags.bSirenOrAlarm;
}

void CVehicleSA::SetSirenOrAlarmActive ( bool bActive )
{
   ((CVehicleSAInterface *)this->GetInterface())->m_nVehicleFlags.bSirenOrAlarm = bActive;
}

DWORD * CVehicleSA::GetMemoryValue ( DWORD dwOffset )
{
    if ( dwOffset <= SIZEOF_CHELI )
        return (DWORD *)((DWORD)(this->GetInterface()) + dwOffset);
    else
        return NULL;
}

/*

bool CVehicleSA::isInRect(FLOAT fX1, FLOAT fY1, FLOAT fX2, FLOAT fY2, bool bShowHotspot)
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_IN_RECT_STILL,&this->internalID,&fX1,&fY1,&fX2,&fY2,&bShowHotspot);
}

//-----------------------------------------------------------

bool CVehicleSA::isInCube(CVector * v3dCorner1, CVector * v3dCorner2, bool bShowHotspot)
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_IN_CUBE_STILL,&this->internalID,*v3dCorner1,*v3dCorner2,&bShowHotspot);
}

//-----------------------------------------------------------

bool CVehicleSA::isNearPoint(FLOAT fX, FLOAT fY, FLOAT fRadiusX, FLOAT fRadiusY, bool bShowHotspot, bool bStill)
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_NEAR_POINT,&this->internalID,&fX,&fY,&fRadiusX,&fRadiusY,&bShowHotspot,&bStill);
}

//-----------------------------------------------------------

bool CVehicleSA::isNearPoint3D(FLOAT fX, FLOAT fY, FLOAT fZ, FLOAT fRadiusX, FLOAT fRadiusY, FLOAT fRadiusZ, bool bShowHotspot, bool bStill)
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_NEAR_POINT_3D,&this->internalID,&fX,&fY,&fZ,&fRadiusX,&fRadiusY,&fRadiusZ,&bShowHotspot,&bStill);
}

//-----------------------------------------------------------

bool CVehicleSA::isStopped()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_STOPPED,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isWrecked()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_WRECKED,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isOnLand()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_ON_LAND,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isCrushed()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_CRUSHED,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isOnRoof()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_FLIPPED,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isAirbourne()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_AIRBORNE,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isUpsidedown()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_UPSIDEDOWN,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isStuck()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_STUCK,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isSunk()
{
	return (bool)CallScriptFunction(GTAVC_SCM_HAS_CAR_SUNK,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isBoundingSphereVisible()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_BOUNDING_SPHERE_VISIBLE,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isDamaged()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_DAMAGED,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isBombActive()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_BOMB_STATUS,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isPassengerSeatFree()
{
	return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_PASSENGER_SEAT_FREE,&this->internalID);
}

//-----------------------------------------------------------

DWORD CVehicleSA::passengerCount()
{
	return ((CVehicleSAInterface *)this->GetInterface())->PassengerCount;
}

//-----------------------------------------------------------

DWORD CVehicleSA::maxPassengers()
{
	DWORD dwMaxPassengers = 0;
	CallScriptFunction(GTAVC_SCM_GET_CAR_MAX_PASSENGERS,&this->internalID, &dwMaxPassengers);
	return dwMaxPassengers;
}

//-----------------------------------------------------------

void CVehicleSA::setDensity(DWORD dwDensity)
{
	CallScriptFunction(GTAVC_SCM_SET_CAR_DENSITY,&this->internalID,&dwDensity);
}

//-----------------------------------------------------------

void CVehicleSA::makeHeavy(bool bHeavy)
{
	CallScriptFunction(GTAVC_SCM_MAKE_CAR_HEAVY,&this->internalID,&bHeavy);
}
*/
//-----------------------------------------------------------
/**
 * \todo Make a eDoorState enum
 */
/*
void CVehicleSA::setDoorsStatus(DWORD dwStatus)
{
	CallScriptFunction(GTAVC_SCM_SET_CAR_DOOR_STATUS,&this->internalID,&dwStatus);
}

//-----------------------------------------------------------

void CVehicleSA::setTaxiLight(bool bOn)
{
	CallScriptFunction(GTAVC_SCM_SET_CAR_TAXIAVAILABLE,&this->internalID,&bOn);
}

//-----------------------------------------------------------

void CVehicleSA::setSpeed(FLOAT fSpeed)
{
	CallScriptFunction(GTAVC_SCM_SET_CAR_SPEED,&this->internalID,&fSpeed);
}

//-----------------------------------------------------------

FLOAT CVehicleSA::getSpeed()
{
	FLOAT fReturn;
	CallScriptFunction(GTAVC_SCM_GET_CAR_SPEED,&this->internalID, &fReturn);
	return fReturn;
}

//-----------------------------------------------------------

void CVehicleSA::setSpeedInstantly(FLOAT fSpeed)
{
	CallScriptFunction(GTAVC_SCM_SET_CAR_SPEED_INSTANTLY,&this->internalID,&fSpeed);
}
//-----------------------------------------------------------

void CVehicleSA::setImmunities(bool bBullet, bool bFire, bool bExplosion, bool bDamage, bool bUnknown)
{
	CallScriptFunction(GTAVC_SCM_SET_CAR_IMMUNITIES,&this->internalID,&bBullet,&bFire,&bExplosion,&bDamage,&bUnknown);
}
*/

//-----------------------------------------------------------
FLOAT CVehicleSA::GetHealth()
{
	CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
	return vehicle->m_nHealth;
}
//-----------------------------------------------------------
void CVehicleSA::SetHealth( FLOAT fHealth )
{
	CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
	vehicle->m_nHealth = fHealth;
    if ( fHealth >= 250.0f )
        vehicle->m_fBurningTime = 0.0f;
}

// SHould be plane funcs

void CVehicleSA::SetLandingGearDown ( bool bLandingGearDown )
{
	DWORD dwFunc = FUNC_CPlane__SetGearUp;
	if ( bLandingGearDown )
		dwFunc = FUNC_CPlane__SetGearDown;

	DWORD dwThis = (DWORD)this->GetInterface();
	_asm
	{
		mov		ecx, dwThis
		call	dwFunc
	}
}

float CVehicleSA::GetLandingGearPosition ( )
{
	DWORD dwThis = (DWORD)this->GetInterface();
	return *(float *)(dwThis + 2508);
}

void CVehicleSA::SetLandingGearPosition ( float fPosition )
{
	DWORD dwThis = (DWORD)this->GetInterface();
	*(float *)(dwThis + 2508) = fPosition;
}

bool CVehicleSA::IsLandingGearDown ( )
{
	DWORD dwThis = (DWORD)this->GetInterface();
	if ( *(float *)(dwThis + 2508) == 0.0f )
		return true;
	else
		return false;
}

void CVehicleSA::Fix ( void )
{
	DEBUG_TRACE("void CVehicleSA::Fix ( void )");
    DWORD dwThis = (DWORD) GetInterface();

    DWORD dwFunc = 0;
    CModelInfo* pModelInfo = pGame->GetModelInfo ( this->GetModelIndex() );
    if ( pModelInfo )
    {
        if ( pModelInfo->IsCar() || pModelInfo->IsMonsterTruck() || pModelInfo->IsTrailer() )
            dwFunc = FUNC_CAutomobile__Fix;
        else if ( pModelInfo->IsPlane() )
            dwFunc = FUNC_CPlane__Fix;
        else if ( pModelInfo->IsHeli() )
            dwFunc = FUNC_CHeli__Fix;
        else if ( pModelInfo->IsBike() )
            dwFunc = FUNC_CBike_Fix;

        if ( dwFunc)
        {
            _asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }
        }
    }
}

CDamageManager * CVehicleSA::GetDamageManager()
{
	DEBUG_TRACE("CDamageManager * CVehicleSA::GetDamageManager()");
	return this->damageManager;
}

void CVehicleSA::BlowUp ( CEntity* pCreator, unsigned long ulUnknown )
{
    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(this->GetInterface()->vtbl);
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = vehicleVTBL->BlowUpCar;

    DWORD dwCreator = (DWORD)pCreator;

    _asm
    {
        push        ulUnknown
        push        dwCreator
        mov         ecx, dwThis
        call        dwFunc
    }
}


void CVehicleSA::BlowUpCutSceneNoExtras ( unsigned long ulUnknown1, unsigned long ulUnknown2, unsigned long ulUnknown3, unsigned long ulUnknown4 )
{
    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(this->GetInterface()->vtbl);
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = vehicleVTBL->BlowUpCarCutSceneNoExtras;

    _asm
    {
        push        ulUnknown1
        push        ulUnknown2
        push        ulUnknown3
        push        ulUnknown4
        mov         ecx, dwThis
        call        dwFunc
    }
}


void CVehicleSA::FadeOut ( bool bFadeOut )
{
	CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
    vehicle->bDistanceFade = bFadeOut;
	vehicle->m_nVehicleFlags.bFadeOut = bFadeOut;
}

bool CVehicleSA::IsFadingOut ( void )
{
	CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
	return vehicle->m_nVehicleFlags.bFadeOut;
}


unsigned char CVehicleSA::GetNumberGettingIn ( void )
{
    return GetVehicleInterface ()->m_nNumGettingIn;
}


unsigned char CVehicleSA::GetPassengerCount ( void )
{
    return GetVehicleInterface ()->m_nNumPassengers;
}


unsigned char CVehicleSA::GetMaxPassengerCount ( void )
{
	// ACHTUNG: MAX PASSENGER HACK (27/02/2007) :: PLEASE REVERT
	#pragma message(__LOC__ "(IJs) 8 max passenger hack was installed here.")

	return 8;
    //return ((CVehicleSAInterface*)this->internalInterface)->m_nMaxPassengers;
}

bool CVehicleSA::SetTowLink ( CVehicle* pVehicle )
{
	DEBUG_TRACE("bool CAutomobileSA::SetTowLink ( CVehicle* pVehicle )");

	bool bReturn = false;

	CVehicleSA* pVehicleSA = dynamic_cast < CVehicleSA* > ( pVehicle );

	if ( pVehicleSA )
	{
		DWORD dwThis = (DWORD) GetInterface();
		CVehicleSAInterface* pVehicleInt = pVehicleSA->GetVehicleInterface();

		CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(this->GetInterface()->vtbl);
		DWORD dwFunc = vehicleVTBL->SetTowLink;
		_asm
		{
			mov     ecx, dwThis
			push    1
			push    pVehicleInt
	        
			call    dwFunc
			mov     bReturn, al
		}
	}
	return bReturn;
}

bool CVehicleSA::BreakTowLink ( void )
{
	DEBUG_TRACE("bool CVehicleSA::BreakTowLink ( void )");
    DWORD dwThis = (DWORD) GetInterface();

    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(this->GetInterface()->vtbl);
    DWORD dwFunc = vehicleVTBL->BreakTowLink;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis       
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

CVehicle * CVehicleSA::GetTowedVehicle ( void )
{
	DEBUG_TRACE("CVehicle * CVehicleSA::GetTowedVehicle ( void )");
    CVehicleSAInterface * pTowedVehicle = (CVehicleSAInterface *)*(DWORD *)((DWORD)this->GetInterface() + 1224);
    if ( pTowedVehicle )
        return pGame->GetPools()->GetVehicle ( (DWORD *)pTowedVehicle );
    return NULL;
}

CVehicle * CVehicleSA::GetTowedByVehicle ( void )
{
	DEBUG_TRACE("CVehicle * CVehicleSA::GetTowedVehicle ( void )");
    CVehicleSAInterface * pTowedVehicle = (CVehicleSAInterface *)*(DWORD *)((DWORD)this->GetInterface() + 1220);
    if ( pTowedVehicle )
        return pGame->GetPools()->GetVehicle ( (DWORD *)pTowedVehicle );
    return NULL;
}

void CVehicleSA::SetWinchType ( eWinchType winchType )
{
    if ( winchType < 4 && winchType != 2 )
    {
        GetVehicleInterface ()->WinchType = winchType;
    }
}

void CVehicleSA::PickupEntityWithWinch ( CEntity* pEntity )
{
    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );

	if ( pEntitySA )
	{
		DWORD dwFunc = FUNC_CVehicle_PickUpEntityWithWinch;
		DWORD dwThis = (DWORD) GetInterface();
		DWORD dwEntityInterface = (DWORD) pEntitySA->GetInterface ();

		_asm
		{
			push    dwEntityInterface
			mov     ecx, dwThis
			call    dwFunc
		}
	}
}

void CVehicleSA::ReleasePickedUpEntityWithWinch ( void )
{
    DWORD dwFunc = FUNC_CVehicle_ReleasePickedUpEntityWithWinch;
    DWORD dwThis = (DWORD) GetInterface();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CVehicleSA::SetRopeHeightForHeli ( float fRopeHeight )
{
    DWORD dwFunc = FUNC_CVehicle_SetRopeHeightForHeli;
    DWORD dwThis = (DWORD) GetInterface();

    _asm
    {
        push    fRopeHeight
        mov     ecx, dwThis
        call    dwFunc
    }
}

CPhysical * CVehicleSA::QueryPickedUpEntityWithWinch ( )
{
    DWORD dwFunc = FUNC_CVehicle_QueryPickedUpEntityWithWinch;
    DWORD dwThis = (DWORD) GetInterface();

    CPhysicalSAInterface * phys;
    CPhysical * physRet;
    _asm
    {
        mov     ecx, dwThis       
        call    dwFunc
        mov     phys, eax
    }

    if ( phys )
    {
        CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
	    switch(phys->nType)
	    {
	    case ENTITY_TYPE_PED:
		    physRet = (CPhysical *)pPools->GetPed((DWORD *)phys);
		    break;
	    case ENTITY_TYPE_VEHICLE:
		    physRet = (CPhysical *)pGame->GetPools()->GetVehicle ( (DWORD *)phys );
            break;
	    case ENTITY_TYPE_OBJECT:
		    physRet = (CPhysical *)pPools->GetObject( (DWORD *)phys );
		    break;
	    default:
		    physRet = NULL;
	    }
    }
    return physRet;
}

void CVehicleSA::SetRemap ( int iRemap )
{
    DWORD dwFunc = FUNC_CVehicle__SetRemap;
    DWORD dwThis = (DWORD) GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iRemap
        call    dwFunc
    }
}


int CVehicleSA::GetRemapIndex ( void )
{
    DWORD dwFunc = FUNC_CVehicle__GetRemapIndex;
    DWORD dwThis = (DWORD) GetInterface();
    int iReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     iReturn, eax
    }
    return iReturn;
}

void CVehicleSA::SetRemapTexDictionary ( int iRemapTextureDictionary )
{
    DWORD dwFunc = FUNC_CVehicle__SetRemapTexDictionary;
    DWORD dwThis = (DWORD) GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iRemapTextureDictionary
        call    dwFunc
    }
}

bool CVehicleSA::IsSmokeTrailEnabled ( void )
{
    return ( *(unsigned char*)((DWORD)this->GetInterface() + 2560) == 1 );
}

void CVehicleSA::SetSmokeTrailEnabled ( bool bEnabled )
{
    *(unsigned char*)((DWORD)this->GetInterface() + 2560) = ( bEnabled ) ? 1 : 0;
}


CHandlingEntry* CVehicleSA::GetHandlingData ( void )
{
    return m_pHandlingData;
}


void CVehicleSA::SetHandlingData ( CHandlingEntry* pHandling )
{
    /*
    // Store the handling data we set for later retrival through Get
    m_pHandlingData = static_cast < CHandlingEntrySA* > ( pHandling );

    // Put it in our interface
    GetVehicleInterface ()->pHandlingData = m_pHandlingData->GetInterface ();
    */
}


void CVehicleSA::BurstTyre ( BYTE bTyre )
{
    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(m_pInterface->vtbl);
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = vehicleVTBL->BurstTyre;
    _asm
    {
        mov         ecx, dwThis
        push        1 // not used
        push        bTyre
        call        dwFunc
    }
}


BYTE CVehicleSA::GetBikeWheelStatus ( BYTE bWheel )
{
    if ( bWheel == 0 ) return * ( BYTE * ) ( (DWORD)this->GetInterface() + 0x65C );
    if ( bWheel == 1 ) return * ( BYTE * ) ( (DWORD)this->GetInterface() + 0x65D );
    return 0;
}


void CVehicleSA::SetBikeWheelStatus ( BYTE bWheel, BYTE bStatus )
{
    if ( bWheel == 0 ) * ( BYTE * ) ( (DWORD)this->GetInterface() + 0x65C ) = bStatus;
    else if ( bWheel == 1 ) * ( BYTE * ) ( (DWORD)this->GetInterface() + 0x65D ) = bStatus;
}

void CVehicleSA::SetTaxiLightOn ( bool bLightOn )
{
	DEBUG_TRACE("void CVehicleSA::SetTaxiLight ( bool bLightOn )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwState = (DWORD) bLightOn;
    DWORD dwFunc = FUNC_CAutomobile_SetTaxiLight;

    _asm
    {
        mov     ecx, dwThis
        push    dwState
        call    dwFunc
    }
}

void CVehicleSA::SetGravity ( const CVector* pvecGravity )
{
    if ( pGame->GetPools ()->GetPedFromRef ( 1 )->GetVehicle () == this )
    {
        CCam* pCam = pGame->GetCamera ()->GetCam ( pGame->GetCamera ()->GetActiveCam () );
        pCam->AdjustToNewGravity ( &m_vecGravity, pvecGravity );
    }

    m_vecGravity = *pvecGravity;
}
