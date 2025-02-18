/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CCarEnterExit.h
 *  PURPOSE:     Car enter/exit handling interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CPed;
class CVector;
class CVehicle;

class CCarEnterExit
{
public:
    virtual bool GetNearestCarDoor(CPed* pPed, CVehicle* pVehicle, CVector* pVector, int* pDoor) = 0;
    virtual bool GetNearestCarPassengerDoor(CPed* pPed, CVehicle* pVehicle, CVector* pVector, int* pDoor, bool bUnknown, bool bUnknown2,
                                            bool bCheckIfRoomToGetIn) = 0;
    virtual int  ComputeTargetDoorToExit(CPed* pPed, CVehicle* pVehicle) = 0;
    virtual bool IsRoomForPedToLeaveCar(CVehicle* pVehicle, int iDoor, CVector* pUnknown = 0) = 0;

    virtual void GetPositionToOpenCarDoor(CVector& position, CVehicle* vehicle, std::uint32_t door) const noexcept = 0;
};
