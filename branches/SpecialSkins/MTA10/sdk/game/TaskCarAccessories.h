/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/TaskCarAccessories.h
*  PURPOSE:     Car accessories task interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_TASKCARACCESSORIES
#define __CGAME_TASKCARACCESSORIES

#include "Task.h"

class CTaskSimpleCarSetPedInAsDriver : public virtual CTaskSimple
{
public:
    virtual     ~CTaskSimpleCarSetPedInAsDriver ( void ) {};

    virtual void SetIsWarpingPedIntoCar()=0;
    virtual void SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear)=0;
    virtual void SetNumGettingInToClear(const unsigned char nNumGettingInToClear)=0;
};

class CTaskSimpleCarSetPedOut : public virtual CTaskSimple
{
public:
    virtual     ~CTaskSimpleCarSetPedOut ( void ) {};

    virtual void SetIsWarpingPedOutOfCar()=0;
    virtual void SetKnockedOffBike()=0;
    virtual void SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear)=0;
    virtual void SetNumGettingInToClear(const unsigned char nNumGettingInToClear)=0;
    virtual void PositionPedOutOfCollision(CPed * ped, CVehicle * vehicle, int nDoor)=0;
};

class CTaskSimpleCarSetPedInAsPassenger : public virtual CTaskSimple
{
public:
    virtual     ~CTaskSimpleCarSetPedInAsPassenger ( void ) {};

    virtual void SetIsWarpingPedIntoCar()=0;
    virtual void SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear)=0;
    virtual void SetNumGettingInToClear(const unsigned char nNumGettingInToClear)=0;
};

#endif
