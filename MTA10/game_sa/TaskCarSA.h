/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/TaskCarSA.h
*  PURPOSE:		Car game tasks
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASKCAR
#define __CGAMESA_TASKCAR

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <game/TaskCar.h>

#include "CVehicleSA.h"
#include "TaskSA.h"

// temporary
class CAnimBlendAssociation;
typedef DWORD CTaskUtilityLineUpPedWithCar;
typedef DWORD AnimationId;

#define FUNC_CTaskComplexEnterCarAsDriver__Constructor			    0x6402F0
#define FUNC_CTaskComplexEnterCarAsPassenger__Constructor           0x640340
#define FUNC_CTaskComplexEnterBoatAsDriver__Constructor             0x63B5E0
#define FUNC_CTaskComplexLeaveCar__Constructor                      0x63B8C0

// ##############################################################################
// ## Name:    CTaskComplexEnterCar                                    
// ## Purpose: Makes the ped enter the specified vehicle
// ## Notes:   Shouldn't be used directly, use CTaskComplexEnterCarAsDriver or
// ##          CTaskComplexEnterCarAsPassenger instead
// ##############################################################################

class CTaskComplexEnterCarSAInterface : public CTaskComplexSAInterface
{
public:
    CVehicle* m_pTargetVehicle;
    unsigned char m_bAsDriver:1;
	unsigned char m_bQuitAfterOpeningDoor:1;
	unsigned char m_bQuitAfterDraggingPedOut:1;
	unsigned char m_bCarryOnAfterFallingOff:1;
	
    int m_iTargetDoor;
    int m_iTargetDoorOppositeToFlag;
    int m_iTargetSeat;
    int m_iDraggedPedDownTime;
    int m_iMoveState;
    
    unsigned char m_nNumGettingInSet;
    unsigned char m_nCamMovementChoice;

    CVector m_vTargetDoorPos;
    CTaskUtilityLineUpPedWithCar* m_pTaskUtilityLineUpPedWithCar;
    bool m_bIsAborting;
    CPed* m_pDraggedPed;
    unsigned char m_nDoorFlagsSet;
    float m_fCruiseSpeed;
    int m_iEnterCarStartTime;
};

class CTaskComplexEnterCarSA : public virtual CTaskComplexSA
{
public:
    CTaskComplexEnterCarSA ( void ) {};
    CTaskComplexEnterCarSA ( CVehicle* pTargetVehicle, const bool bAsDriver, const bool bQuitAfterOpeningDoor, const bool bQuitAfterDraggingPedOut, const bool bCarryOnAfterFallingOff=false );

    int     GetTargetDoor() 
    { 
        CTaskComplexEnterCarSAInterface * thisInterface = (CTaskComplexEnterCarSAInterface*)this->GetInterface();
        return thisInterface->m_iTargetDoor;
    }
    void    SetTargetDoor( int iDoor )
    {
        CTaskComplexEnterCarSAInterface * thisInterface = (CTaskComplexEnterCarSAInterface*)this->GetInterface();
        thisInterface->m_iTargetDoor = iDoor;
    }

    int     GetEnterCarStartTime()     
    { 
        CTaskComplexEnterCarSAInterface * thisInterface = (CTaskComplexEnterCarSAInterface*)this->GetInterface();
        return thisInterface->m_iEnterCarStartTime;
    }
};

// ##############################################################################
// ## Name:    CTaskComplexEnterCarAsDriver                                    
// ## Purpose: Makes the ped enter the specified vehicle
// ##############################################################################

class CTaskComplexEnterCarAsDriverSAInterface : public CTaskComplexEnterCarSAInterface
{
public:

};

class CTaskComplexEnterCarAsDriverSA : public virtual CTaskComplexEnterCarSA, public virtual CTaskComplexEnterCarAsDriver
{
public:
    CTaskComplexEnterCarAsDriverSA ( void ) {};
	CTaskComplexEnterCarAsDriverSA ( CVehicle* pTargetVehicle );
	
    // from CTaskComplexEnterCarSA
    int     GetTargetDoor() { return CTaskComplexEnterCarSA::GetTargetDoor(); };
    void    SetTargetDoor(int iDoor) { return CTaskComplexEnterCarSA::SetTargetDoor(iDoor); };
    int     GetEnterCarStartTime() { return CTaskComplexEnterCarSA::GetEnterCarStartTime(); };
};

// ##############################################################################
// ## Name:    CTaskComplexEnterCarAsPassenger                                    
// ## Purpose: Makes the ped enter the specified vehicle as a passenger
// ##############################################################################

class CTaskComplexEnterCarAsPassengerSAInterface : public CTaskComplexEnterCarSAInterface
{
public:

};

class CTaskComplexEnterCarAsPassengerSA : public virtual CTaskComplexEnterCarSA, public virtual CTaskComplexEnterCarAsPassenger
{
public:
    CTaskComplexEnterCarAsPassengerSA ( void ) {};
	CTaskComplexEnterCarAsPassengerSA ( CVehicle* pTargetVehicle, const int iTargetSeat=0, const bool bCarryOnAfterFallingOff=false );
	
    // from CTaskComplexEnterCarSA
    int     GetTargetDoor() { return CTaskComplexEnterCarSA::GetTargetDoor(); };
    void    SetTargetDoor(int iDoor) { return CTaskComplexEnterCarSA::SetTargetDoor(iDoor); };
    int     GetEnterCarStartTime() { return CTaskComplexEnterCarSA::GetEnterCarStartTime(); };
};

// ##############################################################################
// ## Name:    CTaskComplexEnterBoatAsDriver                                    
// ## Purpose: Makes the ped enter the specified boat as the driver
// ##############################################################################

class CTaskComplexEnterBoatAsDriverSAInterface : public CTaskComplexSAInterface
{
public:

};

class CTaskComplexEnterBoatAsDriverSA : public virtual CTaskComplexSA, public virtual CTaskComplexEnterBoatAsDriver
{
public:
    CTaskComplexEnterBoatAsDriverSA ( void ) {};
	CTaskComplexEnterBoatAsDriverSA ( CVehicle* pTargetVehicle );
};

// ##############################################################################
// ## Name:    CTaskComplexLeaveCar                                    
// ## Purpose: Makes the ped leave a specific vehicle
// ##############################################################################

class CTaskComplexLeaveCarSAInterface : public CTaskComplexSAInterface
{
public:
    CVehicle* m_pTargetVehicle;    
    int m_iTargetDoor;
    int m_iDelayTime;
    bool m_bSensibleLeaveCar;
    bool m_bForceGetOut;

    bool m_bDie;

    unsigned char m_nDoorFlagsSet;
    unsigned char m_nNumGettingInSet;
    
    AnimationId m_dieAnim;
    float m_fDieAnimBlendDelta;
    float m_fDieAnimSpeed;
    
    unsigned char m_bIsInAir;
};

class CTaskComplexLeaveCarSA : public virtual CTaskComplexSA, public virtual CTaskComplexLeaveCar
{
public:
    CTaskComplexLeaveCarSA ( void ) {};
	CTaskComplexLeaveCarSA ( CVehicle* pTargetVehicle, const int iTargetDoor=0, const int iDelayTime=0, const bool bSensibleLeaveCar=true, const bool bForceGetOut=false );
};


#endif