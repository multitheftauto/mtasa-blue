/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedIntelligenceSA.cpp
 *  PURPOSE:     Ped entity AI logic
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CPedIntelligenceSA::CPedIntelligenceSA(CPedIntelligenceSAInterface* pedIntelligenceSAInterface, CPed* ped)
{
    this->internalInterface = pedIntelligenceSAInterface;
    this->ped = ped;
    CTaskManagerSAInterface* pTaskManagerInterface = (CTaskManagerSAInterface*)&(pedIntelligenceSAInterface->taskManager);
    this->TaskManager = new CTaskManagerSA(pTaskManagerInterface, this->ped);
    CVehicleScannerSAInterface* pVehicleScannerInterface = (CVehicleScannerSAInterface*)&(pedIntelligenceSAInterface->vehicleScanner);
    this->VehicleScanner = new CVehicleScannerSA(pVehicleScannerInterface);
}

CPedIntelligenceSA::~CPedIntelligenceSA()
{
    delete this->TaskManager;
}

CTaskManager* CPedIntelligenceSA::GetTaskManager()
{
    DEBUG_TRACE("CTaskManager * CPedSA::GetTaskManager( void )");
    return this->TaskManager;
}

CVehicleScanner* CPedIntelligenceSA::GetVehicleScanner()
{
    return this->VehicleScanner;
}

bool CPedIntelligenceSA::IsRespondingToEvent()
{
    DWORD dwFunc = FUNC_IsRespondingToEvent;

    return false;
}

int CPedIntelligenceSA::GetCurrentEventType()
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

CEvent* CPedIntelligenceSA::GetCurrentEvent()
{
    return NULL;
}

bool CPedIntelligenceSA::TestForStealthKill(CPed* pPed, bool bUnk)
{
    bool  bReturn;
    DWORD dwThis = (DWORD)internalInterface;
    DWORD dwPed = (DWORD)pPed->GetInterface();
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

CTaskSimpleUseGunSAInterface* CPedIntelligenceSA::GetTaskUseGun()
{
    CTaskSimpleUseGunSAInterface* pTaskUseGun;
    DWORD                         dwThis = (DWORD)internalInterface;
    DWORD                         dwFunc = FUNC_CPedIntelligence_GetTaskUseGun;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     pTaskUseGun, eax
    }

    return pTaskUseGun;
}

CTaskSAInterface* CPedIntelligenceSA::SetTaskDuckSecondary(unsigned short nLengthOfDuck)
{
    auto SetTaskDuckSecondary = (CTaskSAInterface * (__thiscall*)(CPedIntelligenceSAInterface*, unsigned short))0x601230;
    return SetTaskDuckSecondary(internalInterface, nLengthOfDuck);
}
