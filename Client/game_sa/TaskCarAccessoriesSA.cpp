/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskCarAccessoriesSA.cpp
 *  PURPOSE:     Car accessories game tasks
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "TaskCarAccessoriesSA.h"

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedInAsDriver
// ## Purpose: Puts the specified ped directly into a vehicle
// ##############################################################################

CTaskSimpleCarSetPedInAsDriverSA::CTaskSimpleCarSetPedInAsDriverSA(CVehicle* pTargetVehicle, CTaskUtilityLineUpPedWithCar* pUtility)
{
    CVehicleSA* pTargetVehicleSA = dynamic_cast<CVehicleSA*>(pTargetVehicle);

    if (pTargetVehicleSA)
    {
        CreateTaskInterface(sizeof(CTaskSimpleCarSetPedInAsDriverSAInterface));
        if (!IsValid())
            return;
        DWORD dwFunc = FUNC_CTaskSimpleCarSetPedInAsDriver__Constructor;
        DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
        DWORD dwThisInterface = (DWORD)GetInterface();

        _asm
        {
            mov     ecx, dwThisInterface
            push    pUtility
            push    dwVehiclePtr
            call    dwFunc
        }
    }
}

void CTaskSimpleCarSetPedInAsDriverSA::SetIsWarpingPedIntoCar()
{
    ((CTaskSimpleCarSetPedInAsDriverSAInterface*)GetInterface())->m_bWarpingInToCar = true;
}

void CTaskSimpleCarSetPedInAsDriverSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear)
{
    ((CTaskSimpleCarSetPedInAsDriverSAInterface*)GetInterface())->m_nDoorFlagsToClear = nDoorFlagsToClear;
}

void CTaskSimpleCarSetPedInAsDriverSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear)
{
    ((CTaskSimpleCarSetPedInAsDriverSAInterface*)GetInterface())->m_nNumGettingInToClear = nNumGettingInToClear;
}

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedInAsPassenger
// ## Purpose: Puts the specified ped directly into a vehicle as a passenger
// ##############################################################################

CTaskSimpleCarSetPedInAsPassengerSA::CTaskSimpleCarSetPedInAsPassengerSA(CVehicle* pTargetVehicle, int iTargetDoor, CTaskUtilityLineUpPedWithCar* pUtility)
{
    CVehicleSA* pTargetVehicleSA = dynamic_cast<CVehicleSA*>(pTargetVehicle);

    if (pTargetVehicleSA)
    {
        CreateTaskInterface(sizeof(CTaskSimpleCarSetPedInAsPassengerSAInterface));
        if (!IsValid())
            return;
        DWORD dwFunc = FUNC_CTaskSimpleCarSetPedInAsPassenger__Constructor;
        DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
        DWORD dwThisInterface = (DWORD)GetInterface();
        _asm
        {
            mov     ecx, dwThisInterface
            push    pUtility
            push    iTargetDoor
            push    dwVehiclePtr
            call    dwFunc
        }
    }
}

void CTaskSimpleCarSetPedInAsPassengerSA::SetIsWarpingPedIntoCar()
{
    ((CTaskSimpleCarSetPedInAsPassengerSAInterface*)GetInterface())->m_bWarpingInToCar = true;
}

void CTaskSimpleCarSetPedInAsPassengerSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear)
{
    ((CTaskSimpleCarSetPedInAsPassengerSAInterface*)GetInterface())->m_nDoorFlagsToClear = nDoorFlagsToClear;
}

void CTaskSimpleCarSetPedInAsPassengerSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear)
{
    ((CTaskSimpleCarSetPedInAsPassengerSAInterface*)GetInterface())->m_nNumGettingInToClear = nNumGettingInToClear;
}

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedOut
// ## Purpose: Instantly removes the ped from the vehicle specified
// ##############################################################################

CTaskSimpleCarSetPedOutSA::CTaskSimpleCarSetPedOutSA(CVehicle* pTargetVehicle, int iTargetDoor, bool bSwitchOffEngine)
{
    CVehicleSA* pTargetVehicleSA = dynamic_cast<CVehicleSA*>(pTargetVehicle);

    if (pTargetVehicleSA)
    {
        CreateTaskInterface(sizeof(CTaskSimpleCarSetPedOutSAInterface));
        if (!IsValid())
            return;
        DWORD dwFunc = FUNC_CTaskSimpleCarSetPedOut__Constructor;
        DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
        DWORD dwThisInterface = (DWORD)GetInterface();
        _asm
        {
            mov     ecx, dwThisInterface
            xor     eax, eax
            movzx   eax, bSwitchOffEngine
            push    eax
            push    iTargetDoor
            push    dwVehiclePtr
            call    dwFunc
        }
    }
}

void CTaskSimpleCarSetPedOutSA::SetIsWarpingPedOutOfCar()
{
    ((CTaskSimpleCarSetPedOutSAInterface*)GetInterface())->m_bWarpingOutOfCar = true;
}

void CTaskSimpleCarSetPedOutSA::SetKnockedOffBike()
{
    ((CTaskSimpleCarSetPedOutSAInterface*)GetInterface())->m_bKnockedOffBike = true;
    ((CTaskSimpleCarSetPedOutSAInterface*)GetInterface())->m_bSwitchOffEngine = false;
}

void CTaskSimpleCarSetPedOutSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear)
{
    ((CTaskSimpleCarSetPedOutSAInterface*)GetInterface())->m_nDoorFlagsToClear = nDoorFlagsToClear;
}

void CTaskSimpleCarSetPedOutSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear)
{
    ((CTaskSimpleCarSetPedOutSAInterface*)GetInterface())->m_nNumGettingInToClear = nNumGettingInToClear;
}

void CTaskSimpleCarSetPedOutSA::PositionPedOutOfCollision(CPed* ped, CVehicle* vehicle, int nDoor)
{
    DWORD dwFunc = FUNC_CTaskSimpleCarSetPedOut__PositionPedOutOfCollision;
    DWORD dwVehiclePtr = (DWORD)((CEntitySA*)vehicle)->GetInterface();
    DWORD dwPedPtr = (DWORD)((CEntitySA*)ped)->GetInterface();
    _asm
    {
        push    nDoor
        push    dwVehiclePtr
        push    dwPedPtr
        call    dwFunc
    }
}
