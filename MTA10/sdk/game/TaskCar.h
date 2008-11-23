/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/TaskCar.h
*  PURPOSE:		Car task interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_TASKCAR
#define __CGAME_TASKCAR

#include "Task.h"

enum
{
    DOOR_FRONT_LEFT = 0,
    DOOR_FRONT_RIGHT = 8,
    DOOR_REAR_RIGHT = 9,
    DOOR_REAR_LEFT = 11
};

class CTaskComplexEnterCar : public virtual CTaskComplex
{
public:
    virtual         ~CTaskComplexEnterCar ( void ) {};

    virtual int     GetTargetDoor()=0;
    virtual void    SetTargetDoor(int iDoor)=0;
    virtual int     GetEnterCarStartTime()=0;
};

class CTaskComplexEnterCarAsDriver : public virtual CTaskComplexEnterCar
{
public:
    virtual         ~CTaskComplexEnterCarAsDriver ( void ) {};
};

class CTaskComplexEnterCarAsPassenger : public virtual CTaskComplexEnterCar
{
public:
    virtual         ~CTaskComplexEnterCarAsPassenger ( void ) {};
};

class CTaskComplexEnterBoatAsDriver : public virtual CTaskComplex
{
public:
    virtual         ~CTaskComplexEnterBoatAsDriver ( void ) {};
};

class CTaskComplexLeaveCar : public virtual CTaskComplex
{
public:
    virtual         ~CTaskComplexLeaveCar ( void ) {};
};

#endif