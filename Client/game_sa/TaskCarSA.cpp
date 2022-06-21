/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskCarSA.cpp
 *  PURPOSE:     Car game tasks
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

CTaskComplexEnterCarSA::CTaskComplexEnterCarSA(CVehicle* pTargetVehicle, const bool bAsDriver, const bool bQuitAfterOpeningDoor,
                                               const bool bQuitAfterDraggingPedOut, const bool bCarryOnAfterFallingOff)
    : CTaskComplexSA()
{
}

// ##############################################################################
// ## Name:    CTaskComplexEnterCarAsDriver
// ## Purpose: Makes the ped enter the specified vehicle
// ##############################################################################

CTaskComplexEnterCarAsDriverSA::CTaskComplexEnterCarAsDriverSA(CVehicle* pTargetVehicle) : CTaskComplexEnterCarSA(pTargetVehicle, true, false, false, false)
{
    DEBUG_TRACE("CTaskComplexEnterCarAsDriverSA::CTaskComplexEnterCarAsDriverSA(CVehicle* pTargetVehicle)");

    CVehicleSA* pTargetVehicleSA = dynamic_cast<CVehicleSA*>(pTargetVehicle);

    if (pTargetVehicleSA)
    {
        this->CreateTaskInterface(sizeof(CTaskComplexEnterCarAsDriverSAInterface));
        if (!IsValid())
            return;

        // CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver
        ((CTaskSAInterface * (__thiscall*)(CTaskSAInterface*, CVehicleSAInterface*)) FUNC_CTaskComplexEnterCarAsDriver__Constructor)(
            GetInterface(), pTargetVehicleSA->GetVehicleInterface());
    }
    else
    {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}

// ##############################################################################
// ## Name:    CTaskComplexEnterCarAsPassenger
// ## Purpose: Makes the ped enter the specified vehicle as a passenger
// ##############################################################################

CTaskComplexEnterCarAsPassengerSA::CTaskComplexEnterCarAsPassengerSA(CVehicle* pTargetVehicle, const int iTargetSeat, const bool bCarryOnAfterFallingOff)
    : CTaskComplexEnterCarSA(pTargetVehicle, false, false, false, false)
{
    DEBUG_TRACE("CTaskComplexEnterCarAsPassengerSA::CTaskComplexEnterCarAsPassengerSA(CVehicle* pTargetVehicle)");

    CVehicleSA* pTargetVehicleSA = dynamic_cast<CVehicleSA*>(pTargetVehicle);

    if (pTargetVehicleSA)
    {
        this->CreateTaskInterface(sizeof(CTaskComplexEnterCarAsPassengerSAInterface));
        if (!IsValid())
            return;

        // CTaskComplexEnterCarAsPassenger::CTaskComplexEnterCarAsPassenger
        ((CTaskSAInterface * (__thiscall*)(CTaskSAInterface*, CVehicleSAInterface*, int, bool)) FUNC_CTaskComplexEnterCarAsPassenger__Constructor)(
            GetInterface(), pTargetVehicleSA->GetVehicleInterface(), iTargetSeat, bCarryOnAfterFallingOff);
    }
    else
    {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}

// ##############################################################################
// ## Name:    CTaskComplexEnterBoatAsDriver
// ## Purpose: Makes the ped enter the specified boat as the driver
// ##############################################################################

CTaskComplexEnterBoatAsDriverSA::CTaskComplexEnterBoatAsDriverSA(CVehicle* pTargetVehicle) : CTaskComplexSA()
{
    DEBUG_TRACE("CTaskComplexEnterBoatAsDriverSA::CTaskComplexEnterBoatAsDriverSA(CVehicle* pTargetVehicle)");

    CVehicleSA* pTargetVehicleSA = dynamic_cast<CVehicleSA*>(pTargetVehicle);

    if (pTargetVehicleSA)
    {
        this->CreateTaskInterface(sizeof(CTaskComplexEnterBoatAsDriverSAInterface));
        if (!IsValid())
            return;

        // CTaskComplexEnterBoatAsDriver::CTaskComplexEnterBoatAsDriver
        ((CTaskSAInterface * (__thiscall*)(CTaskSAInterface*, CVehicleSAInterface*)) FUNC_CTaskComplexEnterBoatAsDriver__Constructor)(
            GetInterface(), pTargetVehicleSA->GetVehicleInterface());
    }
    else
    {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}

// ##############################################################################
// ## Name:    CTaskComplexLeaveCar
// ## Purpose: Makes the ped leave a specific vehicle
// ##############################################################################

CTaskComplexLeaveCarSA::CTaskComplexLeaveCarSA(CVehicle* pTargetVehicle, const int iTargetDoor, const int iDelayTime, const bool bSensibleLeaveCar,
                                               const bool bForceGetOut)
    : CTaskComplexSA()
{
    DEBUG_TRACE(
        "CTaskComplexLeaveCarSA::CTaskComplexLeaveCarSA( CVehicle* pTargetVehicle, const int iTargetDoor, const int iDelayTime, const bool bSensibleLeaveCar, "
        "const bool bForceGetOut )");

    CVehicleSA* pTargetVehicleSA = dynamic_cast<CVehicleSA*>(pTargetVehicle);

    if (pTargetVehicleSA)
    {
        this->CreateTaskInterface(sizeof(CTaskComplexLeaveCarSAInterface));
        if (!IsValid())
            return;

        DWORD      dwDoorIdx = 0;
        static int s_iCarNodeIndexes[6] = {0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09};

        if (iTargetDoor >= 0 && iTargetDoor <= 5)
            dwDoorIdx = s_iCarNodeIndexes[iTargetDoor];

        // CTaskComplexLeaveCar::CTaskComplexLeaveCar
        ((CTaskSAInterface * (__thiscall*)(CTaskSAInterface*, CVehicleSAInterface*, int, int, bool, bool)) FUNC_CTaskComplexLeaveCar__Constructor)(
            GetInterface(), pTargetVehicleSA->GetVehicleInterface(), dwDoorIdx, iDelayTime, bSensibleLeaveCar, bForceGetOut);
    }
    else
    {
        DEBUG_TRACE("pTargetVehicleSA was invalid!");
    }
}
