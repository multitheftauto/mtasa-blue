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
