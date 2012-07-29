/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedIntelligenceSA.cpp
*  PURPOSE:     Ped entity AI logic
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPedIntelligenceSA::CPedIntelligenceSA ( CPedIntelligenceSAInterface * pedIntelligenceSAInterface, CPed * ped )
{
    this->internalInterface = pedIntelligenceSAInterface;
    this->ped = ped;
    CTaskManagerSAInterface * pTaskManagerInterface = (CTaskManagerSAInterface * )&(pedIntelligenceSAInterface->taskManager);
    this->TaskManager = new CTaskManagerSA(pTaskManagerInterface, this->ped );
    CVehicleScannerSAInterface * pVehicleScannerInterface = (CVehicleScannerSAInterface *)&(pedIntelligenceSAInterface->vehicleScanner);
    this->VehicleScanner = new CVehicleScannerSA(pVehicleScannerInterface);
}

CPedIntelligenceSA::~CPedIntelligenceSA ()
{
    delete this->TaskManager;
}

CTaskManager * CPedIntelligenceSA::GetTaskManager( void )
{
    DEBUG_TRACE("CTaskManager * CPedSA::GetTaskManager( void )");
    return this->TaskManager;
}

CVehicleScanner * CPedIntelligenceSA::GetVehicleScanner( void )
{
    return this->VehicleScanner;
}

bool CPedIntelligenceSA::IsRespondingToEvent ( void )
{
    DWORD dwFunc = FUNC_IsRespondingToEvent;

        
    return false;
}

int CPedIntelligenceSA::GetCurrentEventType ( void )
{
    DWORD dwFunc = FUNC_GetCurrentEventType;
    DWORD dwRet = 0;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwRet, eax
    }
    return dwRet;
}

CEvent * CPedIntelligenceSA::GetCurrentEvent ( void )
{
    return NULL;
}


bool CPedIntelligenceSA::TestForStealthKill ( CPed * pPed, bool bUnk )
{
    bool bReturn;
    DWORD dwThis = ( DWORD ) internalInterface;
    DWORD dwPed = ( DWORD ) pPed->GetInterface ();
    DWORD dwFunc = FUNC_CPedIntelligence_TestForStealthKill;
    _asm
    {
        mov     ecx, dwThis
        push    bUnk
        push    dwPed
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


CTaskSimpleUseGunSAInterface* CPedIntelligenceSA::GetTaskUseGun ( void )
{
    CTaskSimpleUseGunSAInterface* pTaskUseGun;
    DWORD dwThis = ( DWORD ) internalInterface;
    DWORD dwFunc = FUNC_CPedIntelligence_GetTaskUseGun;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     pTaskUseGun, eax
    }

    return pTaskUseGun;
}


void CPedIntelligenceSA::DischargeCurrentWeapon ( bool bAllowReloadAnimation )
{
    // Get use gun task if available
    CTaskSimpleUseGunSAInterface* pTaskUseGun = GetTaskUseGun ();

    g_pCore->LogEvent ( 10211, SString ( "CPedIntelligenceSA::DischargeCurrentWeapon - [ped:%s]  pTaskUseGun:%08x"
                                        , *g_pCore->GetEntityDesc ( NULL, ped->GetInterface () )
                                        , pTaskUseGun
                                    ) );

    if ( !pTaskUseGun )
        return;

    if ( !bAllowReloadAnimation )
    {
        // Modify ammo to avoid reload animation
        CWeapon* pWeapon = ped->GetWeapon ( ped->GetCurrentWeaponSlot () );
        uint uiAmmoInClip = pWeapon->GetAmmoInClip ();
        if ( uiAmmoInClip < 2 )
        {
            CWeaponInfoSAInterface* pWeaponInfo = (CWeaponInfoSAInterface*)pTaskUseGun->m_pWeaponInfo;
            pWeapon->SetAmmoInClip ( uiAmmoInClip + pWeaponInfo->m_nAmmo );
        }
    }

    // Flag for shot
    pTaskUseGun->m_nFireGunThisFrame = 1;

    // Do shot
    CPedSAInterface* dwPedInterface = ( CPedSAInterface* ) ped->GetInterface ();
    DWORD dwFunc = FUNC_CTaskSimpleUseGun_SetPedPosition;
    _asm
    {
        mov     ecx, pTaskUseGun
        push    dwPedInterface
        call    dwFunc
    }
}
