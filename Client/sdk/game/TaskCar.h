/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/TaskCar.h
 *  PURPOSE:     Car task interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

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
    virtual ~CTaskComplexEnterCar(){};

    virtual int  GetTargetDoor() = 0;
    virtual void SetTargetDoor(int iDoor) = 0;
    virtual int  GetEnterCarStartTime() = 0;
};

class CTaskComplexEnterCarAsDriver : public virtual CTaskComplexEnterCar
{
public:
    virtual ~CTaskComplexEnterCarAsDriver(){};
};

class CTaskComplexEnterCarAsPassenger : public virtual CTaskComplexEnterCar
{
public:
    virtual ~CTaskComplexEnterCarAsPassenger(){};
};

class CTaskComplexEnterBoatAsDriver : public virtual CTaskComplex
{
public:
    virtual ~CTaskComplexEnterBoatAsDriver(){};
};

class CTaskComplexLeaveCar : public virtual CTaskComplex
{
public:
    virtual ~CTaskComplexLeaveCar(){};

    virtual int GetTargetDoor() = 0;
};
