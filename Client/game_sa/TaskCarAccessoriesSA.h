/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskCarAccessoriesSA.h
 *  PURPOSE:     Car accessories game tasks
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/TaskCarAccessories.h>

#include "CVehicleSA.h"
#include "TaskSA.h"

// temporary
class CAnimBlendAssociation;
typedef DWORD CTaskUtilityLineUpPedWithCar;

#define FUNC_CTaskSimpleCarSetPedInAsDriver__Constructor            0x6470E0
#define FUNC_CTaskSimpleCarSetPedInAsPassenger__Constructor         0x646FE0
#define FUNC_CTaskSimpleCarSetPedOut__Constructor                   0x6478B0

#define FUNC_CTaskSimpleCarSetPedOut__PositionPedOutOfCollision     0x6479B0

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedInAsDriver
// ## Purpose: Puts the specified ped directly into a vehicle
// ## Notes:   Can cause crash if multiplayer dll isn't used and CPlayerPeds are
// ##############################################################################

class CTaskSimpleCarSetPedInAsDriverSAInterface : public CTaskSimpleSAInterface
{
public:
    bool                          m_bIsFinished;
    CAnimBlendAssociation*        m_pAnim;
    CVehicleSAInterface*          m_pTargetVehicle;
    CTaskUtilityLineUpPedWithCar* m_pUtility;
    bool                          m_bWarpingInToCar;
    unsigned char                 m_nDoorFlagsToClear;
    unsigned char                 m_nNumGettingInToClear;
};

class CTaskSimpleCarSetPedInAsDriverSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleCarSetPedInAsDriver
{
public:
    CTaskSimpleCarSetPedInAsDriverSA(){};
    CTaskSimpleCarSetPedInAsDriverSA(CVehicle* pTargetVehicle, CTaskUtilityLineUpPedWithCar* pUtility);

    void SetIsWarpingPedIntoCar();
    void SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear);
    void SetNumGettingInToClear(const unsigned char nNumGettingInToClear);
};

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedInAsPassenger
// ## Purpose: Puts the specified ped directly into a vehicle as a passenger
// ##############################################################################

class CTaskSimpleCarSetPedInAsPassengerSAInterface : public CTaskSimpleSAInterface
{
public:
    bool                          m_bIsFinished;
    CAnimBlendAssociation*        m_pAnim;
    CVehicleSAInterface*          m_pTargetVehicle;
    int                           m_iTargetDoor;
    CTaskUtilityLineUpPedWithCar* m_pUtility;
    bool                          m_bWarpingInToCar;
    unsigned char                 m_nDoorFlagsToClear;
    unsigned char                 m_nNumGettingInToClear;
};

class CTaskSimpleCarSetPedInAsPassengerSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleCarSetPedInAsPassenger
{
public:
    CTaskSimpleCarSetPedInAsPassengerSA(){};
    CTaskSimpleCarSetPedInAsPassengerSA(CVehicle* pTargetVehicle, int iTargetDoor, CTaskUtilityLineUpPedWithCar* pUtility);

    void SetIsWarpingPedIntoCar();
    void SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear);
    void SetNumGettingInToClear(const unsigned char nNumGettingInToClear);
};

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedOut
// ## Purpose: Instantly removes the ped from the vehicle specified
// ##############################################################################

class CTaskSimpleCarSetPedOutSAInterface : public CTaskSimpleSAInterface
{
public:
    CVehicleSAInterface* m_pTargetVehicle;              // 8
    int                  m_iTargetDoor;                 // 12
    bool                 m_bSwitchOffEngine;            // 16
    bool                 m_bWarpingOutOfCar;
    bool                 m_bFallingOutOfCar;            // jumping or falling off car or bike
    bool                 m_bKnockedOffBike;
    unsigned char        m_nDoorFlagsToClear;
    unsigned char        m_nNumGettingInToClear;
};

class CTaskSimpleCarSetPedOutSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleCarSetPedOut
{
public:
    CTaskSimpleCarSetPedOutSA(){};
    CTaskSimpleCarSetPedOutSA(CVehicle* pTargetVehicle, int iTargetDoor, bool bSwitchOffEngine = false);

    void SetIsWarpingPedOutOfCar();
    void SetKnockedOffBike();
    void SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear);
    void SetNumGettingInToClear(const unsigned char nNumGettingInToClear);
    void PositionPedOutOfCollision(CPed* ped, CVehicle* vehicle, int nDoor);
};
