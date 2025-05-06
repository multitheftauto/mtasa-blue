/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCarEnterExitSA.h
 *  PURPOSE:     Header file for car enter and exit handler class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CCarEnterExit.h>

#define FUNC_GetNearestCarDoor                  0x6528F0
#define FUNC_GetNearestCarPassengerDoor         0x650BB0
#define FUNC_ComputeTargetDoorToExit            0x64F110
#define FUNC_IsRoomForPedToLeaveCar             0x6504C0
#define FUNC_GetPositionToOpenCarDoor           0x64E740

class CCarEnterExitSA : public CCarEnterExit
{
public:
    bool GetNearestCarDoor(CPed* pPed, CVehicle* pVehicle, CVector* pVector, int* pDoor);
    bool GetNearestCarPassengerDoor(CPed* pPed, CVehicle* pVehicle, CVector* pVector, int* pDoor, bool bUnknown, bool bUnknown2, bool bCheckIfRoomToGetIn);
    void GetPositionToOpenCarDoor(CVector& position, CVehicle* vehicle, std::uint32_t door) const noexcept;
    int  ComputeTargetDoorToExit(CPed* pPed, CVehicle* pVehicle);
    bool IsRoomForPedToLeaveCar(CVehicle* pVehicle, int iDoor, CVector* pUnknown = 0);
};
