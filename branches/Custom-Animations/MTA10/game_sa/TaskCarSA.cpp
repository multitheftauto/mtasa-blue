/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskCarSA.cpp
*  PURPOSE:     Car game tasks
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// ##############################################################################
// ## Name:    CTaskComplexEnterCar                                    
// ## Purpose: Makes the ped enter the specified vehicle
// ## Notes:   Shouldn't be used directly, use CTaskComplexEnterCarAsDriver or
// ##          CTaskComplexEnterCarAsPassenger instead
// ##############################################################################

CTaskComplexEnterCarSA::CTaskComplexEnterCarSA ( CVehicle* pTargetVehicle, const bool bAsDriver, const bool bQuitAfterOpeningDoor, const bool bQuitAfterDraggingPedOut, const bool bCarryOnAfterFallingOff ):CTaskComplexSA ( )
{

}

// ##############################################################################
// ## Name:    CTaskComplexEnterCarAsDriver                                    
// ## Purpose: Makes the ped enter the specified vehicle
// ##############################################################################

CTaskComplexEnterCarAsDriverSA::CTaskComplexEnterCarAsDriverSA ( CVehicle* pTargetVehicle):CTaskComplexEnterCarSA ( pTargetVehicle, true, false, false, false )
{
    DEBUG_TRACE("CTaskComplexEnterCarAsDriverSA::CTaskComplexEnterCarAsDriverSA(CVehicle* pTargetVehicle)");

    CVehicleSA* pTargetVehicleSA = dynamic_cast < CVehicleSA* > ( pTargetVehicle );

    if ( pTargetVehicleSA )
    {
        this->CreateTaskInterface(sizeof(CTaskComplexEnterCarAsDriverSAInterface));
        DWORD dwFunc = FUNC_CTaskComplexEnterCarAsDriver__Constructor;
        DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
        DWORD dwThisInterface = (DWORD)this->GetInterface();

        _asm
        {
            mov     ecx, dwThisInterface
            push    dwVehiclePtr
            call    dwFunc
        }
    } else {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}


// ##############################################################################
// ## Name:    CTaskComplexEnterCarAsPassenger
// ## Purpose: Makes the ped enter the specified vehicle as a passenger
// ##############################################################################

CTaskComplexEnterCarAsPassengerSA::CTaskComplexEnterCarAsPassengerSA ( CVehicle* pTargetVehicle, const int iTargetSeat, const bool bCarryOnAfterFallingOff ):CTaskComplexEnterCarSA ( pTargetVehicle, false, false, false, false )
{
    DEBUG_TRACE("CTaskComplexEnterCarAsPassengerSA::CTaskComplexEnterCarAsPassengerSA(CVehicle* pTargetVehicle)");

    CVehicleSA* pTargetVehicleSA = dynamic_cast < CVehicleSA* > ( pTargetVehicle );

    if ( pTargetVehicleSA )
    {
        this->CreateTaskInterface(sizeof(CTaskComplexEnterCarAsPassengerSAInterface));
        DWORD dwFunc = FUNC_CTaskComplexEnterCarAsPassenger__Constructor;
        DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface ();
        DWORD dwThisInterface = (DWORD)this->GetInterface();

        _asm
        {
            push    edx
            xor     edx, edx
            movzx   edx, bCarryOnAfterFallingOff
            mov     ecx, dwThisInterface
            push    edx
            push    iTargetSeat
            push    dwVehiclePtr
            call    dwFunc
            pop     edx
        }
    } else {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}

// ##############################################################################
// ## Name:    CTaskComplexEnterBoatAsDriver
// ## Purpose: Makes the ped enter the specified boat as the driver
// ##############################################################################

CTaskComplexEnterBoatAsDriverSA::CTaskComplexEnterBoatAsDriverSA ( CVehicle* pTargetVehicle ):CTaskComplexSA ( )
{
    DEBUG_TRACE("CTaskComplexEnterBoatAsDriverSA::CTaskComplexEnterBoatAsDriverSA(CVehicle* pTargetVehicle)");

    CVehicleSA* pTargetVehicleSA = dynamic_cast < CVehicleSA* > ( pTargetVehicle );

    if ( pTargetVehicleSA )
    {
        this->CreateTaskInterface(sizeof(CTaskComplexEnterBoatAsDriverSAInterface));
        DWORD dwFunc = FUNC_CTaskComplexEnterBoatAsDriver__Constructor;
        DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
        DWORD dwThisInterface = (DWORD)this->GetInterface();

        _asm
        {
            mov     ecx, dwThisInterface
            push    dwVehiclePtr
            call    dwFunc
        }
    } else {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}


// ##############################################################################
// ## Name:    CTaskComplexLeaveCar                                    
// ## Purpose: Makes the ped leave a specific vehicle
// ##############################################################################

CTaskComplexLeaveCarSA::CTaskComplexLeaveCarSA( CVehicle* pTargetVehicle, const int iTargetDoor, const int iDelayTime, const bool bSensibleLeaveCar, const bool bForceGetOut ):CTaskComplexSA ( )
{
    DEBUG_TRACE("CTaskComplexLeaveCarSA::CTaskComplexLeaveCarSA( CVehicle* pTargetVehicle, const int iTargetDoor, const int iDelayTime, const bool bSensibleLeaveCar, const bool bForceGetOut )");

    CVehicleSA* pTargetVehicleSA = dynamic_cast < CVehicleSA* > ( pTargetVehicle );

    if ( pTargetVehicleSA )
    {
        this->CreateTaskInterface(sizeof(CTaskComplexLeaveCarSAInterface));
        DWORD dwFunc = FUNC_CTaskComplexLeaveCar__Constructor;
        DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
        DWORD dwThisInterface = (DWORD)this->GetInterface();

        _asm
        {
            mov     ecx, dwThisInterface
            push    ebx
            xor     ebx, ebx
            movzx   ebx, bForceGetOut
            push    ebx
            movzx   ebx, bSensibleLeaveCar
            push    ebx
            push    iDelayTime
            push    iTargetDoor
            push    dwVehiclePtr
            call    dwFunc
            pop     ebx
        }
    } else {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}
