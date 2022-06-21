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
    // CPedIntelligence::GetCurrentEventType
    return ((int(__thiscall*)(CPedIntelligenceSAInterface*))FUNC_GetCurrentEventType)(this->GetInterface());
}

CEvent* CPedIntelligenceSA::GetCurrentEvent()
{
    return NULL;
}

bool CPedIntelligenceSA::TestForStealthKill(CPed* pPed, bool bUnk)
{
    // CPedIntelligence::TestForStealthKill
    return ((bool(__thiscall*)(CPedIntelligenceSAInterface*, CPedSAInterface*, bool))FUNC_CPedIntelligence_TestForStealthKill)(this->GetInterface(),
                                                                                                                               pPed->GetPedInterface(), bUnk);
}

CTaskSimpleUseGunSAInterface* CPedIntelligenceSA::GetTaskUseGun()
{
    // CPedIntelligence::GetTaskUseGun
    return ((CTaskSimpleUseGunSAInterface*(__thiscall*)(CPedIntelligenceSAInterface*))FUNC_CPedIntelligence_GetTaskUseGun)(this->GetInterface());
}

CTaskSAInterface* CPedIntelligenceSA::SetTaskDuckSecondary(unsigned short nLengthOfDuck)
{
    auto SetTaskDuckSecondary = (CTaskSAInterface * (__thiscall*)(CPedIntelligenceSAInterface*, unsigned short))0x601230;
    return SetTaskDuckSecondary(internalInterface, nLengthOfDuck);
}
