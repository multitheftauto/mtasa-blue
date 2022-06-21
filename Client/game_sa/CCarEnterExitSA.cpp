/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCarEnterExitSA.cpp
 *  PURPOSE:     Car enter and exit handler
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

bool CCarEnterExitSA::GetNearestCarDoor(CPed* pPed, CVehicle* pVehicle, CVector* pVector, int* pDoor)
{
    CPedSA*     pPedSA = dynamic_cast<CPedSA*>(pPed);
    CVehicleSA* pVehicleSA = dynamic_cast<CVehicleSA*>(pVehicle);

    if (pPedSA && pVehicleSA)
    {
        // CCarEnterExit::GetNearestCarDoor
        return ((bool(__cdecl*)(CPedSAInterface*, CVehicleSAInterface*, CVector*, int*))(FUNC_GetNearestCarDoor))(
            pPedSA->GetPedInterface(), pVehicleSA->GetVehicleInterface(), pVector, pDoor);
    }

    return false;
}

bool CCarEnterExitSA::GetNearestCarPassengerDoor(CPed* pPed, CVehicle* pVehicle, CVector* pVector, int* pDoor, bool bUnknown, bool bUnknown2,
                                                 bool bCheckIfRoomToGetIn)
{
    CPedSA*     pPedSA = dynamic_cast<CPedSA*>(pPed);
    CVehicleSA* pVehicleSA = dynamic_cast<CVehicleSA*>(pVehicle);

    if (pPedSA && pVehicleSA)
    {
        // CCarEnterExit::GetNearestCarPassengerDoor
        return ((bool(__cdecl*)(CPedSAInterface*, CVehicleSAInterface*, CVector*, int*, bool, bool, bool))(FUNC_GetNearestCarPassengerDoor))(
            pPedSA->GetPedInterface(), pVehicleSA->GetVehicleInterface(), pVector, pDoor, bUnknown, bUnknown2, bCheckIfRoomToGetIn);
    }

    return false;
}

int CCarEnterExitSA::ComputeTargetDoorToExit(CPed* pPed, CVehicle* pVehicle)
{
    int   door = -1;

    CPedSA*     pPedSA = dynamic_cast<CPedSA*>(pPed);
    CVehicleSA* pVehicleSA = dynamic_cast<CVehicleSA*>(pVehicle);

    if (pPedSA && pVehicleSA)
    {
        // CCarEnterExit::ComputeTargetDoorToExit
        door = ((int(__cdecl*)(CVehicleSAInterface*, CPedSAInterface*))(FUNC_ComputeTargetDoorToExit))(pVehicleSA->GetVehicleInterface(),
                                                                                                      pPedSA->GetPedInterface());

        switch (door)
        {
            case 0x10:
                door = 0;
                break;
            case 0x11:
                door = 1;
                break;
            case 0x0A:
                door = 2;
                break;
            case 0x08:
                door = 3;
                break;
            case 0x0B:
                door = 4;
                break;
            case 0x09:
                door = 5;
                break;
            default:
                door = -1;
                break;
        }

        if (door >= 2 && door <= 5)
        {
            // Try to find a door with room to leave
            if (IsRoomForPedToLeaveCar(pVehicle, door, 0) == false)
            {
                int newDoor = -1;
                switch (door)
                {
                    case 2:
                        if (!pVehicle->GetPassenger(1))
                            newDoor = 3;
                        break;
                    case 3:
                        if (!pVehicle->GetPassenger(0))
                            newDoor = 2;
                        break;
                    case 4:
                        if (!pVehicle->GetPassenger(3))
                            newDoor = 5;
                        break;
                    case 5:
                        if (!pVehicle->GetPassenger(2))
                            newDoor = 4;
                        break;
                }

                if (newDoor != -1 && IsRoomForPedToLeaveCar(pVehicle, newDoor, 0) == true)
                    door = newDoor;
            }
        }
    }

    return door;
}

bool CCarEnterExitSA::IsRoomForPedToLeaveCar(CVehicle* pVehicle, int iDoor, CVector* pUnknown)
{
    if (iDoor >= 0 && iDoor <= 5)
    {
        static int  s_iCarNodeIndexes[6] = {0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09};
        CVehicleSA* pVehicleSA = dynamic_cast<CVehicleSA*>(pVehicle);
        DWORD       dwIdx = s_iCarNodeIndexes[iDoor];
        if (pVehicleSA)
        {
            // CCarEnterExit::IsRoomForPedToLeaveCar
            return ((bool(__cdecl*)(CVehicleSAInterface*, int, CVector*))(FUNC_IsRoomForPedToLeaveCar))(pVehicleSA->GetVehicleInterface(), dwIdx, pUnknown);
        }
    }

    return true;
}
