/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskCarAccessoriesSA.cpp
*  PURPOSE:     Car accessories game tasks
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedInAsDriver                                    
// ## Purpose: Puts the specified ped directly into a vehicle
// ##############################################################################

CTaskSimpleCarSetPedInAsDriverSA::CTaskSimpleCarSetPedInAsDriverSA(CVehicle* pTargetVehicle, CTaskUtilityLineUpPedWithCar* pUtility)
{
    DEBUG_TRACE("CTaskSimpleCarSetPedInAsDriverSA::CTaskSimpleCarSetPedInAsDriverSA(CVehicle* pTargetVehicle, CTaskUtilityLineUpPedWithCar* pUtility)");

    CVehicleSA* pTargetVehicleSA = dynamic_cast < CVehicleSA* > ( pTargetVehicle );

    if ( pTargetVehicleSA )
    {
        this->CreateTaskInterface(sizeof(CTaskSimpleCarSetPedInAsDriverSAInterface));
        if ( !IsValid () ) return;
        DWORD dwFunc = FUNC_CTaskSimpleCarSetPedInAsDriver__Constructor;
        DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
        DWORD dwThisInterface = (DWORD)this->GetInterface();

        _asm
        {
            mov     ecx, dwThisInterface
            push    pUtility
            push    dwVehiclePtr
            call    dwFunc
        }
    } else {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}


void CTaskSimpleCarSetPedInAsDriverSA::SetIsWarpingPedIntoCar()
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsDriverSA::SetIsWarpingPedIntoCar()");
    ((CTaskSimpleCarSetPedInAsDriverSAInterface *)this->GetInterface())->m_bWarpingInToCar=true;
}

void CTaskSimpleCarSetPedInAsDriverSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsDriverSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear) ");
    ((CTaskSimpleCarSetPedInAsDriverSAInterface *)this->GetInterface())->m_nDoorFlagsToClear=nDoorFlagsToClear;
}

void CTaskSimpleCarSetPedInAsDriverSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsDriverSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear) ");
    ((CTaskSimpleCarSetPedInAsDriverSAInterface *)this->GetInterface())->m_nNumGettingInToClear=nNumGettingInToClear;
}

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedInAsPassenger                                    
// ## Purpose: Puts the specified ped directly into a vehicle as a passenger
// ##############################################################################

CTaskSimpleCarSetPedInAsPassengerSA::CTaskSimpleCarSetPedInAsPassengerSA(CVehicle* pTargetVehicle, int iTargetDoor, CTaskUtilityLineUpPedWithCar* pUtility)
{
    DEBUG_TRACE("CTaskSimpleCarSetPedInAsPassengerSA::CTaskSimpleCarSetPedInAsPassengerSA(CVehicle* pTargetVehicle, int iTargetDoor, CTaskUtilityLineUpPedWithCar* pUtility)");

    CVehicleSA* pTargetVehicleSA = dynamic_cast < CVehicleSA* > ( pTargetVehicle );

    if ( pTargetVehicleSA )
    {
        this->CreateTaskInterface(sizeof(CTaskSimpleCarSetPedInAsPassengerSAInterface));
        if ( !IsValid () ) return;
        DWORD dwFunc = FUNC_CTaskSimpleCarSetPedInAsPassenger__Constructor;
        DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
        DWORD dwThisInterface = (DWORD)this->GetInterface();
        _asm
        {
            mov     ecx, dwThisInterface
            push    pUtility
            push    iTargetDoor
            push    dwVehiclePtr
            call    dwFunc
        }
    } else {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}


void CTaskSimpleCarSetPedInAsPassengerSA::SetIsWarpingPedIntoCar()
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsPassengerSA::SetIsWarpingPedIntoCar()");
    ((CTaskSimpleCarSetPedInAsPassengerSAInterface *)this->GetInterface())->m_bWarpingInToCar=true;
}

void CTaskSimpleCarSetPedInAsPassengerSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsPassengerSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear) ");
    ((CTaskSimpleCarSetPedInAsPassengerSAInterface *)this->GetInterface())->m_nDoorFlagsToClear=nDoorFlagsToClear;
}

void CTaskSimpleCarSetPedInAsPassengerSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsPassengerSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear) ");
    ((CTaskSimpleCarSetPedInAsPassengerSAInterface *)this->GetInterface())->m_nNumGettingInToClear=nNumGettingInToClear;
}

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedOut                                    
// ## Purpose: Instantly removes the ped from the vehicle specified
// ##############################################################################

CTaskSimpleCarSetPedOutSA::CTaskSimpleCarSetPedOutSA(CVehicle* pTargetVehicle, int iTargetDoor, bool bSwitchOffEngine)
{
    DEBUG_TRACE("CTaskSimpleCarSetPedOutSA::CTaskSimpleCarSetPedOutSA(CVehicle* pTargetVehicle, int iTargetDoor, bool bSwitchOffEngine)");

    CVehicleSA* pTargetVehicleSA = dynamic_cast < CVehicleSA* > ( pTargetVehicle );

    if ( pTargetVehicleSA )
    {
        this->CreateTaskInterface(sizeof(CTaskSimpleCarSetPedOutSAInterface));
        if ( !IsValid () ) return;
        DWORD dwFunc = FUNC_CTaskSimpleCarSetPedOut__Constructor;
        DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
        DWORD dwThisInterface = (DWORD)this->GetInterface();
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
    } else {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}

void CTaskSimpleCarSetPedOutSA::SetIsWarpingPedOutOfCar()
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedOutSA::SetIsWarpingPedOutOfCar()");
    ((CTaskSimpleCarSetPedOutSAInterface *)this->GetInterface())->m_bWarpingOutOfCar=true;
}

void CTaskSimpleCarSetPedOutSA::SetKnockedOffBike() 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedOutSA::SetKnockedOffBike() ");
    ((CTaskSimpleCarSetPedOutSAInterface*)this->GetInterface())->m_bKnockedOffBike=true;
    ((CTaskSimpleCarSetPedOutSAInterface*)this->GetInterface())->m_bSwitchOffEngine=false;
}

void CTaskSimpleCarSetPedOutSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedOutSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear) ");
    ((CTaskSimpleCarSetPedOutSAInterface *)this->GetInterface())->m_nDoorFlagsToClear=nDoorFlagsToClear;
}

void CTaskSimpleCarSetPedOutSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedOutSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear) ");
    ((CTaskSimpleCarSetPedOutSAInterface *)this->GetInterface())->m_nNumGettingInToClear=nNumGettingInToClear;
}

void CTaskSimpleCarSetPedOutSA::PositionPedOutOfCollision(CPed * ped, CVehicle * vehicle, int nDoor)
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedOutSA::PositionPedOutOfCollision(CPed * ped, CVehicle * vehicle, int nDoor)");
    DWORD dwFunc = FUNC_CTaskSimpleCarSetPedOut__PositionPedOutOfCollision;
    DWORD dwVehiclePtr = (DWORD)((CEntitySA *)vehicle)->GetInterface();
    DWORD dwPedPtr = (DWORD)((CEntitySA *)ped)->GetInterface();
    _asm
    {
        push    nDoor
        push    dwVehiclePtr
        push    dwPedPtr
        call    dwFunc
    }
}
